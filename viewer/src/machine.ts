
export interface Rule {
    a: number;
    b: number;
}

class Position {
    h: number;
    v: number;
    w: number;
    x: number;
    y: number;
    z: number;

    constructor(properties?: Position) {
        if (properties) {
            this.h = properties.h;
            this.v = properties.v;
            this.w = properties.w;
            this.x = properties.x;
            this.y = properties.y;
            this.z = properties.z;
        } else {
            this.h = this.v = this.w = this.x = this.y = this.z = 0;
        }
    }
}

export class DviFont {
    name: string;
    checksum: number;
    scaleFactor: number;
    designSize: number;
    faceindex: number;
    height: number;
    depth: number;
    rbga: number;
    extend: number;
    slant: number;
    embolden: number;
    isnative: boolean;

    constructor(properties: DviFont) {
        
    }
}

export class Machine {

    body: string;
    style: string;
    pointsPerDviUnit: number;

    svgDepth: number;
    color: string;
    colorStack: string[];

    paperwidth: number;
    paperheight: number;
    currentpage: number;

    fonts: Map<number, DviFont>;
    font: DviFont;
    usedfonts: string[];
    stack: Position[];
    position: Position;

    constructor() {
        this.fonts = new Map();
        this.body = "";
        this.style = "";
        this.usedfonts = [];
        this.color = "black";
        this.svgDepth = 0;
        this.pointsPerDviUnit = 0;
    }

    getBody(): string {
        return this.body;
    }

    getStyle(): string {
        return this.style;
    }

    pushColor(c: string) {
        this.colorStack.push(this.color);
        this.color = c;
    }

    popColor() {
        this.color = this.colorStack.pop();
    }

    setPapersize(width: number, height: number) {
        this.paperwidth = width;
        this.paperheight = height;
        this.style += `#page${this.currentpage} { position:relative; width:${this.paperwidth}px; height:${this.paperheight}px; border-width: thin; }\n`;
    }

    putSVG(svg: string) {
        let left = this.position.h * this.pointsPerDviUnit;
        let top = this.position.v * this.pointsPerDviUnit;

        this.svgDepth += (svg.match(/<svg>/g) || []).length;
        this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;

        svg = svg.replace("<svg>", `<svg width="10px" height="10px" viewBox="-5 -5 10 10" style="overflow: visible; position: absolute;">`);

        svg = svg.replace(/{\?x}/g, left.toString());
        svg = svg.replace(/{\?y}/g, top.toString());

        this.body += svg;
    }

    push() {
        this.stack.push(new Position(this.position));
    }

    pop() {
        this.position = this.stack.pop();
    }

    beginPage(page: number) {
        this.stack = [];
        this.position = new Position();
        this.currentpage = page + 1;
        this.body += `<div id='page${this.currentpage}'>`;
    }

    endPage() {
        this.body += "</div>";
    }

    post(p: any) { }

    postPost(p: any) { }


    moveRight(distance: number) {
        this.position.h += distance;
    }

    moveDown(distance: number) {
        this.position.v += distance;
    }

    setFont(fontnum: number) {
        if (this.fonts.has(fontnum)) {
            this.font = this.fonts.get(fontnum);
            if (!this.usedfonts.includes(this.font.name)) {
                this.usedfonts.push(this.font.name);
                if (this.font.isnative) {
                    if (this.font.name.endsWith(".ttf") || this.font.name.endsWith(".otf")) {
                        //Local font
                    } else {
                        //Remote font
                        this.style += `@font-face { font-family:${this.font.name}; src:url(https://texlive.swiftlatex.com/${this.font.name}.otf); } \n`;
                    }

                } else {
                    this.style += `@font-face { font-family:${this.font.name}; src:url(fonts/output/${this.font.name}.woff); } \n`;
                }
            }
        } else {
            throw Error(`Could not find font ${fontnum}.`);
        }
    }

    preamble(numerator: number, denominator: number, magnification: number, comment: string) {

        let dviUnit = magnification * numerator / 1000.0 / denominator;

        this.pointsPerDviUnit = dviUnit * 72.27 / 100000.0 / 2.54;
    }

