(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[31],{

/***/ "../../node_modules/css-loader/index.js!../../packages/mini-browser/src/browser/style/index.css":
/*!*********************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/mini-browser/src/browser/style/index.css ***!
  \*********************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

var escape = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/url/escape.js */ "../../node_modules/css-loader/lib/url/escape.js");
exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2018 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n:root {\n    --theia-private-mini-browser-height: var(--theia-content-line-height);\n}\n\n.theia-mini-browser {\n    display: flex;\n    flex-direction: column;\n    height: 100%;\n}\n\n.theia-mini-browser-icon {\n    mask: url(" + escape(__webpack_require__(/*! ./mini-browser.svg */ "../../packages/mini-browser/src/browser/style/mini-browser.svg")) + ");\n    -webkit-mask: url(" + escape(__webpack_require__(/*! ./mini-browser.svg */ "../../packages/mini-browser/src/browser/style/mini-browser.svg")) + ");\n}\n\n.theia-mini-browser-toolbar {\n    margin-top: 8px;\n    display: flex;\n    align-items: center;\n    justify-content: space-evenly;\n    padding: 0 10px;\n}\n\n.theia-mini-browser-toolbar-read-only {\n    margin-top: 8px;\n    display: flex;\n    align-items: center;\n    justify-content: space-evenly;\n    padding: 0 10px;\n}\n\n.theia-mini-browser-toolbar .theia-input {\n    width: 100%;\n    line-height: var(--theia-private-mini-browser-height);\n    margin-left: 4px;\n    margin-right: 4px;\n}\n\n.theia-mini-browser-toolbar-read-only .theia-input {\n    width: 100%;\n    line-height: var(--theia-private-mini-browser-height);\n    margin-left: 4px;\n    margin-right: 4px;\n    cursor: pointer;\n    background: var(--theia-input-background);\n    border: none;\n    text-decoration: underline;\n    outline: none;\n    white-space: nowrap;\n    overflow: hidden;\n    text-overflow: ellipsis;\n    color: var(--theia-input-foreground);\n}\n\n.theia-mini-browser-toolbar-read-only .theia-input:hover {\n    color: var(--theia-button-hoverBackground);\n}\n\n.theia-mini-browser-button {\n    min-width: 1rem;\n    text-align: center;\n    flex-grow: 0;\n    font-family: FontAwesome;\n    font-size: calc(var(--theia-content-font-size) * 0.8);\n    color: var(--theia-icon-foreground);\n    margin: 0px 4px 0px 4px;\n}\n\n.theia-mini-browser-button:not(.theia-mini-browser-button-disabled):hover {\n    cursor: pointer;\n}\n\n.theia-mini-browser-button-disabled {\n    opacity: var(--theia-mod-disabled-opacity);\n}\n\n.theia-mini-browser-previous::before {\n    content: \"\\F053\";\n}\n\n.theia-mini-browser-next::before {\n    content: \"\\F054\";\n}\n\n.theia-mini-browser-refresh::before {\n    content: \"\\F021\";\n}\n\n.theia-mini-browser-open::before {\n    content: \"\\F08E\";\n}\n\n.theia-mini-browser-content-area {\n    position: relative;\n    display: flex;\n    height: 100%;\n    width: 100%;\n    flex-direction: column;\n    overflow: hidden;\n    margin-top: 6px;\n}\n\n.theia-mini-browser-pdf-container {\n    width: 100%;\n    height: 100%;\n}\n\n.theia-mini-browser-load-indicator {\n    position: absolute;\n    top: 0;\n    right: 0;\n    bottom: 0;\n    left: 0;\n    z-index: 10;\n    background: var(--theia-editor-background);\n    background-image: var(--theia-preloader);\n    background-size: 60px 60px;\n    background-repeat: no-repeat;\n    background-position: center;\n    transition: opacity 0.8s;\n}\n\n.theia-mini-browser-load-indicator.theia-fade-out {\n    opacity: 0;\n}\n\n.theia-mini-browser-error-bar {\n    height: 19px;\n    padding-left: var(--theia-ui-padding);\n    background-color: var(--theia-inputValidation-errorBorder);\n    color: var(--theia-editor-foreground);\n    font-size: var(--theia-statusBar-font-size);\n    z-index: 1000; /* Above the transparent overlay (`z-index: 999;`). */\n}\n\n.theia-mini-browser-error-bar span {\n    margin-top: 3px;\n    margin-left: var(--theia-ui-padding);\n}\n\n.theia-mini-browser-content-area iframe {\n    flex-grow: 1;\n    border: none; margin: 0; padding: 0;\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/mini-browser/lib/browser/location-mapper-service.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/location-mapper-service.js ***!
  \********************************************************************************************************/
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
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var types_1 = __webpack_require__(/*! @theia/core/lib/common/types */ "../../packages/core/lib/common/types.js");
var contribution_provider_1 = __webpack_require__(/*! @theia/core/lib/common/contribution-provider */ "../../packages/core/lib/common/contribution-provider.js");
var s3storagesystem_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
/**
 * Contribution for the `LocationMapperService`.
 */
exports.LocationMapper = Symbol('LocationMapper');
/**
 * Location mapper service.
 */
var LocationMapperService = /** @class */ (function () {
    function LocationMapperService() {
    }
    LocationMapperService.prototype.map = function (location) {
        return __awaiter(this, void 0, void 0, function () {
            var contributions;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.prioritize(location)];
                    case 1:
                        contributions = _a.sent();
                        if (contributions.length === 0) {
                            return [2 /*return*/, this.defaultMapper()(location)];
                        }
                        return [2 /*return*/, contributions[0].map(location)];
                }
            });
        });
    };
    LocationMapperService.prototype.defaultMapper = function () {
        return function (location) { return new browser_1.Endpoint().httpScheme + "//" + location; };
    };
    LocationMapperService.prototype.prioritize = function (location) {
        return __awaiter(this, void 0, void 0, function () {
            var prioritized;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, types_1.Prioritizeable.prioritizeAll(this.getContributions(), function (contribution) { return contribution.canHandle(location); })];
                    case 1:
                        prioritized = _a.sent();
                        return [2 /*return*/, prioritized.map(function (p) { return p.value; })];
                }
            });
        });
    };
    LocationMapperService.prototype.getContributions = function () {
        return this.contributions.getContributions();
    };
    __decorate([
        inversify_1.inject(contribution_provider_1.ContributionProvider),
        inversify_1.named(exports.LocationMapper),
        __metadata("design:type", Object)
    ], LocationMapperService.prototype, "contributions", void 0);
    LocationMapperService = __decorate([
        inversify_1.injectable()
    ], LocationMapperService);
    return LocationMapperService;
}());
exports.LocationMapperService = LocationMapperService;
/**
 * HTTP location mapper.
 */
var HttpLocationMapper = /** @class */ (function () {
    function HttpLocationMapper() {
    }
    HttpLocationMapper.prototype.canHandle = function (location) {
        return location.startsWith('http://') ? 1 : 0;
    };
    HttpLocationMapper.prototype.map = function (location) {
        return location;
    };
    HttpLocationMapper = __decorate([
        inversify_1.injectable()
    ], HttpLocationMapper);
    return HttpLocationMapper;
}());
exports.HttpLocationMapper = HttpLocationMapper;
// /**
//  * HTTPS location mapper.
//  */
var HttpsLocationMapper = /** @class */ (function () {
    function HttpsLocationMapper() {
    }
    HttpsLocationMapper.prototype.canHandle = function (location) {
        return location.startsWith('https://') ? 1 : 0;
    };
    HttpsLocationMapper.prototype.map = function (location) {
        return location;
    };
    HttpsLocationMapper = __decorate([
        inversify_1.injectable()
    ], HttpsLocationMapper);
    return HttpsLocationMapper;
}());
exports.HttpsLocationMapper = HttpsLocationMapper;
/**
 * Location mapper for locations without a scheme.
 */
var LocationWithoutSchemeMapper = /** @class */ (function () {
    function LocationWithoutSchemeMapper() {
    }
    LocationWithoutSchemeMapper.prototype.canHandle = function (location) {
        return new uri_1.default(location).scheme === '' ? 1 : 0;
    };
    LocationWithoutSchemeMapper.prototype.map = function (location) {
        return "http://" + location;
    };
    LocationWithoutSchemeMapper = __decorate([
        inversify_1.injectable()
    ], LocationWithoutSchemeMapper);
    return LocationWithoutSchemeMapper;
}());
exports.LocationWithoutSchemeMapper = LocationWithoutSchemeMapper;
/**
 * `file` URI location mapper.
 */
