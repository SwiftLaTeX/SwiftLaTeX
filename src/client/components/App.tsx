import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import debounce from 'lodash/debounce';
import isString from 'lodash/isString';
import isBoolean from 'lodash/isBoolean';
import EditorView from './EditorView';
import updateEntry from '../actions/updateEntry';
import AnimatedLogo from './shared/AnimatedLogo';
import { LaTeXEngine, CompileResult } from '../swiftlatex/latexEngine';
import JSZip from 'jszip';
import {
    arrayBufferToJson,
    arrayBufferToString,
    isImageFile,
    isOpenTypeFontFile,
    triggerDownloadBlob,
} from '../utils/fileUtilities';
import { initializeStorage, BackendStorage } from '../storage/index';
import { EventReporter } from '../utils/eventReport';
import * as Y from 'yjs';
import { WebsocketProvider } from 'y-websocket';
import { IndexeddbPersistence } from 'y-indexeddb';
import {
    EngineVersion,
    Annotation,
    FileManagerEntry,
    SaveStatus,
    ProjectEntry,
    DEFAULT_ENGINE_VERSION, ProjectFileEntry,
} from '../types';
import { DvipdfmxEngine } from '../swiftlatex/dvipdfmxEngine';
import * as monaco from 'monaco-editor';
import { YTextEvent } from 'yjs';

enum SessionStatus {
    Init,
    LoadComponents,
    LoadFiles,
    Ready,
    LoadComponentsFailed,
    LoadFilesFailed,
}

type State = {
    id: string;
    username: string;
    name: string;
    shareEnabled: boolean;
    isEngineBusy: boolean;
    sessionStatus: SessionStatus;
    sendCodeOnChangeEnabled: boolean;
    saveStatus: SaveStatus;
    fileEntries: FileManagerEntry[];
    engineErrors: Annotation[] | undefined;
    engineLogs: string;
    entryPoint: string;
    engine: EngineVersion;
};


export default class App extends React.Component<any, State> {
    constructor(props: any) {
        super(props);
        const urlParams = new URLSearchParams(window.location.search);
        const project_id = urlParams.get('p') || '';
        this.state = {
            id: project_id,
            username: '',
            name: '',
            shareEnabled: false,
            isEngineBusy: false,
            sessionStatus: SessionStatus.Init,
            sendCodeOnChangeEnabled: true,
            saveStatus: 'changed',
            fileEntries: [],
            engineLogs: '',
            engineErrors: undefined,
            entryPoint: 'main.tex',
            engine: DEFAULT_ENGINE_VERSION,
        };
        EventReporter.reportPageView();
    }

    componentDidMount() {
        const project_id = this.state.id;
        if (project_id === '') {
            this.setState({ sessionStatus: SessionStatus.LoadFilesFailed });
            return;
        }

        this._initSession().then();
    }

    _latexEngine: LaTeXEngine = undefined as any;
    _backendStorage: BackendStorage | undefined = undefined;


    async _loadFileFromStorage(tmp: any): Promise<FileManagerEntry | undefined> {
        const importedEntry: FileManagerEntry = {
            item: {
                type: tmp.type,
                asset: tmp.asset,
                uri: tmp.uri,
                content: '',
                id: tmp.id,
                path: tmp.path,
            },
            state: {},
        };

        if (tmp.type === 'file') {
            try {
                if (!tmp.asset) {
                    /* Is a text file */
                    const tmp_content_buf = await this._backendStorage!.get('asset', tmp.id);
                    if (!tmp_content_buf) {
                        throw `Unable to fetch asset ${tmp.id}`;
                    }
                    importedEntry.item.content = arrayBufferToString(tmp_content_buf);
                    return importedEntry;
                } else if (isImageFile(tmp.path) || isOpenTypeFontFile(tmp.path)) {
                    const tmp_content_buf = await this._backendStorage!.get('asset', tmp.id);
                    if (!tmp_content_buf) {
                        throw `Unable to fetch asset ${tmp.id}`;
                    }
                    importedEntry.item.content = tmp_content_buf;
                    return importedEntry;
                }
            } catch (e) {
                console.error('Failed to load ' + tmp.path);
            }
        } else {
            return importedEntry;
        }
        return undefined;
    }

