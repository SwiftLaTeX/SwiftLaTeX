(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[39],{

/***/ "../../packages/output/lib/browser/output-frontend-module.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/browser/output-frontend-module.js ***!
  \*************************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var output_widget_1 = __webpack_require__(/*! ./output-widget */ "../../packages/output/lib/browser/output-widget.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var output_contribution_1 = __webpack_require__(/*! ./output-contribution */ "../../packages/output/lib/browser/output-contribution.js");
var output_toolbar_contribution_1 = __webpack_require__(/*! ./output-toolbar-contribution */ "../../packages/output/lib/browser/output-toolbar-contribution.js");
var output_channel_1 = __webpack_require__(/*! ../common/output-channel */ "../../packages/output/lib/common/output-channel.js");
var output_preferences_1 = __webpack_require__(/*! ../common/output-preferences */ "../../packages/output/lib/common/output-preferences.js");
var tab_bar_toolbar_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-toolbar */ "../../packages/core/lib/browser/shell/tab-bar-toolbar.js");
exports.default = new inversify_1.ContainerModule(function (bind, unbind, isBound, rebind) {
    output_preferences_1.bindOutputPreferences(bind);
    bind(output_widget_1.OutputWidget).toSelf();
    bind(output_channel_1.OutputChannelManager).toSelf().inSingletonScope();
    bind(browser_1.WidgetFactory).toDynamicValue(function (context) { return ({
        id: output_widget_1.OUTPUT_WIDGET_KIND,
        createWidget: function () { return context.container.get(output_widget_1.OutputWidget); }
    }); });
    browser_1.bindViewContribution(bind, output_contribution_1.OutputContribution);
    bind(output_toolbar_contribution_1.OutputToolbarContribution).toSelf().inSingletonScope();
    bind(tab_bar_toolbar_1.TabBarToolbarContribution).toService(output_toolbar_contribution_1.OutputToolbarContribution);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/browser/output-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/output/lib/browser/output-toolbar-contribution.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/output/lib/browser/output-toolbar-contribution.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Arm and others.
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
var output_widget_1 = __webpack_require__(/*! ./output-widget */ "../../packages/output/lib/browser/output-widget.js");
var output_channel_1 = __webpack_require__(/*! ../common/output-channel */ "../../packages/output/lib/common/output-channel.js");
var output_contribution_1 = __webpack_require__(/*! ./output-contribution */ "../../packages/output/lib/browser/output-contribution.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var OutputToolbarContribution = /** @class */ (function () {
    function OutputToolbarContribution() {
        var _this = this;
        this.NONE = '<no channels>';
        this.changeChannel = function (event) {
            var channelName = event.target.value;
            if (channelName !== _this.NONE) {
                _this.outputChannelManager.selectedChannel = _this.outputChannelManager.getChannel(channelName);
            }
        };
    }
    OutputToolbarContribution.prototype.registerToolbarItems = function (toolbarRegistry) {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                toolbarRegistry.registerItem({
                    id: 'channels',
                    render: function () { return _this.renderChannelSelector(); },
                    isVisible: function (widget) { return (widget instanceof output_widget_1.OutputWidget); },
                    onDidChange: this.outputChannelManager.onListOrSelectionChange
                });
                toolbarRegistry.registerItem({
                    id: output_contribution_1.OutputCommands.CLEAR_OUTPUT_TOOLBAR.id,
                    command: output_contribution_1.OutputCommands.CLEAR_OUTPUT_TOOLBAR.id,
                    tooltip: 'Clear Output',
                    priority: 1,
                });
                return [2 /*return*/];
            });
        });
    };
    OutputToolbarContribution.prototype.renderChannelSelector = function () {
        var channelOptionElements = [];
        this.outputChannelManager.getVisibleChannels().forEach(function (channel) {
            channelOptionElements.push(React.createElement("option", { value: channel.name, key: channel.name }, channel.name));
        });
        if (channelOptionElements.length === 0) {
            channelOptionElements.push(React.createElement("option", { key: this.NONE, value: this.NONE }, this.NONE));
        }
        return React.createElement("select", { className: 'theia-select', id: output_widget_1.OutputWidget.IDs.CHANNEL_LIST, key: output_widget_1.OutputWidget.IDs.CHANNEL_LIST, value: this.outputChannelManager.selectedChannel ? this.outputChannelManager.selectedChannel.name : this.NONE, onChange: this.changeChannel }, channelOptionElements);
    };
    __decorate([
        inversify_1.inject(output_channel_1.OutputChannelManager),
        __metadata("design:type", output_channel_1.OutputChannelManager)
    ], OutputToolbarContribution.prototype, "outputChannelManager", void 0);
    OutputToolbarContribution = __decorate([
        inversify_1.injectable()
    ], OutputToolbarContribution);
    return OutputToolbarContribution;
}());
exports.OutputToolbarContribution = OutputToolbarContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/output/lib/browser/output-toolbar-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=39.bundle.js.map