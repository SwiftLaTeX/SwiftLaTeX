import { Machine } from "./machine";

enum Opcode {
    set_char = 0,
    set1 = 128,
    set2 = 129,
    set3 = 130,
    set4 = 131,
    set_rule = 132,
    put_char = 133,
    put2 = 134,
    put3 = 135,
    put4 = 136,
    put_rule = 137,
    nop = 138,
    bop = 139,
    eop = 140,
    push = 141,
    pop = 142,
    right = 143,
    right2 = 144,
    right3 = 145,
    right4 = 146,
    w = 147,
    w1 = 148,
    w2 = 149,
    w3 = 150,
    w4 = 151,
    x = 152,
    x1 = 153,
    x2 = 154,
    x3 = 155,
    x4 = 156,
    down = 157,
    down2 = 158,
    down3 = 159,
    down4 = 160,
    y = 161,
    y1 = 162,
    y2 = 163,
    y3 = 164,
    y4 = 165,
    z = 166,
    z1 = 167,
    z2 = 168,
    z3 = 169,
    z4 = 170,
    fnt = 171,
    fnt1 = 235,
    fnt2 = 236,
    fnt3 = 237,
    fnt4 = 238,
    xxx = 239,
    xxx2 = 240,
    xxx3 = 241,
    xxx4 = 242,
    fnt_def = 243,
    fnt_def2 = 244,
    fnt_def3 = 245,
    fnt_def4 = 246,
    pre = 247,
    post = 248,
    post_post = 249,
    define_native_font = 252,
    set_glyphs = 253,
    set_text_and_glyphs = 254
}

export class DviCommand {
    length: number;
    special: boolean;

    constructor(properties) {
        this.special = false;
        Object.assign(this, properties);
    }

    execute(machine: Machine) { }

    toString(): string {
        return "DviCommand { }";
    }
}

// 133	put1	c[1]	typeset a character
// 134	put2	c[2]
// 135	put3	c[3]
// 136	put4	c[4]

class PutChar extends DviCommand {
    opcode: Opcode.put_char;
    c: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.put_char;
    }

    execute(machine: Machine) {
        //machine.putText(Buffer.from([this.c]));
    }

    toString(): string {
        return `PutChar { c: '${String.fromCharCode(this.c)}' }`;
    }
}

// 0...127	set_char_i		typeset a character and move right
// 128	set1	c[1]	                typeset a character and move right
// 129	set2	c[2]
// 130	set3	c[3]
// 131	set4	c[4]

class SetChar extends DviCommand {
    opcode: Opcode.set_char;

    c: number;
    text_height: number;
    text_width: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.set_char;
    }

    execute(machine: Machine) {
        var width = machine.setChar(this.c, this.text_height, this.text_width);
        machine.moveRight(width);
    }

    toString(): string {
        return `SetChar { c: '${String.fromCharCode(this.c)}' }`;
    }
}

// class SetText extends DviCommand {
//     t: Buffer;

//     constructor(properties) {
//         super(properties);
//     }

//     execute(machine: Machine) {
//         var width = machine.putText(this.t);
//         machine.moveRight(width);
//     }

//     toString() {
//         return `SetText { t: "${this.t.toString()}" }`;
//     }
// }

// 137	put_rule	a[4], b[4]	typeset a rule

class PutRule extends DviCommand {
    opcode: Opcode.put_rule;

    a: number;
    b: number;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.put_rule;
    }

    execute(machine: Machine) {
        machine.putRule(this);
    }

    toString(): string {
        return `PutRule { a: ${this.a}, b: ${this.b} }`;
    }
}

// 132	set_rule	a[4], b[4]	typeset a rule and move right

class SetRule extends DviCommand {
    opcode: Opcode.set_rule;

    a: number;
    b: number;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.set_rule;
    }

    execute(machine: Machine) {
        machine.putRule(this);
        machine.moveRight(this.b);
    }

    toString(): string {
        return `SetRule { a: ${this.a}, b: ${this.b} }`;
    }
}