var FileLocationMapper = /** @class */ (function () {
    function FileLocationMapper() {
    }
    FileLocationMapper.prototype.canHandle = function (location) {
        return location.startsWith('file://') ? 1 : 0;
    };
    FileLocationMapper.prototype.map = function (location) {
        var _this = this;
        var uri = new uri_1.default(location);
        if (uri.scheme !== 'file') {
            throw new Error("Only URIs with 'file' scheme can be mapped to an URL. URI was: " + uri + ".");
        }
        var dstPath = location.substr(7);
        return new Promise(function (resolve, reject) {
            _this._s3fs._resolvePath(dstPath).then(function (node) {
                if (!node) {
                    reject(new Error("Failed to resolve. URI was: " + uri + "."));
                }
                var contentType = _this.guessContentType(location);
                resolve(_this._s3fs.opts.endpoint + "/" + _this._s3fs.opts.bucket + "/" + _this._s3fs.opts.prefix + node.node_id + "?response-content-type=" + contentType);
            });
        });
        // let rawLocation = uri.path.toString();
        // if (rawLocation.charAt(0) === '/') {
        //     rawLocation = rawLocation.substr(1);
        // }
        // return new MiniBrowserEndpoint().getRestUrl().resolve(rawLocation).toString();
    };
    FileLocationMapper.prototype.guessContentType = function (path) {
        if (path.endsWith('.jpg') || path.endsWith('.jpeg')) {
            return 'image/jpeg';
        }
        if (path.endsWith('.bmp')) {
            return 'image/bmp';
        }
        if (path.endsWith('.png')) {
            return 'image/png';
        }
        if (path.endsWith('.gif')) {
            return 'image/gif';
        }
        if (path.endsWith('.svg')) {
            return 'image/svg+xml';
        }
        if (path.endsWith('.pdf')) {
            return 'application/pdf';
        }
        if (path.endsWith('.html')) {
            return 'text/html';
        }
        return 'application/octet-stream';
    };
    __decorate([
        inversify_1.inject(s3storagesystem_1.S3StorageSystem),
        __metadata("design:type", s3storagesystem_1.S3StorageSystem)
    ], FileLocationMapper.prototype, "_s3fs", void 0);
    FileLocationMapper = __decorate([
        inversify_1.injectable()
    ], FileLocationMapper);
    return FileLocationMapper;
}());
exports.FileLocationMapper = FileLocationMapper;
var MiniBrowserEndpoint = /** @class */ (function (_super) {
    __extends(MiniBrowserEndpoint, _super);
    function MiniBrowserEndpoint() {
        return _super.call(this, { path: 'mini-browser' }) || this;
    }
    return MiniBrowserEndpoint;
}(browser_1.Endpoint));
exports.MiniBrowserEndpoint = MiniBrowserEndpoint;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/location-mapper-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser-content-style.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser-content-style.js ***!
  \***********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
Object.defineProperty(exports, "__esModule", { value: true });
/********************************************************************************
 * Copyright (C) 2019 TypeFox and others.
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
var MiniBrowserContentStyle;
(function (MiniBrowserContentStyle) {
    MiniBrowserContentStyle.MINI_BROWSER = 'theia-mini-browser';
    MiniBrowserContentStyle.TOOLBAR = 'theia-mini-browser-toolbar';
    MiniBrowserContentStyle.TOOLBAR_READ_ONLY = 'theia-mini-browser-toolbar-read-only';
    MiniBrowserContentStyle.PRE_LOAD = 'theia-mini-browser-load-indicator';
    MiniBrowserContentStyle.FADE_OUT = 'theia-fade-out';
    MiniBrowserContentStyle.CONTENT_AREA = 'theia-mini-browser-content-area';
    MiniBrowserContentStyle.PDF_CONTAINER = 'theia-mini-browser-pdf-container';
    MiniBrowserContentStyle.PREVIOUS = 'theia-mini-browser-previous';
    MiniBrowserContentStyle.NEXT = 'theia-mini-browser-next';
    MiniBrowserContentStyle.REFRESH = 'theia-mini-browser-refresh';
    MiniBrowserContentStyle.OPEN = 'theia-mini-browser-open';
    MiniBrowserContentStyle.BUTTON = 'theia-mini-browser-button';
    MiniBrowserContentStyle.DISABLED = 'theia-mini-browser-button-disabled';
    MiniBrowserContentStyle.TRANSPARENT_OVERLAY = 'theia-transparent-overlay';
    MiniBrowserContentStyle.ERROR_BAR = 'theia-mini-browser-error-bar';
})(MiniBrowserContentStyle = exports.MiniBrowserContentStyle || (exports.MiniBrowserContentStyle = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser-content-style'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser-content.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser-content.js ***!
  \*****************************************************************************************************/
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
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
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
var PDFObject = __webpack_require__(/*! pdfobject */ "../../node_modules/pdfobject/pdfobject.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var logger_1 = __webpack_require__(/*! @theia/core/lib/common/logger */ "../../packages/core/lib/common/logger.js");
var event_1 = __webpack_require__(/*! @theia/core/lib/common/event */ "../../packages/core/lib/common/event.js");
var filesystem_1 = __webpack_require__(/*! @theia/filesystem/lib/common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var keybinding_1 = __webpack_require__(/*! @theia/core/lib/browser/keybinding */ "../../packages/core/lib/browser/keybinding.js");
var window_service_1 = __webpack_require__(/*! @theia/core/lib/browser/window/window-service */ "../../packages/core/lib/browser/window/window-service.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var filesystem_watcher_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var disposable_1 = __webpack_require__(/*! @theia/core/lib/common/disposable */ "../../packages/core/lib/common/disposable.js");
var widget_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/widget */ "../../packages/core/lib/browser/widgets/widget.js");
var location_mapper_service_1 = __webpack_require__(/*! ./location-mapper-service */ "../../packages/mini-browser/lib/browser/location-mapper-service.js");
var application_shell_mouse_tracker_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/application-shell-mouse-tracker */ "../../packages/core/lib/browser/shell/application-shell-mouse-tracker.js");
var debounce = __webpack_require__(/*! lodash.debounce */ "../../node_modules/lodash.debounce/index.js");
var mini_browser_content_style_1 = __webpack_require__(/*! ./mini-browser-content-style */ "../../packages/mini-browser/lib/browser/mini-browser-content-style.js");
/**
 * Initializer properties for the embedded browser widget.
 */
