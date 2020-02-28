(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[34],{

/***/ "../../packages/core/lib/browser/browser-clipboard-service.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/browser-clipboard-service.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 RedHat and others.
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
var browser_1 = __webpack_require__(/*! ./browser */ "../../packages/core/lib/browser/browser.js");
var logger_1 = __webpack_require__(/*! ../common/logger */ "../../packages/core/lib/common/logger.js");
var message_service_1 = __webpack_require__(/*! ../common/message-service */ "../../packages/core/lib/common/message-service.js");
var BrowserClipboardService = /** @class */ (function () {
    function BrowserClipboardService() {
    }
    BrowserClipboardService.prototype.readText = function () {
        return __awaiter(this, void 0, void 0, function () {
            var permission, e1_1, e2_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 7]);
                        return [4 /*yield*/, this.queryPermission('clipboard-read')];
                    case 1:
                        permission = _a.sent();
                        return [3 /*break*/, 7];
                    case 2:
                        e1_1 = _a.sent();
                        this.logger.error('Failed checking a clipboard-read permission.', e1_1);
                        _a.label = 3;
                    case 3:
                        _a.trys.push([3, 5, , 6]);
                        return [4 /*yield*/, this.getClipboardAPI().readText()];
                    case 4: return [2 /*return*/, _a.sent()];
                    case 5:
                        e2_1 = _a.sent();
                        this.logger.error('Failed reading clipboard content.', e2_1);
                        if (browser_1.isFirefox) {
                            this.messageService.warn("Clipboard API is not available.\n                    It can be enabled by 'dom.events.testing.asyncClipboard' preference on 'about:config' page. Then reload Theia.\n                    Note, it will allow FireFox getting full access to the system clipboard.");
                        }
                        return [2 /*return*/, ''];
                    case 6: return [3 /*break*/, 7];
                    case 7:
                        if (permission.state === 'denied') {
                            // most likely, the user intentionally denied the access
                            this.messageService.warn("Access to the clipboard is denied. Check your browser's permission.");
                            return [2 /*return*/, ''];
                        }
                        return [2 /*return*/, this.getClipboardAPI().readText()];
                }
            });
        });
    };
    BrowserClipboardService.prototype.writeText = function (value) {
        return __awaiter(this, void 0, void 0, function () {
            var permission, e1_2, e2_2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 7]);
                        return [4 /*yield*/, this.queryPermission('clipboard-write')];
                    case 1:
                        permission = _a.sent();
                        return [3 /*break*/, 7];
                    case 2:
                        e1_2 = _a.sent();
                        this.logger.error('Failed checking a clipboard-write permission.', e1_2);
                        _a.label = 3;
                    case 3:
                        _a.trys.push([3, 5, , 6]);
                        return [4 /*yield*/, this.getClipboardAPI().writeText(value)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                    case 5:
                        e2_2 = _a.sent();
                        this.logger.error('Failed writing to the clipboard.', e2_2);
                        if (browser_1.isFirefox) {
                            this.messageService.warn("Clipboard API is not available.\n                    It can be enabled by 'dom.events.testing.asyncClipboard' preference on 'about:config' page. Then reload Theia.\n                    Note, it will allow FireFox getting full access to the system clipboard.");
                        }
                        return [2 /*return*/];
                    case 6: return [3 /*break*/, 7];
                    case 7:
                        if (permission.state === 'denied') {
                            // most likely, the user intentionally denied the access
                            this.messageService.warn("Access to the clipboard is denied. Check your browser's permission.");
                            return [2 /*return*/];
                        }
                        return [2 /*return*/, this.getClipboardAPI().writeText(value)];
                }
            });
        });
    };
    BrowserClipboardService.prototype.queryPermission = function (name) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                if ('permissions' in navigator) {
                    return [2 /*return*/, navigator['permissions'].query({ name: name })];
                }
                throw new Error('Permissions API unavailable');
            });
        });
    };
    BrowserClipboardService.prototype.getClipboardAPI = function () {
        if ('clipboard' in navigator) {
            return navigator['clipboard'];
        }
        throw new Error('Async Clipboard API unavailable');
    };
    __decorate([
        inversify_1.inject(message_service_1.MessageService),
        __metadata("design:type", message_service_1.MessageService)
    ], BrowserClipboardService.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(logger_1.ILogger),
        __metadata("design:type", Object)
    ], BrowserClipboardService.prototype, "logger", void 0);
    BrowserClipboardService = __decorate([
        inversify_1.injectable()
    ], BrowserClipboardService);
    return BrowserClipboardService;
}());
exports.BrowserClipboardService = BrowserClipboardService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/browser-clipboard-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/core/lib/browser/clipboard-service.js":
/*!******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/clipboard-service.js ***!
  \******************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 RedHat and others.
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
exports.ClipboardService = Symbol('ClipboardService');

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/clipboard-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/core/lib/browser/window/browser-window-module.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/window/browser-window-module.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 TypeFox and others.
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
var window_service_1 = __webpack_require__(/*! ../../browser/window/window-service */ "../../packages/core/lib/browser/window/window-service.js");
var default_window_service_1 = __webpack_require__(/*! ../../browser/window/default-window-service */ "../../packages/core/lib/browser/window/default-window-service.js");
var frontend_application_1 = __webpack_require__(/*! ../frontend-application */ "../../packages/core/lib/browser/frontend-application.js");
var clipboard_service_1 = __webpack_require__(/*! ../clipboard-service */ "../../packages/core/lib/browser/clipboard-service.js");
var browser_clipboard_service_1 = __webpack_require__(/*! ../browser-clipboard-service */ "../../packages/core/lib/browser/browser-clipboard-service.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    bind(default_window_service_1.DefaultWindowService).toSelf().inSingletonScope();
    bind(window_service_1.WindowService).toService(default_window_service_1.DefaultWindowService);
    bind(frontend_application_1.FrontendApplicationContribution).toService(default_window_service_1.DefaultWindowService);
    bind(clipboard_service_1.ClipboardService).to(browser_clipboard_service_1.BrowserClipboardService).inSingletonScope();
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/window/browser-window-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/core/lib/browser/window/default-window-service.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/window/default-window-service.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 TypeFox and others.
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
var core_preferences_1 = __webpack_require__(/*! ../core-preferences */ "../../packages/core/lib/browser/core-preferences.js");
var contribution_provider_1 = __webpack_require__(/*! ../../common/contribution-provider */ "../../packages/core/lib/common/contribution-provider.js");
var frontend_application_1 = __webpack_require__(/*! ../frontend-application */ "../../packages/core/lib/browser/frontend-application.js");
var DefaultWindowService = /** @class */ (function () {
    function DefaultWindowService() {
    }
    DefaultWindowService.prototype.onStart = function (app) {
        var _this = this;
        this.frontendApplication = app;
        window.addEventListener('beforeunload', function (event) {
            if (!_this.canUnload()) {
                return _this.preventUnload(event);
            }
        });
    };
    DefaultWindowService.prototype.openNewWindow = function (url) {
        window.open(url, undefined, 'noopener');
        return undefined;
    };
    DefaultWindowService.prototype.canUnload = function () {
        var e_1, _a;
        var confirmExit = this.corePreferences['application.confirmExit'];
        if (confirmExit === 'never') {
            return true;
        }
        try {
            for (var _b = __values(this.contributions.getContributions()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var contribution = _c.value;
                if (contribution.onWillStop) {
                    if (!!contribution.onWillStop(this.frontendApplication)) {
                        return false;
                    }
                }
            }
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_1) throw e_1.error; }
        }
        return confirmExit !== 'always';
    };
    /**
     * Ask the user to confirm if they want to unload the window. Prevent it if they do not.
     * @param event The beforeunload event
     */
    DefaultWindowService.prototype.preventUnload = function (event) {
        event.returnValue = '';
        event.preventDefault();
        return '';
    };
    __decorate([
        inversify_1.inject(core_preferences_1.CorePreferences),
        __metadata("design:type", Object)
    ], DefaultWindowService.prototype, "corePreferences", void 0);
    __decorate([
        inversify_1.inject(contribution_provider_1.ContributionProvider),
        inversify_1.named(frontend_application_1.FrontendApplicationContribution),
        __metadata("design:type", Object)
    ], DefaultWindowService.prototype, "contributions", void 0);
    DefaultWindowService = __decorate([
        inversify_1.injectable()
    ], DefaultWindowService);
    return DefaultWindowService;
}());
exports.DefaultWindowService = DefaultWindowService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/window/default-window-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=34.bundle.js.map