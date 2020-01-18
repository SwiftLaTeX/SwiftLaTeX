"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
exports.__esModule = true;
var Opcode;
(function (Opcode) {
    Opcode[Opcode["set_char"] = 0] = "set_char";
    Opcode[Opcode["set1"] = 128] = "set1";
    Opcode[Opcode["set2"] = 129] = "set2";
    Opcode[Opcode["set3"] = 130] = "set3";
    Opcode[Opcode["set4"] = 131] = "set4";
    Opcode[Opcode["set_rule"] = 132] = "set_rule";
    Opcode[Opcode["put_char"] = 133] = "put_char";
    Opcode[Opcode["put2"] = 134] = "put2";
    Opcode[Opcode["put3"] = 135] = "put3";
    Opcode[Opcode["put4"] = 136] = "put4";
    Opcode[Opcode["put_rule"] = 137] = "put_rule";
    Opcode[Opcode["nop"] = 138] = "nop";
    Opcode[Opcode["bop"] = 139] = "bop";
    Opcode[Opcode["eop"] = 140] = "eop";
    Opcode[Opcode["push"] = 141] = "push";
    Opcode[Opcode["pop"] = 142] = "pop";
    Opcode[Opcode["right"] = 143] = "right";
    Opcode[Opcode["right2"] = 144] = "right2";
    Opcode[Opcode["right3"] = 145] = "right3";
    Opcode[Opcode["right4"] = 146] = "right4";
    Opcode[Opcode["w"] = 147] = "w";
    Opcode[Opcode["w1"] = 148] = "w1";
    Opcode[Opcode["w2"] = 149] = "w2";
    Opcode[Opcode["w3"] = 150] = "w3";
    Opcode[Opcode["w4"] = 151] = "w4";
    Opcode[Opcode["x"] = 152] = "x";
    Opcode[Opcode["x1"] = 153] = "x1";
    Opcode[Opcode["x2"] = 154] = "x2";
    Opcode[Opcode["x3"] = 155] = "x3";
    Opcode[Opcode["x4"] = 156] = "x4";
    Opcode[Opcode["down"] = 157] = "down";
    Opcode[Opcode["down2"] = 158] = "down2";
    Opcode[Opcode["down3"] = 159] = "down3";
    Opcode[Opcode["down4"] = 160] = "down4";
    Opcode[Opcode["y"] = 161] = "y";
    Opcode[Opcode["y1"] = 162] = "y1";
    Opcode[Opcode["y2"] = 163] = "y2";
    Opcode[Opcode["y3"] = 164] = "y3";
    Opcode[Opcode["y4"] = 165] = "y4";
    Opcode[Opcode["z"] = 166] = "z";
    Opcode[Opcode["z1"] = 167] = "z1";
    Opcode[Opcode["z2"] = 168] = "z2";
    Opcode[Opcode["z3"] = 169] = "z3";
    Opcode[Opcode["z4"] = 170] = "z4";
    Opcode[Opcode["fnt"] = 171] = "fnt";
    Opcode[Opcode["fnt1"] = 235] = "fnt1";
    Opcode[Opcode["fnt2"] = 236] = "fnt2";
    Opcode[Opcode["fnt3"] = 237] = "fnt3";
    Opcode[Opcode["fnt4"] = 238] = "fnt4";
    Opcode[Opcode["xxx"] = 239] = "xxx";
    Opcode[Opcode["xxx2"] = 240] = "xxx2";
    Opcode[Opcode["xxx3"] = 241] = "xxx3";
    Opcode[Opcode["xxx4"] = 242] = "xxx4";
    Opcode[Opcode["fnt_def"] = 243] = "fnt_def";
    Opcode[Opcode["fnt_def2"] = 244] = "fnt_def2";
    Opcode[Opcode["fnt_def3"] = 245] = "fnt_def3";
    Opcode[Opcode["fnt_def4"] = 246] = "fnt_def4";
    Opcode[Opcode["pre"] = 247] = "pre";
    Opcode[Opcode["post"] = 248] = "post";
    Opcode[Opcode["post_post"] = 249] = "post_post";
    Opcode[Opcode["define_native_font"] = 252] = "define_native_font";
    Opcode[Opcode["set_glyphs"] = 253] = "set_glyphs";
    Opcode[Opcode["set_text_and_glyphs"] = 254] = "set_text_and_glyphs";
})(Opcode || (Opcode = {}));
var DviCommand = /** @class */ (function () {
    function DviCommand(properties) {
        this.special = false;
        Object.assign(this, properties);
    }
    DviCommand.prototype.execute = function (machine) { };
    DviCommand.prototype.toString = function () {
        return "DviCommand { }";
    };
    return DviCommand;
}());
exports.DviCommand = DviCommand;
// 133	put1	c[1]	typeset a character
// 134	put2	c[2]
// 135	put3	c[3]
// 136	put4	c[4]
var PutChar = /** @class */ (function (_super) {
    __extends(PutChar, _super);
    function PutChar(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.put_char;
        return _this;
    }
    PutChar.prototype.execute = function (machine) {
        //machine.putText(Buffer.from([this.c]));
    };
    PutChar.prototype.toString = function () {
        return "PutChar { c: '" + String.fromCharCode(this.c) + "' }";
    };
    return PutChar;
}(DviCommand));
// 0...127	set_char_i		typeset a character and move right
// 128	set1	c[1]	                typeset a character and move right
// 129	set2	c[2]
// 130	set3	c[3]
// 131	set4	c[4]
var SetChar = /** @class */ (function (_super) {
    __extends(SetChar, _super);
    function SetChar(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_char;
        return _this;
    }
    SetChar.prototype.execute = function (machine) {
        var width = machine.setChar(this.c, this.text_height, this.text_width);
        machine.moveRight(width);
    };
    SetChar.prototype.toString = function () {
        return "SetChar { c: '" + String.fromCharCode(this.c) + "' }";
    };
    return SetChar;
}(DviCommand));
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
var PutRule = /** @class */ (function (_super) {
    __extends(PutRule, _super);
    function PutRule(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.put_rule;
        return _this;
    }
    PutRule.prototype.execute = function (machine) {
        machine.putRule(this);
    };
    PutRule.prototype.toString = function () {
        return "PutRule { a: " + this.a + ", b: " + this.b + " }";
    };
    return PutRule;
}(DviCommand));
// 132	set_rule	a[4], b[4]	typeset a rule and move right
var SetRule = /** @class */ (function (_super) {
    __extends(SetRule, _super);
    function SetRule(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_rule;
        return _this;
    }
    SetRule.prototype.execute = function (machine) {
        machine.putRule(this);
        machine.moveRight(this.b);
    };
    SetRule.prototype.toString = function () {
        return "SetRule { a: " + this.a + ", b: " + this.b + " }";
    };
    return SetRule;
}(DviCommand));
// 138	nop		no operation
var Nop = /** @class */ (function (_super) {
    __extends(Nop, _super);
    function Nop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.nop;
        return _this;
    }
    Nop.prototype.toString = function () {
        return "Nop { }";
    };
    return Nop;
}(DviCommand));
// 139	bop	c_0[4]..c_9[4], p[4]	beginning of page
var Bop = /** @class */ (function (_super) {
    __extends(Bop, _super);
    function Bop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.bop;
        return _this;
    }
    Bop.prototype.execute = function (machine) {
        machine.beginPage(this);
    };
    Bop.prototype.toString = function () {
        return "Bop { ... }";
    };
    return Bop;
}(DviCommand));
// 140	eop		ending of page
var Eop = /** @class */ (function (_super) {
    __extends(Eop, _super);
    function Eop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.eop;
        return _this;
    }
    Eop.prototype.execute = function (machine) {
        if (machine.stack.length)
            throw Error('Stack should be empty at the end of a page.');
        machine.endPage();
    };
    Eop.prototype.toString = function () {
        return "Eop { }";
    };
    return Eop;
}(DviCommand));
// 141	push		save the current positions
var Push = /** @class */ (function (_super) {
    __extends(Push, _super);
    function Push(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.push;
        return _this;
    }
    Push.prototype.execute = function (machine) {
        machine.push();
    };
    Push.prototype.toString = function () {
        return "Push { }";
    };
    return Push;
}(DviCommand));
// 142	pop		restore previous positions
var Pop = /** @class */ (function (_super) {
    __extends(Pop, _super);
    function Pop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.pop;
        return _this;
    }
    Pop.prototype.execute = function (machine) {
        machine.pop();
    };
    Pop.prototype.toString = function () {
        return "Pop { }";
    };
    return Pop;
}(DviCommand));
// 143	right1	b[1]	move right
// 144	right2	b[2]
// 145	right3	b[3]
// 146	right4	b[4]
var MoveRight = /** @class */ (function (_super) {
    __extends(MoveRight, _super);
    function MoveRight(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.right;
        return _this;
    }
    MoveRight.prototype.execute = function (machine) {
        machine.moveRight(this.b);
    };
    MoveRight.prototype.toString = function () {
        return "MoveRight { b: " + this.b + " }";
    };
    return MoveRight;
}(DviCommand));
// 147	w0		move right by w
// 148	w1	b[1]	move right and set w
// 149	w2	b[2]
// 150	w3	b[3]
// 151	w4	b[4]
var MoveW = /** @class */ (function (_super) {
    __extends(MoveW, _super);
    function MoveW(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.w;
        return _this;
    }
    MoveW.prototype.execute = function (machine) {
        if (this.length > 1)
            machine.position.w = this.b;
        machine.moveRight(machine.position.w);
    };
    MoveW.prototype.toString = function () {
        if (this.length > 1)
            return "MoveW { b: " + this.b + " }";
        else
            return "MoveW0 { }";
    };
    return MoveW;
}(DviCommand));
// 152	x0		move right by x
// 153	x1	b[1]	move right and set x
// 154	x2	b[2]
// 155	x3	b[3]
// 156	x4	b[4]
var MoveX = /** @class */ (function (_super) {
    __extends(MoveX, _super);
    function MoveX(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.x;
        return _this;
    }
    MoveX.prototype.execute = function (machine) {
        if (this.length > 1)
            machine.position.x = this.b;
        machine.moveRight(machine.position.x);
    };
    MoveX.prototype.toString = function () {
        if (this.length > 1)
            return "MoveX { b: " + this.b + " }";
        else
            return "MoveX0 { }";
    };
    return MoveX;
}(DviCommand));
// 157	down1	a[1]	move down
// 158	down2	a[2]
// 159	down3	a[3]
// 160	down4	a[4]
var MoveDown = /** @class */ (function (_super) {
    __extends(MoveDown, _super);
    function MoveDown(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.down;
        return _this;
    }
    MoveDown.prototype.execute = function (machine) {
        machine.moveDown(this.a);
    };
    MoveDown.prototype.toString = function () {
        return "MoveDown { a: " + this.a + " }";
    };
    return MoveDown;
}(DviCommand));
// 161	y0		move down by y
// 162	y1	a[1]	move down and set y
// 163	y2	a[2]
// 164	y3	a[3]
// 165	y4	a[4]
var MoveY = /** @class */ (function (_super) {
    __extends(MoveY, _super);
    function MoveY(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.y;
        return _this;
    }
    MoveY.prototype.execute = function (machine) {
        if (this.length > 1)
            machine.position.y = this.a;
        machine.moveDown(machine.position.y);
    };
    MoveY.prototype.toString = function () {
        if (this.length > 1)
            return "MoveY { a: " + this.a + " }";
        else
            return "MoveY0 { }";
    };
    return MoveY;
}(DviCommand));
// 166	z0		move down by z
// 167	z1	a[1]	move down and set z
// 168	z2	a[2]
// 169	z3	a[3]
// 170	z4	a[4]
var MoveZ = /** @class */ (function (_super) {
    __extends(MoveZ, _super);
    function MoveZ(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.z;
        return _this;
    }
    MoveZ.prototype.execute = function (machine) {
        if (this.length > 1)
            machine.position.z = this.a;
        machine.moveDown(machine.position.z);
    };
    MoveZ.prototype.toString = function () {
        if (this.length > 1)
            return "MoveZ { a: " + this.a + " }";
        else
            return "MoveZ0 { }";
    };
    return MoveZ;
}(DviCommand));
// 171...234	fnt_num_i		set current font to i
// 235	fnt1	k[1]	set current font
// 236	fnt2	k[2]
// 237	fnt3	k[3]
// 238	fnt4	k[4]
var SetFont = /** @class */ (function (_super) {
    __extends(SetFont, _super);
    function SetFont(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.fnt;
        return _this;
    }
    SetFont.prototype.execute = function (machine) {
        //console.log("Setting mainfont to " + this.k);
        if (machine.fonts[this.k]) {
            machine.setFont(machine.fonts[this.k]);
        }
        else
            throw Error("Could not find font " + this.k + ".");
    };
    SetFont.prototype.toString = function () {
        return "SetFont { k: " + this.k + " }";
    };
    return SetFont;
}(DviCommand));
// 239	xxx1	k[1], x[k]	extension to DVI primitives
// 240	xxx2	k[2], x[k]
// 241	xxx3	k[3], x[k]
// 242	xxx4	k[4], x[k]
function _intToHex(n) {
    return ("00" + Math.round(n).toString(16)).substr(-2);
}
function _texColor(name) {
    if (name == 'gray 0')
        return 'black';
    if (name == 'gray 1')
        return 'white';
    if (name.startsWith('rgb ')) {
        return '#' + name.split(' ').slice(1).map(function (x) { return _intToHex(parseFloat(x) * 255); }).join('');
    }
    if (name.startsWith('gray ')) {
        var x = name.split(' ')[1];
        return _texColor('rgb ' + x + ' ' + x + ' ' + x);
    }
    return 'black';
}
var Special = /** @class */ (function (_super) {
    __extends(Special, _super);
    function Special(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.xxx;
        _this.special = true;
        return _this;
    }
    Special.prototype.toString = function () {
        return "Special { x: '" + this.x + "' }";
    };
    Special.prototype.execute = function (machine) {
        if (this.x.startsWith('dvisvgm:raw ')) {
            var svg = this.x.replace(/^dvisvgm:raw /, '');
            machine.putSVG(svg);
        }
        else if (this.x.startsWith('pdf:pagesize')) {
            var papersize = this.x.replace(/^pdf:pagesize /, '');
            var regex = /width ([0-9\.]+)pt height ([0-9\.]+)pt/gm;
            var m = regex.exec(papersize);
            var paperWidth = Number(m[1]);
            var paperHeight = Number(m[2]);
            machine.setPapersize(paperWidth, paperHeight);
        }
        else if (this.x.startsWith('color push ')) {
            var color = _texColor(this.x.replace(/^color push /, ''));
            machine.pushColor(color);
        }
        else if (this.x.startsWith('color pop ')) {
            machine.popColor();
        }
        else if (this.x.startsWith('pdf:image bbox 0 0 ')) {
            var image = this.x.replace(/pdf:image bbox 0 0 /, '').split(' ');
            var imageWidth = Number(image[0]);
            var imageHeight = Number(image[1]);
            var url = image[6].substr(1).slice(0, -1);
            machine.putImage(imageWidth, imageHeight, url);
        }
    };
    return Special;
}(DviCommand));
// 243	fnt_def1	k[1], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]	define the meaning of a font number
// 244	fnt_def2	k[2], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]
// 245	fnt_def3	k[3], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]
// 246	fnt_def4	k[4], c[4], s[4], d[4], 
// a[1], l[1], n[a+l]
var FontDefinition = /** @class */ (function (_super) {
    __extends(FontDefinition, _super);
    function FontDefinition(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.fnt_def;
        return _this;
    }
    FontDefinition.prototype.execute = function (machine) {
        //console.log("Defining Local Font name: " + this.n + " index: " + this.k);
        machine.fonts[this.k] = machine.loadFont({
            name: this.n,
            checksum: this.c,
            scaleFactor: this.s,
            designSize: this.d
        }, false);
    };
    FontDefinition.prototype.toString = function () {
        return "FontDefinition { k: " + this.k + ", n: '" + this.n + "', ... }";
    };
    return FontDefinition;
}(DviCommand));
// 247	pre	i[1], num[4], den[4], mag[4],  k[1], x[k]	preamble
var Preamble = /** @class */ (function (_super) {
    __extends(Preamble, _super);
    function Preamble(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.pre;
        return _this;
    }
    Preamble.prototype.execute = function (machine) {
        if (this.num <= 0)
            throw Error('Invalid numerator (must be > 0)');
        if (this.den <= 0)
            throw Error('Invalid denominator (must be > 0)');
        if (this.i != 7) {
            throw Error("DVI format must be 2. (" + this.i + ") ");
        }
        machine.preamble(this.num, this.den, this.mag, this.x);
    };
    Preamble.prototype.toString = function () {
        return "Preamble { i: " + this.i + ", num: " + this.num + ", den: " + this.den + ", mag: " + this.mag + ", x: '" + this.x + "' }";
    };
    return Preamble;
}(DviCommand));
// 248	post	p[4], num[4], den[4], mag[4], l[4], u[4], s[2], t[2]
// < font definitions >	postamble beginning
var Post = /** @class */ (function (_super) {
    __extends(Post, _super);
    function Post(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.post;
        return _this;
    }
    Post.prototype.execute = function (machine) {
        machine.post(this);
    };
    Post.prototype.toString = function () {
        return "Post { p: " + this.p + ", num: " + this.num + ", den: " + this.den + ", mag: " + this.mag + ", ... }";
    };
    return Post;
}(DviCommand));
// 249	post_post	q[4], i[1]; 223's	postamble ending
var PostPost = /** @class */ (function (_super) {
    __extends(PostPost, _super);
    function PostPost(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.post_post;
        return _this;
    }
    PostPost.prototype.execute = function (machine) {
        machine.postPost(this);
    };
    PostPost.prototype.toString = function () {
        return "PostPost { q: " + this.q + ", i: " + this.i + " }";
    };
    return PostPost;
}(DviCommand));
//252 
var NativeFontDefinition = /** @class */ (function (_super) {
    __extends(NativeFontDefinition, _super);
    function NativeFontDefinition(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.define_native_font;
        return _this;
    }
    NativeFontDefinition.prototype.execute = function (machine) {
        //console.log("Defining Native Font name: " + this.filename + " index: " + this.fontnumber);
        machine.fonts[this.fontnumber] = machine.loadFont({
            name: this.filename,
            fontsize: this.fontsize,
            faceindex: this.faceindex,
            height: this.height,
            depth: this.depth,
            rbga: this.rbga,
            extend: this.extend,
            slant: this.slant,
            embolden: this.embolden
        }, true);
    };
    NativeFontDefinition.prototype.toString = function () {
        return "NativeFontDefinition { filename: " + this.filename + ", fontnumber: " + this.fontnumber + ", length: " + this.length + ", rbga: " + this.rbga + "}";
    };
    return NativeFontDefinition;
}(DviCommand));
//253 
var _glyphLocation = /** @class */ (function () {
    function _glyphLocation() {
    }
    _glyphLocation.prototype.toString = function () {
        return "Glyph { xoffset: " + this.xoffset + ", yoffset: " + this.yoffset + "}";
    };
    return _glyphLocation;
}());
var SetGlyph = /** @class */ (function (_super) {
    __extends(SetGlyph, _super);
    // glyphLocations: _glyphLocation[];
    // glyphIds: number[];
    function SetGlyph(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_text_and_glyphs;
        return _this;
    }
    SetGlyph.prototype.execute = function (machine) {
        var width = machine.setNativeText(this.text, this.width);
        machine.moveRight(width);
    };
    SetGlyph.prototype.toString = function () {
        return "SetGlyph { count: " + this.textcount + " }";
    };
    return SetGlyph;
}(DviCommand));
function parseCommand(opcode, buffer) {
    if ((opcode >= Opcode.set_char) && (opcode < Opcode.set1)) {
        throw Error("SwiftLaTeX does not generate simple setchar");
        //return new SetChar({ c: opcode, length: 1 });
    }
    if ((opcode >= Opcode.fnt) && (opcode < Opcode.fnt1))
        return new SetFont({ k: opcode - 171, length: 1 });
    // Technically these are undefined opcodes, but we'll pretend they are NOPs
    if ((opcode == 250) || (opcode == 251) || (opcode == 255)) {
        throw Error("Undefined opcode " + opcode);
        //return new Nop({ length: 1 });
    }
    switch (opcode) {
        case Opcode.set1:
            if (buffer.length < 9)
                throw Error("not enough bytes to process opcode " + opcode);
            return new SetChar({
                c: buffer.readUInt8(0),
                text_height: buffer.readUInt32BE(1),
                text_width: buffer.readUInt32BE(5),
                length: 9 + 1
            });
        case Opcode.set2:
        case Opcode.set3:
        case Opcode.set4:
            throw Error("SwiftLaTeX does not generate set_char234");
        case Opcode.set_rule:
            if (buffer.length < 8)
                throw Error("not enough bytes to process opcode " + opcode);
            return new SetRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9
            });
        case Opcode.put_char:
        case Opcode.put2:
        case Opcode.put3:
        case Opcode.put4:
            throw Error("SwiftLaTeX engine does not generate put_char");
        // if (buffer.length < opcode - Opcode.put_char + 1) throw Error(`not enough bytes to process opcode ${opcode}`);
        // return new PutChar({
        //     c: buffer.readIntBE(0, opcode - Opcode.put_char + 1),
        //     length: opcode - Opcode.put_char + 1 + 1
        // });
        case Opcode.put_rule:
            if (buffer.length < 8)
                throw Error("not enough bytes to process opcode " + opcode);
            return new PutRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9
            });
        case Opcode.nop:
            return new Nop({ length: 1 });
        case Opcode.bop:
            if (buffer.length < 44)
                throw Error("not enough bytes to process opcode " + opcode);
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
            if (buffer.length < opcode - Opcode.right + 1)
                throw Error("not enough bytes to process opcode " + opcode);
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
            if (buffer.length < opcode - Opcode.w)
                throw Error("not enough bytes to process opcode " + opcode);
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
            if (buffer.length < opcode - Opcode.x)
                throw Error("not enough bytes to process opcode " + opcode);
            return new MoveX({
                b: buffer.readIntBE(0, opcode - Opcode.x),
                length: opcode - Opcode.x + 1
            });
        case Opcode.down:
        case Opcode.down2:
        case Opcode.down3:
        case Opcode.down4:
            if (buffer.length < opcode - Opcode.down + 1)
                throw Error("not enough bytes to process opcode " + opcode);
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
            if (buffer.length < opcode - Opcode.y)
                throw Error("not enough bytes to process opcode " + opcode);
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
            if (buffer.length < opcode - Opcode.z)
                throw Error("not enough bytes to process opcode " + opcode);
            return new MoveZ({
                a: buffer.readIntBE(0, opcode - Opcode.z),
                length: opcode - Opcode.z + 1
            });
        case Opcode.fnt1:
        case Opcode.fnt2:
        case Opcode.fnt3:
        case Opcode.fnt4:
            if (buffer.length < opcode - Opcode.fnt1 + 1)
                throw Error("not enough bytes to process opcode " + opcode);
            return new SetFont({
                k: buffer.readIntBE(0, opcode - Opcode.fnt1 + 1),
                length: opcode - Opcode.fnt1 + 1 + 1
            });
        case Opcode.xxx:
        case Opcode.xxx2:
        case Opcode.xxx3:
        case Opcode.xxx4: {
            var i = opcode - Opcode.xxx + 1;
            if (buffer.length < i)
                throw Error("not enough bytes to process opcode " + opcode);
            var k = buffer.readUIntBE(0, i);
            if (buffer.length < i + k)
                throw Error("not enough bytes to process opcode " + opcode);
            return new Special({
                x: buffer.slice(i, i + k).toString(),
                length: i + k + 1
            });
        }
        case Opcode.fnt_def:
        case Opcode.fnt_def2:
        case Opcode.fnt_def3:
        case Opcode.fnt_def4: {
            var i = opcode - Opcode.fnt_def + 1;
            if (buffer.length < i)
                throw Error("not enough bytes to process opcode " + opcode);
            var k = buffer.readIntBE(0, i);
            if (buffer.length < i + 14)
                throw Error("not enough bytes to process opcode " + opcode);
            var c = buffer.readUInt32BE(i + 0);
            var s = buffer.readUInt32BE(i + 4);
            var d = buffer.readUInt32BE(i + 8);
            var a = buffer.readUInt8(i + 12);
            var l = buffer.readUInt8(i + 13);
            if (buffer.length < i + 14 + a + l)
                throw Error("not enough bytes to process opcode " + opcode);
            var n = buffer.slice(i + 14, i + 14 + a + l).toString();
            return new FontDefinition({
                k: k,
                c: c,
                s: s,
                d: d,
                a: a,
                l: l,
                n: n,
                length: i + 14 + a + l + 1
            });
        }
        case Opcode.pre: {
            if (buffer.length < 14)
                throw Error("not enough bytes to process opcode " + opcode);
            var i = buffer.readUInt8(0);
            var num = buffer.readUInt32BE(1);
            var den = buffer.readUInt32BE(5);
            var mag = buffer.readUInt32BE(9);
            var k = buffer.readUInt8(13);
            if (buffer.length < 14 + k + 8)
                throw Error("not enough bytes to process opcode " + opcode);
            var comment = buffer.slice(14, 14 + k).toString();
            return new Preamble({
                i: i,
                num: num,
                den: den,
                mag: mag,
                x: comment,
                length: 14 + k + 1
            });
        }
        case Opcode.post:
            if (buffer.length < 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2)
                throw Error("not enough bytes to process opcode " + opcode);
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
                throw Error("not enough bytes to process opcode " + opcode);
            return new PostPost({
                q: buffer.readUInt32BE(0),
                i: buffer.readUInt8(4),
                length: 6
            });
        case Opcode.define_native_font:
            {
                if (buffer.length < 19)
                    throw Error("not enough bytes to process opcode " + opcode);
                var fontnum = buffer.readUInt32BE(0);
                var fontsize = buffer.readUInt32BE(4) / 65536;
                var flag = buffer.readUInt16BE(8);
                var filenamelen = buffer.readUInt8(10);
                var filename = buffer.slice(11, 11 + filenamelen).toString();
                var faceindex = buffer.readUInt32BE(11 + filenamelen);
                var height = buffer.readUInt32BE(11 + filenamelen + 4);
                var depth = buffer.readUInt32BE(11 + filenamelen + 8);
                var res = new NativeFontDefinition({
                    fontnumber: fontnum,
                    fontsize: fontsize,
                    flag: flag,
                    height: height,
                    depth: depth,
                    filenamelen: filenamelen,
                    filename: filename,
                    faceindex: faceindex
                });
                var XDV_FLAGS_COLORED = 0x0200;
                var XDV_FLAG_EXTEND = 0x1000;
                var XDV_FLAG_SLANT = 0x2000;
                var XDV_FLAG_EMBOLDEN = 0x4000;
                var length_1 = 23 + filenamelen;
                if ((flag & XDV_FLAGS_COLORED) != 0) {
                    res.rbga = buffer.readUInt32BE(length_1);
                    length_1 += 4;
                }
                if ((flag & XDV_FLAG_EXTEND) != 0) {
                    res.extend = buffer.readUInt32BE(length_1) / 65536.0;
                    length_1 += 4;
                }
                if ((flag & XDV_FLAG_SLANT) != 0) {
                    res.slant = buffer.readUInt32BE(length_1) / 65536.0;
                    length_1 += 4;
                }
                if ((flag & XDV_FLAG_EMBOLDEN) != 0) {
                    res.embolden = buffer.readUInt32BE(length_1) / 65536.0;
                    length_1 += 4;
                }
                res.length = length_1 + 1;
                return res;
            }
        case Opcode.set_glyphs:
            {
                if (buffer.length < 18)
                    throw Error("not enough bytes to process opcode " + opcode);
                var width = buffer.readUInt32BE(0);
                var glyphcount = buffer.readUInt16BE(4);
                var _x = buffer.readUInt16BE(6);
                var _y = buffer.readUInt16BE(10);
                var _glyph = buffer.readUInt16BE(14);
                var real_char = buffer.readUInt16BE(16);
                if (glyphcount != 1)
                    throw Error("SwiftLaTeX only generate single glyphs");
                console.log("Warning, set glyph is not fully implemented " + String.fromCharCode(real_char));
                var res = new SetGlyph({
                    text: [real_char],
                    textcount: 1,
                    width: width,
                    glyphcount: glyphcount,
                    length: 19
                });
                //console.log(res);
                return res;
            }
        case Opcode.set_text_and_glyphs:
            {
                if (buffer.length < 16)
                    throw Error("not enough bytes to process opcode " + opcode);
                var textcount = buffer.readUInt16BE(0);
                if (buffer.length < 2 + textcount * 2)
                    throw Error("not enough bytes to process opcode " + opcode + " for textcount");
                var text = [];
                for (var j = 0; j < textcount; j++) {
                    var n = buffer.readUInt16BE(2 + j * 2);
                    text.push(n);
                }
                var width = buffer.readUInt32BE(2 + textcount * 2);
                var glyphcount = buffer.readUInt16BE(2 + textcount * 2 + 4);
                //console.log(`How many count? ${glyphcount}`);
                if (buffer.length < 2 + textcount * 2 + 6 + glyphcount * 10)
                    throw Error("not enough bytes to process opcode " + opcode + " " + buffer.length + " " + glyphcount);
                var res = new SetGlyph({
                    text: text,
                    textcount: textcount,
                    width: width,
                    glyphcount: glyphcount,
                    length: 2 + textcount * 2 + 6 + glyphcount * 10 + 1
                });
                // let texts: number[] = [];
                // let glyphLocations: _glyphLocation[] = [];
                // let glyphIDs: number[] = [];
                // for(let j = 0; j < count; j ++) {
                //     texts.push(buffer.readUInt16BE(6 + j * 2));
                // }
                // for (let j = 0; j < count; j++) {
                //     let glyphLocation: _glyphLocation = new _glyphLocation();
                //     let xoffset = buffer.readUInt32BE(6 + count * 2 +j * 8);
                //     let yoffset = buffer.readUInt32BE(6 + count * 2 + j * 8 + 4);
                //     glyphLocation.xoffset = xoffset;
                //     glyphLocation.yoffset = yoffset;
                //     glyphLocations.push(glyphLocation);
                // }
                // for (let j = 0; j < count; j++) {
                //     let glyphID = buffer.readUInt16BE(6 + count * 10 + j * 2);
                //     glyphIDs.push(glyphID);
                // }
                // res.glyphLocations = glyphLocations;
                // res.glyphIds = glyphIDs;
                // res.texts = texts;
                // res.length = 6 + count * 12 + 1;
                //console.log(res);
                return res;
            }
    }
    throw Error("routine for " + opcode + " is not implemented");
}
function parseDVI(dviContent, machine) {
    var buffer = Buffer.from(dviContent.buffer);
    var isAfterPostamble = false;
    var offset = 0;
    while (offset < buffer.length) {
        var opcode = buffer.readUInt8(offset);
        if (isAfterPostamble) {
            if (opcode == 223) {
                offset++;
                continue;
            }
            else {
                throw Error('Only 223 bytes are permitted after the post-postamble.');
            }
        }
        var command = parseCommand(opcode, buffer.slice(offset + 1));
        // console.log(command);
        if (command) {
            command.execute(machine);
            offset += command.length;
            if (command.opcode == Opcode.post_post)
                isAfterPostamble = true;
        }
        else {
            console.error("Invaild DVI File detected\n");
            break;
        }
    }
}
exports.parseDVI = parseDVI;