// 138	nop		no operation

class Nop extends DviCommand {
    opcode: Opcode.nop;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.nop;
    }

    toString(): string {
        return `Nop { }`;
    }
}

// 139	bop	c_0[4]..c_9[4], p[4]	beginning of page

class Bop extends DviCommand {
    opcode: Opcode.bop;
    c_0: number;
    c_1: number;
    c_2: number;
    c_3: number;
    c_4: number;
    c_5: number;
    c_6: number;
    c_7: number;
    c_8: number;
    c_9: number;
    p: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.bop;
    }

    execute(machine: Machine) {
        machine.beginPage(this);
    }

    toString(): string {
        return `Bop { ... }`;
    }
}

// 140	eop		ending of page

class Eop extends DviCommand {
    opcode: Opcode.eop;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.eop;
    }

    execute(machine: Machine) {
        if (machine.stack.length)
            throw Error('Stack should be empty at the end of a page.');

        machine.endPage();
    }

    toString(): string {
        return `Eop { }`;
    }
}

// 141	push		save the current positions

class Push extends DviCommand {
    opcode: Opcode.push;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.push;
    }

    execute(machine: Machine) {
        machine.push();
    }

    toString(): string {
        return `Push { }`;
    }
}

// 142	pop		restore previous positions

class Pop extends DviCommand {
    opcode: Opcode.pop;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.pop;
    }

    execute(machine: Machine) {
        machine.pop();
    }

    toString(): string {
        return `Pop { }`;
    }
}

// 143	right1	b[1]	move right
// 144	right2	b[2]
// 145	right3	b[3]
// 146	right4	b[4]

class MoveRight extends DviCommand {
    opcode: Opcode.right;
    b: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.right;
    }

    execute(machine: Machine) {
        machine.moveRight(this.b);
    }

    toString(): string {
        return `MoveRight { b: ${this.b} }`;
    }
}

// 147	w0		move right by w
// 148	w1	b[1]	move right and set w
// 149	w2	b[2]
// 150	w3	b[3]
// 151	w4	b[4]

class MoveW extends DviCommand {
    opcode: Opcode.w;
    b: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.w;
    }

    execute(machine: Machine) {
        if (this.length > 1) machine.position.w = this.b;
        machine.moveRight(machine.position.w);
    }

    toString(): string {
        if (this.length > 1)
            return `MoveW { b: ${this.b} }`;
        else
            return `MoveW0 { }`;
    }
}

// 152	x0		move right by x
// 153	x1	b[1]	move right and set x
// 154	x2	b[2]
// 155	x3	b[3]
// 156	x4	b[4]

class MoveX extends DviCommand {
    opcode: Opcode.x;
    b: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.x;
    }

    execute(machine: Machine) {
        if (this.length > 1) machine.position.x = this.b;
        machine.moveRight(machine.position.x);
    }

    toString(): string {
        if (this.length > 1)
            return `MoveX { b: ${this.b} }`;
        else
            return `MoveX0 { }`;
    }
}

// 157	down1	a[1]	move down
// 158	down2	a[2]
// 159	down3	a[3]
// 160	down4	a[4]

class MoveDown extends DviCommand {
    opcode: Opcode.down;
    a: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.down;
    }

    execute(machine: Machine) {
        machine.moveDown(this.a);
    }

    toString(): string {
        return `MoveDown { a: ${this.a} }`;
    }
}

// 161	y0		move down by y
// 162	y1	a[1]	move down and set y
// 163	y2	a[2]
// 164	y3	a[3]
// 165	y4	a[4]

class MoveY extends DviCommand {
    opcode: Opcode.y;
    a: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.y;
    }

    execute(machine: Machine) {
        if (this.length > 1) machine.position.y = this.a;
        machine.moveDown(machine.position.y);
    }

    toString(): string {
        if (this.length > 1)
            return `MoveY { a: ${this.a} }`;
        else
            return `MoveY0 { }`;
    }
}

