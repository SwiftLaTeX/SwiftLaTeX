import { getBaseName, removeExtension } from './utils/fileUtilities';
import * as d3Selection from 'd3-selection';
import { EventReporter } from './utils/eventReport';

type CachedFileEntry = {
    fid: number;
    path: string;
};

type BatchEntry = {
    type: string;
    data: any;
};

type CursorPosition = {
    path: string;
    line: number;
    column: number;
};

const PUBLIC_PDFVIEWER_ENDPOINT = '/pdfviewer?uri=';

export class DviViewer {
    totalPage = 1;
    currentPage = 1;
    zoomRatio = 1;
    cachedFileList: CachedFileEntry[] = [];
    batchingQueue: BatchEntry[] = [];
    resourcesMap: any = {};
    isBatching = false;
    bufferedDocument: Document = undefined as any;
    cursorAttachElement: SVGTSpanElement | undefined = undefined;
    lastKnownCursorPosition: CursorPosition | undefined = undefined;

    constructor() {
        d3Selection.select('#toolbar-next').on('click', this.handleNextPage.bind(this));
        d3Selection.select('#toolbar-previous').on('click', this.handlePrevPage.bind(this));
        d3Selection.select('#toolbar-zoomin').on('click', this.handleZoomin.bind(this));
        d3Selection.select('#toolbar-zoomout').on('click', this.handleZoomOut.bind(this));
        d3Selection.select('#toolbar-input').on('change', this.handleInputChanged.bind(this));
        document.addEventListener('click', this._clickEventHandler.bind(this));
        window.addEventListener('message', this.receiveMessage.bind(this), false);
    }

    receiveMessage(event: any) {
        const data = event.data;
        const cmd = data.cmd;
        if (cmd === 'setContent') {
            const rawMeat = data.source;
            const rawMeatString = new TextDecoder('utf-8').decode(rawMeat);
            this.resourcesMap = data.resources;
            const domParser = new DOMParser();
            this.bufferedDocument = domParser.parseFromString(rawMeatString, 'text/xml');
            this.showPage();
        } else if (cmd === 'setCursor') {
            this.lastKnownCursorPosition = data;
            const res = this.showCursor(data.path, data.line, data.column);
            if (this.isBatching) {
                if (res) {
                    this.batchingQueue.push({ type: 'setCursor', data });
                } else {
                    this.isBatching = false; /* Give Up */
                    this.batchingQueue = [];
                    console.error('SetCursor return false, stop batching');
                }
            }
        } else if (cmd === 'typeContent') {
            let res = false;
            if (data.isInsert) {
                res = this.appendCharacter(data.delta);
            } else {
                res = this.deleteCharacter(data.delta);
            }
            if (this.isBatching) {
                if (res) {
                    this.batchingQueue.push({ type: 'typeContent', data });
                } else {
                    this.isBatching = false; /* Give Up */
                    this.batchingQueue = [];
                    console.error('TypeContent return false, stop batching');
                }
            }
        } else if (cmd === 'compileStart') {
            if (this.cursorAttachElement) {
                /* Not necessary to batch */
                this.batchingQueue.push({ type: 'setCursor', data: this.lastKnownCursorPosition });
                this.isBatching = true;
            }
        } else if (cmd === 'compileEnd') {
            this.isBatching = false;
        } else if (cmd === 'compileError') {
            /* Todo */
        }
    }

    _lookupResources(path: string): string | undefined {
        if (!path) return undefined;
        if (path.startsWith('./')) {
            path = path.slice(2);
        }
        if (path in this.resourcesMap) {
            return this.resourcesMap[path];
        }
        return undefined;
    }

    loadFonts() {
        let newFontData = '';
        const children = this.bufferedDocument.getElementsByTagName('fontdef');
        for (let j = 0; j < children.length; j++) {
            const child = children[j];
            const fonttype = child.getAttribute('fonttype');
            const fonturl = child.getAttribute('fonturl')!;
            const fontsize = child.getAttribute('fontsize');
            const fontid = child.getAttribute('fontid');
            const fontbaseWithExt = getBaseName(fonturl);
            const fontbase = removeExtension(fontbaseWithExt);
            if (fonttype === 'native') {
                if (fonturl.startsWith('/tex/')) {
                    const astyle = `<style>@font-face {font-family:${fontbase}; src:url(https://texlive.swiftlatex.com/${fontbaseWithExt});} .ff${fontid} {font-family: ${fontbase}; font-size:${fontsize}px}</style>\n`;
                    newFontData += astyle;
                } else {
                    const remoteUrl = this._lookupResources(fonturl);
                    if (!remoteUrl) {
                        console.error('Unable to locate user font');
                    } else {
                        const astyle = `<style>@font-face {font-family:${fontbase}; src:url(${remoteUrl});} .ff${fontid} {font-family: ${fontbase}; font-size:${fontsize}px}</style>\n`;
                        newFontData += astyle;
                    }
                }
            } else {
                const astyle = `<style>@font-face {font-family:${fontbase}; src:url(https://texlive.swiftlatex.com/fonts/${fontbase}.woff);} .ff${fontid} {font-family: ${fontbase}; font-size:${fontsize}px}</style>\n`;
                newFontData += astyle;
            }
        }
        const originFontData = document.getElementById('fontLoader')!.innerHTML;

        if (newFontData !== originFontData) {
            document.getElementById('fontLoader')!.innerHTML = newFontData;
        }
    }

