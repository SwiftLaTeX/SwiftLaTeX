(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[12],{

/***/ "../../node_modules/css-loader/index.js!../../packages/output/src/browser/style/output.css":
/*!****************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/output/src/browser/style/output.css ***!
  \****************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2018 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n #outputView {\n\tfont-size: var(--theia-ui-font-size1);\n}\n\n#outputView #outputContents {\n    overflow: auto;\n    height: inherit;\n    padding: 6px;\n    box-sizing: border-box;\n}\n\n.theia-side-panel #outputView #outputContents {\n    margin-left: 14px;\n}\n\n#outputView #outputChannelList {\n    line-height: var(--theia-content-line-height);\n    font-size: var(--theia-ui-font-size1);\n    \n    -webkit-appearance: none;\n    -moz-appearance: none;\n    background-position: calc(100% - 6px) 8px, calc(100% - 2px) 8px, 100% 0;\n    background-size: 4px 5px;\n    background-repeat: no-repeat;\n    padding-left: 3px;\n    padding-right: 15px;\n}\n\n#outputView #outputClear {\n    background-image: var(--theia-icon-clear);\n    background-position: center;\n    background-repeat: no-repeat;\n    display: inline-block;\n    width: 21px;\n    opacity: var(--theia-mod-disabled-opacity);;\n}\n\n#outputView #outputClear.enabled {\n    opacity: 0.7;\n    cursor: pointer;\n}\n\n#outputView #outputClear.enabled:hover {\n    opacity: 1;\n}\n\n.output-tab-icon::before {\n    content: \"\\F024\"\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/output/lib/browser/output-contribution.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/browser/output-contribution.js ***!
  \**********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 TypeFox and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the Eclipse
 * Public License v. 2.0 are satisfied: GNU General Public License, version 2
 * with the GNU Classpath Exception which is available at
 * https://www.gnu.org/software/classpath/license.html.
 *
 * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
 ********************************************************************************/
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
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var view_contribution_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/view-contribution */ "../../packages/core/lib/browser/shell/view-contribution.js");
var output_widget_1 = __webpack_require__(/*! ./output-widget */ "../../packages/output/lib/browser/output-widget.js");
var OutputCommands;
(function (OutputCommands) {
    var OUTPUT_CATEGORY = 'Output';
    OutputCommands.CLEAR_OUTPUT_TOOLBAR = {
        id: 'output:clear',
        category: OUTPUT_CATEGORY,
        label: 'Clear Output',
        iconClass: 'clear-all'
    };
})(OutputCommands = exports.OutputCommands || (exports.OutputCommands = {}));
var OutputContribution = /** @class */ (function (_super) {
    __extends(OutputContribution, _super);
    function OutputContribution() {
        return _super.call(this, {
            widgetId: output_widget_1.OUTPUT_WIDGET_KIND,
            widgetName: 'Output',
            defaultWidgetOptions: {
                area: 'bottom'
            },
            toggleCommandId: 'output:toggle',
            toggleKeybinding: 'ctrlcmd+shift+u'
        }) || this;
    }
    OutputContribution.prototype.registerCommands = function (commands) {
        var _this = this;
        _super.prototype.registerCommands.call(this, commands);
        commands.registerCommand(OutputCommands.CLEAR_OUTPUT_TOOLBAR, {
            isEnabled: function (widget) { return _this.withWidget(widget, function () { return true; }); },
            isVisible: function (widget) { return _this.withWidget(widget, function () { return true; }); },
            execute: function (widget) { return _this.withWidget(widget, function (outputWidget) { return _this.clear(outputWidget); }); }
        });
    };
    OutputContribution.prototype.clear = function (widget) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                widget.clear();
                return [2 /*return*/];
            });
        });
    };
    OutputContribution.prototype.withWidget = function (widget, cb) {
        if (widget === void 0) { widget = this.tryGetWidget(); }
        if (widget instanceof output_widget_1.OutputWidget && widget.id === output_widget_1.OUTPUT_WIDGET_KIND) {
            return cb(widget);
        }
        return false;
    };
    OutputContribution = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], OutputContribution);
    return OutputContribution;
}(view_contribution_1.AbstractViewContribution));
exports.OutputContribution = OutputContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/browser/output-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/output/lib/browser/output-widget.js":
/*!****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/browser/output-widget.js ***!
  \****************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 TypeFox and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the Eclipse
 * Public License v. 2.0 are satisfied: GNU General Public License, version 2
 * with the GNU Classpath Exception which is available at
 * https://www.gnu.org/software/classpath/license.html.
 *
 * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
 ********************************************************************************/
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
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var __values = (this && this.__values) || function (o) {
    var m = typeof Symbol === "function" && o[Symbol.iterator], i = 0;
    if (m) return m.call(o);
    return {
        next: function () {
            if (o && i >= o.length) o = void 0;
            return { value: o && o[i++], done: !o };
        }
    };
};
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var output_channel_1 = __webpack_require__(/*! ../common/output-channel */ "../../packages/output/lib/common/output-channel.js");
var react_widget_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/react-widget */ "../../packages/core/lib/browser/widgets/react-widget.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
__webpack_require__(/*! ../../src/browser/style/output.css */ "../../packages/output/src/browser/style/output.css");
exports.OUTPUT_WIDGET_KIND = 'outputView';
var OutputWidget = /** @class */ (function (_super) {
    __extends(OutputWidget, _super);
    function OutputWidget() {
        var _this = _super.call(this) || this;
        _this.id = exports.OUTPUT_WIDGET_KIND;
        _this.title.label = 'Output';
        _this.title.caption = 'Output';
        _this.title.iconClass = 'fa output-tab-icon';
        _this.title.closable = true;
        _this.addClass('theia-output');
        _this.node.tabIndex = 0;
        return _this;
    }
    OutputWidget_1 = OutputWidget;
    OutputWidget.prototype.init = function () {
        var _this = this;
        this.outputChannelManager.getChannels().forEach(this.registerListener.bind(this));
        this.toDispose.push(this.outputChannelManager.onChannelAdded(function (channel) {
            _this.registerListener(channel);
            _this.update();
        }));
        this.toDispose.push(this.outputChannelManager.onSelectedChannelChange(function (event) {
            _this.update();
        }));
        this.update();
    };
    OutputWidget.prototype.onActivateRequest = function (msg) {
        _super.prototype.onActivateRequest.call(this, msg);
        var channelSelector = document.getElementById(OutputWidget_1.IDs.CHANNEL_LIST);
        if (channelSelector) {
            channelSelector.focus();
        }
        else {
            this.node.focus();
        }
    };
    OutputWidget.prototype.registerListener = function (outputChannel) {
        var _this = this;
        this.toDispose.push(outputChannel.onContentChange(function (c) {
            if (outputChannel === _this.outputChannelManager.selectedChannel) {
                _this.update();
            }
        }));
    };
    OutputWidget.prototype.render = function () {
        return React.createElement(React.Fragment, null, this.renderChannelContents());
    };
    OutputWidget.prototype.clear = function () {
        if (this.outputChannelManager.selectedChannel) {
            this.outputChannelManager.selectedChannel.clear();
        }
    };
    OutputWidget.prototype.renderChannelContents = function () {
        return React.createElement("div", { id: OutputWidget_1.IDs.CONTENTS }, this.renderLines());
    };
    OutputWidget.prototype.renderLines = function () {
        var e_1, _a, e_2, _b;
        var id = 0;
        var result = [];
        var style = {
            whiteSpace: 'pre',
            fontFamily: 'monospace',
        };
        if (this.outputChannelManager.selectedChannel) {
            try {
                for (var _c = __values(this.outputChannelManager.selectedChannel.getLines()), _d = _c.next(); !_d.done; _d = _c.next()) {
                    var text = _d.value;
                    var lines = text.split(/[\n\r]+/);
                    try {
                        for (var lines_1 = (e_2 = void 0, __values(lines)), lines_1_1 = lines_1.next(); !lines_1_1.done; lines_1_1 = lines_1.next()) {
                            var line = lines_1_1.value;
                            result.push(React.createElement("div", { style: style, key: id++ }, line));
                        }
                    }
                    catch (e_2_1) { e_2 = { error: e_2_1 }; }
                    finally {
                        try {
                            if (lines_1_1 && !lines_1_1.done && (_b = lines_1.return)) _b.call(lines_1);
                        }
                        finally { if (e_2) throw e_2.error; }
                    }
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (_d && !_d.done && (_a = _c.return)) _a.call(_c);
                }
                finally { if (e_1) throw e_1.error; }
            }
        }
        if (result.length === 0) {
            result.push(React.createElement("div", { style: style, key: id++ }, '<no output yet>'));
        }
        return result;
    };
    OutputWidget.prototype.onUpdateRequest = function (msg) {
        _super.prototype.onUpdateRequest.call(this, msg);
        setTimeout(function () {
            var div = document.getElementById(OutputWidget_1.IDs.CONTENTS);
            if (div && div.children.length > 0) {
                div.children[div.children.length - 1].scrollIntoView(false);
            }
        });
    };
    var OutputWidget_1;
    __decorate([
        inversify_1.inject(output_channel_1.OutputChannelManager),
        __metadata("design:type", output_channel_1.OutputChannelManager)
    ], OutputWidget.prototype, "outputChannelManager", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], OutputWidget.prototype, "init", null);
    OutputWidget = OutputWidget_1 = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], OutputWidget);
    return OutputWidget;
}(react_widget_1.ReactWidget));
exports.OutputWidget = OutputWidget;
(function (OutputWidget) {
    var IDs;
    (function (IDs) {
        IDs.CONTENTS = 'outputContents';
        IDs.CHANNEL_LIST = 'outputChannelList';
    })(IDs = OutputWidget.IDs || (OutputWidget.IDs = {}));
})(OutputWidget = exports.OutputWidget || (exports.OutputWidget = {}));
exports.OutputWidget = OutputWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/browser/output-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/output/lib/common/output-channel.js":
/*!****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/common/output-channel.js ***!
  \****************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 TypeFox and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the Eclipse
 * Public License v. 2.0 are satisfied: GNU General Public License, version 2
 * with the GNU Classpath Exception which is available at
 * https://www.gnu.org/software/classpath/license.html.
 *
 * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
 ********************************************************************************/
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var __read = (this && this.__read) || function (o, n) {
    var m = typeof Symbol === "function" && o[Symbol.iterator];
    if (!m) return o;
    var i = m.call(o), r, ar = [], e;
    try {
        while ((n === void 0 || n-- > 0) && !(r = i.next()).done) ar.push(r.value);
    }
    catch (error) { e = { error: error }; }
    finally {
        try {
            if (r && !r.done && (m = i["return"])) m.call(i);
        }
        finally { if (e) throw e.error; }
    }
    return ar;
};
var __spread = (this && this.__spread) || function () {
    for (var ar = [], i = 0; i < arguments.length; i++) ar = ar.concat(__read(arguments[i]));
    return ar;
};
Object.defineProperty(exports, "__esModule", { value: true });
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var output_preferences_1 = __webpack_require__(/*! ./output-preferences */ "../../packages/output/lib/common/output-preferences.js");
var vscode_ws_jsonrpc_1 = __webpack_require__(/*! vscode-ws-jsonrpc */ "../../node_modules/vscode-ws-jsonrpc/lib/index.js");
var OutputChannelManager = /** @class */ (function () {
    function OutputChannelManager() {
        this.channels = new Map();
        this.channelDeleteEmitter = new core_1.Emitter();
        this.channelAddedEmitter = new core_1.Emitter();
        this.selectedChannelEmitter = new core_1.Emitter();
        this.listOrSelectionEmitter = new core_1.Emitter();
        this.onChannelDelete = this.channelDeleteEmitter.event;
        this.onChannelAdded = this.channelAddedEmitter.event;
        this.onSelectedChannelChange = this.selectedChannelEmitter.event;
        this.onListOrSelectionChange = this.listOrSelectionEmitter.event;
        this.toDispose = new vscode_ws_jsonrpc_1.DisposableCollection();
    }
    OutputChannelManager.prototype.init = function () {
        var _this = this;
        this.getChannels().forEach(this.registerListener.bind(this));
        this.toDispose.push(this.onChannelAdded(function (channel) {
            _this.listOrSelectionEmitter.fire(undefined);
            _this.registerListener(channel);
        }));
        this.toDispose.push(this.onChannelDelete(function (event) {
            _this.listOrSelectionEmitter.fire(undefined);
            if (_this.selectedChannel && _this.selectedChannel.name === event.channelName) {
                _this.selectedChannel = _this.getVisibleChannels()[0];
            }
        }));
    };
    OutputChannelManager.prototype.registerListener = function (outputChannel) {
        var _this = this;
        if (!this.selectedChannel) {
            this.selectedChannel = outputChannel;
        }
        this.toDispose.push(outputChannel.onVisibilityChange(function (event) {
            if (event.visible) {
                _this.selectedChannel = outputChannel;
            }
            else if (outputChannel === _this.selectedChannel) {
                _this.selectedChannel = _this.getVisibleChannels()[0];
            }
        }));
    };
    OutputChannelManager.prototype.getChannel = function (name) {
        var existing = this.channels.get(name);
        if (existing) {
            return existing;
        }
        var channel = new OutputChannel(name, this.preferences);
        this.channels.set(name, channel);
        this.channelAddedEmitter.fire(channel);
        return channel;
    };
    OutputChannelManager.prototype.deleteChannel = function (name) {
        this.channels.delete(name);
        this.channelDeleteEmitter.fire({ channelName: name });
    };
    OutputChannelManager.prototype.getChannels = function () {
        return Array.from(this.channels.values());
    };
    OutputChannelManager.prototype.getVisibleChannels = function () {
        return this.getChannels().filter(function (channel) { return channel.isVisible; });
    };
    OutputChannelManager.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    Object.defineProperty(OutputChannelManager.prototype, "selectedChannel", {
        get: function () {
            return this.selectedChannelValue;
        },
        set: function (channel) {
            this.selectedChannelValue = channel;
            this.selectedChannelEmitter.fire(undefined);
            this.listOrSelectionEmitter.fire(undefined);
        },
        enumerable: true,
        configurable: true
    });
    __decorate([
        inversify_1.inject(output_preferences_1.OutputPreferences),
        __metadata("design:type", Object)
    ], OutputChannelManager.prototype, "preferences", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], OutputChannelManager.prototype, "init", null);
    OutputChannelManager = __decorate([
        inversify_1.injectable()
    ], OutputChannelManager);
    return OutputChannelManager;
}());
exports.OutputChannelManager = OutputChannelManager;
var OutputChannel = /** @class */ (function () {
    function OutputChannel(name, preferences) {
        this.name = name;
        this.preferences = preferences;
        this.visibilityChangeEmitter = new core_1.Emitter();
        this.contentChangeEmitter = new core_1.Emitter();
        this.lines = [];
        this.visible = true;
        this.onVisibilityChange = this.visibilityChangeEmitter.event;
        this.onContentChange = this.contentChangeEmitter.event;
    }
    OutputChannel.prototype.append = function (value) {
        if (this.currentLine === undefined) {
            this.currentLine = value;
        }
        else {
            this.currentLine += value;
        }
        this.contentChangeEmitter.fire(this);
    };
    OutputChannel.prototype.appendLine = function (line) {
        if (this.currentLine !== undefined) {
            this.lines.push(this.currentLine + line);
            this.currentLine = undefined;
        }
        else {
            this.lines.push(line);
        }
        var maxChannelHistory = this.preferences['output.maxChannelHistory'];
        if (this.lines.length > maxChannelHistory) {
            this.lines.splice(0, this.lines.length - maxChannelHistory);
        }
        this.contentChangeEmitter.fire(this);
    };
    OutputChannel.prototype.clear = function () {
        this.lines.length = 0;
        this.currentLine = undefined;
        this.contentChangeEmitter.fire(this);
    };
    OutputChannel.prototype.setVisibility = function (visible) {
        this.visible = visible;
        this.visibilityChangeEmitter.fire({ visible: visible });
    };
    OutputChannel.prototype.getLines = function () {
        if (this.currentLine !== undefined) {
            return __spread(this.lines, [this.currentLine]);
        }
        else {
            return this.lines;
        }
    };
    Object.defineProperty(OutputChannel.prototype, "isVisible", {
        get: function () {
            return this.visible;
        },
        enumerable: true,
        configurable: true
    });
    return OutputChannel;
}());
exports.OutputChannel = OutputChannel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/common/output-channel'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/output/lib/common/output-preferences.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/common/output-preferences.js ***!
  \********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 TypeFox and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the Eclipse
 * Public License v. 2.0 are satisfied: GNU General Public License, version 2
 * with the GNU Classpath Exception which is available at
 * https://www.gnu.org/software/classpath/license.html.
 *
 * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
 ********************************************************************************/