    async _loadProjectFiles() {
        const manifest_buffer = await this._backendStorage!.get('manifest', this.state.id);
        if (!manifest_buffer) {
            throw `Unable to fetch manifest ${this.state.id}`;
        }
        const manifest = arrayBufferToJson(manifest_buffer) as ProjectEntry;
        const username = manifest.username;
        const name = manifest.name;
        let entryPoint = manifest.entryPoint;
        const fileEntries = manifest.fileEntries;
        const shareEnabled = !(!manifest.shareEnabled); /* Haha, javascript */
        if (!isString(username) || !isString(name) || !isString(entryPoint) || !fileEntries || !Array.isArray(fileEntries)) {
            throw new Error('Malformed Manifest');
        }

        const importedEntries: FileManagerEntry[] = [];
        let entryPointFound = false;
        const fileLoadingPromises = [];
        for (let j = 0; j < fileEntries.length; j++) {
            const tmp = fileEntries[j];
            if (
                !isString(tmp.type) ||
                !isString(tmp.id) ||
                !isString(tmp.path) ||
                !isString(tmp.uri) ||
                !isBoolean(tmp.asset)
            ) {
                throw new Error('Malformed Manifest');
            }
            fileLoadingPromises.push(this._loadFileFromStorage(tmp));
        }

        /* Multithreading */
        while (fileLoadingPromises.length) {
            // 10 at at time
            const batch = await Promise.all(fileLoadingPromises.splice(0, 10));
            for (const tmp of batch) {
                if (tmp) {
                    if (tmp.item.path === entryPoint) {
                        entryPointFound = true;
                        tmp.state.isOpen = true;
                        tmp.state.isFocused = true;
                    }
                    importedEntries.push(tmp);
                }
            }
        }

        if (!entryPointFound) {
            entryPoint = '';
        }

        this.setState({
            username,
            name,
            fileEntries: importedEntries,
            entryPoint,
            shareEnabled: shareEnabled,
        });

        if (shareEnabled) {
            /* Start YJS Instance */
            await this._initYJSComponent();
        }
    }

    async _initSession() {
        /* Load Components */
        try {
            this._backendStorage = initializeStorage();
        } catch {
            this.setState({
                sessionStatus: SessionStatus.LoadComponentsFailed,
            });
            return;
        }

        this.setState({ sessionStatus: SessionStatus.LoadComponents });
        try {
            this._latexEngine = new LaTeXEngine();
            await this._latexEngine.loadEngine();
        } catch {
            this.setState({
                sessionStatus: SessionStatus.LoadComponentsFailed,
            });
            return;
        }

        this.setState({ sessionStatus: SessionStatus.LoadFiles });
        try {
            await this._loadProjectFiles();
        } catch (e) {
            this.setState({
                sessionStatus: SessionStatus.LoadFilesFailed,
            });
            return;
        }


        this.setState({ sessionStatus: SessionStatus.Ready });
    }

    _handleChangeEngineVersion = async (engine: EngineVersion) => {
        if (this.state.isEngineBusy || this.state.engine === engine) {
            return;
        }
        this.setState({ isEngineBusy: true });
        EventReporter.reportEvent('editor', 'switchEngine', engine);
        this._latexEngine && this._latexEngine.closeWorker();
        try {
            this._latexEngine = new LaTeXEngine(engine);
            await this._latexEngine.loadEngine();
            this._requiredFlushInEngine = true;
            this.setState({ engine, isEngineBusy: false });
        } catch {
            this.setState({ sessionStatus: SessionStatus.LoadComponentsFailed });
        }
    };