    loadFileLists() {
        this.cachedFileList = [];
        const children = this.bufferedDocument.getElementsByTagName('file');
        for (let j = 0; j < children.length; j++) {
            const child = children[j];
            const fid = parseInt(child.getAttribute('fid')!) + 1;
            let path = child.getAttribute('url')!;
            if (path.startsWith('./')) {
                path = path.slice(2);
            }
            this.cachedFileList.push({ fid, path });
        }
    }

    replayBatchingQueue() {
        for (let j = 0; j < this.batchingQueue.length; j++) {
            const tmp = this.batchingQueue[j];
            const data = tmp.data;
            const cmdType = tmp.type;
            if (cmdType === 'setCursor') {
                this.showCursor(data.path, data.line, data.column);
            } else if (cmdType === 'typeContent') {
                if (data.isInsert) {
                    this.appendCharacter(data.delta);
                } else {
                    this.deleteCharacter(data.delta);
                }
            }
        }
        this.batchingQueue = [];
        this.isBatching = false;
    }

    showPage() {
        if (!this.bufferedDocument) return;
        this.cursorAttachElement = undefined;
        /* Update Input Field */
        const pages = this.bufferedDocument.getElementsByTagName('page');
        this.totalPage = pages.length;
        if (this.currentPage > this.totalPage) {
            this.currentPage = this.totalPage;
        }
        (document.getElementById(
            'toolbar-input'
        )! as HTMLInputElement).value = `${this.currentPage}/${this.totalPage}`;

        /* File List */
        this.loadFileLists();

        /* Font */
        this.loadFonts();

        const htmlData = pages[this.currentPage - 1].childNodes[0].nodeValue!;
        document.getElementById('viewer')!.innerHTML = htmlData;

        /* Replay Event */
        if (this.batchingQueue.length > 0) {
            /* Replay batching queue */
            this.replayBatchingQueue();
        } else {
            if (this.lastKnownCursorPosition) {
                this.showCursor(
                    this.lastKnownCursorPosition.path,
                    this.lastKnownCursorPosition.line,
                    this.lastKnownCursorPosition.column
                );
            }
        }

        /* Display Image */
        const elements = document.getElementsByClassName('userPic');
        for (let i = 0; i < elements.length; i++) {
            this._loadImage(elements[i]);
        }
    }

    pathToFid(path: string) {
        for (let j = 0; j < this.cachedFileList.length; j++) {
            const tmp = this.cachedFileList[j];
            if (tmp.path === path) {
                return tmp.fid;
            }
        }
        return 0;
    }

    fidToPath(fid: number) {
        for (let j = 0; j < this.cachedFileList.length; j++) {
            const tmp = this.cachedFileList[j];
            if (tmp.fid === fid) {
                return tmp.path;
            }
        }
        return '';
    }

    _loadImage(target: Element) {
        if (target.hasAttribute('loaded')) return;
        target.setAttribute('loaded', '1');
        if (!target.hasAttribute('url')) return;
        const url = target.getAttribute('url')!;
        const remoteUrl = this._lookupResources(url);
        if (!remoteUrl) return;
        if (url.endsWith('pdf')) {
            target.setAttribute('href', PUBLIC_PDFVIEWER_ENDPOINT + encodeURIComponent(remoteUrl));
        } else {
            target.setAttribute('href', remoteUrl);
        }
    }

    _clickEventHandler(e: MouseEvent) {
        const tspan = e.target as Element;
        if (tspan.tagName !== 'tspan') {
            return;
        }
        if (tspan.hasAttribute('l')) {
            const line = parseInt(tspan.getAttribute('l')!);
            const column = parseInt(tspan.getAttribute('c')!);
            const fileID = parseInt(tspan.getAttribute('f')!);
            const path = this.fidToPath(fileID);
            const command = tspan.getAttribute('t');
            if (line > 0 && path.length > 0) {
                EventReporter.reportEvent('viewer', 'clickText');
                const msg = {
                    cmd: 'setCursor',
                    line,
                    column,
                    path,
                    command,
                };
                if (window.top) {
                    window.top.postMessage(msg, '*');
                }
                if (window.opener) {
                    window.opener.postMessage(msg, '*');
                    window.open('', 'parent-window')!.focus();
                }
            }
        }
    }