    putRule(rule: Rule) {
        let a = rule.a * this.pointsPerDviUnit;
        let b = rule.b * this.pointsPerDviUnit;
        let left = this.position.h * this.pointsPerDviUnit;
        let bottom = this.position.v * this.pointsPerDviUnit;
        let top = bottom - a;

        this.body += `<span style="background: ${this.color}; position: absolute; top: ${top}px; left: ${left}px; width:${b}px; height: ${a}px;"></span>\n`;
    }

    _to_legal_unicode(c: number): number {
        if ((c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c == 0xAD)) {
            return c + 0xE000;
        } else {
            return c;
        }
    }

    setChar(c: number, text_height: number, text_width: number): number {

        let textWidth = 0;
        let textHeight = 0;
        c = this._to_legal_unicode(c);
        let htmlText = String.fromCharCode(c);
        let cssleft = this.position.h * this.pointsPerDviUnit;
        let cssheight = text_height * this.pointsPerDviUnit;
        let csstop = this.position.v * this.pointsPerDviUnit;
        let fontsize = this.font.designSize / 65536.0;
        if (this.svgDepth == 0) {
            this.body += `<div style="line-height: 0; color: ${this.color}; font-family: ${this.font.name}; font-size: ${fontsize}px; position: absolute; top: ${Number(csstop - cssheight).toFixed(2)}px; left: ${Number(cssleft).toFixed(2)}px;">${htmlText}<span style="display: inline-block; vertical-align: ${Number(cssheight).toFixed(2)}px; "></span></div>\n`;
        } else {
            let bottom = this.position.v * this.pointsPerDviUnit;
            this.body += `<text alignment-baseline="baseline" y="${bottom}" x="${cssleft}" style="font-family: ${this.font.name};" font-size="${fontsize}">${htmlText}</text>\n`;
        }

        return text_width;
    }


    setNativeText(text: number[], width: number): number {
        let htmlText = "";
        for (let j = 0; j < text.length; j++) {
            htmlText += String.fromCharCode(text[j]);
        }
        let cssleft = this.position.h * this.pointsPerDviUnit;
        let csstop = this.position.v * this.pointsPerDviUnit;
        let fontsize = this.font.designSize;
        let lineheight = (this.font.height + this.font.depth) / 1048576.0;
        let textheight = lineheight * fontsize; /*Todo, not sure whether it is correct*/
        this.body += `<span style="line-height: ${Number(lineheight).toFixed(2)}; color: ${this.color}; white-space:pre; font-family: ${this.font.name}; font-size: ${fontsize}px; position: absolute; top: ${Number(csstop - textheight).toFixed(2)}px; left: ${Number(cssleft).toFixed(2)}px;">${htmlText}</span>\n`;
        return width;
    }


    putImage(width: number, height: number, url: string) {
        let cssleft = this.position.h * this.pointsPerDviUnit;

        let csstop = this.position.v * this.pointsPerDviUnit;
        this.body += `<div data-url="${url}" style="top: ${Number(csstop - height).toFixed(2)}px; left: ${Number(cssleft).toFixed(2)}px; position: absolute; height:${Number(height).toFixed(2)}px; width:${Number(width).toFixed(2)}px; background-color:grey;"></div>`
    }

    loadFont(properties: any, fontnumber: number, isnative: boolean) {
        let f = new DviFont(properties);
        if (!isnative) {
            f.name = properties.name;
            f.checksum = properties.checksum;
            f.scaleFactor = properties.scaleFactor;
            f.designSize = properties.designSize;
            f.isnative = false;
        } else {
            f.name = properties.name;
            f.designSize = properties.fontsize;
            f.faceindex = properties.faceindex;
            f.height = properties.height;
            f.depth = properties.depth;
            f.rbga = properties.rgba;
            f.extend = properties.extend;
            f.slant = properties.slant;
            f.embolden = properties.embolden;
            f.isnative = true;
        }
        this.fonts.set(fontnumber, f);
    }

}