    _compareFileTreeStructure(
        nextFileEntries: FileManagerEntry[],
        originalFileEntries: FileManagerEntry[],
    ) {
        if (originalFileEntries.length !== nextFileEntries.length) {
            return true;
        } else {
            for (let i = 0; i < nextFileEntries.length; i++) {
                if (
                    nextFileEntries[i].item.id !== originalFileEntries[i].item.id
                    || nextFileEntries[i].item.path !== originalFileEntries[i].item.path
                ) {
                    return true;
                }
            }
        }
        return false;
    }

    _copyEntryBeforeUpload = (tmp: FileManagerEntry) => {
        const entry = {
            uri: tmp.item.uri,
            asset: tmp.item.asset,
            type: tmp.item.type,
            path: tmp.item.path,
            id: tmp.item.id,
        };
        return entry;
    };

    _uploadFileTree = async () => {
        /* This function uploads all modified files and generate a manifest file for project infos */
        const currentStateEntries = this.state.fileEntries;
        const fileEntriesCopy = [];

        for (let j = 0; j < currentStateEntries.length; j++) {
            const tmp = currentStateEntries[j];
            const copyTmp = this._copyEntryBeforeUpload(tmp);
            /* Handle upload modified code files */
            if (tmp.item.type === 'file') {
                if (!tmp.item.asset) {
                    let needUpload = false;
                    if (tmp.item.uri === '') {
                        /* Without a url, indicate it is a newly create file, always need to upload */
                        needUpload = true;
                    } else {
                        /* Not empty uri, we need to compare and see whether it changes or not */
                        if (this._requiredUploadFiles.includes(tmp.item.id)) {
                            needUpload = true;
                        }
                    }
                    if (needUpload) {
                        console.log('Uploading ' + tmp.item.path);
                        const theBlob = new Blob([tmp.item.content], { type: 'text/tex' });
                        try {
                            const remoteUrl = await this._backendStorage!.put(
                                'asset',
                                tmp.item.id,
                                theBlob,
                            );
                            copyTmp.uri = remoteUrl;
                            if (remoteUrl !== tmp.item.uri) {
                                this.setState((state: State) => ({
                                    fileEntries: state.fileEntries.map((entry) => {
                                        if (entry.item.path === tmp.item.path) {
                                            return updateEntry(entry, { item: { uri: remoteUrl } });
                                        }
                                        return entry;
                                    }),
                                }));
                            }
                        } catch (e) {
                            console.error(
                                'An upload error is detected, silently ignored?' + tmp.item.path,
                            );
                        }
                        // console.log('Uploading finish' + tmp.item.path);
                    }
                }
            }
            fileEntriesCopy.push(copyTmp);
        }
        this._requiredUploadFiles = [];
        return fileEntriesCopy;
    };

    _requiredFlushInEngine = true;
    _requiredUpdateFilesInEngine: string[] = [];
    _requiredUploadFiles: string[] = [];

    componentDidUpdate(_: any, __: State) {

        if (this.state.sessionStatus !== SessionStatus.Ready) {
            return;
        }

        if (this.state.isEngineBusy) {
            return;
        }

        if (this.state.sendCodeOnChangeEnabled) {
            if (this.state.fileEntries.length > 0 && this.state.entryPoint) {
                if (this._requiredFlushInEngine || this._requiredUpdateFilesInEngine.length > 0) {
                    this._fireEngine();
                }
            }
        }

        if (this.state.saveStatus !== 'published') {
            // /* This part is for sync manifest */
            this._syncManifest();
        }
    }

    componentWillUnmount() {
        this._latexEngine && this._latexEngine.closeWorker();
        this.ydoc && this.ydoc.destroy();
        this.indexeddbProvider && this.indexeddbProvider.destroy();
        this.websocketProvider && this.websocketProvider.destroy();
    }

    _previewRef = React.createRef<Window>();

    _editorRef = React.createRef();

    _broadcastChannel: BroadcastChannel = undefined as any;

    _handleToggleSendCode = () =>
        this.setState((state) => ({ sendCodeOnChangeEnabled: !state.sendCodeOnChangeEnabled }));

    _handleSubmitTitle = async (name: string) => {
        if (!name) {
            name = 'Fancy Project';
        }
        this.setState({ name, saveStatus: 'changed' });
    };

