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
exports.__esModule = true;
var parser_1 = require("../parser");
var PushColor = /** @class */ (function (_super) {
    __extends(PushColor, _super);
    function PushColor(color) {
        var _this = _super.call(this, {}) || this;
        _this.color = color;
        return _this;
    }
    PushColor.prototype.execute = function (machine) {
        machine.pushColor(this.color);
    };
    PushColor.prototype.toString = function () {
        return "PushColor { color: '" + this.color + "' }";
    };
    return PushColor;
}(parser_1.DviCommand));
var PopColor = /** @class */ (function (_super) {
    __extends(PopColor, _super);
    function PopColor() {
        return _super.call(this, {}) || this;
    }
    PopColor.prototype.execute = function (machine) {
        machine.popColor();
    };
    PopColor.prototype.toString = function () {
        return "PopColor { }";
    };
    return PopColor;
}(parser_1.DviCommand));
function intToHex(n) {
    return ("00" + Math.round(n).toString(16)).substr(-2);
}
function texColor(name) {
    if (name == 'gray 0')
        return 'black';
    if (name == 'gray 1')
        return 'white';
    if (name.startsWith('rgb ')) {
        return '#' + name.split(' ').slice(1).map(function (x) { return intToHex(parseFloat(x) * 255); }).join('');
    }
    if (name.startsWith('gray ')) {
        var x = name.split(' ')[1];
        return texColor('rgb ' + x + ' ' + x + ' ' + x);
    }
    return 'black';
}
function default_1(commands) {
    return __asyncGenerator(this, arguments, function () {
        var queue, commands_1, commands_1_1, command, color, e_1_1;
        var e_1, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    queue = [];
                    _b.label = 1;
                case 1:
                    _b.trys.push([1, 17, 18, 23]);
                    commands_1 = __asyncValues(commands);
                    _b.label = 2;
                case 2: return [4 /*yield*/, __await(commands_1.next())];
                case 3:
                    if (!(commands_1_1 = _b.sent(), !commands_1_1.done)) return [3 /*break*/, 16];
                    command = commands_1_1.value;
                    if (!!command.special) return [3 /*break*/, 6];
                    return [4 /*yield*/, __await(command)];
                case 4: return [4 /*yield*/, _b.sent()];
                case 5:
                    _b.sent();
                    return [3 /*break*/, 15];
                case 6:
                    if (!!command.x.startsWith('color ')) return [3 /*break*/, 9];
                    return [4 /*yield*/, __await(command)];
                case 7: return [4 /*yield*/, _b.sent()];
                case 8:
                    _b.sent();
                    return [3 /*break*/, 15];
                case 9:
                    if (!command.x.startsWith('color push ')) return [3 /*break*/, 12];
                    color = texColor(command.x.replace(/^color push /, ''));
                    return [4 /*yield*/, __await(new PushColor(color))];
                case 10: return [4 /*yield*/, _b.sent()];
                case 11:
                    _b.sent();
                    _b.label = 12;
                case 12:
                    if (!command.x.startsWith('color pop')) return [3 /*break*/, 15];
                    return [4 /*yield*/, __await(new PopColor())];
                case 13: return [4 /*yield*/, _b.sent()];
                case 14:
                    _b.sent();
                    _b.label = 15;
                case 15: return [3 /*break*/, 2];
                case 16: return [3 /*break*/, 23];
                case 17:
                    e_1_1 = _b.sent();
                    e_1 = { error: e_1_1 };
                    return [3 /*break*/, 23];
                case 18:
                    _b.trys.push([18, , 21, 22]);
                    if (!(commands_1_1 && !commands_1_1.done && (_a = commands_1["return"]))) return [3 /*break*/, 20];
                    return [4 /*yield*/, __await(_a.call(commands_1))];
                case 19:
                    _b.sent();
                    _b.label = 20;
                case 20: return [3 /*break*/, 22];
                case 21:
                    if (e_1) throw e_1.error;
                    return [7 /*endfinally*/];
                case 22: return [7 /*endfinally*/];
                case 23: return [2 /*return*/];
            }
        });
    });
}
exports["default"] = default_1;