// 166	z0		move down by z
// 167	z1	a[1]	move down and set z
// 168	z2	a[2]
// 169	z3	a[3]
// 170	z4	a[4]

class MoveZ extends DviCommand {
    opcode: Opcode.z;
    a: number;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.z;
    }

    execute(machine: Machine) {
        if (this.length > 1) machine.position.z = this.a;
        machine.moveDown(machine.position.z);
    }

    toString(): string {
        if (this.length > 1)
            return `MoveZ { a: ${this.a} }`;
        else
            return `MoveZ0 { }`;
    }
}

// 171...234	fnt_num_i		set current font to i
// 235	fnt1	k[1]	set current font
// 236	fnt2	k[2]
// 237	fnt3	k[3]
// 238	fnt4	k[4]

class SetFont extends DviCommand {
    opcode: Opcode.fnt;
    k: number;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.fnt;
    }

    execute(machine: Machine) {
        //console.log("Setting mainfont to " + this.k);
        if (machine.fonts[this.k]) {
            machine.setFont(machine.fonts[this.k]);
        } else
            throw Error(`Could not find font ${this.k}.`);
    }

    toString(): string {
        return `SetFont { k: ${this.k} }`;
    }
}

// 239	xxx1	k[1], x[k]	extension to DVI primitives
// 240	xxx2	k[2], x[k]
// 241	xxx3	k[3], x[k]
// 242	xxx4	k[4], x[k]

class Special extends DviCommand {
    opcode: Opcode.xxx;

    x: string;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.xxx;
        this.special = true;
    }

    toString(): string {
        return `Special { x: '${this.x}' }`;
    }
}


// 243	fnt_def1	k[1], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]	define the meaning of a font number
// 244	fnt_def2	k[2], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]
// 245	fnt_def3	k[3], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]
// 246	fnt_def4	k[4], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]

class FontDefinition extends DviCommand {
    opcode: Opcode.fnt_def;
    k: number; // font id
    c: number; // checksum
    s: number; // fixed-point scale factor (applied to char widths of font)
    d: number; // design-size factors with the magnification (`s/1000`)
    a: number; // length of directory path of font (`./` if a = 0)
    l: number; // length of font name
    n: string; // font name (first `a` bytes is dir, remaining `l` = name)

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.fnt_def;
    }

    execute(machine: Machine) {
        //console.log("Defining Local Font name: " + this.n + " index: " + this.k);
        machine.fonts[this.k] = machine.loadFont({
            name: this.n,
            checksum: this.c,
            scaleFactor: this.s,
            designSize: this.d
        });
    }

    toString(): string {
        return `FontDefinition { k: ${this.k}, n: '${this.n}', ... }`;
    }
}

// 247	pre	i[1], num[4], den[4], mag[4],  k[1], x[k]	preamble

class Preamble extends DviCommand {
    opcode: Opcode.pre;
    i: number;
    num: number;
    den: number;
    mag: number;
    x: string;
    constructor(properties) {
        super(properties);
        this.opcode = Opcode.pre;
    }

    execute(machine: Machine) {
        if (this.num <= 0)
            throw Error('Invalid numerator (must be > 0)');

        if (this.den <= 0)
            throw Error('Invalid denominator (must be > 0)');

        if (this.i != 7) {
            throw Error(`DVI format must be 2. (${this.i}) `);
        }

        machine.preamble(this.num, this.den, this.mag, this.x);
    }

    toString(): string {
        return `Preamble { i: ${this.i}, num: ${this.num}, den: ${this.den}, mag: ${this.mag}, x: '${this.x}' }`;
    }
}

// 248	post	p[4], num[4], den[4], mag[4], l[4], u[4], s[2], t[2]
// < font definitions >	postamble beginning

class Post extends DviCommand {
    opcode: Opcode.post;

    p: number;
    num: number;
    den: number;
    mag: number;
    l: number;
    u: number;
    s: number;
    t: number;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.post;
    }

    execute(machine: Machine) {
        machine.post(this);
    }

    toString(): string {
        return `Post { p: ${this.p}, num: ${this.num}, den: ${this.den}, mag: ${this.mag}, ... }`;
    }
}