    _syncManifestNoDebounce = async () => {
        EventReporter.reportEvent('editor', 'syncCode');
        const fileEntriesCopy = await this._uploadFileTree();
        const manifest = {
            name: this.state.name,
            entryPoint: this.state.entryPoint,
            username: this.state.username,
            modifiedTime: new Date().toString(),
            fileEntries: fileEntriesCopy,
            shareEnabled: this.state.shareEnabled,
            pid: this.state.id,
        } as ProjectEntry;
        const jsonStr = JSON.stringify(manifest);
        // console.log(jsonStr);
        const manifestBlob = new Blob([jsonStr]);
        try {
            await this._backendStorage!.put('manifest', this.state.id, manifestBlob);
        } catch (e) {
            console.error('Unable to upload manifest!');
        }

        this.setState({ saveStatus: 'published' });
    };

    _syncManifest = debounce(this._syncManifestNoDebounce, 3000);

    _generateResourceUrlMap = () => {
        const resourceMap: any = {};
        for (let i = 0; i < this.state.fileEntries.length; i++) {
            const entry = this.state.fileEntries[i];
            if (entry.item.asset) {
                resourceMap[entry.item.path] = entry.item.uri;
            }
        }
        return resourceMap;
    };

    _fireEngineNoDebounce = async () => {
        if (!this._latexEngine || !this._latexEngine.isReady()) return false;
        const currentFileEntries = this.state.fileEntries;

        if (this._requiredFlushInEngine) {
            this._latexEngine.flushCache();
            for (let j = 0; j < currentFileEntries.length; j++) {
                const tmp = currentFileEntries[j];
                if (tmp.item.type === 'folder') {
                    console.log('Make fresh dir ' + tmp.item.path);
                    this._latexEngine.makeMemFSFolder(tmp.item.path);
                }
            }
            for (let j = 0; j < currentFileEntries.length; j++) {
                const tmp = currentFileEntries[j];
                if (tmp.item.type === 'file') {
                    console.log('Write fresh file ' + tmp.item.path);
                    this._latexEngine.writeMemFSFile(tmp.item.path, tmp.item.content);
                }
            }
        } else {
            for (let j = 0; j < currentFileEntries.length; j++) {
                const tmp = currentFileEntries[j];
                if (this._requiredUpdateFilesInEngine.includes(tmp.item.path)) {
                    console.log('Update modified file ' + tmp.item.path);
                    this._latexEngine.writeMemFSFile(tmp.item.path, tmp.item.content);
                }
            }
        }
        this._requiredFlushInEngine = false;
        this._requiredUpdateFilesInEngine = [];

        this._latexEngine.setEngineMainFile(this.state.entryPoint);
        this.setState({ isEngineBusy: true });
        const preview = this._previewRef.current;
        if (preview) {
            preview.postMessage({ cmd: 'compileStart' }, '*');
        }
        console.log('Engine compilation start');
        const start_compile_time = performance.now();
        const r: CompileResult = await this._latexEngine.compileLaTeX();
        const past_time = performance.now() - start_compile_time;
        console.log('Engine compilation finish ' + past_time);
        EventReporter.reportTiming('editor', 'compile_timing', past_time);
        if (preview) {
            preview.postMessage({ cmd: 'compileEnd' }, '*');
        }
        this.setState({ isEngineBusy: false, engineLogs: r.log, engineErrors: r.errors });
        if (r.pdf) {
            EventReporter.reportEvent('editor', 'compile_ok');
            if (preview) {
                let content = r.pdf;
                preview.postMessage(
                    {
                        cmd: 'setContent',
                        source: content,
                        resources: this._generateResourceUrlMap(),
                    },
                    '*',
                );
            }
            return true;
        } else {
            EventReporter.reportEvent('editor', 'compile_failed', r.errors);
            if (preview) {
                preview.postMessage({ cmd: 'compileError' }, '*');
            }
            this._requiredFlushInEngine = true;
            return false;
        }
    };

    _fireEngine = debounce(this._fireEngineNoDebounce, 1000);