    handleNextPage() {
        if (this.currentPage + 1 <= this.totalPage) {
            this.currentPage += 1;
            this.showPage();
        }
    }

    handlePrevPage() {
        if (this.currentPage - 1 >= 1) {
            this.currentPage -= 1;
            this.showPage();
        }
    }

    handleZoomin() {
        console.log('Zoom in');
        this.zoomRatio += 0.1;
        this.doZoom();
    }

    doZoom() {
        document.getElementById('viewer')!.style.transform = `scale(${this.zoomRatio})`;
    }

    handleZoomOut() {
        if (this.zoomRatio >= 0.1) {
            this.zoomRatio -= 0.1;
            this.doZoom();
        }
    }

    handleInputChanged() {
        if (!this.bufferedDocument) {
            return;
        }
        const inputElement = document.getElementById('toolbar-input')! as HTMLInputElement;
        let input_val = inputElement.value;
        if (input_val.includes('/')) {
            input_val = input_val.split('/')[0];
        }
        const vv = parseInt(input_val);
        if (!isNaN(vv)) {
            if (vv >= 1 && vv <= this.totalPage && vv !== this.currentPage) {
                this.currentPage = vv;
                this.showPage();
                return;
            }
        }
        inputElement.value = `${this.currentPage}/${this.totalPage}`;
    }

    showCursor(path: string, line: number, column: number) {
        if (!this.bufferedDocument) {
            return false;
        }

        /* Clean up */
        d3Selection.select('.cursor').remove();
        d3Selection.select('.fuzzyspan').remove();

        this.cursorAttachElement = undefined;

        /* Start looking up */
        let fuzzyCursorEnabled = false;
        let fuzzySpace = false;
        const fid = this.pathToFid(path);
        if (fid === 0) {
            return false;
        }

        const lineFilter = d3Selection.selectAll(`tspan[l="${line}"][f="${fid}"]`);
        let r = lineFilter.filter(`tspan[c="${column}"]`);
        if (r.empty()) {
            if (column <= 1) {
                /* No hope */
                return;
            }

            /* Fuzzy logic, try to locate prev text span */
            const tryAttempt = 5;
            let i = 1;
            for (; i < tryAttempt; i++) {
                const potentialColumn = column - i;
                if (potentialColumn < 1) break;
                r = lineFilter.filter(`tspan[c="${potentialColumn}"]`);
                if (!r.empty()) {
                    column = potentialColumn + 1;
                    fuzzyCursorEnabled = true;
                    if (i > 1) fuzzySpace = true;
                    break;
                }
            }
            /* Only fuzzy cursor survive */
            if (!fuzzyCursorEnabled) return;
        }

        this.cursorAttachElement = r.node() as SVGTSpanElement;
        const bbox = this.cursorAttachElement.getBBox();
        let bbox_x = bbox.x;
        const parentTag = this.cursorAttachElement
            .parentNode! as Element; /* for locating baseline */
        const bbox_y = parseInt(parentTag.getAttribute('y')!) - 10;
        if (fuzzyCursorEnabled) {
            bbox_x = bbox.x + bbox.width;
            if (fuzzySpace) bbox_x += bbox.width; /* add one more */
            /* We create a dummy text tag */
            const newTspanTag = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
            newTspanTag.setAttribute('c', column.toString());
            newTspanTag.setAttribute('l', line.toString());
            newTspanTag.setAttribute('f', fid.toString());
            newTspanTag.setAttribute('t', '0');
            newTspanTag.setAttribute('class', 'fuzzyspan');
            newTspanTag.textContent = '_';
            if (fuzzySpace) newTspanTag.textContent += '_'; /* add one more */
            parentTag.insertBefore(newTspanTag, this.cursorAttachElement.nextSibling);
            this.cursorAttachElement = newTspanTag;
        }
        d3Selection
            .select('svg')
            .append('rect')
            .attr('x', bbox_x)
            .attr('y', bbox_y)
            .attr('width', 1)
            .attr('height', 10)
            .attr('class', 'cursor')
            .attr('fill', 'red')
            .attr('fill', 'red');
        return true;
    }