// 249	post_post	q[4], i[1]; 223's	postamble ending

class PostPost extends DviCommand {
    opcode: Opcode.post_post;

    q: number;
    i: number;

    constructor(properties) {
        super(properties);
        this.opcode = Opcode.post_post;
    }

    execute(machine: Machine) {
        machine.postPost(this);
    }

    toString(): string {
        return `PostPost { q: ${this.q}, i: ${this.i} }`;
    }
}

//252 

class NativeFontDefinition extends DviCommand {
    opcode: Opcode.define_native_font;
    fontnumber: number;
    fontsize: number;
    flag: number;
    filenamelen: number;
    filename: string;
    faceindex: number;
    rbga: number;
    extend: number;
    slant: number;
    embolden: number;

    constructor(properties: any) {
        super(properties);
        this.opcode = Opcode.define_native_font;
    }
    execute(machine: Machine) {
        //console.log("Defining Native Font name: " + this.filename + " index: " + this.fontnumber);
        machine.fonts[this.fontnumber] = machine.loadNativeFont({
            name: this.filename,
            fontsize: this.fontsize,
            faceindex: this.faceindex,
            rbga: this.rbga,
            extend: this.extend,
            slant: this.slant,
            embolden: this.embolden
        });
    }
    toString(): string {
        return `NativeFontDefinition { filename: ${this.filename}, fontnumber: ${this.fontnumber}, length: ${this.length}, rbga: ${this.rbga}}`;
    }
}


//253 

class _glyphLocation {
    xoffset: number;
    yoffset: number;
    toString(): string {
        return `Glyph { xoffset: ${this.xoffset}, yoffset: ${this.yoffset}}`;
    }
}

class SetGlyph extends DviCommand {
    opcode: Opcode.set_glyphs;
    width: number;
    count: number;
    glyphLocations: _glyphLocation[];
    glyphIds: number[];
    constructor(properties: any) {
        super(properties);
        this.opcode = Opcode.set_glyphs;
    }
    execute(machine: Machine) {

    }
    toString(): string {
        return `SetGlyph { count: ${this.count} }`;
    }
}



// 250 251 255	undefined	

// cat src/parser.ts | grep interface | sed 's/interface //g' | tr -d ' {' | tr '\n' '|' | sed 's/|/ | /g' | xsel -b

type Command =
    SetChar | SetRule | PutChar | PutRule | Nop | Bop | Eop | Push | Pop |
    MoveRight | MoveW | MoveX | MoveDown | MoveY | MoveZ | SetFont | Special |
    FontDefinition | Preamble | Post | PostPost | NativeFontDefinition | SetGlyph;