    _handleOnSendCode = () => {
        if (this.state.entryPoint) {
            this._requiredUpdateFilesInEngine.push(this.state.entryPoint);
            this.setState({ saveStatus: 'changed' });
        }
    };

    _handleChangeCode = (content: string, path: string) => {

        const updatedEntry = this.state.fileEntries.map((entry) => {
            if (
                entry.item.type === 'file' &&
                !entry.item.asset &&
                entry.item.path === path
            ) {
                if (!this._requiredUpdateFilesInEngine.includes(path)) {
                    this._requiredUpdateFilesInEngine.push(path);
                }
                if (!this._requiredUploadFiles.includes(entry.item.id)) {
                    this._requiredUploadFiles.push(entry.item.id);
                }
                return updateEntry(entry, { item: { content } });
            }
            return entry;
        });
        this.setState({
            saveStatus: 'changed',
            engineErrors: undefined,
            fileEntries: updatedEntry,
        });
    };

    _handleFileEntriesChange = (nextFileEntries: FileManagerEntry[]): Promise<void> => {
        const fileStructureChanged = this._compareFileTreeStructure(
            this.state.fileEntries,
            nextFileEntries,
        );
        if (fileStructureChanged) {
            if (this.state.shareEnabled) {
                this._publicTreeToRemote(nextFileEntries);
            }

            if(this._editorRef.current) {
                // Maybe rename or remove, we could free up some memory
                const paths = nextFileEntries.map(e => e.item.path);
                (this._editorRef.current as any).cleanUpModels(paths);
            }

            this._requiredFlushInEngine = true;
            this.setState({ saveStatus: 'changed' });
        }
        return new Promise((resolve) => {
            this.setState({ fileEntries: nextFileEntries }, resolve);
        });
    };

    _handleDownloadAsync = async () => {
        EventReporter.reportEvent('editor', 'download');
        this.setState({ isEngineBusy: true });

        const zipobj = new JSZip();
        for (let j = 0; j < this.state.fileEntries.length; j++) {
            const tmp = this.state.fileEntries[j];
            if (tmp.item.type === 'file') {
                zipobj.file(tmp.item.path, tmp.item.content);
            } else if (tmp.item.type === 'folder') {
            }
        }
        const blobFile = await zipobj.generateAsync({ type: 'blob' });
        triggerDownloadBlob(blobFile, 'export.zip');

        this.setState({ isEngineBusy: false });
    };

    _findFocusedEntry = (entries: FileManagerEntry[]): FileManagerEntry | undefined =>
        // @ts-ignore
        entries.find(({ item, state }) => item.type === 'file' && state.isFocused === true);

    _uploadAssetAsync = async (fileObj: File, id: string) => {
        return this._backendStorage!.put('asset', id, fileObj);
    };

    _handleSetEntryPoint = (path: string) => {
        this._requiredFlushInEngine = true;
        this.setState({ entryPoint: path, saveStatus: 'changed' });
    };

    _handlePDFTeXExport = async () => {
        let pdfCompileOk = true;
        this.setState({ isEngineBusy: true });
        let pdfResult: CompileResult = undefined as any;
        for (let j = 0; j < 3; j++) {
            pdfResult = await this._latexEngine.compileLaTeX(true);
            this.setState({ engineLogs: pdfResult.log, engineErrors: pdfResult.errors });
            if (!pdfResult.pdf) {
                pdfCompileOk = false;
                break;
            }
        }
        this.setState({ isEngineBusy: false });
        if (pdfCompileOk) {
            triggerDownloadBlob(new Blob([pdfResult.pdf!]), 'export.pdf');
        } else {
            this._requiredFlushInEngine = true; /* Error, we require flush */
        }
    };

