export interface Rule {
    a: number;
    b: number;
}
declare class Position {
    h: number;
    v: number;
    w: number;
    x: number;
    y: number;
    z: number;
    constructor(properties?: Position);
}
export declare class DviFont {
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
    constructor(properties: DviFont);
}
export declare class Machine {
    body: string;
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
    constructor();
    getBody(): string;
    getHead(): string;
    pushColor(c: string): void;
    popColor(): void;
    setPapersize(width: number, height: number): void;
    putSVG(svg: string): void;
    push(): void;
    pop(): void;
    beginPage(page: any): void;
    endPage(): void;
    post(p: any): void;
    postPost(p: any): void;
    moveRight(distance: number): void;
    moveDown(distance: number): void;
    setFont(font: DviFont): void;
    preamble(numerator: number, denominator: number, magnification: number, comment: string): void;
    putRule(rule: Rule): void;
    _to_legal_unicode(c: number): number;
    setChar(c: number, text_height: number, text_width: number): number;
    setNativeText(text: number[], width: number): number;
    putImage(width: number, height: number, url: string): void;
    loadFont(properties: any): DviFont;
    loadNativeFont(properties: any): DviFont;
}
export {};
