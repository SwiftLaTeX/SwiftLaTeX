"use strict";
//  var path = execSync('kpsewhich ' + name + '.tfm').toString().split("\n")[0];
exports.__esModule = true;
var Position = /** @class */ (function () {
    function Position(properties) {
        if (properties) {
            this.h = properties.h;
            this.v = properties.v;
            this.w = properties.w;
            this.x = properties.x;
            this.y = properties.y;
            this.z = properties.z;
        }
        else {
            this.h = this.v = this.w = this.x = this.y = this.z = 0;
        }
    }
    return Position;
}());
var DviFont = /** @class */ (function () {
    function DviFont(properties) {
    }
    return DviFont;
}());
exports.DviFont = DviFont;
var Machine = /** @class */ (function () {
    function Machine() {
        this.fonts = [];
    }
    Machine.prototype.preamble = function (numerator, denominator, magnification, comment) {
    };
    Machine.prototype.pushColor = function (c) {
    };
    Machine.prototype.popColor = function () {
    };
    Machine.prototype.setPapersize = function (width, height) {
    };
    Machine.prototype.push = function () {
        this.stack.push(new Position(this.position));
    };
    Machine.prototype.pop = function () {
        this.position = this.stack.pop();
    };
    Machine.prototype.beginPage = function (page) {
        this.stack = [];
        this.position = new Position();
    };
    Machine.prototype.endPage = function () { };
    Machine.prototype.post = function (p) { };
    Machine.prototype.postPost = function (p) { };
    Machine.prototype.putRule = function (rule) {
    };
    Machine.prototype.moveRight = function (distance) {
        this.position.h += distance;
    };
    Machine.prototype.moveDown = function (distance) {
        this.position.v += distance;
    };
    Machine.prototype.setFont = function (font) {
        this.font = font;
        //console.log(font);
    };
    Machine.prototype.putSVG = function (svg) {
    };
    // Returns the width of the text
    Machine.prototype.setChar = function (text, text_height, text_width) {
        return 0;
    };
    // Returns the width of the text
    Machine.prototype.setNativeText = function (text) {
        return 0;
    };
    Machine.prototype.loadFont = function (properties) {
        var f = new DviFont(properties);
        f.name = properties.name;
        f.checksum = properties.checksum;
        f.scaleFactor = properties.scaleFactor;
        f.designSize = properties.designSize;
        return f;
    };
    Machine.prototype.loadNativeFont = function (properties) {
        var f = new DviFont(properties);
        f.name = properties.name;
        f.designSize = properties.fontsize;
        f.faceindex = properties.faceindex;
        f.rbga = properties.rgba;
        f.extend = properties.extend;
        f.slant = properties.slant;
        f.embolden = properties.embolden;
        return f;
    };
    return Machine;
}());
exports.Machine = Machine;