var MiniBrowserProps = /** @class */ (function () {
    function MiniBrowserProps() {
    }
    MiniBrowserProps = __decorate([
        inversify_1.injectable()
    ], MiniBrowserProps);
    return MiniBrowserProps;
}());
exports.MiniBrowserProps = MiniBrowserProps;
(function (MiniBrowserProps) {
    /**
     * Enumeration of the supported `sandbox` options for the `iframe`.
     */
    var SandboxOptions;
    (function (SandboxOptions) {
        /**
         * Allows form submissions.
         */
        SandboxOptions[SandboxOptions["allow-forms"] = 0] = "allow-forms";
        /**
         * Allows popups, such as `window.open()`, `showModalDialog()`, `target=”_blank”`, etc.
         */
        SandboxOptions[SandboxOptions["allow-popups"] = 1] = "allow-popups";
        /**
         * Allows pointer lock.
         */
        SandboxOptions[SandboxOptions["allow-pointer-lock"] = 2] = "allow-pointer-lock";
        /**
         * Allows the document to maintain its origin. Pages loaded from https://example.com/ will retain access to that origin’s data.
         */
        SandboxOptions[SandboxOptions["allow-same-origin"] = 3] = "allow-same-origin";
        /**
         * Allows JavaScript execution. Also allows features to trigger automatically (as they’d be trivial to implement via JavaScript).
         */
        SandboxOptions[SandboxOptions["allow-scripts"] = 4] = "allow-scripts";
        /**
         * Allows the document to break out of the frame by navigating the top-level `window`.
         */
        SandboxOptions[SandboxOptions["allow-top-navigation"] = 5] = "allow-top-navigation";
        /**
         * Allows the embedded browsing context to open modal windows.
         */
        SandboxOptions[SandboxOptions["allow-modals"] = 6] = "allow-modals";
        /**
         * Allows the embedded browsing context to disable the ability to lock the screen orientation.
         */
        SandboxOptions[SandboxOptions["allow-orientation-lock"] = 7] = "allow-orientation-lock";
        /**
         * Allows a sandboxed document to open new windows without forcing the sandboxing flags upon them.
         * This will allow, for example, a third-party advertisement to be safely sandboxed without forcing the same restrictions upon a landing page.
         */
        SandboxOptions[SandboxOptions["allow-popups-to-escape-sandbox"] = 8] = "allow-popups-to-escape-sandbox";
        /**
         * Allows embedders to have control over whether an iframe can start a presentation session.
         */
        SandboxOptions[SandboxOptions["allow-presentation"] = 9] = "allow-presentation";
        /**
         * Allows the embedded browsing context to navigate (load) content to the top-level browsing context only when initiated by a user gesture.
         * If this keyword is not used, this operation is not allowed.
         */
        SandboxOptions[SandboxOptions["allow-top-navigation-by-user-activation"] = 10] = "allow-top-navigation-by-user-activation";
    })(SandboxOptions = MiniBrowserProps.SandboxOptions || (MiniBrowserProps.SandboxOptions = {}));
    (function (SandboxOptions) {
        /**
         * The default `sandbox` options, if other is not provided.
         *
         * See: https://www.html5rocks.com/en/tutorials/security/sandboxed-iframes/
         */
        SandboxOptions.DEFAULT = [
            SandboxOptions['allow-same-origin'],
            SandboxOptions['allow-scripts'],
            SandboxOptions['allow-popups'],
            SandboxOptions['allow-forms'],
            SandboxOptions['allow-modals']
        ];
    })(SandboxOptions = MiniBrowserProps.SandboxOptions || (MiniBrowserProps.SandboxOptions = {}));
})(MiniBrowserProps = exports.MiniBrowserProps || (exports.MiniBrowserProps = {}));
exports.MiniBrowserProps = MiniBrowserProps;
exports.MiniBrowserContentFactory = Symbol('MiniBrowserContentFactory');
var MiniBrowserContent = /** @class */ (function (_super) {
    __extends(MiniBrowserContent, _super);
    function MiniBrowserContent(props) {
        var _this = _super.call(this) || this;
        _this.props = props;
        _this.submitInputEmitter = new event_1.Emitter();
        _this.navigateBackEmitter = new event_1.Emitter();
        _this.navigateForwardEmitter = new event_1.Emitter();
        _this.refreshEmitter = new event_1.Emitter();
        _this.openEmitter = new event_1.Emitter();
        _this.toDisposeOnGo = new disposable_1.DisposableCollection();
        _this.node.tabIndex = 0;
        _this.addClass(mini_browser_content_style_1.MiniBrowserContentStyle.MINI_BROWSER);
        _this.input = _this.createToolbar(_this.node).input;
        var contentArea = _this.createContentArea(_this.node);
        _this.frame = contentArea.frame;
        _this.transparentOverlay = contentArea.transparentOverlay;
        _this.loadIndicator = contentArea.loadIndicator;
        _this.errorBar = contentArea.errorBar;
        _this.pdfContainer = contentArea.pdfContainer;
        _this.initialHistoryLength = history.length;
        _this.toDispose.pushAll([
            _this.submitInputEmitter,
            _this.navigateBackEmitter,
            _this.navigateForwardEmitter,
            _this.refreshEmitter,
            _this.openEmitter
        ]);
        return _this;
    }
    MiniBrowserContent.prototype.init = function () {
        var _this = this;
        this.toDispose.push(this.mouseTracker.onMousedown(function (e) {
            if (_this.frame.style.display !== 'none') {
                _this.transparentOverlay.style.display = 'block';
            }
        }));
        this.toDispose.push(this.mouseTracker.onMouseup(function (e) {
            if (_this.frame.style.display !== 'none') {
                _this.transparentOverlay.style.display = 'none';
            }
        }));
        var startPage = this.props.startPage;
        if (startPage) {
            setTimeout(function () { return _this.go(startPage); }, 500);
            this.listenOnContentChange(startPage);
        }
    };
    MiniBrowserContent.prototype.onActivateRequest = function (msg) {
        _super.prototype.onActivateRequest.call(this, msg);
        if (this.getToolbarProps() !== 'hide') {
            this.input.focus();
        }
        else {
            this.node.focus();
        }
    };
    MiniBrowserContent.prototype.listenOnContentChange = function (location) {
        return __awaiter(this, void 0, void 0, function () {
            var fileUri_1, watcher, onFileChange;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!location.startsWith('file://')) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.fileSystem.exists(location)];
                    case 1:
                        if (!_a.sent()) return [3 /*break*/, 3];
                        fileUri_1 = new uri_1.default(location);
                        return [4 /*yield*/, this.fileSystemWatcher.watchFileChanges(fileUri_1)];
                    case 2:
                        watcher = _a.sent();
                        this.toDispose.push(watcher);
                        onFileChange = function (event) {
                            if (filesystem_watcher_1.FileChangeEvent.isChanged(event, fileUri_1)) {
                                _this.go(location, {
                                    showLoadIndicator: false
                                });
                            }
                        };
                        this.toDispose.push(this.fileSystemWatcher.onFilesChanged(debounce(onFileChange, 500)));
                        _a.label = 3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    MiniBrowserContent.prototype.createToolbar = function (parent) {
        var toolbar = document.createElement('div');
        toolbar.classList.add(this.getToolbarProps() === 'read-only' ? mini_browser_content_style_1.MiniBrowserContentStyle.TOOLBAR_READ_ONLY : mini_browser_content_style_1.MiniBrowserContentStyle.TOOLBAR);
        parent.appendChild(toolbar);
        this.createPrevious(toolbar);
        this.createNext(toolbar);
        this.createRefresh(toolbar);
        var input = this.createInput(toolbar);
        input.readOnly = this.getToolbarProps() === 'read-only';
        this.createOpen(toolbar);
        if (this.getToolbarProps() === 'hide') {
            toolbar.style.display = 'none';
        }
        return Object.assign(toolbar, { input: input });
    };
    MiniBrowserContent.prototype.getToolbarProps = function () {
        return !this.props.startPage ? 'show' : this.props.toolbar || 'show';
    };
    // eslint-disable-next-line max-len
    MiniBrowserContent.prototype.createContentArea = function (parent) {
        var _this = this;
        var contentArea = document.createElement('div');
        contentArea.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.CONTENT_AREA);
        var loadIndicator = document.createElement('div');
        loadIndicator.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.PRE_LOAD);
        loadIndicator.style.display = 'none';
        var errorBar = this.createErrorBar();
        var frame = this.createIFrame();
        this.submitInputEmitter.event(function (input) { return _this.go(input, {
            preserveFocus: false
        }); });
        this.navigateBackEmitter.event(this.handleBack.bind(this));
        this.navigateForwardEmitter.event(this.handleForward.bind(this));
        this.refreshEmitter.event(this.handleRefresh.bind(this));
        this.openEmitter.event(this.handleOpen.bind(this));
        var transparentOverlay = document.createElement('div');
        transparentOverlay.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.TRANSPARENT_OVERLAY);
        transparentOverlay.style.display = 'none';
        var pdfContainer = document.createElement('div');
        pdfContainer.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.PDF_CONTAINER);
        pdfContainer.id = this.id + "-pdf-container";
        pdfContainer.style.display = 'none';
        contentArea.appendChild(errorBar);
        contentArea.appendChild(transparentOverlay);
        contentArea.appendChild(pdfContainer);
        contentArea.appendChild(loadIndicator);
        contentArea.appendChild(frame);
        parent.appendChild(contentArea);
        return Object.assign(contentArea, { frame: frame, loadIndicator: loadIndicator, errorBar: errorBar, pdfContainer: pdfContainer, transparentOverlay: transparentOverlay });
    };
    MiniBrowserContent.prototype.createIFrame = function () {
        var _a;
        var frame = document.createElement('iframe');
        var sandbox = (this.props.sandbox || MiniBrowserProps.SandboxOptions.DEFAULT).map(function (name) { return MiniBrowserProps.SandboxOptions[name]; });
        (_a = frame.sandbox).add.apply(_a, __spread(sandbox));
        this.toDispose.push(widget_1.addEventListener(frame, 'load', this.onFrameLoad.bind(this)));
        this.toDispose.push(widget_1.addEventListener(frame, 'error', this.onFrameError.bind(this)));
        return frame;
    };
    MiniBrowserContent.prototype.createErrorBar = function () {
        var errorBar = document.createElement('div');
        errorBar.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.ERROR_BAR);
        errorBar.style.display = 'none';
        var icon = document.createElement('span');
        icon.classList.add('fa', 'problem-tab-icon');
        errorBar.appendChild(icon);
        var message = document.createElement('span');
        errorBar.appendChild(message);
        return Object.assign(errorBar, { message: message });
    };
    MiniBrowserContent.prototype.onFrameLoad = function () {
        clearTimeout(this.frameLoadTimeout);
        this.maybeResetBackground();
        this.hideLoadIndicator();
        this.hideErrorBar();
    };
    MiniBrowserContent.prototype.onFrameError = function () {
        clearTimeout(this.frameLoadTimeout);
        this.maybeResetBackground();
        this.hideLoadIndicator();
        this.showErrorBar('An error occurred while loading this page');
    };
    MiniBrowserContent.prototype.onFrameTimeout = function () {
        clearTimeout(this.frameLoadTimeout);
        this.maybeResetBackground();
        this.hideLoadIndicator();
        this.showErrorBar('Still loading...');
    };
    MiniBrowserContent.prototype.showLoadIndicator = function () {
        this.loadIndicator.classList.remove(mini_browser_content_style_1.MiniBrowserContentStyle.FADE_OUT);
        this.loadIndicator.style.display = 'block';
    };
    MiniBrowserContent.prototype.hideLoadIndicator = function () {
        var _this = this;
        // Start the fade-out transition.
        this.loadIndicator.classList.add(mini_browser_content_style_1.MiniBrowserContentStyle.FADE_OUT);
        // Actually hide the load indicator after the transition is finished.
        var preloadStyle = window.getComputedStyle(this.loadIndicator);
        var transitionDuration = browser_1.parseCssTime(preloadStyle.transitionDuration, 0);
        setTimeout(function () {
            // But don't hide it if it was shown again since the transition started.
            if (_this.loadIndicator.classList.contains(mini_browser_content_style_1.MiniBrowserContentStyle.FADE_OUT)) {
                _this.loadIndicator.style.display = 'none';
                _this.loadIndicator.classList.remove(mini_browser_content_style_1.MiniBrowserContentStyle.FADE_OUT);
            }
        }, transitionDuration);
    };
    MiniBrowserContent.prototype.showErrorBar = function (message) {
        this.errorBar.message.textContent = message;
        this.errorBar.style.display = 'block';
    };
    MiniBrowserContent.prototype.hideErrorBar = function () {
        this.errorBar.message.textContent = '';
        this.errorBar.style.display = 'none';
    };
    MiniBrowserContent.prototype.maybeResetBackground = function () {
        if (this.props.resetBackground === true) {
            this.frame.style.backgroundColor = 'white';
        }
    };
    MiniBrowserContent.prototype.handleBack = function () {
        if (history.length - this.initialHistoryLength > 0) {
            history.back();
        }
    };
    MiniBrowserContent.prototype.handleForward = function () {
        if (history.length > this.initialHistoryLength) {
            history.forward();
        }
    };
    MiniBrowserContent.prototype.handleRefresh = function () {
        // Initial pessimism; use the location of the input.
        var location = this.props.startPage;
        // Use the the location from the `input`.
        if (this.input && this.input.value) {
            location = this.input.value;
        }
        try {
            var contentDocument = this.frame.contentDocument;
            if (contentDocument && contentDocument.location) {
                location = contentDocument.location.href;
            }
        }
        catch (_a) {
            // Security exception due to CORS when trying to access the `location.href` of the content document.
        }
        if (location) {
            this.go(location, {
                preserveFocus: false
            });
        }
    };
    MiniBrowserContent.prototype.handleOpen = function () {
        var location = this.frameSrc() || this.input.value;
        if (location) {
            this.windowService.openNewWindow(location);
        }
    };
    MiniBrowserContent.prototype.createInput = function (parent) {
        var _this = this;
        var input = document.createElement('input');
        input.type = 'text';
        input.classList.add('theia-input');
        this.toDispose.pushAll([
            widget_1.addEventListener(input, 'keydown', this.handleInputChange.bind(this)),
            widget_1.addEventListener(input, 'click', function () {
                if (_this.getToolbarProps() === 'read-only') {
                    _this.handleOpen();
                }
                else {
                    if (input.value) {
                        input.select();
                    }
                }
            })
        ]);
        parent.appendChild(input);
        return input;
    };
    MiniBrowserContent.prototype.handleInputChange = function (e) {
        var _this = this;
        var key = browser_1.KeyCode.createKeyCode(e).key;
        if (key && browser_1.Key.ENTER.keyCode === key.keyCode && this.getToolbarProps() === 'show') {
            var srcElement = e.srcElement;
            if (srcElement instanceof HTMLInputElement) {
                this.mapLocation(srcElement.value).then(function (location) { return _this.submitInputEmitter.fire(location); });
            }
        }
    };
    MiniBrowserContent.prototype.createPrevious = function (parent) {
        return this.onClick(this.createButton(parent, 'Show The Previous Page', mini_browser_content_style_1.MiniBrowserContentStyle.PREVIOUS), this.navigateBackEmitter);
    };
    MiniBrowserContent.prototype.createNext = function (parent) {
        return this.onClick(this.createButton(parent, 'Show The Next Page', mini_browser_content_style_1.MiniBrowserContentStyle.NEXT), this.navigateForwardEmitter);
    };
    MiniBrowserContent.prototype.createRefresh = function (parent) {
        return this.onClick(this.createButton(parent, 'Reload This Page', mini_browser_content_style_1.MiniBrowserContentStyle.REFRESH), this.refreshEmitter);
    };
    MiniBrowserContent.prototype.createOpen = function (parent) {
        var button = this.onClick(this.createButton(parent, 'Open In A New Window', mini_browser_content_style_1.MiniBrowserContentStyle.OPEN), this.openEmitter);
        return button;
    };
    MiniBrowserContent.prototype.createButton = function (parent, title) {
        var _a;
        var className = [];
        for (var _i = 2; _i < arguments.length; _i++) {
            className[_i - 2] = arguments[_i];
        }
        var button = document.createElement('div');
        button.title = title;
        (_a = button.classList).add.apply(_a, __spread(className, [mini_browser_content_style_1.MiniBrowserContentStyle.BUTTON]));
        parent.appendChild(button);
        return button;
    };
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    MiniBrowserContent.prototype.onClick = function (element, emitter) {
        this.toDispose.push(widget_1.addEventListener(element, 'click', function () {
            if (!element.classList.contains(mini_browser_content_style_1.MiniBrowserContentStyle.DISABLED)) {
                emitter.fire(undefined);
            }
        }));
        return element;
    };
    MiniBrowserContent.prototype.mapLocation = function (location) {
        return this.locationMapper.map(location);
    };
    MiniBrowserContent.prototype.setInput = function (value) {
        if (this.input.value !== value) {
            this.input.value = value;
        }
    };
    MiniBrowserContent.prototype.frameSrc = function () {
        var src = this.frame.src;
        try {
            var contentWindow = this.frame.contentWindow;
            if (contentWindow) {
                src = contentWindow.location.href;
            }
        }
        catch (_a) {
            // CORS issue. Ignored.
        }
        if (src === 'about:blank') {
            src = '';
        }
        return src;
    };
    MiniBrowserContent.prototype.contentDocument = function () {
        try {
            var contentDocument = this.frame.contentDocument;
            // eslint-disable-next-line no-null/no-null
            if (contentDocument === null) {
                var contentWindow = this.frame.contentWindow;
                if (contentWindow) {
                    contentDocument = contentWindow.document;
                }
            }
            return contentDocument;
        }
        catch (_a) {
            // eslint-disable-next-line no-null/no-null
            return null;
        }
    };
    MiniBrowserContent.prototype.go = function (location, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _a, showLoadIndicator, preserveFocus, url, e_1;
            var _this = this;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = __assign({ showLoadIndicator: true, preserveFocus: true }, options), showLoadIndicator = _a.showLoadIndicator, preserveFocus = _a.preserveFocus;
                        if (!location) return [3 /*break*/, 4];
                        _b.label = 1;
                    case 1:
                        _b.trys.push([1, 3, , 4]);
                        this.toDisposeOnGo.dispose();
                        return [4 /*yield*/, this.mapLocation(location)];
                    case 2:
                        url = _b.sent();
                        this.setInput(url);
                        if (this.getToolbarProps() === 'read-only') {
                            this.input.title = "Open " + url + " In A New Window";
                        }
                        clearTimeout(this.frameLoadTimeout);
                        this.frameLoadTimeout = window.setTimeout(this.onFrameTimeout.bind(this), 4000);
                        if (showLoadIndicator) {
                            this.showLoadIndicator();
                        }
                        if (location.endsWith('.pdf') || url.endsWith('.pdf')) {
                            this.pdfContainer.style.display = 'block';
                            this.frame.style.display = 'none';
                            PDFObject.embed(url, this.pdfContainer, {
                                // eslint-disable-next-line max-len, @typescript-eslint/quotes
                                fallbackLink: "<p style=\"padding: 0px 15px 0px 15px\">Your browser does not support inline PDFs. Click on this <a href='[url]' target=\"_blank\">link</a> to open the PDF in a new tab.</p>"
                            });
                            clearTimeout(this.frameLoadTimeout);
                            this.hideLoadIndicator();
                            if (!preserveFocus) {
                                this.pdfContainer.focus();
                            }
                        }
                        else {
                            this.pdfContainer.style.display = 'none';
                            this.frame.style.display = 'block';
                            this.frame.src = url;
                            // The load indicator will hide itself if the content of the iframe was loaded.
                            if (!preserveFocus) {
                                this.frame.addEventListener('load', function () {
                                    var window = _this.frame.contentWindow;
                                    if (window) {
                                        window.focus();
                                    }
                                }, { once: true });
                            }
                        }
                        // Delegate all the `keypress` events from the `iframe` to the application.
                        this.toDisposeOnGo.push(widget_1.addEventListener(this.frame, 'load', function () {
                            try {
                                var contentDocument_1 = _this.frame.contentDocument;
                                if (contentDocument_1) {
                                    var keypressHandler_1 = function (e) { return _this.keybindings.run(e); };
                                    contentDocument_1.addEventListener('keypress', keypressHandler_1, true);
                                    _this.toDisposeOnDetach.push(disposable_1.Disposable.create(function () { return contentDocument_1.removeEventListener('keypress', keypressHandler_1); }));
                                }
                            }
                            catch (_a) {
                                // There is not much we could do with the security exceptions due to CORS.
                            }
                        }));
                        return [3 /*break*/, 4];
                    case 3:
                        e_1 = _b.sent();
                        clearTimeout(this.frameLoadTimeout);
                        this.hideLoadIndicator();
                        this.showErrorBar(String(e_1));
                        console.log(e_1);
                        return [3 /*break*/, 4];
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(logger_1.ILogger),
        __metadata("design:type", Object)
    ], MiniBrowserContent.prototype, "logger", void 0);
    __decorate([
        inversify_1.inject(window_service_1.WindowService),
        __metadata("design:type", Object)
    ], MiniBrowserContent.prototype, "windowService", void 0);
    __decorate([
        inversify_1.inject(location_mapper_service_1.LocationMapperService),
        __metadata("design:type", location_mapper_service_1.LocationMapperService)
    ], MiniBrowserContent.prototype, "locationMapper", void 0);
    __decorate([
        inversify_1.inject(keybinding_1.KeybindingRegistry),
        __metadata("design:type", keybinding_1.KeybindingRegistry)
    ], MiniBrowserContent.prototype, "keybindings", void 0);
    __decorate([
        inversify_1.inject(application_shell_mouse_tracker_1.ApplicationShellMouseTracker),
        __metadata("design:type", application_shell_mouse_tracker_1.ApplicationShellMouseTracker)
    ], MiniBrowserContent.prototype, "mouseTracker", void 0);
    __decorate([
        inversify_1.inject(filesystem_1.FileSystem),
        __metadata("design:type", Object)
    ], MiniBrowserContent.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(filesystem_watcher_1.FileSystemWatcher),
        __metadata("design:type", filesystem_watcher_1.FileSystemWatcher)
    ], MiniBrowserContent.prototype, "fileSystemWatcher", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], MiniBrowserContent.prototype, "init", null);
    MiniBrowserContent = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(MiniBrowserProps)),
        __metadata("design:paramtypes", [MiniBrowserProps])
    ], MiniBrowserContent);
    return MiniBrowserContent;
}(widget_1.BaseWidget));
exports.MiniBrowserContent = MiniBrowserContent;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser-content'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser-dummy-server.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser-dummy-server.js ***!
  \**********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen and Gerald Weber.
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var CODE_EDITOR_PRIORITY = 100;
var MiniBrowserDummyServer = /** @class */ (function () {
    function MiniBrowserDummyServer() {
    }
    MiniBrowserDummyServer.prototype.supportedFileExtensions = function () {
        return __awaiter(this, void 0, void 0, function () {
            var priority;
            return __generator(this, function (_a) {
                priority = [
                    { extension: 'jpg', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'jpeg', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'bmp', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'gif', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'png', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'pdf', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'svg', priority: CODE_EDITOR_PRIORITY + 1 },
                    { extension: 'html', priority: 1 }
                ];
                return [2 /*return*/, priority];
            });
        });
    };
    MiniBrowserDummyServer = __decorate([
        inversify_1.injectable()
    ], MiniBrowserDummyServer);
    return MiniBrowserDummyServer;
}());
exports.MiniBrowserDummyServer = MiniBrowserDummyServer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser-dummy-server'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser-frontend-module.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser-frontend-module.js ***!
  \*************************************************************************************************************/
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
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/mini-browser/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var opener_service_1 = __webpack_require__(/*! @theia/core/lib/browser/opener-service */ "../../packages/core/lib/browser/opener-service.js");
var widget_manager_1 = __webpack_require__(/*! @theia/core/lib/browser/widget-manager */ "../../packages/core/lib/browser/widget-manager.js");
var contribution_provider_1 = __webpack_require__(/*! @theia/core/lib/common/contribution-provider */ "../../packages/core/lib/common/contribution-provider.js");
// import { WebSocketConnectionProvider } from '@theia/core/lib/browser/messaging/ws-connection-provider';
var frontend_application_1 = __webpack_require__(/*! @theia/core/lib/browser/frontend-application */ "../../packages/core/lib/browser/frontend-application.js");
var tab_bar_toolbar_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-toolbar */ "../../packages/core/lib/browser/shell/tab-bar-toolbar.js");
var command_1 = __webpack_require__(/*! @theia/core/lib/common/command */ "../../packages/core/lib/common/command.js");
var menu_1 = __webpack_require__(/*! @theia/core/lib/common/menu */ "../../packages/core/lib/common/menu.js");
var mini_browser_open_handler_1 = __webpack_require__(/*! ./mini-browser-open-handler */ "../../packages/mini-browser/lib/browser/mini-browser-open-handler.js");
var mini_browser_service_1 = __webpack_require__(/*! ../common/mini-browser-service */ "../../packages/mini-browser/lib/common/mini-browser-service.js");
var mini_browser_1 = __webpack_require__(/*! ./mini-browser */ "../../packages/mini-browser/lib/browser/mini-browser.js");
var mini_browser_content_1 = __webpack_require__(/*! ./mini-browser-content */ "../../packages/mini-browser/lib/browser/mini-browser-content.js");
var location_mapper_service_1 = __webpack_require__(/*! ./location-mapper-service */ "../../packages/mini-browser/lib/browser/location-mapper-service.js");
var mini_browser_dummy_server_1 = __webpack_require__(/*! ./mini-browser-dummy-server */ "../../packages/mini-browser/lib/browser/mini-browser-dummy-server.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    bind(mini_browser_content_1.MiniBrowserContent).toSelf();
    bind(mini_browser_content_1.MiniBrowserContentFactory).toFactory(function (context) { return function (props) {
        var container = context.container;
        var child = container.createChild();
        child.bind(mini_browser_content_1.MiniBrowserProps).toConstantValue(props);
        return child.get(mini_browser_content_1.MiniBrowserContent);
    }; });
    bind(mini_browser_1.MiniBrowser).toSelf();
    bind(widget_manager_1.WidgetFactory).toDynamicValue(function (context) { return ({
        id: mini_browser_1.MiniBrowser.ID,
        createWidget: function (options) {
            return __awaiter(this, void 0, void 0, function () {
                var container, child, uri;
                return __generator(this, function (_a) {
                    container = context.container;
                    child = container.createChild();
                    uri = new uri_1.default(options.uri);
                    child.bind(mini_browser_1.MiniBrowserOptions).toConstantValue({ uri: uri });
                    return [2 /*return*/, child.get(mini_browser_1.MiniBrowser)];
                });
            });
        }
    }); }).inSingletonScope();
    bind(mini_browser_open_handler_1.MiniBrowserOpenHandler).toSelf().inSingletonScope();
    bind(opener_service_1.OpenHandler).toService(mini_browser_open_handler_1.MiniBrowserOpenHandler);
    bind(frontend_application_1.FrontendApplicationContribution).toService(mini_browser_open_handler_1.MiniBrowserOpenHandler);
    bind(command_1.CommandContribution).toService(mini_browser_open_handler_1.MiniBrowserOpenHandler);
    bind(menu_1.MenuContribution).toService(mini_browser_open_handler_1.MiniBrowserOpenHandler);
    bind(tab_bar_toolbar_1.TabBarToolbarContribution).toService(mini_browser_open_handler_1.MiniBrowserOpenHandler);
    contribution_provider_1.bindContributionProvider(bind, location_mapper_service_1.LocationMapper);
    bind(location_mapper_service_1.LocationMapper).to(location_mapper_service_1.FileLocationMapper).inSingletonScope();
    bind(location_mapper_service_1.LocationMapper).to(location_mapper_service_1.HttpLocationMapper).inSingletonScope();
    bind(location_mapper_service_1.LocationMapper).to(location_mapper_service_1.HttpsLocationMapper).inSingletonScope();
    bind(location_mapper_service_1.LocationWithoutSchemeMapper).toSelf().inSingletonScope();
    bind(location_mapper_service_1.LocationMapper).toService(location_mapper_service_1.LocationWithoutSchemeMapper);
    bind(location_mapper_service_1.LocationMapperService).toSelf().inSingletonScope();
    bind(mini_browser_service_1.MiniBrowserService).to(mini_browser_dummy_server_1.MiniBrowserDummyServer).inSingletonScope();
    // bind(MiniBrowserService).toDynamicValue(context => WebSocketConnectionProvider.createProxy(context.container, MiniBrowserServicePath)).inSingletonScope();
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser-open-handler.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser-open-handler.js ***!
  \**********************************************************************************************************/
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
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
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
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var navigatable_1 = __webpack_require__(/*! @theia/core/lib/browser/navigatable */ "../../packages/core/lib/browser/navigatable.js");
var opener_service_1 = __webpack_require__(/*! @theia/core/lib/browser/opener-service */ "../../packages/core/lib/browser/opener-service.js");
var label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/label-provider */ "../../packages/core/lib/browser/label-provider.js");
var mini_browser_service_1 = __webpack_require__(/*! ../common/mini-browser-service */ "../../packages/mini-browser/lib/common/mini-browser-service.js");
var mini_browser_1 = __webpack_require__(/*! ./mini-browser */ "../../packages/mini-browser/lib/browser/mini-browser.js");
var location_mapper_service_1 = __webpack_require__(/*! ./location-mapper-service */ "../../packages/mini-browser/lib/browser/location-mapper-service.js");
var MiniBrowserCommands;
(function (MiniBrowserCommands) {
    MiniBrowserCommands.PREVIEW = {
        id: 'mini-browser.preview',
        label: 'Open Preview',
        iconClass: 'theia-open-preview-icon'
    };
    MiniBrowserCommands.OPEN_SOURCE = {
        id: 'mini-browser.open.source',
        iconClass: 'theia-open-file-icon'
    };
    MiniBrowserCommands.OPEN_URL = {
        id: 'mini-browser.openUrl',
        category: 'Preview',
        label: 'Open URL'
    };
})(MiniBrowserCommands = exports.MiniBrowserCommands || (exports.MiniBrowserCommands = {}));
var MiniBrowserOpenHandler = /** @class */ (function (_super) {
    __extends(MiniBrowserOpenHandler, _super);
    function MiniBrowserOpenHandler() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        /**
         * Instead of going to the backend with each file URI to ask whether it can handle the current file or not,
         * we have this map of extension and priority pairs that we populate at application startup.
         * The real advantage of this approach is the following: [Phosphor cannot run async code when invoking `isEnabled`/`isVisible`
         * for the command handlers](https://github.com/eclipse-theia/theia/issues/1958#issuecomment-392829371)
         * so the menu item would be always visible for the user even if the file type cannot be handled eventually.
         * Hopefully, we could get rid of this hack once we have migrated the existing Phosphor code to [React](https://github.com/eclipse-theia/theia/issues/1915).
         */
        _this.supportedExtensions = new Map();
        _this.id = mini_browser_1.MiniBrowser.ID;
        _this.label = 'Preview';
        return _this;
    }
    MiniBrowserOpenHandler_1 = MiniBrowserOpenHandler;
    MiniBrowserOpenHandler.prototype.onStart = function () {
        var _this = this;
        (function () { return __awaiter(_this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.miniBrowserService.supportedFileExtensions()];
                    case 1: return [2 /*return*/, (_a.sent()).forEach(function (entry) {
                            var extension = entry.extension, priority = entry.priority;
                            _this.supportedExtensions.set(extension, priority);
                        })];
                }
            });
        }); })();
    };
    MiniBrowserOpenHandler.prototype.canHandle = function (uri) {
        // It does not guard against directories. For instance, a folder with this name: `Hahahah.html`.
        // We could check with the FS, but then, this method would become async again.
        var extension = uri.toString().split('.').pop();
        if (extension) {
            return this.supportedExtensions.get(extension.toLocaleLowerCase()) || 0;
        }
        return 0;
    };
    MiniBrowserOpenHandler.prototype.open = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var widget, area, panelLayout, minSize_1;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, _super.prototype.open.call(this, uri, options)];
                    case 1:
                        widget = _a.sent();
                        area = this.shell.getAreaFor(widget);
                        if (area === 'right' || area === 'left') {
                            panelLayout = area === 'right' ? this.shell.getLayoutData().rightPanel : this.shell.getLayoutData().leftPanel;
                            minSize_1 = this.shell.mainPanel.node.offsetWidth / 2;
                            if (panelLayout && panelLayout.size && panelLayout.size <= minSize_1) {
                                requestAnimationFrame(function () { return _this.shell.resize(minSize_1, area); });
                            }
                        }
                        return [2 /*return*/, widget];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.getOrCreateWidget = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var props, widget;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.options(uri, options)];
                    case 1:
                        props = _a.sent();
                        return [4 /*yield*/, _super.prototype.getOrCreateWidget.call(this, uri, props)];
                    case 2:
                        widget = _a.sent();
                        widget.setProps(props);
                        return [2 /*return*/, widget];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.options = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var result, startPage, name_1, iconClass, resetBackground;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.defaultOptions()];
                    case 1:
                        result = _a.sent();
                        if (!uri) return [3 /*break*/, 3];
                        startPage = uri.toString(true);
                        name_1 = this.labelProvider.getName(uri);
                        iconClass = this.labelProvider.getIcon(uri) + " file-icon";
                        return [4 /*yield*/, this.resetBackground(uri)];
                    case 2:
                        resetBackground = _a.sent();
                        result = __assign({}, result, { startPage: startPage,
                            name: name_1,
                            iconClass: iconClass, 
                            // Make sure the toolbar is not visible. We have the `iframe.src` anyway.
                            toolbar: 'hide', resetBackground: resetBackground });
                        _a.label = 3;
                    case 3:
                        if (options) {
                            // Explicit options overrule everything.
                            result = __assign({}, result, options);
                        }
                        return [2 /*return*/, result];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.resetBackground = function (uri) {
        var scheme = uri.scheme;
        var uriStr = uri.toString();
        return scheme === 'http'
            || scheme === 'https'
            || (scheme === 'file'
                && (uriStr.endsWith('html') || uriStr.endsWith('.htm')));
    };
    MiniBrowserOpenHandler.prototype.defaultOptions = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, {
                        mode: 'activate',
                        widgetOptions: { area: 'main' },
                        sandbox: mini_browser_1.MiniBrowserProps.SandboxOptions.DEFAULT,
                        toolbar: 'show'
                    }];
            });
        });
    };
    MiniBrowserOpenHandler.prototype.registerCommands = function (commands) {
        var _this = this;
        commands.registerCommand(MiniBrowserCommands.PREVIEW, {
            execute: function (widget) { return _this.preview(widget); },
            isEnabled: function (widget) { return _this.canPreviewWidget(widget); },
            isVisible: function (widget) { return _this.canPreviewWidget(widget); }
        });
        commands.registerCommand(MiniBrowserCommands.OPEN_SOURCE, {
            execute: function (widget) { return _this.openSource(widget); },
            isEnabled: function (widget) { return !!_this.getSourceUri(widget); },
            isVisible: function (widget) { return !!_this.getSourceUri(widget); }
        });
        commands.registerCommand(MiniBrowserCommands.OPEN_URL, {
            execute: function (arg) { return _this.openUrl(arg); }
        });
    };
    MiniBrowserOpenHandler.prototype.registerMenus = function (menus) {
        menus.registerMenuAction(['editor_context_menu', 'navigation'], {
            commandId: MiniBrowserCommands.PREVIEW.id
        });
    };
    MiniBrowserOpenHandler.prototype.registerToolbarItems = function (toolbar) {
        toolbar.registerItem({
            id: MiniBrowserCommands.PREVIEW.id,
            command: MiniBrowserCommands.PREVIEW.id,
            tooltip: 'Open Preview to the Side'
        });
        // toolbar.registerItem({
        //     id: MiniBrowserCommands.OPEN_SOURCE.id,
        //     command: MiniBrowserCommands.OPEN_SOURCE.id,
        //     tooltip: 'Open Source'
        // });
    };
    MiniBrowserOpenHandler.prototype.canPreviewWidget = function (widget) {
        var uri = this.getUriToPreview(widget);
        return !!uri && !!this.canHandle(uri);
    };
    MiniBrowserOpenHandler.prototype.getUriToPreview = function (widget) {
        var current = this.getWidgetToPreview(widget);
        return current && current.getResourceUri();
    };
    MiniBrowserOpenHandler.prototype.getWidgetToPreview = function (widget) {
        var current = widget ? widget : this.shell.currentWidget;
        // MiniBrowser is NavigatableWidget and should be excluded from widgets to preview
        return !(current instanceof mini_browser_1.MiniBrowser) && navigatable_1.NavigatableWidget.is(current) && current || undefined;
    };
    MiniBrowserOpenHandler.prototype.preview = function (widget) {
        return __awaiter(this, void 0, void 0, function () {
            var ref, uri;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        ref = this.getWidgetToPreview(widget);
                        if (!ref) {
                            return [2 /*return*/];
                        }
                        uri = ref.getResourceUri();
                        if (!uri) {
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, this.open(uri, {
                                mode: 'reveal',
                                widgetOptions: { ref: ref, mode: 'open-to-right' }
                            })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.openSource = function (ref) {
        return __awaiter(this, void 0, void 0, function () {
            var uri;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        uri = this.getSourceUri(ref);
                        if (!uri) return [3 /*break*/, 2];
                        return [4 /*yield*/, opener_service_1.open(this.openerService, uri, {
                                widgetOptions: { ref: ref, mode: 'open-to-left' }
                            })];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2: return [2 /*return*/];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.getSourceUri = function (ref) {
        var uri = ref instanceof mini_browser_1.MiniBrowser && ref.getResourceUri() || undefined;
        if (!uri || uri.scheme === 'http' || uri.scheme === 'https') {
            return undefined;
        }
        return uri;
    };
    MiniBrowserOpenHandler.prototype.openUrl = function (arg) {
        return __awaiter(this, void 0, void 0, function () {
            var url, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        if (!arg) return [3 /*break*/, 1];
                        _a = arg;
                        return [3 /*break*/, 3];
                    case 1: return [4 /*yield*/, this.quickInputService.open({
                            prompt: 'URL to open',
                            placeHolder: 'Type a URL'
                        })];
                    case 2:
                        _a = _b.sent();
                        _b.label = 3;
                    case 3:
                        url = _a;
                        if (!url) return [3 /*break*/, 5];
                        return [4 /*yield*/, this.openPreview(url)];
                    case 4:
                        _b.sent();
                        _b.label = 5;
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.openPreview = function (startPage) {
        return __awaiter(this, void 0, void 0, function () {
            var props, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = this.getOpenPreviewProps;
                        return [4 /*yield*/, this.locationMapperService.map(startPage)];
                    case 1: return [4 /*yield*/, _a.apply(this, [_b.sent()])];
                    case 2:
                        props = _b.sent();
                        return [2 /*return*/, this.open(MiniBrowserOpenHandler_1.PREVIEW_URI, props)];
                }
            });
        });
    };
    MiniBrowserOpenHandler.prototype.getOpenPreviewProps = function (startPage) {
        return __awaiter(this, void 0, void 0, function () {
            var resetBackground;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.resetBackground(new uri_1.default(startPage))];
                    case 1:
                        resetBackground = _a.sent();
                        return [2 /*return*/, {
                                name: 'Preview',
                                startPage: startPage,
                                toolbar: 'read-only',
                                widgetOptions: {
                                    area: 'right'
                                },
                                resetBackground: resetBackground,
                                iconClass: 'theia-mini-browser-icon'
                            }];
                }
            });
        });
    };
    var MiniBrowserOpenHandler_1;
    MiniBrowserOpenHandler.PREVIEW_URI = new uri_1.default().withScheme('__minibrowser__preview__');
    __decorate([
        inversify_1.inject(opener_service_1.OpenerService),
        __metadata("design:type", Object)
    ], MiniBrowserOpenHandler.prototype, "openerService", void 0);
    __decorate([
        inversify_1.inject(label_provider_1.LabelProvider),
        __metadata("design:type", label_provider_1.LabelProvider)
    ], MiniBrowserOpenHandler.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(browser_1.QuickInputService),
        __metadata("design:type", browser_1.QuickInputService)
    ], MiniBrowserOpenHandler.prototype, "quickInputService", void 0);
    __decorate([
        inversify_1.inject(mini_browser_service_1.MiniBrowserService),
        __metadata("design:type", Object)
    ], MiniBrowserOpenHandler.prototype, "miniBrowserService", void 0);
    __decorate([
        inversify_1.inject(location_mapper_service_1.LocationMapperService),
        __metadata("design:type", location_mapper_service_1.LocationMapperService)
    ], MiniBrowserOpenHandler.prototype, "locationMapperService", void 0);
    MiniBrowserOpenHandler = MiniBrowserOpenHandler_1 = __decorate([
        inversify_1.injectable()
    ], MiniBrowserOpenHandler);
    return MiniBrowserOpenHandler;
}(navigatable_1.NavigatableWidgetOpenHandler));
exports.MiniBrowserOpenHandler = MiniBrowserOpenHandler;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser-open-handler'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/browser/mini-browser.js":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/browser/mini-browser.js ***!
  \*********************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var disposable_1 = __webpack_require__(/*! @theia/core/lib/common/disposable */ "../../packages/core/lib/common/disposable.js");