function parseCommand(opcode: Opcode, buffer: Buffer): Command | void {

    if ((opcode >= Opcode.set_char) && (opcode < Opcode.set1)) {
        throw Error(`SwiftLaTeX does not generate simple setchar`);
        //return new SetChar({ c: opcode, length: 1 });
    }

    if ((opcode >= Opcode.fnt) && (opcode < Opcode.fnt1))
        return new SetFont({ k: opcode - 171, length: 1 });

    // Technically these are undefined opcodes, but we'll pretend they are NOPs
    if ((opcode == 250) || (opcode == 251) || (opcode == 255)) {
        throw Error(`Undefined opcode ${opcode}`);
        //return new Nop({ length: 1 });
    }

    switch (opcode) {
        case Opcode.set1:
            if (buffer.length <  9)
                throw Error(`not enough bytes to process opcode ${opcode}`);
            return new SetChar({
                c: buffer.readUInt8(0),
                text_height: buffer.readUInt32BE(1),
                text_width: buffer.readUInt32BE(5),
                length: 9 + 1
            });

        case Opcode.set2:
        case Opcode.set3:
        case Opcode.set4:
            throw Error(`SwiftLaTeX does not generate set_char234`);

        case Opcode.set_rule:
            if (buffer.length < 8)
                throw Error(`not enough bytes to process opcode ${opcode}`);
            return new SetRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9
            });

        case Opcode.put_char:
        case Opcode.put2:
        case Opcode.put3:
        case Opcode.put4:
            throw Error(`SwiftLaTeX engine does not generate put_char`);
            // if (buffer.length < opcode - Opcode.put_char + 1) throw Error(`not enough bytes to process opcode ${opcode}`);
            // return new PutChar({
            //     c: buffer.readIntBE(0, opcode - Opcode.put_char + 1),
            //     length: opcode - Opcode.put_char + 1 + 1
            // });

        case Opcode.put_rule:
            if (buffer.length < 8) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new PutRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9
            });

        case Opcode.nop:
            return new Nop({ length: 1 });

        case Opcode.bop:
            if (buffer.length < 44) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new Bop({
                c_0: buffer.readUInt32BE(0),
                c_1: buffer.readUInt32BE(4),
                c_2: buffer.readUInt32BE(8),
                c_3: buffer.readUInt32BE(12),
                c_4: buffer.readUInt32BE(16),
                c_5: buffer.readUInt32BE(20),
                c_6: buffer.readUInt32BE(24),
                c_7: buffer.readUInt32BE(28),
                c_8: buffer.readUInt32BE(32),
                c_9: buffer.readUInt32BE(36),
                p: buffer.readUInt32BE(40),
                length: 45
            });

        case Opcode.eop:
            return new Eop({ length: 1 });

        case Opcode.push:
            return new Push({ length: 1 });

        case Opcode.pop:
            return new Pop({ length: 1 });

        case Opcode.right:
        case Opcode.right2:
        case Opcode.right3:
        case Opcode.right4:
            if (buffer.length < opcode - Opcode.right + 1) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveRight({
                b: buffer.readIntBE(0, opcode - Opcode.right + 1),
                length: opcode - Opcode.right + 1 + 1
            });

        case Opcode.w:
            return new MoveW({ b: 0, length: 1 });

        case Opcode.w1:
        case Opcode.w2:
        case Opcode.w3:
        case Opcode.w4:
            if (buffer.length < opcode - Opcode.w) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveW({
                b: buffer.readIntBE(0, opcode - Opcode.w),
                length: opcode - Opcode.w + 1
            });

        case Opcode.x:
            return new MoveX({ b: 0, length: 1 });

        case Opcode.x1:
        case Opcode.x2:
        case Opcode.x3:
        case Opcode.x4:
            if (buffer.length < opcode - Opcode.x) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveX({
                b: buffer.readIntBE(0, opcode - Opcode.x),
                length: opcode - Opcode.x + 1
            });

        case Opcode.down:
        case Opcode.down2:
        case Opcode.down3:
        case Opcode.down4:
            if (buffer.length < opcode - Opcode.down + 1) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveDown({
                a: buffer.readIntBE(0, opcode - Opcode.down + 1),
                length: opcode - Opcode.down + 1 + 1
            });

        case Opcode.y:
            return new MoveY({ a: 0, length: 1 });

        case Opcode.y1:
        case Opcode.y2:
        case Opcode.y3:
        case Opcode.y4:
            if (buffer.length < opcode - Opcode.y) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveY({
                a: buffer.readIntBE(0, opcode - Opcode.y),
                length: opcode - Opcode.y + 1
            });

        case Opcode.z:
            return new MoveZ({ a: 0, length: 1 });

        case Opcode.z1:
        case Opcode.z2:
        case Opcode.z3:
        case Opcode.z4:
            if (buffer.length < opcode - Opcode.z) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new MoveZ({
                a: buffer.readIntBE(0, opcode - Opcode.z),
                length: opcode - Opcode.z + 1
            });

        case Opcode.fnt1:
        case Opcode.fnt2:
        case Opcode.fnt3:
        case Opcode.fnt4:
            if (buffer.length < opcode - Opcode.fnt1 + 1) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new SetFont({
                k: buffer.readIntBE(0, opcode - Opcode.fnt1 + 1),
                length: opcode - Opcode.fnt1 + 1 + 1
            });

        case Opcode.xxx:
        case Opcode.xxx2:
        case Opcode.xxx3:
        case Opcode.xxx4: {
            let i = opcode - Opcode.xxx + 1;
            if (buffer.length < i) throw Error(`not enough bytes to process opcode ${opcode}`);
            let k = buffer.readUIntBE(0, i);
            if (buffer.length < i + k) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new Special({
                x: buffer.slice(i, i + k).toString(),
                length: i + k + 1
            });
        }

        case Opcode.fnt_def:
        case Opcode.fnt_def2:
        case Opcode.fnt_def3:
        case Opcode.fnt_def4: {
            let i = opcode - Opcode.fnt_def + 1;
            if (buffer.length < i) throw Error(`not enough bytes to process opcode ${opcode}`);
            let k = buffer.readIntBE(0, i);
            if (buffer.length < i + 14) throw Error(`not enough bytes to process opcode ${opcode}`);
            let c = buffer.readUInt32BE(i + 0);
            let s = buffer.readUInt32BE(i + 4);
            let d = buffer.readUInt32BE(i + 8);
            let a = buffer.readUInt8(i + 12);
            let l = buffer.readUInt8(i + 13);
            if (buffer.length < i + 14 + a + l) throw Error(`not enough bytes to process opcode ${opcode}`);
            let n = buffer.slice(i + 14, i + 14 + a + l).toString();
            return new FontDefinition({
                k: k,
                c: c,
                s: s,
                d: d,
                a: a,
                l: l,
                n: n,
                length: i + 14 + a + l + 1,
            });
        }

        case Opcode.pre: {
            if (buffer.length < 14) throw Error(`not enough bytes to process opcode ${opcode}`);
            let i = buffer.readUInt8(0);
            let num = buffer.readUInt32BE(1);
            let den = buffer.readUInt32BE(5);
            let mag = buffer.readUInt32BE(9);
            let k = buffer.readUInt8(13);
            if (buffer.length < 14 + k + 8) throw Error(`not enough bytes to process opcode ${opcode}`);
            let comment = buffer.slice(14, 14 + k).toString();
            return new Preamble({
                i: i,
                num: num,
                den: den,
                mag: mag,
                x: comment,
                
                length: 14 + k  + 1
            });
        }

        case Opcode.post:
            if (buffer.length < 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2) throw Error(`not enough bytes to process opcode ${opcode}`);
            return new Post({
                p: buffer.readUInt32BE(0),
                num: buffer.readUInt32BE(4),
                den: buffer.readUInt32BE(8),
                mag: buffer.readUInt32BE(12),
                l: buffer.readUInt32BE(16),
                u: buffer.readUInt32BE(20),
                s: buffer.readUInt16BE(24),
                t: buffer.readUInt16BE(26),
                length: 29
            });

        case Opcode.post_post:
            if (buffer.length < 5)
                throw Error(`not enough bytes to process opcode ${opcode}`);
            return new PostPost({
                q: buffer.readUInt32BE(0),
                i: buffer.readUInt8(4),
                length: 6
            });

        case Opcode.define_native_font:
            {


                if (buffer.length < 19)
                    throw Error(`not enough bytes to process opcode ${opcode}`);
                let fontnum = buffer.readUInt32BE(0);
                let fontsize = buffer.readUInt32BE(4) / 65536;
                let flag = buffer.readUInt16BE(8);
                let filenamelen = buffer.readUInt8(10);
                let filename = buffer.slice(11, 11 + filenamelen).toString();
                let faceindex = buffer.readUInt32BE(11 + filenamelen);
                let res = new NativeFontDefinition({
                    fontnumber: fontnum,
                    fontsize: fontsize,
                    flag: flag,
                    filenamelen: filenamelen,
                    filename: filename,
                    faceindex: faceindex
                });
                const XDV_FLAGS_COLORED = 0x0200;
                const XDV_FLAG_EXTEND = 0x1000;
                const XDV_FLAG_SLANT = 0x2000;
                const XDV_FLAG_EMBOLDEN = 0x4000;
                let length = 15 + filenamelen;

                if ((flag & XDV_FLAGS_COLORED) != 0) {
                    res.rbga = buffer.readUInt32BE(length);
                    length += 4;


                }
                if ((flag & XDV_FLAG_EXTEND) != 0) {
                    res.extend = buffer.readUInt32BE(length) / 65536.0;
                    length += 4;

                }
                if ((flag & XDV_FLAG_SLANT) != 0) {
                    res.slant = buffer.readUInt32BE(length) / 65536.0;
                    length += 4;

                }
                if ((flag & XDV_FLAG_EMBOLDEN) != 0) {
                    res.embolden = buffer.readUInt32BE(length) / 65536.0;
                    length += 4;

                }
                res.length = length + 1;
                return res;
            }
        case Opcode.set_glyphs:
            {

                if (buffer.length < 16)
                    throw Error(`not enough bytes to process opcode ${opcode}`);
                let width = buffer.readUInt32BE(0);
                let count = buffer.readUInt16BE(4);
                let glyphLocations: _glyphLocation[] = [];
                let glyphIDs: number[] = [];
                let res = new SetGlyph({
                    width: width,
                    count: count,
                });
                if (buffer.length < 6 + count * 10)
                    throw Error(`not enough bytes to process opcode ${opcode} ${buffer.length} ${count}`);
                for (let j = 0; j < count; j++) {
                    let glyphLocation: _glyphLocation = new _glyphLocation();
                    let xoffset = buffer.readUInt32BE(6 + j * 8);
                    let yoffset = buffer.readUInt32BE(6 + j * 8 + 4);
                    glyphLocation.xoffset = xoffset;
                    glyphLocation.yoffset = yoffset;
                    glyphLocations.push(glyphLocation);
                }
                for (let j = 0; j < count; j++) {
                    let glyphID = buffer.readUInt16BE(6 + count * 8 + j * 2);
                    glyphIDs.push(glyphID);
                }
                res.glyphLocations = glyphLocations;
                res.glyphIds = glyphIDs;
                res.length = 6 + count * 10 + 1;
                //console.log(res);
                return res;
            }

        case Opcode.set_text_and_glyphs:
            throw Error('Not implemented. 254');
    }

    throw Error(`routine for ${opcode} is not implemented`);
}

