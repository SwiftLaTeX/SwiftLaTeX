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
    rbga: number;
    extend: number;
    slant: number;
    embolden: number;

    constructor(properties: DviFont) {

    }
}

export class Machine {
    fonts: DviFont[];
    font: DviFont;
    stack: Position[];
    position: Position;

    constructor() {
        this.fonts = [];
    }

    preamble(numerator: number, denominator: number, magnification: number, comment: string) {
    }

    pushColor(c: string) {
    }

    popColor() {
    }

    setPapersize(width: number, height: number) {
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

    putRule(rule: Rule) {
    }

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

    putSVG(svg: string) {
    }

    // Returns the width of the text
    setChar(text: number, text_height:number, text_width:number): number {
        return 0;
    }

    // Returns the width of the text
    setNativeText(text: Buffer): number {
        return 0;
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
        f.rbga = properties.rgba;
        f.extend = properties.extend;
        f.slant = properties.slant;
        f.embolden = properties.embolden;
        return f;
    }

}

