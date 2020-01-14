/// <reference types="node" />
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
    pushColor(c: string): void;
    popColor(): void;
    setPapersize(width: number, height: number): void;
    putSVG(svg: string): void;
    constructor(o: Writable);
    preamble(numerator: number, denominator: number, magnification: number, comment: string): void;
    putRule(rule: Rule): void;
    _to_legal_unicode(c: number): number;
    setChar(c: number, text_height: number, text_width: number): number;
    setNativeText(text: number[], width: number): number;
}