export async function* dviParser(stream) {
    let buffer = Buffer.alloc(0);
    let isAfterPostamble = false;

    for await (const chunk of stream) {
        buffer = Buffer.concat([buffer, chunk]);
    }

    let offset = 0;

    while (offset < buffer.length) {
        let opcode: Opcode = buffer.readUInt8(offset);

        if (isAfterPostamble) {
            if (opcode == 223) {
                offset++;
                continue;
            } else {
                throw Error('Only 223 bytes are permitted after the post-postamble.');
            }
        }

        let command = parseCommand(opcode, buffer.slice(offset + 1));

        if (command) {
            yield command;
            offset += command.length;

            if (command.opcode == Opcode.post_post)
                isAfterPostamble = true;
        } else {
            break;
        }
    }



}

export async function execute(commands, machine) {
    for await (const command of commands) {
        // console.log(command.toString());
        command.execute(machine);
    }
}

export async function* merge(commands, filter, merge) {
    let queue = [];

    for await (const command of commands) {
        if (filter(command)) {
            queue.push(command);
        } else {
            if (queue.length > 0) {
                yield* merge(queue);
                queue = [];
            }

            yield command;
        }
    }

    if (queue.length > 0) yield* merge(queue);
}

// export function mergeText(commands) {
//     return merge(commands,
//         command => (command instanceof SetChar),
//         function*(queue) {
//             let text = Buffer.from(queue.map(command => command.c));
//             yield new SetText({ t: text });
//         });
// }
