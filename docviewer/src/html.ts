import { Machine, Rule } from "./machine";
import { Writable } from 'stream';

export default class HTMLMachine extends Machine {
    output: Writable;
    pointsPerDviUnit: number;

    svgDepth: number;
    color: string;
    colorStack: string[];

    paperwidth: number;
    paperheight: number;

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
    }

    putSVG(svg: string) {
        let left = this.position.h * this.pointsPerDviUnit;
        let top = this.position.v * this.pointsPerDviUnit;

        this.svgDepth += (svg.match(/<svg>/g) || []).length;
        this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;

        svg = svg.replace("<svg>", `<svg width="10pt" height="10pt" viewBox="-5 -5 10 10" style="overflow: visible; position: absolute;">`);

        svg = svg.replace(/{\?x}/g, left.toString());
        svg = svg.replace(/{\?y}/g, top.toString());

        this.output.write(svg);
    }

    constructor(o: Writable) {
        super();
        this.output = o;
        this.color = 'black';
        this.colorStack = [];
        this.svgDepth = 0;
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

        this.output.write(`<span style="background: ${this.color}; position: absolute; top: ${top}pt; left: ${left}pt; width:${b}pt; height: ${a}pt;"></span>\n`);
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
            this.output.write(`<div style="line-height: 0; color: ${this.color}; font-family: ${this.font.name}; font-size: ${fontsize}pt; position: absolute; top: ${csstop - cssheight}pt; left: ${cssleft}pt;">${htmlText}<span style="display: inline-block; vertical-align: ${cssheight}pt; "></span></div>\n`);
        } else {
            let bottom = this.position.v * this.pointsPerDviUnit;
            // No 'pt' on fontsize since those units are potentially scaled
            this.output.write(`<text alignment-baseline="baseline" y="${bottom}" x="${cssleft}" style="font-family: ${this.font.name};" font-size="${fontsize}">${htmlText}</text>\n`);
        }

        return text_width;
    }


    setNativeText(text: Buffer): number {
        return 0;
    }
}