Object.defineProperty(exports, "__esModule", { value: true });
var preferences_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences */ "../../packages/core/lib/browser/preferences/index.js");
exports.OutputConfigSchema = {
    'type': 'object',
    'properties': {
        'output.maxChannelHistory': {
            'type': 'number',
            'description': 'The maximum number of entries in an output channel.',
            'default': 1000
        }
    }
};
exports.OutputPreferences = Symbol('OutputPreferences');
function createOutputPreferences(preferences) {
    return preferences_1.createPreferenceProxy(preferences, exports.OutputConfigSchema);
}
exports.createOutputPreferences = createOutputPreferences;
function bindOutputPreferences(bind) {
    bind(exports.OutputPreferences).toDynamicValue(function (ctx) {
        var preferences = ctx.container.get(preferences_1.PreferenceService);
        return createOutputPreferences(preferences);
    });
    bind(preferences_1.PreferenceContribution).toConstantValue({ schema: exports.OutputConfigSchema });
}
exports.bindOutputPreferences = bindOutputPreferences;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/common/output-preferences'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/output/src/browser/style/output.css":
/*!****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/src/browser/style/output.css ***!
  \****************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./output.css */ "../../node_modules/css-loader/index.js!../../packages/output/src/browser/style/output.css");

if(typeof content === 'string') content = [[module.i, content, '']];

var transform;
var insertInto;



var options = {"hmr":true}

options.transform = transform
options.insertInto = undefined;

var update = __webpack_require__(/*! ../../../../../node_modules/style-loader/lib/addStyles.js */ "../../node_modules/style-loader/lib/addStyles.js")(content, options);

if(content.locals) module.exports = content.locals;

if(false) {}

/***/ })

}]);
//# sourceMappingURL=12.bundle.js.map