    _handleXeTeXExport = async () => {
        /* Only XeLaTeX requires DVIPDFMX */
        const export_engine = new DvipdfmxEngine();
        await export_engine.loadEngine();
        let dviCompileOk = true;
        /* Compile DVI First */

        this.setState({ isEngineBusy: true });
        let xdvResult: CompileResult = undefined as any;
        for (let j = 0; j < 3; j++) {
            xdvResult = await this._latexEngine.compileLaTeX(true);
            this.setState({ engineLogs: xdvResult.log, engineErrors: xdvResult.errors });
            if (!xdvResult.pdf) {
                dviCompileOk = false;
                break;
            }
        }

        /* Feed XDV and Resources to DVIPDFMX */
        if (dviCompileOk) {
            const currentFileEntries = this.state.fileEntries;
            for (let j = 0; j < currentFileEntries.length; j++) {
                const tmp = currentFileEntries[j];
                if (tmp.item.type === 'folder') {
                    // console.error('Export: Make fresh dir ' + tmp.item.path);
                    export_engine.makeMemFSFolder(tmp.item.path);
                }
            }
            for (let j = 0; j < currentFileEntries.length; j++) {
                const tmp = currentFileEntries[j];
                if (tmp.item.type === 'file') {
                    // console.error('Export: write fresh file ' + tmp.item.path);
                    export_engine.writeMemFSFile(tmp.item.path, tmp.item.content);
                }
            }
            const xdvPath =
                this.state.entryPoint.substr(0, this.state.entryPoint.length - 4) + '.xdv';
            // console.error('Export: write xdv file ' + xdvPath);
            export_engine.writeMemFSFile(xdvPath, xdvResult.pdf!);
            export_engine.setEngineMainFile(this.state.entryPoint);
            const r: CompileResult = await export_engine.compilePDF();
            if (r.pdf) {
                triggerDownloadBlob(new Blob([r.pdf]), 'export.pdf');
            } else {
                this.setState({ engineLogs: r.log, engineErrors: r.errors });
            }
        }
        this.setState({ isEngineBusy: false });
        export_engine.closeWorker();

        /* Weird */
        if (!dviCompileOk) {
            this._requiredFlushInEngine = true;
        }
    };

    _handleExportPDF = async () => {
        if (!this._latexEngine || !this._latexEngine.isReady()) {
            return;
        }

        if (!this.state.entryPoint || this.state.fileEntries.length === 0) {
            return;
        }

        if (this._requiredFlushInEngine || this._requiredUpdateFilesInEngine.length > 0) {
            return;
        }

        EventReporter.reportEvent('editor', 'export');

        if (this.state.engine === 'PDFLaTeX') {
            await this._handlePDFTeXExport();
        } else {
            await this._handleXeTeXExport();
        }
    };

    _handleShareProject = async () => {
        if (this.state.shareEnabled) {
            return true;
        }
        try {
            this.setState({ isEngineBusy: true });
            const publicUrl = await this._backendStorage!.getPublicUrl('manifest', this.state.id);
            if (publicUrl) {
                const manifestUrl = encodeURIComponent(publicUrl);
                const fetchResult = await fetch(`/s/create?p=${this.state.id}&uri=${manifestUrl}`);
                if (fetchResult.status === 200) {
                    this.setState({ shareEnabled: true, saveStatus: 'changed' });
                    await this._initYJSComponent();
                    return true;
                }
            }
        } catch (e) {
            console.error(e.message);
        } finally {
            this.setState({ isEngineBusy: false });
        }

        return false;
    };


