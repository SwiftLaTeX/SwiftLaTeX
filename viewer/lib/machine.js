"use strict";
//  var path = execSync('kpsewhich ' + name + '.tfm').toString().split("\n")[0];
Object.defineProperty(exports, "__esModule", { value: true });
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
        this.body = "";
        this.color = "black";
    }
    Machine.prototype.getBody = function () {
        return this.body;
    };
    Machine.prototype.getHead = function () {
        return "";
    };
    Machine.prototype.pushColor = function (c) {
        this.colorStack.push(this.color);
        this.color = c;
    };
    Machine.prototype.popColor = function () {
        this.color = this.colorStack.pop();
    };
    Machine.prototype.setPapersize = function (width, height) {
        this.paperwidth = width;
        this.paperheight = height;
    };
    Machine.prototype.putSVG = function (svg) {
        var left = this.position.h * this.pointsPerDviUnit;
        var top = this.position.v * this.pointsPerDviUnit;
        this.svgDepth += (svg.match(/<svg>/g) || []).length;
        this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;
        svg = svg.replace("<svg>", "<svg width=\"10pt\" height=\"10pt\" viewBox=\"-5 -5 10 10\" style=\"overflow: visible; position: absolute;\">");
        svg = svg.replace(/{\?x}/g, left.toString());
        svg = svg.replace(/{\?y}/g, top.toString());
        this.body += svg;
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
    Machine.prototype.preamble = function (numerator, denominator, magnification, comment) {
        var dviUnit = magnification * numerator / 1000.0 / denominator;
        var resolution = 300.0; // ppi
        var tfm_conv = (25400000.0 / numerator) * (denominator / 473628672) / 16.0;
        var conv = (numerator / 254000.0) * (resolution / denominator);
        conv = conv * (magnification / 1000.0);
        this.pointsPerDviUnit = dviUnit * 72.27 / 100000.0 / 2.54;
    };
    Machine.prototype.putRule = function (rule) {
        var a = rule.a * this.pointsPerDviUnit;
        var b = rule.b * this.pointsPerDviUnit;
        var left = this.position.h * this.pointsPerDviUnit;
        var bottom = this.position.v * this.pointsPerDviUnit;
        var top = bottom - a;
        this.body += "<span style=\"background: " + this.color + "; position: absolute; top: " + top + "pt; left: " + left + "pt; width:" + b + "pt; height: " + a + "pt;\"></span>\n";
    };
    Machine.prototype._to_legal_unicode = function (c) {
        if ((c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c == 0xAD)) {
            return c + 0xE000;
        }
        else {
            return c;
        }
    };
    Machine.prototype.setChar = function (c, text_height, text_width) {
        var textWidth = 0;
        var textHeight = 0;
        c = this._to_legal_unicode(c);
        var htmlText = String.fromCharCode(c);
        var cssleft = this.position.h * this.pointsPerDviUnit;
        var cssheight = text_height * this.pointsPerDviUnit;
        var csstop = this.position.v * this.pointsPerDviUnit;
        var fontsize = this.font.designSize / 65536.0;
        if (this.svgDepth == 0) {
            this.body += "<div style=\"line-height: 0; color: " + this.color + "; font-family: " + this.font.name + "; font-size: " + fontsize + "pt; position: absolute; top: " + (csstop - cssheight) + "pt; left: " + cssleft + "pt;\">" + htmlText + "<span style=\"display: inline-block; vertical-align: " + cssheight + "pt; \"></span></div>\n";
        }
        else {
            var bottom = this.position.v * this.pointsPerDviUnit;
            // No 'pt' on fontsize since those units are potentially scaled
            this.body += "<text alignment-baseline=\"baseline\" y=\"" + bottom + "\" x=\"" + cssleft + "\" style=\"font-family: " + this.font.name + ";\" font-size=\"" + fontsize + "\">" + htmlText + "</text>\n";
        }
        return text_width;
    };
    Machine.prototype.setNativeText = function (text, width) {
        var htmlText = "";
        for (var j = 0; j < text.length; j++) {
            htmlText += String.fromCharCode(text[j]);
        }
        var cssleft = this.position.h * this.pointsPerDviUnit;
        var csstop = this.position.v * this.pointsPerDviUnit;
        var fontsize = this.font.designSize;
        var lineheight = (this.font.height + this.font.depth) / 1048576.0;
        var textheight = lineheight * fontsize; /*Todo, not sure whether it is correct*/
        this.body += "<span style=\"line-height: " + lineheight + "; color: " + this.color + "; white-space:pre; font-family: " + this.font.name + "; font-size: " + fontsize + "pt; position: absolute; top: " + (csstop - textheight) + "pt; left: " + cssleft + "pt;\">" + htmlText + "</span>\n";
        return width;
    };
    Machine.prototype.putImage = function (width, height, url) {
        var cssleft = this.position.h * this.pointsPerDviUnit;
        var csstop = this.position.v * this.pointsPerDviUnit;
        this.body += "<div data-url=\"" + url + "\" style=\"top: " + (csstop - height) + "pt; left: " + cssleft + "pt; position: absolute; height:" + height + "pt; width:" + width + "pt; background-color:grey;\"></div>";
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
        f.height = properties.height;
        f.depth = properties.depth;
        f.rbga = properties.rgba;
        f.extend = properties.extend;
        f.slant = properties.slant;
        f.embolden = properties.embolden;
        return f;
    };
    return Machine;
}());
exports.Machine = Machine;
