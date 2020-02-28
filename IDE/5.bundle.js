(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[5],{

/***/ "../../packages/filesystem/lib/browser/file-tree/file-tree.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/file-tree.js ***!
  \**************************************************************************************************/
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
var common_1 = __webpack_require__(/*! ../../common */ "../../packages/filesystem/lib/common/index.js");
var FileTree = /** @class */ (function (_super) {
    __extends(FileTree, _super);
    function FileTree() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    FileTree.prototype.resolveChildren = function (parent) {
        return __awaiter(this, void 0, void 0, function () {
            var fileStat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!FileStatNode.is(parent)) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.resolveFileStat(parent)];
                    case 1:
                        fileStat = _a.sent();
                        if (fileStat) {
                            return [2 /*return*/, this.toNodes(fileStat, parent)];
                        }
                        return [2 /*return*/, []];
                    case 2: return [2 /*return*/, _super.prototype.resolveChildren.call(this, parent)];
                }
            });
        });
    };
    FileTree.prototype.resolveFileStat = function (node) {
        return this.fileSystem.getFileStat(node.fileStat.uri).then(function (fileStat) {
            if (fileStat) {
                node.fileStat = fileStat;
                return fileStat;
            }
            return undefined;
        });
    };
    FileTree.prototype.toNodes = function (fileStat, parent) {
        return __awaiter(this, void 0, void 0, function () {
            var result;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!fileStat.children) {
                            return [2 /*return*/, []];
                        }
                        return [4 /*yield*/, Promise.all(fileStat.children.map(function (child) { return __awaiter(_this, void 0, void 0, function () { return __generator(this, function (_a) {
                                return [2 /*return*/, this.toNode(child, parent)];
                            }); }); }))];
                    case 1:
                        result = _a.sent();
                        return [2 /*return*/, result.sort(DirNode.compare)];
                }
            });
        });
    };
    FileTree.prototype.toNode = function (fileStat, parent) {
        var uri = new uri_1.default(fileStat.uri);
        var id = this.toNodeId(uri, parent);
        var node = this.getNode(id);
        if (fileStat.isDirectory) {
            if (DirNode.is(node)) {
                node.fileStat = fileStat;
                return node;
            }
            return {
                id: id, uri: uri, fileStat: fileStat, parent: parent,
                expanded: false,
                selected: false,
                children: []
            };
        }
        if (FileNode.is(node)) {
            node.fileStat = fileStat;
            return node;
        }
        return {
            id: id, uri: uri, fileStat: fileStat, parent: parent,
            selected: false
        };
    };
    FileTree.prototype.toNodeId = function (uri, parent) {
        return uri.path.toString();
    };
    __decorate([
        inversify_1.inject(common_1.FileSystem),
        __metadata("design:type", Object)
    ], FileTree.prototype, "fileSystem", void 0);
    FileTree = __decorate([
        inversify_1.injectable()
    ], FileTree);
    return FileTree;
}(browser_1.TreeImpl));
exports.FileTree = FileTree;
var FileStatNode;
(function (FileStatNode) {
    function is(node) {
        return !!node && 'fileStat' in node;
    }
    FileStatNode.is = is;
    function getUri(node) {
        if (is(node)) {
            return node.fileStat.uri;
        }
        return undefined;
    }
    FileStatNode.getUri = getUri;
})(FileStatNode = exports.FileStatNode || (exports.FileStatNode = {}));
var FileNode;
(function (FileNode) {
    function is(node) {
        return FileStatNode.is(node) && !node.fileStat.isDirectory;
    }
    FileNode.is = is;
})(FileNode = exports.FileNode || (exports.FileNode = {}));
var DirNode;
(function (DirNode) {
    function is(node) {
        return FileStatNode.is(node) && node.fileStat.isDirectory;
    }
    DirNode.is = is;
    function compare(node, node2) {
        return DirNode.dirCompare(node, node2) || uriCompare(node, node2);
    }
    DirNode.compare = compare;
    function uriCompare(node, node2) {
        if (FileStatNode.is(node)) {
            if (FileStatNode.is(node2)) {
                return node.uri.displayName.localeCompare(node2.uri.displayName);
            }
            return 1;
        }
        if (FileStatNode.is(node2)) {
            return -1;
        }
        return 0;
    }
    DirNode.uriCompare = uriCompare;
    function dirCompare(node, node2) {
        var a = DirNode.is(node) ? 1 : 0;
        var b = DirNode.is(node2) ? 1 : 0;
        return b - a;
    }
    DirNode.dirCompare = dirCompare;
    function createRoot(fileStat) {
        var uri = new uri_1.default(fileStat.uri);
        var id = fileStat.uri;
        return {
            id: id, uri: uri, fileStat: fileStat,
            visible: true,
            parent: undefined,
            children: [],
            expanded: true,
            selected: false
        };
    }
    DirNode.createRoot = createRoot;
    function getContainingDir(node) {
        var containing = node;
        while (!!containing && !is(containing)) {
            containing = containing.parent;
        }
        return containing;
    }
    DirNode.getContainingDir = getContainingDir;
})(DirNode = exports.DirNode || (exports.DirNode = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree/file-tree'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-upload-service.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-upload-service.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var cancellation_1 = __webpack_require__(/*! @theia/core/lib/common/cancellation */ "../../packages/core/lib/common/cancellation.js");
var promise_util_1 = __webpack_require__(/*! @theia/core/lib/common/promise-util */ "../../packages/core/lib/common/promise-util.js");
var message_service_1 = __webpack_require__(/*! @theia/core/lib/common/message-service */ "../../packages/core/lib/common/message-service.js");
// import { Endpoint } from '@theia/core/lib/browser/endpoint';
var s3storagesystem_1 = __webpack_require__(/*! ./s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
var filesystem_browserfs_1 = __webpack_require__(/*! ./filesystem-browserfs */ "../../packages/filesystem/lib/browser/filesystem-browserfs.js");
var throttle = __webpack_require__(/*! lodash.throttle */ "../../node_modules/lodash.throttle/index.js");
var MAXFILESIZE = 2048 * 1023;
var MAXFILENUMBER = 128;
var FileUploadService = /** @class */ (function () {
    function FileUploadService() {
    }
    FileUploadService_1 = FileUploadService;
    FileUploadService.prototype.init = function () {
        this.uploadForm = this.createUploadForm();
    };
    FileUploadService.prototype.createUploadForm = function () {
        var _this = this;
        var targetInput = document.createElement('input');
        targetInput.type = 'text';
        targetInput.name = FileUploadService_1.TARGET;
        targetInput.classList.add('theia-input');
        var fileInput = document.createElement('input');
        fileInput.type = 'file';
        fileInput.classList.add('theia-input');
        fileInput.name = FileUploadService_1.UPLOAD;
        fileInput.multiple = true;
        var form = document.createElement('form');
        form.style.display = 'none';
        form.enctype = 'multipart/form-data';
        form.append(targetInput);
        form.append(fileInput);
        document.body.appendChild(form);
        fileInput.addEventListener('change', function () {
            if (_this.deferredUpload && fileInput.value) {
                var source_1 = new FormData(form);
                // clean up to allow upload to the same folder twice
                fileInput.value = '';
                var targetUri_1 = new uri_1.default(source_1.get(FileUploadService_1.TARGET));
                var _a = _this.deferredUpload, resolve = _a.resolve, reject = _a.reject;
                _this.deferredUpload = undefined;
                var onDidUpload_1 = _this.uploadForm.onDidUpload;
                _this.withProgress(function (progress, token) { return _this.doUpload(targetUri_1, { source: source_1, progress: progress, token: token, onDidUpload: onDidUpload_1 }); }, _this.uploadForm.progress).then(resolve, reject);
            }
        });
        return { targetInput: targetInput, fileInput: fileInput };
    };
    FileUploadService.prototype.upload = function (targetUri, params) {
        if (params === void 0) { params = {}; }
        return __awaiter(this, void 0, void 0, function () {
            var source, onDidUpload;
            var _this = this;
            return __generator(this, function (_a) {
                source = params.source, onDidUpload = params.onDidUpload;
                if (source) {
                    return [2 /*return*/, this.withProgress(function (progress, token) { return _this.doUpload(new uri_1.default(String(targetUri)), { source: source, progress: progress, token: token, onDidUpload: onDidUpload }); }, params.progress)];
                }
                this.deferredUpload = new promise_util_1.Deferred();
                this.uploadForm.targetInput.value = String(targetUri);
                this.uploadForm.fileInput.click();
                this.uploadForm.progress = params.progress;
                this.uploadForm.onDidUpload = params.onDidUpload;
                return [2 /*return*/, this.deferredUpload.promise];
            });
        });
    };
    FileUploadService.prototype.doUpload = function (targetUri, _a) {
        var source = _a.source, progress = _a.progress, token = _a.token, onDidUpload = _a.onDidUpload;
        return __awaiter(this, void 0, void 0, function () {
            var result, total, done, totalFiles, doneFiles, reportProgress, deferredUpload, urilist_1, filelist_1, _loop_1, this_1, i, e_1;
            var _this = this;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        result = { uploaded: [] };
                        total = 0;
                        done = 0;
                        totalFiles = 0;
                        doneFiles = 0;
                        reportProgress = throttle(function () { return progress.report({
                            message: doneFiles + " out of " + totalFiles,
                            work: { done: done, total: total }
                        }); }, 60);
                        deferredUpload = new promise_util_1.Deferred();
                        // const endpoint = new Endpoint({ path: '/file-upload' });
                        // const socketOpen = new Deferred<void>();
                        // const socket = new WebSocket(endpoint.getWebSocketUrl().toString());
                        // socket.onerror = e => {
                        //     socketOpen.reject(e);
                        //     deferredUpload.reject(e);
                        // };
                        // socket.onclose = ({ code, reason }) => deferredUpload.reject(new Error(String(reason || code)));
                        // socket.onmessage = ({ data }) => {
                        //     const response = JSON.parse(data);
                        //     if (response.uri) {
                        //         doneFiles++;
                        //         result.uploaded.push(response.uri);
                        //         reportProgress();
                        //         if (onDidUpload) {
                        //             onDidUpload(response.uri);
                        //         }
                        //         return;
                        //     }
                        //     if (response.done) {
                        //         done = response.done;
                        //         reportProgress();
                        //         return;
                        //     }
                        //     if (response.ok) {
                        //         deferredUpload.resolve(result);
                        //     } else if (response.error) {
                        //         deferredUpload.reject(new Error(response.error));
                        //     } else {
                        //         console.error('unknown upload response: ' + response);
                        //     }
                        //     socket.close();
                        // };
                        // socket.onopen = () => socketOpen.resolve();
                        // const rejectAndClose = (e: Error) => {
                        //     deferredUpload.reject(e);
                        //     if (socket.readyState === 1) {
                        //         socket.close();
                        //     }
                        // };
                        token.onCancellationRequested(function () { return deferredUpload.reject(cancellation_1.cancelled()); });
                        _b.label = 1;
                    case 1:
                        _b.trys.push([1, 7, , 8]);
                        urilist_1 = [];
                        filelist_1 = [];
                        return [4 /*yield*/, this.index(targetUri, source, {
                                token: token,
                                progress: progress,
                                accept: function (_a) {
                                    var uri = _a.uri, file = _a.file;
                                    return __awaiter(_this, void 0, void 0, function () {
                                        return __generator(this, function (_b) {
                                            if (totalFiles > MAXFILENUMBER) {
                                                return [2 /*return*/];
                                            }
                                            total += file.size;
                                            totalFiles++;
                                            urilist_1.push(uri);
                                            filelist_1.push(file);
                                            return [2 /*return*/];
                                        });
                                    });
                                }
                            })];
                    case 2:
                        _b.sent();
                        reportProgress();
                        _loop_1 = function (i) {
                            var uri, file_1, dstPathWithScheme, dstPath, content, _dstUri, e_2;
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0:
                                        cancellation_1.checkCancelled(token);
                                        _a.label = 1;
                                    case 1:
                                        _a.trys.push([1, 6, , 7]);
                                        uri = urilist_1[i];
                                        file_1 = filelist_1[i];
                                        if (!(file_1.size > 0 && file_1.size < MAXFILESIZE)) return [3 /*break*/, 5];
                                        dstPathWithScheme = uri.toString();
                                        dstPath = uri.toString().substr(7);
                                        console.log('uploading ' + dstPath);
                                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                                var fr = new FileReader();
                                                fr.onload = function () {
                                                    resolve(fr.result);
                                                };
                                                fr.readAsArrayBuffer(file_1);
                                            })];
                                    case 2:
                                        content = _a.sent();
                                        _dstUri = new uri_1.default(dstPath);
                                        return [4 /*yield*/, this_1.s3fs.ensureDirExist(filesystem_browserfs_1.FileUriLite.fsPath(_dstUri.parent))];
                                    case 3:
                                        _a.sent();
                                        // console.log('writing ' + content);
                                        return [4 /*yield*/, this_1.s3fs.writeFile(filesystem_browserfs_1.FileUriLite.fsPath(_dstUri), new Uint8Array(content))];
                                    case 4:
                                        // console.log('writing ' + content);
                                        _a.sent();
                                        // await new Promise(resolve => setTimeout(resolve, 3000));
                                        done += file_1.size;
                                        doneFiles++;
                                        result.uploaded.push(dstPathWithScheme);
                                        reportProgress();
                                        if (onDidUpload) {
                                            onDidUpload(dstPathWithScheme);
                                        }
                                        _a.label = 5;
                                    case 5: return [3 /*break*/, 7];
                                    case 6:
                                        e_2 = _a.sent();
                                        deferredUpload.reject(e_2);
                                        return [3 /*break*/, 7];
                                    case 7: return [2 /*return*/];
                                }
                            });
                        };
                        this_1 = this;
                        i = 0;
                        _b.label = 3;
                    case 3:
                        if (!(i < totalFiles)) return [3 /*break*/, 6];
                        return [5 /*yield**/, _loop_1(i)];
                    case 4:
                        _b.sent();
                        _b.label = 5;
                    case 5:
                        i++;
                        return [3 /*break*/, 3];
                    case 6:
                        deferredUpload.resolve(result);
                        return [3 /*break*/, 8];
                    case 7:
                        e_1 = _b.sent();
                        deferredUpload.reject(e_1);
                        return [3 /*break*/, 8];
                    case 8: return [2 /*return*/, deferredUpload.promise];
                }
            });
        });
    };
    FileUploadService.prototype.withProgress = function (cb, _a) {
        var text = (_a === void 0 ? { text: 'Uploading Files...' } : _a).text;
        return __awaiter(this, void 0, void 0, function () {
            var cancellationSource, token, progress;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        cancellationSource = new cancellation_1.CancellationTokenSource();
                        token = cancellationSource.token;
                        return [4 /*yield*/, this.messageService.showProgress({ text: text, options: { cancelable: true } }, function () { return cancellationSource.cancel(); })];
                    case 1:
                        progress = _b.sent();
                        _b.label = 2;
                    case 2:
                        _b.trys.push([2, , 4, 5]);
                        return [4 /*yield*/, cb(progress, token)];
                    case 3: return [2 /*return*/, _b.sent()];
                    case 4:
                        progress.cancel();
                        return [7 /*endfinally*/];
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.index = function (targetUri, source, context) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!(source instanceof FormData)) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.indexFormData(targetUri, source, context)];
                    case 1:
                        _a.sent();
                        return [3 /*break*/, 4];
                    case 2: return [4 /*yield*/, this.indexDataTransfer(targetUri, source, context)];
                    case 3:
                        _a.sent();
                        _a.label = 4;
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexFormData = function (targetUri, formData, context) {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, file, e_3_1;
            var e_3, _c;
            return __generator(this, function (_d) {
                switch (_d.label) {
                    case 0:
                        _d.trys.push([0, 5, 6, 7]);
                        _a = __values(formData.getAll(FileUploadService_1.UPLOAD)), _b = _a.next();
                        _d.label = 1;
                    case 1:
                        if (!!_b.done) return [3 /*break*/, 4];
                        file = _b.value;
                        if (!(file instanceof File)) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.indexFile(targetUri, file, context)];
                    case 2:
                        _d.sent();
                        _d.label = 3;
                    case 3:
                        _b = _a.next();
                        return [3 /*break*/, 1];
                    case 4: return [3 /*break*/, 7];
                    case 5:
                        e_3_1 = _d.sent();
                        e_3 = { error: e_3_1 };
                        return [3 /*break*/, 7];
                    case 6:
                        try {
                            if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                        }
                        finally { if (e_3) throw e_3.error; }
                        return [7 /*endfinally*/];
                    case 7: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexDataTransfer = function (targetUri, dataTransfer, context) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cancellation_1.checkCancelled(context.token);
                        if (!dataTransfer.items) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.indexDataTransferItemList(targetUri, dataTransfer.items, context)];
                    case 1:
                        _a.sent();
                        return [3 /*break*/, 4];
                    case 2: return [4 /*yield*/, this.indexFileList(targetUri, dataTransfer.files, context)];
                    case 3:
                        _a.sent();
                        _a.label = 4;
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexFileList = function (targetUri, files, context) {
        return __awaiter(this, void 0, void 0, function () {
            var i, file;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < files.length)) return [3 /*break*/, 4];
                        file = files[i];
                        if (!file) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.indexFile(targetUri, file, context)];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3:
                        i++;
                        return [3 /*break*/, 1];
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexFile = function (targetUri, file, context) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, context.accept({
                            uri: targetUri.resolve(file.name),
                            file: file
                        })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexDataTransferItemList = function (targetUri, items, context) {
        return __awaiter(this, void 0, void 0, function () {
            var entries, i, entry;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cancellation_1.checkCancelled(context.token);
                        entries = [];
                        for (i = 0; i < items.length; i++) {
                            entry = items[i].webkitGetAsEntry();
                            entries.push(entry);
                        }
                        return [4 /*yield*/, this.indexEntries(targetUri, entries, context)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexEntry = function (targetUri, entry, context) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cancellation_1.checkCancelled(context.token);
                        if (!entry) {
                            return [2 /*return*/];
                        }
                        if (!entry.isDirectory) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.indexDirectoryEntry(targetUri, entry, context)];
                    case 1:
                        _a.sent();
                        return [3 /*break*/, 4];
                    case 2: return [4 /*yield*/, this.indexFileEntry(targetUri, entry, context)];
                    case 3:
                        _a.sent();
                        _a.label = 4;
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    /**
     *  Read all entries within a folder by block of 100 files or folders until the
     *  whole folder has been read.
     */
    FileUploadService.prototype.indexDirectoryEntry = function (targetUri, entry, context) {
        return __awaiter(this, void 0, void 0, function () {
            var newTargetUri;
            var _this = this;
            return __generator(this, function (_a) {
                cancellation_1.checkCancelled(context.token);
                newTargetUri = targetUri.resolve(entry.name);
                return [2 /*return*/, new Promise(function (resolve, reject) { return __awaiter(_this, void 0, void 0, function () {
                        var reader, getEntries;
                        var _this = this;
                        return __generator(this, function (_a) {
                            reader = entry.createReader();
                            getEntries = function () { return reader.readEntries(function (results) { return __awaiter(_this, void 0, void 0, function () {
                                var e_4;
                                return __generator(this, function (_a) {
                                    switch (_a.label) {
                                        case 0:
                                            _a.trys.push([0, 4, , 5]);
                                            if (!(!context.token.isCancellationRequested && results && results.length)) return [3 /*break*/, 2];
                                            return [4 /*yield*/, this.indexEntries(newTargetUri, results, context)];
                                        case 1:
                                            _a.sent();
                                            getEntries(); // loop to read all getEntries
                                            return [3 /*break*/, 3];
                                        case 2:
                                            resolve();
                                            _a.label = 3;
                                        case 3: return [3 /*break*/, 5];
                                        case 4:
                                            e_4 = _a.sent();
                                            reject(e_4);
                                            return [3 /*break*/, 5];
                                        case 5: return [2 /*return*/];
                                    }
                                });
                            }); }, reject); };
                            getEntries();
                            return [2 /*return*/];
                        });
                    }); })];
            });
        });
    };
    FileUploadService.prototype.indexEntries = function (targetUri, entries, context) {
        return __awaiter(this, void 0, void 0, function () {
            var i;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cancellation_1.checkCancelled(context.token);
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < entries.length)) return [3 /*break*/, 4];
                        return [4 /*yield*/, this.indexEntry(targetUri, entries[i], context)];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3:
                        i++;
                        return [3 /*break*/, 1];
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    FileUploadService.prototype.indexFileEntry = function (targetUri, entry, context) {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cancellation_1.checkCancelled(context.token);
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                try {
                                    entry.file(function (file) { return _this.indexFile(targetUri, file, context).then(resolve, reject); }, reject);
                                }
                                catch (e) {
                                    reject(e);
                                }
                            })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    var FileUploadService_1;
    FileUploadService.TARGET = 'target';
    FileUploadService.UPLOAD = 'upload';
    __decorate([
        inversify_1.inject(message_service_1.MessageService),
        __metadata("design:type", message_service_1.MessageService)
    ], FileUploadService.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(s3storagesystem_1.S3StorageSystem),
        __metadata("design:type", s3storagesystem_1.S3StorageSystem)
    ], FileUploadService.prototype, "s3fs", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileUploadService.prototype, "init", null);
    FileUploadService = FileUploadService_1 = __decorate([
        inversify_1.injectable()
    ], FileUploadService);
    return FileUploadService;
}());
exports.FileUploadService = FileUploadService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-upload-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/filesystem-browserfs.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/filesystem-browserfs.js ***!
  \***************************************************************************************************/
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
var vscode_languageserver_types_1 = __webpack_require__(/*! vscode-languageserver-types */ "../../node_modules/vscode-languageserver-types/lib/esm/main.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var vscode_uri_1 = __webpack_require__(/*! vscode-uri */ "../../node_modules/vscode-uri/lib/esm/index.js");
var lsp_types_1 = __webpack_require__(/*! @theia/core/lib/common/lsp-types */ "../../packages/core/lib/common/lsp-types.js");
var filesystem_1 = __webpack_require__(/*! ../common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var s3storagesystem_1 = __webpack_require__(/*! ./s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
var FileSystemBrowserOptions = /** @class */ (function () {
    function FileSystemBrowserOptions() {
    }
    FileSystemBrowserOptions.DEFAULT = {
        encoding: 'utf8',
        overwrite: false,
        recursive: true,
        moveToTrash: false
    };
    FileSystemBrowserOptions = __decorate([
        inversify_1.injectable()
    ], FileSystemBrowserOptions);
    return FileSystemBrowserOptions;
}());
exports.FileSystemBrowserOptions = FileSystemBrowserOptions;
/* eslint-disable @typescript-eslint/no-explicit-any */
var FileUriLite;
(function (FileUriLite) {
    function create(fsPath_) {
        return new uri_1.default(vscode_uri_1.default.file(fsPath_));
    }
    FileUriLite.create = create;
    function fsPath(uri) {
        if (typeof uri === 'string') {
            return fsPath(new uri_1.default(uri));
        }
        else {
            var fsPathFromVsCodeUri = uri.codeUri.fsPath;
            return fsPathFromVsCodeUri;
        }
    }
    FileUriLite.fsPath = fsPath;
})(FileUriLite = exports.FileUriLite || (exports.FileUriLite = {}));
var BrowserFileSystem = /** @class */ (function () {
    // private _s3fs_lock: boolean;
    function BrowserFileSystem(options, _s3fs) {
        if (options === void 0) { options = FileSystemBrowserOptions.DEFAULT; }
        this.options = options;
        this._s3fs = _s3fs;
    }
    BrowserFileSystem.prototype.setClient = function (client) {
        this.client = client;
    };
    BrowserFileSystem.prototype.getFileStat = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var uri_, stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        uri_ = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(uri_, 1)];
                    case 1:
                        stat = _a.sent();
                        return [2 /*return*/, stat];
                }
            });
        });
    };
    BrowserFileSystem.prototype.exists = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            return [2 /*return*/, false];
                        }
                        return [2 /*return*/, true];
                }
            });
        });
    };
    BrowserFileSystem.prototype.resolveContent = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat, encoding, contentBuffer, content;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            throw filesystem_1.FileSystemError.FileNotFound(uri);
                        }
                        if (stat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileIsDirectory(uri, 'Cannot resolve the content.');
                        }
                        encoding = this.doGetEncoding(options);
                        if (encoding !== 'utf8') {
                            throw filesystem_1.FileSystemError.FileExists(_uri, 'Unsupported File Encoding. ' + encoding);
                        }
                        return [4 /*yield*/, this._s3fs.readFile(FileUriLite.fsPath(_uri))];
                    case 2:
                        contentBuffer = _a.sent();
                        content = contentBuffer.toString();
                        return [2 /*return*/, { stat: stat, content: content }];
                }
            });
        });
    };
    BrowserFileSystem.prototype.setContent = function (file, content, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat, encoding, newStat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(file.uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            throw filesystem_1.FileSystemError.FileNotFound(file.uri);
                        }
                        if (stat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileIsDirectory(file.uri, 'Cannot set the content.');
                        }
                        encoding = this.doGetEncoding(options);
                        if (encoding !== 'utf8') {
                            throw filesystem_1.FileSystemError.FileExists(_uri, 'Unsupported File Encoding. ' + encoding);
                        }
                        // const encodedContent = iconv.encode(content, encoding);
                        return [4 /*yield*/, this._s3fs.writeFile(FileUriLite.fsPath(_uri), content)];
                    case 2:
                        // const encodedContent = iconv.encode(content, encoding);
                        _a.sent();
                        return [4 /*yield*/, this.doGetStat(_uri, 1)];
                    case 3:
                        newStat = _a.sent();
                        if (newStat) {
                            return [2 /*return*/, newStat];
                        }
                        // this.fileSystemWatcherServer.kernel_notify(file.uri);
                        throw filesystem_1.FileSystemError.FileNotFound(file.uri, 'Error occurred while writing file content.');
                }
            });
        });
    };
    BrowserFileSystem.prototype.updateContent = function (file, contentChanges, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat, encoding, contentBuffer, content, newContent, newStat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(file.uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            throw filesystem_1.FileSystemError.FileNotFound(file.uri);
                        }
                        if (stat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileIsDirectory(file.uri, 'Cannot set the content.');
                        }
                        if (contentChanges.length === 0 && !(options && options.overwriteEncoding)) {
                            return [2 /*return*/, stat];
                        }
                        encoding = this.doGetEncoding(options);
                        if (encoding !== 'utf8') {
                            throw filesystem_1.FileSystemError.FileExists(_uri, 'Unsupported File Encoding. ' + encoding);
                        }
                        return [4 /*yield*/, this._s3fs.readFile(FileUriLite.fsPath(_uri))];
                    case 2:
                        contentBuffer = _a.sent();
                        content = contentBuffer.toString();
                        newContent = this.applyContentChanges(content, contentChanges);
                        return [4 /*yield*/, this._s3fs.writeFile(FileUriLite.fsPath(_uri), newContent)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, this.doGetStat(_uri, 1)];
                    case 4:
                        newStat = _a.sent();
                        if (newStat) {
                            return [2 /*return*/, newStat];
                        }
                        // this.fileSystemWatcherServer.kernel_notify(file.uri);
                        throw filesystem_1.FileSystemError.FileNotFound(file.uri, 'Error occurred while writing file content.');
                }
            });
        });
    };
    BrowserFileSystem.prototype.applyContentChanges = function (content, contentChanges) {
        var e_1, _a;
        var document = vscode_languageserver_types_1.TextDocument.create('', '', 1, content);
        try {
            for (var contentChanges_1 = __values(contentChanges), contentChanges_1_1 = contentChanges_1.next(); !contentChanges_1_1.done; contentChanges_1_1 = contentChanges_1.next()) {
                var change = contentChanges_1_1.value;
                var newContent = void 0;
                if (lsp_types_1.TextDocumentContentChangeDelta.is(change)) {
                    var start = document.offsetAt(change.range.start);
                    var end = document.offsetAt(change.range.end);
                    newContent = document.getText().substr(0, start) + change.text + document.getText().substr(end);
                }
                else {
                    newContent = change.text;
                }
                document = vscode_languageserver_types_1.TextDocument.create(document.uri, document.languageId, document.version, newContent);
            }
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (contentChanges_1_1 && !contentChanges_1_1.done && (_a = contentChanges_1.return)) _a.call(contentChanges_1);
            }
            finally { if (e_1) throw e_1.error; }
        }
        return document.getText();
    };
    BrowserFileSystem.prototype.move = function (sourceUri, targetUri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.client) {
                            this.client.willMove(sourceUri, targetUri);
                        }
                        return [4 /*yield*/, this.doMove(sourceUri, targetUri, options)];
                    case 1:
                        result = _a.sent();
                        if (this.client) {
                            this.client.didMove(sourceUri, targetUri, false);
                        }
                        return [2 /*return*/, result];
                }
            });
        });
    };
    BrowserFileSystem.prototype.doMove = function (sourceUri, targetUri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _sourceUri, _targetUri, overwrite, _a, sourceStat, targetStat, stats;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _sourceUri = new uri_1.default(sourceUri);
                        _targetUri = new uri_1.default(targetUri);
                        overwrite = this.doGetOverwrite(options);
                        return [4 /*yield*/, Promise.all([this.doGetStat(_sourceUri, 1), this.doGetStat(_targetUri, 1)])];
                    case 1:
                        _a = __read.apply(void 0, [_b.sent(), 2]), sourceStat = _a[0], targetStat = _a[1];
                        if (!sourceStat) {
                            throw filesystem_1.FileSystemError.FileNotFound(sourceUri);
                        }
                        if (targetStat && !overwrite) {
                            throw filesystem_1.FileSystemError.FileExists(targetUri, "Did you set the 'overwrite' flag to true?");
                        }
                        // Different types. Files <-> Directory.
                        if (targetStat && sourceStat.isDirectory !== targetStat.isDirectory) {
                            if (targetStat.isDirectory) {
                                throw filesystem_1.FileSystemError.FileIsDirectory(targetStat.uri, "Cannot move '" + sourceStat.uri + "' file to an existing location.");
                            }
                            throw filesystem_1.FileSystemError.FileNotDirectory(targetStat.uri, "Cannot move '" + sourceStat.uri + "' directory to an existing location.");
                        }
                        return [4 /*yield*/, this._s3fs.ensureDirExist(FileUriLite.fsPath(_targetUri.parent))];
                    case 2:
                        _b.sent();
                        return [4 /*yield*/, this._s3fs.rename(FileUriLite.fsPath(_sourceUri), FileUriLite.fsPath(_targetUri))];
                    case 3:
                        _b.sent();
                        return [4 /*yield*/, this.doGetStat(_targetUri, 1)];
                    case 4:
                        stats = _b.sent();
                        if (stats) {
                            return [2 /*return*/, stats];
                        }
                        throw filesystem_1.FileSystemError.FileNotFound(_targetUri, "Error occurred when doing recursive move '" + sourceUri + "' to '" + targetUri + "'.");
                }
            });
        });
    };
    BrowserFileSystem.prototype.copy = function (sourceUri, targetUri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _sourceUri, _targetUri, recursive, overwrite, _a, sourceStat, targetStat, newStat;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _sourceUri = new uri_1.default(sourceUri);
                        _targetUri = new uri_1.default(targetUri);
                        recursive = this.doGetRecursive(options);
                        overwrite = this.doGetOverwrite(options);
                        return [4 /*yield*/, Promise.all([
                                this.doGetStat(_sourceUri, 0),
                                this.doGetStat(_targetUri, 0),
                            ])];
                    case 1:
                        _a = __read.apply(void 0, [_b.sent(), 2]), sourceStat = _a[0], targetStat = _a[1];
                        if (!sourceStat) {
                            throw filesystem_1.FileSystemError.FileNotFound(sourceUri);
                        }
                        if (targetStat && !overwrite) {
                            throw filesystem_1.FileSystemError.FileExists(targetUri, "Did you set the 'overwrite' flag to true?");
                        }
                        if (targetStat && targetStat.uri === sourceStat.uri) {
                            throw filesystem_1.FileSystemError.FileExists(targetUri, 'Cannot perform copy, source and destination are the same.');
                        }
                        if (targetStat && !targetStat.isDirectory && sourceStat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileExists(targetUri, 'Cannot perform copy, source is directory and destination is a file.');
                        }
                        // await this.doCopyFile(_sourceUri, _targetUri, overwrite, recursive);
                        if (!recursive && sourceStat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileExists(targetUri, 'Cannot perform copy directory when recursive is off');
                        }
                        return [4 /*yield*/, this._s3fs.ensureDirExist(FileUriLite.fsPath(_targetUri.parent))];
                    case 2:
                        _b.sent();
                        if (!sourceStat.isDirectory) return [3 /*break*/, 4];
                        return [4 /*yield*/, this._s3fs.copyFolder(FileUriLite.fsPath(_sourceUri), FileUriLite.fsPath(_targetUri))];
                    case 3:
                        _b.sent();
                        return [3 /*break*/, 6];
                    case 4: return [4 /*yield*/, this._s3fs.copyFile(FileUriLite.fsPath(_sourceUri), FileUriLite.fsPath(_targetUri))];
                    case 5:
                        _b.sent();
                        _b.label = 6;
                    case 6: return [4 /*yield*/, this.doGetStat(_targetUri, 1)];
                    case 7:
                        newStat = _b.sent();
                        if (newStat) {
                            return [2 /*return*/, newStat];
                        }
                        throw filesystem_1.FileSystemError.FileNotFound(targetUri, "Error occurred while copying " + sourceUri + " to " + targetUri + ".");
                }
            });
        });
    };
    BrowserFileSystem.prototype.createFile = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, parentUri, _a, stat, parentStat, content, encoding, newStat;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        parentUri = _uri.parent;
                        return [4 /*yield*/, Promise.all([this.doGetStat(_uri, 0), this.doGetStat(parentUri, 0)])];
                    case 1:
                        _a = __read.apply(void 0, [_b.sent(), 2]), stat = _a[0], parentStat = _a[1];
                        if (stat) {
                            throw filesystem_1.FileSystemError.FileExists(uri, 'Error occurred while creating the file.');
                        }
                        if (!!parentStat) return [3 /*break*/, 3];
                        return [4 /*yield*/, this._s3fs.ensureDirExist(FileUriLite.fsPath(parentUri))];
                    case 2:
                        _b.sent();
                        _b.label = 3;
                    case 3:
                        content = this.doGetContent(options);
                        encoding = this.doGetEncoding(options);
                        if (encoding !== 'utf8') {
                            throw filesystem_1.FileSystemError.FileExists(uri, 'Unsupported File Encoding. ' + encoding);
                        }
                        // const encodedNewContent = iconv.encode(content, encoding);
                        return [4 /*yield*/, this._s3fs.writeFile(FileUriLite.fsPath(_uri), content)];
                    case 4:
                        // const encodedNewContent = iconv.encode(content, encoding);
                        _b.sent();
                        return [4 /*yield*/, this.doGetStat(_uri, 1)];
                    case 5:
                        newStat = _b.sent();
                        if (newStat) {
                            return [2 /*return*/, newStat];
                        }
                        throw filesystem_1.FileSystemError.FileNotFound(uri, 'Error occurred while creating the file.');
                }
            });
        });
    };
    BrowserFileSystem.prototype.createFolder = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat, newStat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (stat) {
                            if (stat.isDirectory) {
                                return [2 /*return*/, stat];
                            }
                            throw filesystem_1.FileSystemError.FileExists(uri, 'Error occurred while creating the directory: path is a file.');
                        }
                        return [4 /*yield*/, this._s3fs.ensureDirExist(FileUriLite.fsPath(_uri))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, this.doGetStat(_uri, 1)];
                    case 3:
                        newStat = _a.sent();
                        if (newStat) {
                            return [2 /*return*/, newStat];
                        }
                        throw filesystem_1.FileSystemError.FileNotFound(uri, 'Error occurred while creating the directory.');
                }
            });
        });
    };
    BrowserFileSystem.prototype.touchFile = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                throw filesystem_1.FileSystemError.FileExists(uri, 'Touchfile not implemented delete');
            });
        });
    };
    BrowserFileSystem.prototype.delete = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            throw filesystem_1.FileSystemError.FileNotFound(uri);
                        }
                        return [4 /*yield*/, this._s3fs.delete(FileUriLite.fsPath(_uri))];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    BrowserFileSystem.prototype.getEncoding = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            throw filesystem_1.FileSystemError.FileNotFound(uri);
                        }
                        if (stat.isDirectory) {
                            throw filesystem_1.FileSystemError.FileIsDirectory(uri, 'Cannot get the encoding.');
                        }
                        return [2 /*return*/, this.options.encoding];
                }
            });
        });
    };
    BrowserFileSystem.prototype.guessEncoding = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                console.log('guessEncoding is dummy: ' + uri);
                return [2 /*return*/, 'utf8'];
            });
        });
    };
    BrowserFileSystem.prototype.getRoots = function () {
        return __awaiter(this, void 0, void 0, function () {
            var cwdRoot, rootUri, root;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        cwdRoot = '/';
                        rootUri = FileUriLite.create(cwdRoot);
                        return [4 /*yield*/, this.doGetStat(rootUri, 1)];
                    case 1:
                        root = _a.sent();
                        if (root) {
                            return [2 /*return*/, [root]];
                        }
                        return [2 /*return*/, []];
                }
            });
        });
    };
    BrowserFileSystem.prototype.getCurrentUserHome = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, this.getFileStat(FileUriLite.create('/').toString())];
            });
        });
    };
    BrowserFileSystem.prototype.getDrives = function () {
        return new Promise(function (resolve, reject) { resolve(['/']); });
    };
    BrowserFileSystem.prototype.dispose = function () {
        // NOOP
    };
    BrowserFileSystem.prototype.access = function (uri, mode) {
        if (mode === void 0) { mode = filesystem_1.FileAccess.Constants.F_OK; }
        return __awaiter(this, void 0, void 0, function () {
            var _uri, stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _uri = new uri_1.default(uri);
                        return [4 /*yield*/, this.doGetStat(_uri, 0)];
                    case 1:
                        stat = _a.sent();
                        if (!stat) {
                            return [2 /*return*/, false];
                        }
                        return [2 /*return*/, true];
                }
            });
        });
    };
    BrowserFileSystem.prototype.getFsPath = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                if (!uri.startsWith('file:/')) {
                    return [2 /*return*/, undefined];
                }
                else {
                    return [2 /*return*/, FileUriLite.fsPath(uri)];
                }
                return [2 /*return*/];
            });
        });
    };
    BrowserFileSystem.prototype.doGetStat = function (uri, depth) {
        return __awaiter(this, void 0, void 0, function () {
            var s3obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._s3fs.stat(FileUriLite.fsPath(uri))];
                    case 1:
                        s3obj = _a.sent();
                        if (s3obj) {
                            if (s3obj.isDir) {
                                return [2 /*return*/, this.doCreateDirectoryStat(uri, s3obj, depth)];
                            }
                            else {
                                return [2 /*return*/, this.doCreateFileStat(uri, s3obj)];
                            }
                        }
                        else {
                            return [2 /*return*/, undefined];
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    BrowserFileSystem.prototype.doCreateFileStat = function (uri, stat) {
        return {
            uri: uri.toString(),
            lastModification: stat.modifiedTime.getTime(),
            isDirectory: stat.isDir,
            size: stat.size
        };
    };
    BrowserFileSystem.prototype.doCreateDirectoryStat = function (uri, stat, depth) {
        return __awaiter(this, void 0, void 0, function () {
            var children, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        if (!(depth > 0)) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.doGetChildren(uri, depth)];
                    case 1:
                        _a = _b.sent();
                        return [3 /*break*/, 3];
                    case 2:
                        _a = [];
                        _b.label = 3;
                    case 3:
                        children = _a;
                        return [2 /*return*/, {
                                uri: uri.toString(),
                                lastModification: stat.modifiedTime.getTime(),
                                isDirectory: true,
                                children: children
                            }];
                }
            });
        });
    };
    BrowserFileSystem.prototype.doGetChildren = function (uri, depth) {
        return __awaiter(this, void 0, void 0, function () {
            var files, children;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._s3fs.readdir(FileUriLite.fsPath(uri))];
                    case 1:
                        files = _a.sent();
                        children = [];
                        files.forEach(function (v) {
                            // console.log(v.uri);
                            var _uri = new uri_1.default(v.uri);
                            // console.log(v.uri);
                            // console.log(v.isDir);
                            children.push(_this.doCreateFileStat(_uri, v));
                        });
                        return [2 /*return*/, children];
                }
            });
        });
    };
    BrowserFileSystem.prototype.doGetEncoding = function (option) {
        return option && typeof (option.encoding) !== 'undefined'
            ? option.encoding
            : this.options.encoding;
    };
    BrowserFileSystem.prototype.doGetOverwrite = function (option) {
        return option && typeof (option.overwrite) !== 'undefined'
            ? option.overwrite
            : this.options.overwrite;
    };
    BrowserFileSystem.prototype.doGetRecursive = function (option) {
        return option && typeof (option.recursive) !== 'undefined'
            ? option.recursive
            : this.options.recursive;
    };
    BrowserFileSystem.prototype.doGetContent = function (option) {
        return (option && option.content) || '';
    };
    BrowserFileSystem = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(FileSystemBrowserOptions)), __param(0, inversify_1.optional()),
        __param(1, inversify_1.inject(s3storagesystem_1.S3StorageSystem)),
        __metadata("design:paramtypes", [FileSystemBrowserOptions,
            s3storagesystem_1.S3StorageSystem])
    ], BrowserFileSystem);
    return BrowserFileSystem;
}());
exports.BrowserFileSystem = BrowserFileSystem;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/filesystem-browserfs'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/common/filesystem-utils.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/common/filesystem-utils.js ***!
  \**********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 Ericsson and others.
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
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var FileSystemUtils;
(function (FileSystemUtils) {
    /**
     * Tildify path, replacing `home` with `~` if user's `home` is present at the beginning of the path.
     * This is a non-operation for Windows.
     *
     * @param resourcePath
     * @param home
     */
    function tildifyPath(resourcePath, home) {
        var path = new common_1.Path(resourcePath);
        var isWindows = path.root && common_1.Path.isDrive(path.root.base);
        if (!isWindows && home && resourcePath.indexOf(home + "/") === 0) {
            return resourcePath.replace(home + "/", '~/');
        }
        return resourcePath;
    }
    FileSystemUtils.tildifyPath = tildifyPath;
    /**
     * Generate unique URI for a given parent which does not collide
     *
     * @param parentUri the `URI` of the parent
     * @param parent the `FileStat` of the parent
     * @param name the resource name
     * @param ext the resource extension
     */
    function generateUniqueResourceURI(parentUri, parent, name, ext) {
        if (ext === void 0) { ext = ''; }
        var children = !parent.children ? [] : parent.children.map(function (child) { return new uri_1.default(child.uri); });
        var index = 1;
        var base = name + ext;
        while (children.some(function (child) { return child.path.base === base; })) {
            index = index + 1;
            base = name + '_' + index + ext;
        }
        return parentUri.resolve(base);
    }
    FileSystemUtils.generateUniqueResourceURI = generateUniqueResourceURI;
})(FileSystemUtils = exports.FileSystemUtils || (exports.FileSystemUtils = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/common/filesystem-utils'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/common/index.js":
/*!***********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/common/index.js ***!
  \***********************************************************************************/
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
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
__export(__webpack_require__(/*! ./filesystem */ "../../packages/filesystem/lib/common/filesystem.js"));
__export(__webpack_require__(/*! ./filesystem-utils */ "../../packages/filesystem/lib/common/filesystem-utils.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/common'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=5.bundle.js.map