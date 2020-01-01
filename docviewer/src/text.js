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
        var e_1, _a;
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
        try {
            for (var _b = __values(this.snippets), _c = _b.next(); !_c.done; _c = _b.next()) {
                var snippet = _c.value;
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
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b["return"])) _a.call(_b);
            }
            finally { if (e_1) throw e_1.error; }
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