    ydoc: Y.Doc = undefined as any;
    yfiles: Y.Array<ProjectFileEntry> = undefined as any;
    websocketProvider: WebsocketProvider = undefined as any;
    indexeddbProvider: IndexeddbPersistence = undefined as any;
    ytextmap: Map<string, Y.Text> = new Map<string, Y.Text>();
    yJustPublishTree = false;
    _mergeRemoteContentToLocal = async () => {
        let importedEntries: FileManagerEntry[] = [];

        let newAssets: { [key: string]: ArrayBuffer } = {};
        // Download and save the newly added asset files
        for (let remoteEntry of this.yfiles) {
            if (remoteEntry.type === 'file' && remoteEntry.asset) {
                const existingEntry = this.state.fileEntries.find(localEntry => {
                    return localEntry.item.id === remoteEntry.id;
                });
                if (!existingEntry) {
                    const uri = remoteEntry.uri;
                    const assetRequest = await fetch(`/s/fetch?uri=${encodeURIComponent(uri)}`);
                    if (!assetRequest.ok) {
                        throw 'Unable to pull shared resources';
                    }
                    const arrayBlob = await assetRequest.arrayBuffer();
                    newAssets[remoteEntry.id] = arrayBlob;
                    await this._backendStorage!.put('asset', remoteEntry.id, new Blob([arrayBlob]));
                }
            }
        }

        // Make new fileEntries
        for (let remoteEntry of this.yfiles) {
            let content: string | ArrayBuffer = undefined as any;
            const existingEntry = this.state.fileEntries.find(localEntry => {
                return localEntry.item.id === remoteEntry.id;
            });
            if (remoteEntry.type === 'file') {
                if (remoteEntry.asset) {
                    if (!existingEntry) {
                        content = newAssets[remoteEntry.id];
                    } else {
                        content = existingEntry.item.content;
                    }
                } else {
                    const ytext = this.ydoc.getText(remoteEntry.id);
                    content = ytext.toString();
                    if (!this.ytextmap.has(remoteEntry.id)) {
                        this.ytextmap.set(remoteEntry.id, ytext);
                        /* Bind event */
                        ytext.observe((event) => {
                            this._yTextEventListener(event, ytext, remoteEntry.id);
                        });
                    }
                }
            }

            let importedEntry: FileManagerEntry = {
                item: {
                    type: remoteEntry.type,
                    asset: remoteEntry.asset,
                    uri: remoteEntry.uri,
                    content: content,
                    id: remoteEntry.id,
                    path: remoteEntry.path,
                },
                state: existingEntry ? existingEntry.state : {},
            };

            importedEntries.push(importedEntry);
        }
        this._requiredFlushInEngine = true;
        this.setState({ fileEntries: importedEntries, saveStatus: 'changed' });
    };

    _yTextEventListener = (event: YTextEvent, ytext: Y.Text, id: string) => {
        const found = this.state.fileEntries.find(element => {
            return element.item.id === id;
        });
        if (found) {
            if (found.state.isFocused) {
                // It is the current editing document. Consider injecting the event to monaco
                // monaco will call handleChangeCode later.
                if (this._editorRef.current) {
                    (this._editorRef.current as any).injectPeerEditingEvents(event);
                }
            } else {
                // It is the document that is not currently edited. Directly update it.
                this._handleChangeCode(ytext.toString(), found.item.path);
            }
        }
    };

    _handleYJSEditingEvent = (e: monaco.editor.IModelContentChangedEvent) => {
        if (this.state.shareEnabled && this.ydoc) {
            const currentEntry = this._findFocusedEntry(this.state.fileEntries);
            if (currentEntry) {
                const ytext = this.ytextmap.get(currentEntry.item.id);
                if (ytext) {
                    this.ydoc.transact(() => {
                        e.changes
                            .sort((change1, change2) => change2.rangeOffset - change1.rangeOffset)
                            .forEach((change) => {
                                ytext.delete(change.rangeOffset, change.rangeLength);
                                ytext.insert(change.rangeOffset, change.text);
                            });
                    });
                }
            }
        }
    };

    _publicTreeToRemote = (fileEntries: FileManagerEntry[]) => {

        // Pump all editable files to yjs
        for (const file of fileEntries) {
            if (file.item.type === 'file' && !file.item.asset) {
                if (!this.ytextmap.has(file.item.id)) {
                    const ytext = this.ydoc.getText(file.item.id);
                    this.ytextmap.set(file.item.id, ytext);
                    ytext.insert(0, file.item.content as string);
                    /* Bind event */
                    ytext.observe((event) => {
                        this._yTextEventListener(event, ytext, file.item.id);
                    });
                }
            }
        }

        // Pump the file entries
        const rs: ProjectFileEntry[] = [];
        for (const file of fileEntries) {
            let r: ProjectFileEntry = {
                id: file.item.id,
                uri: file.item.uri,
                path: file.item.path,
                asset: file.item.asset,
                type: file.item.type,
            };
            rs.push(r);
        }
        this.ydoc.transact(() => {
            this.yJustPublishTree = true;
            this.yfiles.delete(0, this.yfiles.length);
            this.yfiles.push(rs);
        });
    };