    appendCharacter(character: string) {
        if (!this.bufferedDocument) {
            return false;
        }
        if (!this.cursorAttachElement) {
            return false;
        }
        const column = parseInt(this.cursorAttachElement.getAttribute('c')!);
        const line = parseInt(this.cursorAttachElement.getAttribute('l')!);
        const fid = parseInt(this.cursorAttachElement.getAttribute('f')!);
        const textTag = this.cursorAttachElement.parentNode! as Element;
        const newTspanTag = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
        newTspanTag.setAttribute('c', column.toString());
        newTspanTag.setAttribute('l', line.toString());
        newTspanTag.setAttribute('f', fid.toString());
        newTspanTag.setAttribute('t', '0');
        newTspanTag.textContent = character;
        textTag.insertBefore(newTspanTag, this.cursorAttachElement);

        const updateColumns = d3Selection
            .selectAll(`tspan[l="${line}"]`)
            .filter(`tspan[f="${fid}"]`);
        updateColumns.each(function (_) {
            const that = this as SVGTSpanElement;
            if (that === newTspanTag) return;
            const originalColumn = parseInt(that.getAttribute('c')!);
            if (originalColumn >= column) {
                that.setAttribute('c', (originalColumn + 1).toString());
            }
        });

        /* Measure the newly inserted width */
        const width = newTspanTag.getBBox().width;
        const baseline = textTag.getAttribute('y');
        let j = 0;
        let tmp1 = textTag.nextElementSibling;
        while (tmp1 && j < 128) {
            j++;
            const originalBaseline = tmp1.getAttribute('y');
            if (originalBaseline !== baseline) {
                break;
            }
            const originalX = parseInt(tmp1.getAttribute('x')!);
            tmp1.setAttribute('x', (originalX + width).toString());
            tmp1 = tmp1.nextElementSibling;
        }

        /* Update the position of the text span in the same row */
        d3Selection.select('.cursor').remove();
        this.cursorAttachElement = undefined;
        return true;
    }

    deleteCharacter(character: string) {
        if (!this.bufferedDocument) {
            return false;
        }
        if (!this.cursorAttachElement) {
            return false;
        }
        const column = parseInt(this.cursorAttachElement.getAttribute('c')!);
        const line = parseInt(this.cursorAttachElement.getAttribute('l')!);
        const fid = parseInt(this.cursorAttachElement.getAttribute('f')!);
        let prevSib = this.cursorAttachElement.previousElementSibling as SVGTSpanElement;
        if (!prevSib) {
            // console.error("Attempt to find ");
            /* We need to check previous text span*/
            const parentTextTag = this.cursorAttachElement.parentNode! as Element;
            const prevParentTextTag = parentTextTag.previousElementSibling;
            if (!prevParentTextTag) {
                // console.error("No prev text");
                return false;
            }
            const lastTspan = prevParentTextTag.lastElementChild;
            if (!lastTspan) {
                // console.error("No last tspan");
                return false;
            }
            /* We need them to be on the same line */
            const lastTspanColumn = parseInt(lastTspan.getAttribute('c')!);
            const lastTspanLine = parseInt(lastTspan.getAttribute('l')!);
            if (lastTspanLine === line && lastTspanColumn + 1 === column) {
                prevSib = lastTspan as SVGTSpanElement;
            } else {
                // console.log(lastTspanLine + ' ' + line + ' ' + lastTspanColumn + ' ' + column);
                // console.error("Here failed");
                return false;
            }
        }

        const prevParentTag = prevSib.parentNode! as Element;
        if (prevSib.textContent !== character) {
            return false;
        }

        /* Update Column */
        const updateColumns = d3Selection
            .selectAll(`tspan[l="${line}"]`)
            .filter(`tspan[f="${fid}"]`);
        updateColumns.each(function (_) {
            const that = this as SVGTSpanElement;
            const originalColumn = parseInt(that.getAttribute('c')!);
            if (originalColumn >= column) {
                that.setAttribute('c', (originalColumn - 1).toString());
            }
        });

        /* Measure the newly deleted width */
        const width = prevSib.getBBox().width;
        const baseline = prevParentTag.getAttribute('y');
        let tmp1 = prevParentTag.nextElementSibling;
        let j = 0;
        while (tmp1 && j < 128) {
            j++;
            const originalBaseline = tmp1.getAttribute('y');
            if (originalBaseline !== baseline) {
                break;
            }
            const originalX = parseInt(tmp1.getAttribute('x')!);
            tmp1.setAttribute('x', (originalX - width).toString());
            tmp1 = tmp1.nextElementSibling;
        }

        /* Clean up */
        d3Selection.select(prevSib).remove();
        if (prevParentTag.childElementCount === 0) {
            d3Selection.select(prevParentTag).remove();
        }

        d3Selection.select('.cursor').remove();
        this.cursorAttachElement = undefined;
        return true;
    }
}

(window as any).SwiftLaTeXViewer = new DviViewer();
