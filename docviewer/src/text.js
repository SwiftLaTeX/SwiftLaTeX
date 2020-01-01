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
var machine_1 = require("./machine");
var epsilon = 0.00001;
var TextMachine = /** @class */ (function (_super) {
    __extends(TextMachine, _super);
    function TextMachine(o) {
        var _this = _super.call(this) || this;
        _this.output = o;
        _this.snippets = [];
        return _this;
    }
    TextMachine.prototype.putRule = function (rule) {
    };
    TextMachine.prototype.beginPage = function (page) {
        _super.prototype.beginPage.call(this, page);
        this.snippets = [];
    };
    TextMachine.prototype.endPage = function () {
        this.snippets = this.snippets.sort(function (a, b) {
            if (a[1] < b[1])
                return -1;
            if (a[1] > b[1])
                return 1;
            if (a[0] < b[0])
                return -1;
            if (a[0] > b[0])
                return 1;
            return 0;
        });
        if (this.snippets.length == 0)
            return;
        var previousH = this.snippets[0][0];
        var previousV = this.snippets[0][1];
        for (var _i = 0, _a = this.snippets; _i < _a.length; _i++) {
            var snippet = _a[_i];
            var h = snippet[0];
            var v = snippet[1];
            var text = snippet[2];
            if (v > previousV)
                this.output.write("\n");
            if (h > previousH + epsilon)
                this.output.write(" ");
            this.output.write(text.toString());
            previousV = v;
            previousH = h;
        }
    };
    TextMachine.prototype.putText = function (text) {
        this.snippets.push([this.position.h, this.position.v, text]);
        return epsilon;
    };
    TextMachine.prototype.postPost = function (p) {
        this.output.end();
    };
    return TextMachine;
}(machine_1.Machine));
exports["default"] = TextMachine;
