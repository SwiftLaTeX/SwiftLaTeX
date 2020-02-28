(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[9],{

/***/ "../../packages/filesystem/lib/browser/download/file-download-command-contribution.js":
/*!**************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/download/file-download-command-contribution.js ***!
  \**************************************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser/browser */ "../../packages/core/lib/browser/browser.js");
var environment_1 = __webpack_require__(/*! @theia/application-package/lib/environment */ "../../dev-packages/application-package/lib/environment.js");
var selection_service_1 = __webpack_require__(/*! @theia/core/lib/common/selection-service */ "../../packages/core/lib/common/selection-service.js");
var uri_command_handler_1 = __webpack_require__(/*! @theia/core/lib/common/uri-command-handler */ "../../packages/core/lib/common/uri-command-handler.js");
var file_download_service_1 = __webpack_require__(/*! ./file-download-service */ "../../packages/filesystem/lib/browser/download/file-download-service.js");
var FileDownloadCommandContribution = /** @class */ (function () {
    function FileDownloadCommandContribution() {
    }
    FileDownloadCommandContribution.prototype.registerCommands = function (registry) {
        var _this = this;
        registry.registerCommand(FileDownloadCommands.DOWNLOAD, new uri_command_handler_1.UriAwareCommandHandler(this.selectionService, {
            execute: function (uris) { return _this.executeDownload(uris); },
            isEnabled: function (uris) { return _this.isDownloadEnabled(uris); },
            isVisible: function (uris) { return _this.isDownloadVisible(uris); },
        }, { multi: true }));
        registry.registerCommand(FileDownloadCommands.COPY_DOWNLOAD_LINK, new uri_command_handler_1.UriAwareCommandHandler(this.selectionService, {
            execute: function (uris) { return _this.executeDownload(uris, { copyLink: true }); },
            isEnabled: function (uris) { return browser_1.isChrome && _this.isDownloadEnabled(uris); },
            isVisible: function (uris) { return browser_1.isChrome && _this.isDownloadVisible(uris); },
        }, { multi: true }));
    };
    FileDownloadCommandContribution.prototype.executeDownload = function (uris, options) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                this.downloadService.download(uris, options);
                return [2 /*return*/];
            });
        });
    };
    FileDownloadCommandContribution.prototype.isDownloadEnabled = function (uris) {
        return !environment_1.environment.electron.is() && uris.length > 0 && uris.every(function (u) { return u.scheme === 'file'; });
    };
    FileDownloadCommandContribution.prototype.isDownloadVisible = function (uris) {
        return this.isDownloadEnabled(uris);
    };
    __decorate([
        inversify_1.inject(file_download_service_1.FileDownloadService),
        __metadata("design:type", file_download_service_1.FileDownloadService)
    ], FileDownloadCommandContribution.prototype, "downloadService", void 0);
    __decorate([
        inversify_1.inject(selection_service_1.SelectionService),
        __metadata("design:type", selection_service_1.SelectionService)
    ], FileDownloadCommandContribution.prototype, "selectionService", void 0);
    FileDownloadCommandContribution = __decorate([
        inversify_1.injectable()
    ], FileDownloadCommandContribution);
    return FileDownloadCommandContribution;
}());
exports.FileDownloadCommandContribution = FileDownloadCommandContribution;
var FileDownloadCommands;
(function (FileDownloadCommands) {
    FileDownloadCommands.DOWNLOAD = {
        id: 'file.download',
        category: 'File',
        label: 'Download'
    };
    FileDownloadCommands.COPY_DOWNLOAD_LINK = {
        id: 'file.copyDownloadLink',
        category: 'File',
        label: 'Copy Download Link'
    };
})(FileDownloadCommands = exports.FileDownloadCommands || (exports.FileDownloadCommands = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/download/file-download-command-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/download/file-download-service.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/download/file-download-service.js ***!
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
var logger_1 = __webpack_require__(/*! @theia/core/lib/common/logger */ "../../packages/core/lib/common/logger.js");
// import { Endpoint } from '@theia/core/lib/browser/endpoint';
var filesystem_1 = __webpack_require__(/*! ../../common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var s3storagesystem_1 = __webpack_require__(/*! ../s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
// import { FileDownloadData } from '../../common/download/file-download-data';
var message_service_1 = __webpack_require__(/*! @theia/core/lib/common/message-service */ "../../packages/core/lib/common/message-service.js");
var widgets_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets */ "../../packages/core/lib/browser/widgets/index.js");
var FileDownloadService = /** @class */ (function () {
    function FileDownloadService() {
        this.downloadCounter = 0;
    }
    FileDownloadService.prototype.handleCopy = function (event, downloadUrl) {
        if (downloadUrl && event.clipboardData) {
            event.clipboardData.setData('text/plain', downloadUrl);
            event.preventDefault();
            this.messageService.info('Copied the download link to the clipboard. The link is valid for 60 seconds.');
        }
    };
    FileDownloadService.prototype.download = function (uris, options) {
        return __awaiter(this, void 0, void 0, function () {
            var cancel, copyLink, progress, key, s3obj, content, tempBlob, downloadUrl_1, toDispose_1, displayName, e_1;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        console.log(uris);
                        cancel = false;
                        if (uris.length === 0) {
                            return [2 /*return*/];
                        }
                        if (!(uris.length > 1)) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.messageService.warn('Please download a file/directory at a time. Tip: to download the whole project, \
             deselect any files first and retry the download command')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                    case 2:
                        copyLink = options && options.copyLink ? true : false;
                        _a.label = 3;
                    case 3:
                        _a.trys.push([3, 12, , 13]);
                        return [4 /*yield*/, this.messageService.showProgress({
                                text: "Preparing download" + (copyLink ? ' link' : '') + "...", options: { cancelable: true }
                            }, function () { cancel = true; })];
                    case 4:
                        progress = _a.sent();
                        key = uris[0].toString().substr(7);
                        return [4 /*yield*/, this.s3fs.stat(key)];
                    case 5:
                        s3obj = _a.sent();
                        if (!!s3obj) return [3 /*break*/, 7];
                        progress.cancel();
                        return [4 /*yield*/, this.messageService.error("File " + key + " does not exists")];
                    case 6:
                        _a.sent();
                        return [2 /*return*/];
                    case 7:
                        content = void 0;
                        if (!s3obj.isDir) return [3 /*break*/, 9];
                        return [4 /*yield*/, this.s3fs.zipDir(key)];
                    case 8:
                        content = _a.sent();
                        return [3 /*break*/, 11];
                    case 9: return [4 /*yield*/, this.s3fs.readFile(key)];
                    case 10:
                        content = (_a.sent());
                        _a.label = 11;
                    case 11:
                        progress.cancel();
                        if (cancel) {
                            return [2 /*return*/];
                        }
                        tempBlob = new Blob([content], { type: 'application/octet-stream' });
                        downloadUrl_1 = URL.createObjectURL(tempBlob);
                        if (copyLink) {
                            setTimeout(function () {
                                /* Release it in a minute */
                                URL.revokeObjectURL(downloadUrl_1);
                            }, 60000);
                            if (document.documentElement) {
                                toDispose_1 = widgets_1.addClipboardListener(document.documentElement, 'copy', function (e) {
                                    toDispose_1.dispose();
                                    _this.handleCopy(e, downloadUrl_1);
                                });
                                document.execCommand('copy');
                            }
                        }
                        else {
                            displayName = uris[0].displayName;
                            if (s3obj.isDir) {
                                if (displayName === '' || displayName === '/') {
                                    displayName = 'root';
                                }
                                displayName += '.zip';
                            }
                            this.forceDownload(downloadUrl_1, displayName);
                        }
                        return [3 /*break*/, 13];
                    case 12:
                        e_1 = _a.sent();
                        this.logger.error("Error occurred when downloading: " + uris.map(function (u) { return u.toString(true); }) + ".", e_1);
                        return [3 /*break*/, 13];
                    case 13: return [2 /*return*/];
                }
            });
        });
    };
    FileDownloadService.prototype.forceDownload = function (blob_url, title) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                try {
                    if (this.anchor === undefined) {
                        this.anchor = document.createElement('a');
                    }
                    this.anchor.href = blob_url;
                    this.anchor.style.display = 'none';
                    this.anchor.download = title;
                    document.body.appendChild(this.anchor);
                    this.anchor.click();
                }
                finally {
                    // make sure anchor is removed from parent
                    if (this.anchor && this.anchor.parentNode) {
                        this.anchor.parentNode.removeChild(this.anchor);
                    }
                    URL.revokeObjectURL(blob_url);
                }
                return [2 /*return*/];
            });
        });
    };
    __decorate([
        inversify_1.inject(logger_1.ILogger),
        __metadata("design:type", Object)
    ], FileDownloadService.prototype, "logger", void 0);
    __decorate([
        inversify_1.inject(filesystem_1.FileSystem),
        __metadata("design:type", Object)
    ], FileDownloadService.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(message_service_1.MessageService),
        __metadata("design:type", message_service_1.MessageService)
    ], FileDownloadService.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(s3storagesystem_1.S3StorageSystem),
        __metadata("design:type", s3storagesystem_1.S3StorageSystem)
    ], FileDownloadService.prototype, "s3fs", void 0);
    FileDownloadService = __decorate([
        inversify_1.injectable()
    ], FileDownloadService);
    return FileDownloadService;
}());
exports.FileDownloadService = FileDownloadService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/download/file-download-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=9.bundle.js.map