var widget_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/widget */ "../../packages/core/lib/browser/widgets/widget.js");
var mini_browser_content_1 = __webpack_require__(/*! ./mini-browser-content */ "../../packages/mini-browser/lib/browser/mini-browser-content.js");
exports.MiniBrowserProps = mini_browser_content_1.MiniBrowserProps;
var MiniBrowserOptions = /** @class */ (function () {
    function MiniBrowserOptions() {
    }
    MiniBrowserOptions = __decorate([
        inversify_1.injectable()
    ], MiniBrowserOptions);
    return MiniBrowserOptions;
}());
exports.MiniBrowserOptions = MiniBrowserOptions;
var MiniBrowser = /** @class */ (function (_super) {
    __extends(MiniBrowser, _super);
    function MiniBrowser() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.toDisposeOnProps = new disposable_1.DisposableCollection();
        return _this;
    }
    MiniBrowser_1 = MiniBrowser;
    MiniBrowser.prototype.init = function () {
        var uri = this.options.uri;
        this.id = MiniBrowser_1.ID + ":" + uri.toString();
        this.title.closable = true;
        this.layout = new widget_1.PanelLayout({ fitPolicy: 'set-no-constraint' });
    };
    MiniBrowser.prototype.getResourceUri = function () {
        return this.options.uri;
    };
    MiniBrowser.prototype.createMoveToUri = function (resourceUri) {
        return this.options.uri && this.options.uri.withPath(resourceUri.path);
    };
    MiniBrowser.prototype.setProps = function (raw) {
        var props = {
            toolbar: raw.toolbar,
            startPage: raw.startPage,
            sandbox: raw.sandbox,
            iconClass: raw.iconClass,
            name: raw.name,
            resetBackground: raw.resetBackground
        };
        if (JSON.stringify(props) === JSON.stringify(this.props)) {
            return;
        }
        this.toDisposeOnProps.dispose();
        this.toDispose.push(this.toDisposeOnProps);
        this.props = props;
        this.title.caption = this.title.label = props.name || 'Browser';
        this.title.iconClass = props.iconClass || MiniBrowser_1.ICON;
        var content = this.createContent(props);
        this.layout.addWidget(content);
        this.toDisposeOnProps.push(content);
    };
    MiniBrowser.prototype.onActivateRequest = function (msg) {
        _super.prototype.onActivateRequest.call(this, msg);
        var widget = this.layout.widgets[0];
        if (widget) {
            widget.activate();
        }
    };
    MiniBrowser.prototype.storeState = function () {
        var props = this.props;
        return { props: props };
    };
    MiniBrowser.prototype.restoreState = function (oldState) {
        if (!this.toDisposeOnProps.disposed) {
            return;
        }
        if ('props' in oldState) {
            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            this.setProps(oldState['props']);
        }
    };
    var MiniBrowser_1;
    MiniBrowser.ID = 'mini-browser';
    MiniBrowser.ICON = 'fa fa-globe';
    __decorate([
        inversify_1.inject(MiniBrowserOptions),
        __metadata("design:type", MiniBrowserOptions)
    ], MiniBrowser.prototype, "options", void 0);
    __decorate([
        inversify_1.inject(mini_browser_content_1.MiniBrowserContentFactory),
        __metadata("design:type", Function)
    ], MiniBrowser.prototype, "createContent", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], MiniBrowser.prototype, "init", null);
    MiniBrowser = MiniBrowser_1 = __decorate([
        inversify_1.injectable()
    ], MiniBrowser);
    return MiniBrowser;
}(widget_1.BaseWidget));
exports.MiniBrowser = MiniBrowser;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/browser/mini-browser'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/lib/common/mini-browser-service.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/lib/common/mini-browser-service.js ***!
  \****************************************************************************************************/
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
exports.MiniBrowserServicePath = '/services/mini-browser-service';
exports.MiniBrowserService = Symbol('MiniBrowserService');

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/mini-browser/lib/common/mini-browser-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/mini-browser/src/browser/style/index.css":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/src/browser/style/index.css ***!
  \*********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/mini-browser/src/browser/style/index.css");