    _initYJSComponent = async () => {
        this.ydoc = new Y.Doc();
        this.indexeddbProvider = new IndexeddbPersistence(this.state.id, this.ydoc);
        await this.indexeddbProvider.whenSynced;
        this.websocketProvider = new WebsocketProvider('wss://share.swiftlatex.com', this.state.id, this.ydoc);
        await new Promise((resolve, _) => {
            this.websocketProvider.on('sync', () => {
                resolve();
            });
        });
        this.yfiles = this.ydoc.getArray('files');
        if (this.yfiles.length === 0) {
            console.error('First Initialized');
            await this._publicTreeToRemote(this.state.fileEntries);
        } else {
            await this._mergeRemoteContentToLocal();
        }

        /* Consider debounce */
        this.yfiles.observe(_ => {
            if (this.yJustPublishTree) {
                this.yJustPublishTree = false; /* Prevent endless loop */
                return;
            }
            this._mergeRemoteContentToLocal().then();
        });

        console.error('YJS initialized');
    };

    render() {
        if (this.state.sessionStatus === SessionStatus.Ready) {
            return (
                <EditorView
                    deviceErrors={this.state.engineErrors}
                    engineLogs={this.state.engineLogs}
                    entry={this._findFocusedEntry(this.state.fileEntries)}
                    fileEntries={this.state.fileEntries}
                    isSystemBusy={this.state.isEngineBusy}
                    name={this.state.name}
                    onChangeCode={this._handleChangeCode}
                    onExportPDF={this._handleExportPDF}
                    onDownloadAsync={this._handleDownloadAsync}
                    onFileEntriesChange={this._handleFileEntriesChange}
                    onSubmitTitle={this._handleSubmitTitle}
                    onSendCode={this._handleOnSendCode}
                    onToggleSendCode={this._handleToggleSendCode}
                    previewRef={this._previewRef}
                    editorRef={this._editorRef}
                    uploadFileAsync={this._uploadAssetAsync}
                    saveStatus={this.state.saveStatus}
                    sendCodeOnChangeEnabled={this.state.sendCodeOnChangeEnabled}
                    entryPoint={this.state.entryPoint}
                    onSetEntryPoint={this._handleSetEntryPoint}
                    onShareProject={this._handleShareProject}
                    engine={this.state.engine}
                    onChangeEngineVersion={this._handleChangeEngineVersion}
                    onYJSEditingEvent={this._handleYJSEditingEvent}
                />
            );
        } else if (
            this.state.sessionStatus === SessionStatus.Init ||
            this.state.sessionStatus === SessionStatus.LoadComponents ||
            this.state.sessionStatus === SessionStatus.LoadFiles
        ) {
            return (
                <div className={css(styles.container)}>
                    <div className={css(styles.logo)}>
                        <AnimatedLogo/>
                    </div>
                    <p className={css(styles.loadingText)}>Loading data...</p>
                </div>
            );
        } else {
            const errorMessage =
                this.state.sessionStatus === SessionStatus.LoadComponentsFailed
                    ? 'Failed to start LaTeX engine.'
                    : 'Project files you are looking for do not exist!';
            return (
                <div className={css(styles.container)}>
                    <p className={css(styles.loadingText)}>Oops! {errorMessage} </p>
                </div>
            );
        }
    }
}

const styles = StyleSheet.create({
    container: {
        flexDirection: 'column',
        display: 'flex',
        height: '100%',
        width: '100%',
        alignItems: 'center',
        justifyContent: 'center',
    },
    logo: {
        transform: 'scale(0.5)',
        opacity: 0.9,
    },
    loadingText: {
        marginTop: 0,
        opacity: 0.7,
        fontSize: 18,
    },
});
