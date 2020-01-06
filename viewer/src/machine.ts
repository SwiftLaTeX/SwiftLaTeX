//  var path = execSync('kpsewhich ' + name + '.tfm').toString().split("\n")[0];


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

    constructor(properties: DviFont) {

    }
}

export class Machine {

    output: string;
    pointsPerDviUnit: number;

    svgDepth: number;
    color: string;
    colorStack: string[];

    paperwidth: number;
    paperheight: number;


    fonts: DviFont[];
    font: DviFont;
    stack: Position[];
    position: Position;

    constructor() {
        this.fonts = [];
        this.output = "";
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
        console.log(this.paperwidth);
        console.log(this.paperheight);
    }

    putSVG(svg: string) {
        let left = this.position.h * this.pointsPerDviUnit;
        let top = this.position.v * this.pointsPerDviUnit;

        this.svgDepth += (svg.match(/<svg>/g) || []).length;
        this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;

        svg = svg.replace("<svg>", `<svg width="10pt" height="10pt" viewBox="-5 -5 10 10" style="overflow: visible; position: absolute;">`);

        svg = svg.replace(/{\?x}/g, left.toString());
        svg = svg.replace(/{\?y}/g, top.toString());

        this.output += svg;
    }

    push() {
        this.stack.push(new Position(this.position));
    }

    pop() {
        this.position = this.stack.pop();
    }

    beginPage(page: any) {
        this.stack = [];
        this.position = new Position();
    }

    endPage() { }

    post(p: any) { }

    postPost(p: any) { }

    
    moveRight(distance: number) {
        this.position.h += distance;
    }

    moveDown(distance: number) {
        this.position.v += distance;
    }

    setFont(font: DviFont) {
        this.font = font;
        //console.log(font);
    }

    preamble(numerator: number, denominator: number, magnification: number, comment: string) {
        let dviUnit = magnification * numerator / 1000.0 / denominator;

        let resolution = 300.0; // ppi
        let tfm_conv = (25400000.0 / numerator) * (denominator / 473628672) / 16.0;
        let conv = (numerator / 254000.0) * (resolution / denominator);
        conv = conv * (magnification / 1000.0);

        this.pointsPerDviUnit = dviUnit * 72.27 / 100000.0 / 2.54;
    }

    putRule(rule: Rule) {
        let a = rule.a * this.pointsPerDviUnit;
        let b = rule.b * this.pointsPerDviUnit;
        let left = this.position.h * this.pointsPerDviUnit;
        let bottom = this.position.v * this.pointsPerDviUnit;
        let top = bottom - a;

        this.output += `<span style="background: ${this.color}; position: absolute; top: ${top}pt; left: ${left}pt; width:${b}pt; height: ${a}pt;"></span>\n`;
    }

    _to_legal_unicode(c: number): number {
        if ((c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c == 0xAD)) {
            return c + 0xE000;
        } else {
            return c;
        }
    }

    setChar(c: number, text_height:number, text_width:number): number {

        let textWidth = 0;
        let textHeight = 0;
        c = this._to_legal_unicode(c);
        let htmlText = String.fromCharCode(c);
        let cssleft = this.position.h * this.pointsPerDviUnit;
        let cssheight = text_height * this.pointsPerDviUnit;
        let csstop = this.position.v * this.pointsPerDviUnit;
        let fontsize = this.font.designSize/65536.0;
        if (this.svgDepth == 0) {
            this.output += `<div style="line-height: 0; color: ${this.color}; font-family: ${this.font.name}; font-size: ${fontsize}pt; position: absolute; top: ${csstop - cssheight}pt; left: ${cssleft}pt;">${htmlText}<span style="display: inline-block; vertical-align: ${cssheight}pt; "></span></div>\n`;
        } else {
            let bottom = this.position.v * this.pointsPerDviUnit;
            // No 'pt' on fontsize since those units are potentially scaled
            this.output += `<text alignment-baseline="baseline" y="${bottom}" x="${cssleft}" style="font-family: ${this.font.name};" font-size="${fontsize}">${htmlText}</text>\n`;
        }

        return text_width;
    }


    setNativeText(text: number[], width: number): number {
        let htmlText = "";
        for(let j = 0; j < text.length; j ++) {
            htmlText += String.fromCharCode(text[j]);
        }
        let cssleft = this.position.h * this.pointsPerDviUnit;
        let csstop = this.position.v * this.pointsPerDviUnit;
        let fontsize = this.font.designSize;
        let lineheight = (this.font.height + this.font.depth)/1048576.0;
        let textheight = lineheight * fontsize; /*Todo, not sure whether it is correct*/
        this.output += `<span style="line-height: ${lineheight}; color: ${this.color}; white-space:pre; font-family: ${this.font.name}; font-size: ${fontsize}pt; position: absolute; top: ${csstop - textheight}pt; left: ${cssleft}pt;">${htmlText}</span>\n`;
        return width;
    }
 

    putImage(width:number, height:number, url:string) {
        let cssleft = this.position.h * this.pointsPerDviUnit;
        
        let csstop = this.position.v * this.pointsPerDviUnit;
        this.output += `<div data-url="${url}" style="top: ${csstop - height}pt; left: ${cssleft}pt; position: absolute; height:${height}pt; width:${width}pt; background-color:grey;"></div>`
    }

    loadFont(properties: any): DviFont {
        var f = new DviFont(properties);
        f.name = properties.name;
        f.checksum = properties.checksum;
        f.scaleFactor = properties.scaleFactor;
        f.designSize = properties.designSize;
        return f;
    }

    loadNativeFont(properties: any): DviFont {
        var f = new DviFont(properties);
        f.name = properties.name;
        f.designSize = properties.fontsize;
        f.faceindex = properties.faceindex;
        f.height = properties.height;
        f.depth = properties.depth;
        f.rbga = properties.rgba;
        f.extend = properties.extend;
        f.slant = properties.slant;
        f.embolden = properties.embolden;
        return f;
    }

}