if(typeof content === 'string') content = [[module.i, content, '']];

var transform;
var insertInto;



var options = {"hmr":true}

options.transform = transform
options.insertInto = undefined;

var update = __webpack_require__(/*! ../../../../../node_modules/style-loader/lib/addStyles.js */ "../../node_modules/style-loader/lib/addStyles.js")(content, options);

if(content.locals) module.exports = content.locals;

if(false) {}

/***/ }),

/***/ "../../packages/mini-browser/src/browser/style/mini-browser.svg":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/mini-browser/src/browser/style/mini-browser.svg ***!
  \****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports) {

module.exports = "data:image/svg+xml;base64,PCEtLUNvcHlyaWdodCAoYykgMjAxOSBLZW5uZXRoIEF1Y2hlbmJlcmcuIC0tPgo8IS0tQ29weXJpZ2h0IChDKSAyMDE5IFR5cGVGb3ggYW5kIG90aGVycy4tLT4KPCEtLUxpY2Vuc2VkIHVuZGVyIHRoZSBNSVQgTGljZW5zZS4tLT4KPHN2ZyB3aWR0aD0iMjVweCIgaGVpZ2h0PSIyNXB4IiB2aWV3Qm94PSIwIDAgMjUgMjUiIHZlcnNpb249IjEuMSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayI+CiAgICA8IS0tIEdlbmVyYXRvcjogU2tldGNoIDUyLjIgKDY3MTQ1KSAtIGh0dHA6Ly93d3cuYm9oZW1pYW5jb2RpbmcuY29tL3NrZXRjaCAtLT4KICAgIDx0aXRsZT5pY29uPC90aXRsZT4KICAgIDxkZXNjPkNyZWF0ZWQgd2l0aCBTa2V0Y2guPC9kZXNjPgogICAgPGcgaWQ9IlBhZ2UtMSIgc3Ryb2tlPSJub25lIiBzdHJva2Utd2lkdGg9IjEiIGZpbGw9Im5vbmUiIGZpbGwtcnVsZT0iZXZlbm9kZCI+CiAgICAgICAgPGcgaWQ9Imljb24iIGZpbGw9IiMyODc5RkYiPgogICAgICAgICAgICA8cG9seWdvbiBpZD0iUGF0aCIgcG9pbnRzPSIxMy44NjcxODc1IDE1LjYyNSAxMy44NjcxODc1IDE0LjA2MjUgMTguNTU0Njg3NSAxNC4wNjI1IDE4LjU1NDY4NzUgMTUuNjI1Ij48L3BvbHlnb24+CiAgICAgICAgICAgIDxwb2x5Z29uIGlkPSJQYXRoIiBwb2ludHM9IjEzLjg2NzE4NzUgMTIuNSAxMy44NjcxODc1IDEwLjkzNzUgMjEuNjc5Njg3NSAxMC45Mzc1IDIxLjY3OTY4NzUgMTIuNSI+PC9wb2x5Z29uPgogICAgICAgICAgICA8cG9seWdvbiBpZD0iUGF0aCIgcG9pbnRzPSIxMy44NjcxODc1IDkuMzc1IDEzLjg2NzE4NzUgNy44MTI1IDIxLjY3OTY4NzUgNy44MTI1IDIxLjY3OTY4NzUgOS4zNzUiPjwvcG9seWdvbj4KICAgICAgICAgICAgPHJlY3QgaWQ9IlJlY3RhbmdsZSIgeD0iMi45Mjk2ODc1IiB5PSI3LjgxMjUiIHdpZHRoPSI3LjgxMjUiIGhlaWdodD0iNy44MTI1Ij48L3JlY3Q+CiAgICAgICAgICAgIDxwYXRoIGQ9Ik0wLDAgTDAsMjUgTDI0Ljk5OTIxODgsMjUgTDI1LDAgTDAsMCBaIE0xNy4xODc1LDEuNTYyNSBMMTcuMTg3NSwzLjEyNSBMNy44MTI1LDMuMTI1IEw3LjgxMjUsMS41NjI1IEwxNy4xODc1LDEuNTYyNSBaIE02LjI1LDEuNTYyNSBMNi4yNSwzLjEyNSBMNC42ODc1LDMuMTI1IEw0LjY4NzUsMS41NjI1IEw2LjI1LDEuNTYyNSBaIE0xLjU2MjUsMS41NjI1IEwzLjEyNSwxLjU2MjUgTDMuMTI1LDMuMTI1IEwxLjU2MjUsMy4xMjUgTDEuNTYyNSwxLjU2MjUgWiBNMjMuNDM2NzE4OCwyMy4wNDY4NzUgTDEuNTYyNSwyMy4wNDY4NzUgTDEuNTYyNSw0LjY4NzUgTDIzLjQzNjcxODgsNC42ODc1IEwyMy40MzY3MTg4LDIzLjA0Njg3NSBaIE0yMy40Mzc1LDMuMTI1IEwyMC4zMTI1LDMuMTI1IEwyMC4zMTI1LDEuNTYyNSBMMjMuNDM3NSwxLjU2MjUgTDIzLjQzNzUsMy4xMjUgWiIgaWQ9IlNoYXBlIiBmaWxsLXJ1bGU9Im5vbnplcm8iPjwvcGF0aD4KICAgICAgICA8L2c+CiAgICA8L2c+Cjwvc3ZnPgo="

/***/ })

}]);
//# sourceMappingURL=31.bundle.js.map