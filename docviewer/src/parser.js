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
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
var __asyncValues = (this && this.__asyncValues) || function (o) {
    if (!Symbol.asyncIterator) throw new TypeError("Symbol.asyncIterator is not defined.");
    var m = o[Symbol.asyncIterator], i;
    return m ? m.call(o) : (o = typeof __values === "function" ? __values(o) : o[Symbol.iterator](), i = {}, verb("next"), verb("throw"), verb("return"), i[Symbol.asyncIterator] = function () { return this; }, i);
    function verb(n) { i[n] = o[n] && function (v) { return new Promise(function (resolve, reject) { v = o[n](v), settle(resolve, reject, v.done, v.value); }); }; }
    function settle(resolve, reject, d, v) { Promise.resolve(v).then(function(v) { resolve({ value: v, done: d }); }, reject); }
};
var __await = (this && this.__await) || function (v) { return this instanceof __await ? (this.v = v, this) : new __await(v); }
var __asyncGenerator = (this && this.__asyncGenerator) || function (thisArg, _arguments, generator) {
    if (!Symbol.asyncIterator) throw new TypeError("Symbol.asyncIterator is not defined.");
    var g = generator.apply(thisArg, _arguments || []), i, q = [];
    return i = {}, verb("next"), verb("throw"), verb("return"), i[Symbol.asyncIterator] = function () { return this; }, i;
    function verb(n) { if (g[n]) i[n] = function (v) { return new Promise(function (a, b) { q.push([n, v, a, b]) > 1 || resume(n, v); }); }; }
    function resume(n, v) { try { step(g[n](v)); } catch (e) { settle(q[0][3], e); } }
    function step(r) { r.value instanceof __await ? Promise.resolve(r.value.v).then(fulfill, reject) : settle(q[0][2], r); }
    function fulfill(value) { resume("next", value); }
    function reject(value) { resume("throw", value); }
    function settle(f, v) { if (f(v), q.shift(), q.length) resume(q[0][0], q[0][1]); }
};
var __asyncDelegator = (this && this.__asyncDelegator) || function (o) {
    var i, p;
    return i = {}, verb("next"), verb("throw", function (e) { throw e; }), verb("return"), i[Symbol.iterator] = function () { return this; }, i;
    function verb(n, f) { i[n] = o[n] ? function (v) { return (p = !p) ? { value: __await(o[n](v)), done: n === "return" } : f ? f(v) : v; } : f; }
};
var __values = (this && this.__values) || function(o) {
    var s = typeof Symbol === "function" && Symbol.iterator, m = s && o[s], i = 0;
    if (m) return m.call(o);
    if (o && typeof o.length === "number") return {
        next: function () {
            if (o && i >= o.length) o = void 0;
            return { value: o && o[i++], done: !o };
        }
    };
    throw new TypeError(s ? "Object is not iterable." : "Symbol.iterator is not defined.");
};
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
        machine.putText(Buffer.from([this.c]));
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
        var text = Buffer.from([this.c]);
        var width = machine.putText(text);
        machine.moveRight(width);
    };
    SetChar.prototype.toString = function () {
        return "SetChar { c: '" + String.fromCharCode(this.c) + "' }";
    };
    return SetChar;
}(DviCommand));
var SetText = /** @class */ (function (_super) {
    __extends(SetText, _super);
    function SetText(properties) {
        return _super.call(this, properties) || this;
    }
    SetText.prototype.execute = function (machine) {
        var width = machine.putText(this.t);
        machine.moveRight(width);
    };
    SetText.prototype.toString = function () {
        return "SetText { t: \"" + this.t.toString() + "\" }";
    };
    return SetText;
}(DviCommand));
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
        });
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
        machine.fonts[this.fontnumber] = machine.loadNativeFont({
            name: this.filename,
            fontsize: this.fontsize,
            faceindex: this.faceindex,
            rbga: this.rbga,
            extend: this.extend,
            slant: this.slant,
            embolden: this.embolden
        });
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
    function SetGlyph(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_glyphs;
        return _this;
    }
    SetGlyph.prototype.execute = function (machine) {
    };
    SetGlyph.prototype.toString = function () {
        return "SetGlyph { count: " + this.count + " }";
    };
    return SetGlyph;
}(DviCommand));
function parseCommand(opcode, buffer) {
    if ((opcode >= Opcode.set_char) && (opcode < Opcode.set1)) {
        return new SetChar({ c: opcode, length: 1 });
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
        case Opcode.set2:
        case Opcode.set3:
        case Opcode.set4:
            if (buffer.length < opcode - Opcode.set1 + 1)
                throw Error("not enough bytes to process opcode " + opcode);
            return new SetChar({
                c: buffer.readUIntBE(0, opcode - Opcode.set1 + 1),
                length: opcode - Opcode.set1 + 1 + 1
            });
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
            if (buffer.length < opcode - Opcode.put_char + 1)
                throw Error("not enough bytes to process opcode " + opcode);
            return new PutChar({
                c: buffer.readIntBE(0, opcode - Opcode.put_char + 1),
                length: opcode - Opcode.put_char + 1 + 1
            });
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
            if (buffer.length < 14 + k)
                throw Error("not enough bytes to process opcode " + opcode);
            return new Preamble({
                i: i,
                num: num,
                den: den,
                mag: mag,
                x: buffer.slice(14, 14 + k).toString(),
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
                var res = new NativeFontDefinition({
                    fontnumber: fontnum,
                    fontsize: fontsize,
                    flag: flag,
                    filenamelen: filenamelen,
                    filename: filename,
                    faceindex: faceindex
                });
                var XDV_FLAGS_COLORED = 0x0200;
                var XDV_FLAG_EXTEND = 0x1000;
                var XDV_FLAG_SLANT = 0x2000;
                var XDV_FLAG_EMBOLDEN = 0x4000;
                var length_1 = 15 + filenamelen;
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
                if (buffer.length < 16)
                    throw Error("not enough bytes to process opcode " + opcode);
                var width = buffer.readUInt32BE(0);
                var count = buffer.readUInt16BE(4);
                var glyphLocations = [];
                var glyphIDs = [];
                var res = new SetGlyph({
                    width: width,
                    count: count
                });
                if (buffer.length < 6 + count * 10)
                    throw Error("not enough bytes to process opcode " + opcode + " " + buffer.length + " " + count);
                for (var j = 0; j < count; j++) {
                    var glyphLocation = new _glyphLocation();
                    var xoffset = buffer.readUInt32BE(6 + j * 8);
                    var yoffset = buffer.readUInt32BE(6 + j * 8 + 4);
                    glyphLocation.xoffset = xoffset;
                    glyphLocation.yoffset = yoffset;
                    glyphLocations.push(glyphLocation);
                }
                for (var j = 0; j < count; j++) {
                    var glyphID = buffer.readUInt16BE(6 + count * 8 + j * 2);
                    glyphIDs.push(glyphID);
                }
                res.glyphLocations = glyphLocations;
                res.glyphIds = glyphIDs;
                res.length = 6 + count * 10 + 1;
                console.log(res);
                return res;
            }
        case Opcode.set_text_and_glyphs:
            throw Error('Not implemented. 254');
    }
    throw Error("routine for " + opcode + " is not implemented");
}
function dviParser(stream) {
    return __asyncGenerator(this, arguments, function dviParser_1() {
        var buffer, isAfterPostamble, stream_1, stream_1_1, chunk, e_1_1, offset, opcode, command;
        var e_1, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    buffer = Buffer.alloc(0);
                    isAfterPostamble = false;
                    _b.label = 1;
                case 1:
                    _b.trys.push([1, 6, 7, 12]);
                    stream_1 = __asyncValues(stream);
                    _b.label = 2;
                case 2: return [4 /*yield*/, __await(stream_1.next())];
                case 3:
                    if (!(stream_1_1 = _b.sent(), !stream_1_1.done)) return [3 /*break*/, 5];
                    chunk = stream_1_1.value;
                    buffer = Buffer.concat([buffer, chunk]);
                    _b.label = 4;
                case 4: return [3 /*break*/, 2];
                case 5: return [3 /*break*/, 12];
                case 6:
                    e_1_1 = _b.sent();
                    e_1 = { error: e_1_1 };
                    return [3 /*break*/, 12];
                case 7:
                    _b.trys.push([7, , 10, 11]);
                    if (!(stream_1_1 && !stream_1_1.done && (_a = stream_1["return"]))) return [3 /*break*/, 9];
                    return [4 /*yield*/, __await(_a.call(stream_1))];
                case 8:
                    _b.sent();
                    _b.label = 9;
                case 9: return [3 /*break*/, 11];
                case 10:
                    if (e_1) throw e_1.error;
                    return [7 /*endfinally*/];
                case 11: return [7 /*endfinally*/];
                case 12:
                    offset = 0;
                    _b.label = 13;
                case 13:
                    if (!(offset < buffer.length)) return [3 /*break*/, 18];
                    opcode = buffer.readUInt8(offset);
                    if (isAfterPostamble) {
                        if (opcode == 223) {
                            offset++;
                            return [3 /*break*/, 13];
                        }
                        else {
                            throw Error('Only 223 bytes are permitted after the post-postamble.');
                        }
                    }
                    command = parseCommand(opcode, buffer.slice(offset + 1));
                    if (!command) return [3 /*break*/, 16];
                    return [4 /*yield*/, __await(command)];
                case 14: return [4 /*yield*/, _b.sent()];
                case 15:
                    _b.sent();
                    offset += command.length;
                    if (command.opcode == Opcode.post_post)
                        isAfterPostamble = true;
                    return [3 /*break*/, 17];
                case 16: return [3 /*break*/, 18];
                case 17: return [3 /*break*/, 13];
                case 18: return [2 /*return*/];
            }
        });
    });
}
exports.dviParser = dviParser;
function execute(commands, machine) {
    var commands_1, commands_1_1;
    var e_2, _a;
    return __awaiter(this, void 0, void 0, function () {
        var command, e_2_1;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    _b.trys.push([0, 5, 6, 11]);
                    commands_1 = __asyncValues(commands);
                    _b.label = 1;
                case 1: return [4 /*yield*/, commands_1.next()];
                case 2:
                    if (!(commands_1_1 = _b.sent(), !commands_1_1.done)) return [3 /*break*/, 4];
                    command = commands_1_1.value;
                    // console.log(command.toString());
                    command.execute(machine);
                    _b.label = 3;
                case 3: return [3 /*break*/, 1];
                case 4: return [3 /*break*/, 11];
                case 5:
                    e_2_1 = _b.sent();
                    e_2 = { error: e_2_1 };
                    return [3 /*break*/, 11];
                case 6:
                    _b.trys.push([6, , 9, 10]);
                    if (!(commands_1_1 && !commands_1_1.done && (_a = commands_1["return"]))) return [3 /*break*/, 8];
                    return [4 /*yield*/, _a.call(commands_1)];
                case 7:
                    _b.sent();
                    _b.label = 8;
                case 8: return [3 /*break*/, 10];
                case 9:
                    if (e_2) throw e_2.error;
                    return [7 /*endfinally*/];
                case 10: return [7 /*endfinally*/];
                case 11: return [2 /*return*/];
            }
        });
    });
}
exports.execute = execute;
function merge(commands, filter, merge) {
    return __asyncGenerator(this, arguments, function merge_1() {
        var queue, commands_2, commands_2_1, command, e_3_1;
        var e_3, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    queue = [];
                    _b.label = 1;
                case 1:
                    _b.trys.push([1, 12, 13, 18]);
                    commands_2 = __asyncValues(commands);
                    _b.label = 2;
                case 2: return [4 /*yield*/, __await(commands_2.next())];
                case 3:
                    if (!(commands_2_1 = _b.sent(), !commands_2_1.done)) return [3 /*break*/, 11];
                    command = commands_2_1.value;
                    if (!filter(command)) return [3 /*break*/, 4];
                    queue.push(command);
                    return [3 /*break*/, 10];
                case 4:
                    if (!(queue.length > 0)) return [3 /*break*/, 7];
                    return [5 /*yield**/, __values(__asyncDelegator(__asyncValues(merge(queue))))];
                case 5: return [4 /*yield*/, __await.apply(void 0, [_b.sent()])];
                case 6:
                    _b.sent();
                    queue = [];
                    _b.label = 7;
                case 7: return [4 /*yield*/, __await(command)];
                case 8: return [4 /*yield*/, _b.sent()];
                case 9:
                    _b.sent();
                    _b.label = 10;
                case 10: return [3 /*break*/, 2];
                case 11: return [3 /*break*/, 18];
                case 12:
                    e_3_1 = _b.sent();
                    e_3 = { error: e_3_1 };
                    return [3 /*break*/, 18];
                case 13:
                    _b.trys.push([13, , 16, 17]);
                    if (!(commands_2_1 && !commands_2_1.done && (_a = commands_2["return"]))) return [3 /*break*/, 15];
                    return [4 /*yield*/, __await(_a.call(commands_2))];
                case 14:
                    _b.sent();
                    _b.label = 15;
                case 15: return [3 /*break*/, 17];
                case 16:
                    if (e_3) throw e_3.error;
                    return [7 /*endfinally*/];
                case 17: return [7 /*endfinally*/];
                case 18:
                    if (!(queue.length > 0)) return [3 /*break*/, 21];
                    return [5 /*yield**/, __values(__asyncDelegator(__asyncValues(merge(queue))))];
                case 19: return [4 /*yield*/, __await.apply(void 0, [_b.sent()])];
                case 20:
                    _b.sent();
                    _b.label = 21;
                case 21: return [2 /*return*/];
            }
        });
    });
}
exports.merge = merge;
function mergeText(commands) {
    return merge(commands, function (command) { return (command instanceof SetChar); }, function (queue) {
        var text;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    text = Buffer.from(queue.map(function (command) { return command.c; }));
                    return [4 /*yield*/, new SetText({ t: text })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
exports.mergeText = mergeText;
