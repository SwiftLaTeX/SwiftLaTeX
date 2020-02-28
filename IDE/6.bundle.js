(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[6],{

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-model.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-model.js ***!
  \************************************************************************************************************/
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var file_tree_1 = __webpack_require__(/*! ../file-tree */ "../../packages/filesystem/lib/browser/file-tree/index.js");
var file_dialog_tree_1 = __webpack_require__(/*! ./file-dialog-tree */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-tree.js");
var FileDialogModel = /** @class */ (function (_super) {
    __extends(FileDialogModel, _super);
    function FileDialogModel() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.onDidOpenFileEmitter = new common_1.Emitter();
        _this._disableFileSelection = false;
        return _this;
    }
    FileDialogModel.prototype.init = function () {
        _super.prototype.init.call(this);
        this.toDispose.push(this.onDidOpenFileEmitter);
    };
    Object.defineProperty(FileDialogModel.prototype, "initialLocation", {
        /**
         * Returns the first valid location that was set by calling the `navigateTo` method. Once the initial location has a defined value, it will not change.
         * Can be `undefined`.
         */
        get: function () {
            return this._initialLocation;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(FileDialogModel.prototype, "disableFileSelection", {
        set: function (isSelectable) {
            this._disableFileSelection = isSelectable;
        },
        enumerable: true,
        configurable: true
    });
    FileDialogModel.prototype.navigateTo = function (nodeOrId) {
        return __awaiter(this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, _super.prototype.navigateTo.call(this, nodeOrId)];
                    case 1:
                        result = _a.sent();
                        if (!this._initialLocation && file_tree_1.FileStatNode.is(result)) {
                            this._initialLocation = result.uri;
                        }
                        return [2 /*return*/, result];
                }
            });
        });
    };
    Object.defineProperty(FileDialogModel.prototype, "onDidOpenFile", {
        get: function () {
            return this.onDidOpenFileEmitter.event;
        },
        enumerable: true,
        configurable: true
    });
    FileDialogModel.prototype.doOpenNode = function (node) {
        if (file_tree_1.FileNode.is(node)) {
            this.onDidOpenFileEmitter.fire(undefined);
        }
        else if (file_tree_1.DirNode.is(node)) {
            this.navigateTo(node);
        }
        else {
            _super.prototype.doOpenNode.call(this, node);
        }
    };
    FileDialogModel.prototype.getNextSelectableNode = function (node) {
        if (node === void 0) { node = this.selectedNodes[0]; }
        var nextNode = node;
        do {
            nextNode = _super.prototype.getNextSelectableNode.call(this, nextNode);
        } while (file_tree_1.FileStatNode.is(nextNode) && !this.isFileStatNodeSelectable(nextNode));
        return nextNode;
    };
    FileDialogModel.prototype.getPrevSelectableNode = function (node) {
        if (node === void 0) { node = this.selectedNodes[0]; }
        var prevNode = node;
        do {
            prevNode = _super.prototype.getPrevSelectableNode.call(this, prevNode);
        } while (file_tree_1.FileStatNode.is(prevNode) && !this.isFileStatNodeSelectable(prevNode));
        return prevNode;
    };
    FileDialogModel.prototype.isFileStatNodeSelectable = function (node) {
        return !(!node.fileStat.isDirectory && this._disableFileSelection);
    };
    __decorate([
        inversify_1.inject(file_dialog_tree_1.FileDialogTree),
        __metadata("design:type", file_dialog_tree_1.FileDialogTree)
    ], FileDialogModel.prototype, "tree", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileDialogModel.prototype, "init", null);
    FileDialogModel = __decorate([
        inversify_1.injectable()
    ], FileDialogModel);
    return FileDialogModel;
}(file_tree_1.FileTreeModel));
exports.FileDialogModel = FileDialogModel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-model'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-service.js":
/*!**************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-service.js ***!
  \**************************************************************************************************************/
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
var file_tree_1 = __webpack_require__(/*! ../file-tree */ "../../packages/filesystem/lib/browser/file-tree/index.js");
var file_dialog_1 = __webpack_require__(/*! ./file-dialog */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog.js");
exports.FileDialogService = Symbol('FileDialogService');
var DefaultFileDialogService = /** @class */ (function () {
    function DefaultFileDialogService() {
    }
    DefaultFileDialogService.prototype.showOpenDialog = function (props, folder) {
        return __awaiter(this, void 0, void 0, function () {
            var title, rootNode, dialog, value;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        title = props.title || 'Open';
                        return [4 /*yield*/, this.getRootNode(folder)];
                    case 1:
                        rootNode = _a.sent();
                        if (!rootNode) return [3 /*break*/, 4];
                        dialog = this.openFileDialogFactory(Object.assign(props, { title: title }));
                        return [4 /*yield*/, dialog.model.navigateTo(rootNode)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, dialog.open()];
                    case 3:
                        value = _a.sent();
                        if (value) {
                            if (!Array.isArray(value)) {
                                return [2 /*return*/, value.uri];
                            }
                            return [2 /*return*/, value.map(function (node) { return node.uri; })];
                        }
                        _a.label = 4;
                    case 4: return [2 /*return*/, undefined];
                }
            });
        });
    };
    DefaultFileDialogService.prototype.showSaveDialog = function (props, folder) {
        return __awaiter(this, void 0, void 0, function () {
            var title, rootNode, dialog;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        title = props.title || 'Save';
                        return [4 /*yield*/, this.getRootNode(folder)];
                    case 1:
                        rootNode = _a.sent();
                        if (!rootNode) return [3 /*break*/, 3];
                        dialog = this.saveFileDialogFactory(Object.assign(props, { title: title }));
                        return [4 /*yield*/, dialog.model.navigateTo(rootNode)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/, dialog.open()];
                    case 3: return [2 /*return*/, undefined];
                }
            });
        });
    };
    DefaultFileDialogService.prototype.getRootNode = function (folderToOpen) {
        return __awaiter(this, void 0, void 0, function () {
            var folder, _a, folderUri, rootUri, rootStat;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = folderToOpen;
                        if (_a) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.fileSystem.getCurrentUserHome()];
                    case 1:
                        _a = (_b.sent());
                        _b.label = 2;
                    case 2:
                        folder = _a;
                        if (!folder) return [3 /*break*/, 4];
                        folderUri = new uri_1.default(folder.uri);
                        rootUri = folder.isDirectory ? folderUri : folderUri.parent;
                        return [4 /*yield*/, this.fileSystem.getFileStat(rootUri.toString())];
                    case 3:
                        rootStat = _b.sent();
                        if (rootStat) {
                            return [2 /*return*/, file_tree_1.DirNode.createRoot(rootStat)];
                        }
                        _b.label = 4;
                    case 4: return [2 /*return*/, undefined];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(common_1.FileSystem),
        __metadata("design:type", Object)
    ], DefaultFileDialogService.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(file_dialog_1.OpenFileDialogFactory),
        __metadata("design:type", Function)
    ], DefaultFileDialogService.prototype, "openFileDialogFactory", void 0);
    __decorate([
        inversify_1.inject(browser_1.LabelProvider),
        __metadata("design:type", browser_1.LabelProvider)
    ], DefaultFileDialogService.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(file_dialog_1.SaveFileDialogFactory),
        __metadata("design:type", Function)
    ], DefaultFileDialogService.prototype, "saveFileDialogFactory", void 0);
    DefaultFileDialogService = __decorate([
        inversify_1.injectable()
    ], DefaultFileDialogService);
    return DefaultFileDialogService;
}());
exports.DefaultFileDialogService = DefaultFileDialogService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-tree-filters-renderer.js":
/*!****************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-tree-filters-renderer.js ***!
  \****************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 Red Hat, Inc. and others.
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
Object.defineProperty(exports, "__esModule", { value: true });
var react_renderer_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/react-renderer */ "../../packages/core/lib/browser/widgets/react-renderer.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
exports.FILE_TREE_FILTERS_LIST_CLASS = 'theia-FileTreeFiltersList';
/**
 * A set of file filters that are used by the dialog. Each entry is a human readable label,
 * like "TypeScript", and an array of extensions, e.g.
 * ```ts
 * {
 * 	'Images': ['png', 'jpg']
 * 	'TypeScript': ['ts', 'tsx']
 * }
 * ```
 */
var FileDialogTreeFilters = /** @class */ (function () {
    function FileDialogTreeFilters() {
    }
    return FileDialogTreeFilters;
}());
exports.FileDialogTreeFilters = FileDialogTreeFilters;
var FileDialogTreeFiltersRenderer = /** @class */ (function (_super) {
    __extends(FileDialogTreeFiltersRenderer, _super);
    function FileDialogTreeFiltersRenderer(filters, fileDialogTree) {
        var _this = _super.call(this) || this;
        _this.filters = filters;
        _this.fileDialogTree = fileDialogTree;
        _this.handleFilterChanged = function (e) { return _this.onFilterChanged(e); };
        return _this;
    }
    FileDialogTreeFiltersRenderer.prototype.doRender = function () {
        var _this = this;
        if (!this.filters) {
            return undefined;
        }
        var fileTypes = ['All Files'];
        Object.keys(this.filters).forEach(function (element) {
            fileTypes.push(element);
        });
        var options = fileTypes.map(function (value) { return _this.renderLocation(value); });
        return React.createElement("select", { className: 'theia-select ' + exports.FILE_TREE_FILTERS_LIST_CLASS, onChange: this.handleFilterChanged }, options);
    };
    FileDialogTreeFiltersRenderer.prototype.renderLocation = function (value) {
        return React.createElement("option", { value: value, key: value }, value);
    };
    FileDialogTreeFiltersRenderer.prototype.onFilterChanged = function (e) {
        var locationList = this.locationList;
        if (locationList) {
            var value = locationList.value;
            var filters = this.filters[value];
            this.fileDialogTree.setFilter(filters);
        }
        e.preventDefault();
        e.stopPropagation();
    };
    Object.defineProperty(FileDialogTreeFiltersRenderer.prototype, "locationList", {
        get: function () {
            var locationList = this.host.getElementsByClassName(exports.FILE_TREE_FILTERS_LIST_CLASS)[0];
            if (locationList instanceof HTMLSelectElement) {
                return locationList;
            }
            return undefined;
        },
        enumerable: true,
        configurable: true
    });
    return FileDialogTreeFiltersRenderer;
}(react_renderer_1.ReactRenderer));
exports.FileDialogTreeFiltersRenderer = FileDialogTreeFiltersRenderer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-tree-filters-renderer'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-tree.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-tree.js ***!
  \***********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2018 Red Hat, Inc. and others.
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
var file_tree_1 = __webpack_require__(/*! ../file-tree */ "../../packages/filesystem/lib/browser/file-tree/index.js");
var FileDialogTree = /** @class */ (function (_super) {
    __extends(FileDialogTree, _super);
    function FileDialogTree() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        /**
         * Extensions for files to be shown
         */
        _this.fileExtensions = [];
        return _this;
    }
    /**
     * Sets extensions for filtering files
     *
     * @param fileExtensions array of extensions
     */
    FileDialogTree.prototype.setFilter = function (fileExtensions) {
        this.fileExtensions = fileExtensions.slice();
        this.refresh();
    };
    FileDialogTree.prototype.toNodes = function (fileStat, parent) {
        return __awaiter(this, void 0, void 0, function () {
            var result;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!fileStat.children) {
                            return [2 /*return*/, []];
                        }
                        return [4 /*yield*/, Promise.all(fileStat.children
                                .filter(function (child) { return _this.isVisible(child); })
                                .map(function (child) { return _this.toNode(child, parent); }))];
                    case 1:
                        result = _a.sent();
                        return [2 /*return*/, result.sort(file_tree_1.DirNode.compare)];
                }
            });
        });
    };
    /**
     * Determines whether file or folder can be shown
     *
     * @param fileStat resource to check
     */
    FileDialogTree.prototype.isVisible = function (fileStat) {
        if (fileStat.isDirectory) {
            return true;
        }
        if (this.fileExtensions.length === 0) {
            return true;
        }
        return !this.fileExtensions.every(function (value) { return !fileStat.uri.endsWith('.' + value); });
    };
    FileDialogTree = __decorate([
        inversify_1.injectable()
    ], FileDialogTree);
    return FileDialogTree;
}(file_tree_1.FileTree));
exports.FileDialogTree = FileDialogTree;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-tree'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-widget.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-widget.js ***!
  \*************************************************************************************************************/
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
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
};
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var file_tree_1 = __webpack_require__(/*! ../file-tree */ "../../packages/filesystem/lib/browser/file-tree/index.js");
var file_dialog_model_1 = __webpack_require__(/*! ./file-dialog-model */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-model.js");
exports.FILE_DIALOG_CLASS = 'theia-FileDialog';
exports.NOT_SELECTABLE_CLASS = 'theia-mod-not-selectable';
var FileDialogWidget = /** @class */ (function (_super) {
    __extends(FileDialogWidget, _super);
    function FileDialogWidget(props, model, contextMenuRenderer) {
        var _this = _super.call(this, props, model, contextMenuRenderer) || this;
        _this.props = props;
        _this.model = model;
        _this._disableFileSelection = false;
        _this.addClass(exports.FILE_DIALOG_CLASS);
        return _this;
    }
    Object.defineProperty(FileDialogWidget.prototype, "disableFileSelection", {
        set: function (isSelectable) {
            this._disableFileSelection = isSelectable;
            this.model.disableFileSelection = isSelectable;
        },
        enumerable: true,
        configurable: true
    });
    FileDialogWidget.prototype.createNodeAttributes = function (node, props) {
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        var attr = _super.prototype.createNodeAttributes.call(this, node, props);
        if (this.shouldDisableSelection(node)) {
            var keys = Object.keys(attr);
            keys.forEach(function (k) {
                if (['className', 'style', 'title'].indexOf(k) < 0) {
                    delete attr[k];
                }
            });
        }
        return attr;
    };
    FileDialogWidget.prototype.createNodeClassNames = function (node, props) {
        var classNames = _super.prototype.createNodeClassNames.call(this, node, props);
        if (this.shouldDisableSelection(node)) {
            [browser_1.SELECTED_CLASS, browser_1.FOCUS_CLASS].forEach(function (name) {
                var ind = classNames.indexOf(name);
                if (ind >= 0) {
                    classNames.splice(ind, 1);
                }
            });
            classNames.push(exports.NOT_SELECTABLE_CLASS);
        }
        return classNames;
    };
    FileDialogWidget.prototype.shouldDisableSelection = function (node) {
        return file_tree_1.FileStatNode.is(node) && !node.fileStat.isDirectory && this._disableFileSelection;
    };
    FileDialogWidget = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_1.TreeProps)),
        __param(1, inversify_1.inject(file_dialog_model_1.FileDialogModel)),
        __param(2, inversify_1.inject(browser_1.ContextMenuRenderer)),
        __metadata("design:paramtypes", [Object, file_dialog_model_1.FileDialogModel, Object])
    ], FileDialogWidget);
    return FileDialogWidget;
}(file_tree_1.FileTreeWidget));
exports.FileDialogWidget = FileDialogWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var location_1 = __webpack_require__(/*! ../location */ "../../packages/filesystem/lib/browser/location/index.js");
var file_dialog_widget_1 = __webpack_require__(/*! ./file-dialog-widget */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-widget.js");
var file_dialog_tree_filters_renderer_1 = __webpack_require__(/*! ./file-dialog-tree-filters-renderer */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-tree-filters-renderer.js");
var widgets_1 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
exports.OpenFileDialogFactory = Symbol('OpenFileDialogFactory');
exports.SaveFileDialogFactory = Symbol('SaveFileDialogFactory');
exports.SAVE_DIALOG_CLASS = 'theia-SaveFileDialog';
exports.NAVIGATION_PANEL_CLASS = 'theia-NavigationPanel';
exports.NAVIGATION_BACK_CLASS = 'theia-NavigationBack';
exports.NAVIGATION_FORWARD_CLASS = 'theia-NavigationForward';
exports.NAVIGATION_HOME_CLASS = 'theia-NavigationHome';
exports.NAVIGATION_LOCATION_LIST_PANEL_CLASS = 'theia-LocationListPanel';
exports.FILTERS_PANEL_CLASS = 'theia-FiltersPanel';
exports.FILTERS_LABEL_CLASS = 'theia-FiltersLabel';
exports.FILTERS_LIST_PANEL_CLASS = 'theia-FiltersListPanel';
exports.FILENAME_PANEL_CLASS = 'theia-FileNamePanel';
exports.FILENAME_LABEL_CLASS = 'theia-FileNameLabel';
exports.FILENAME_TEXTFIELD_CLASS = 'theia-FileNameTextField';
exports.CONTROL_PANEL_CLASS = 'theia-ControlPanel';
var FileDialogProps = /** @class */ (function (_super) {
    __extends(FileDialogProps, _super);
    function FileDialogProps() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return FileDialogProps;
}(browser_2.DialogProps));
exports.FileDialogProps = FileDialogProps;
var OpenFileDialogProps = /** @class */ (function (_super) {
    __extends(OpenFileDialogProps, _super);
    function OpenFileDialogProps() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    OpenFileDialogProps = __decorate([
        inversify_1.injectable()
    ], OpenFileDialogProps);
    return OpenFileDialogProps;
}(FileDialogProps));
exports.OpenFileDialogProps = OpenFileDialogProps;
var SaveFileDialogProps = /** @class */ (function (_super) {
    __extends(SaveFileDialogProps, _super);
    function SaveFileDialogProps() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    SaveFileDialogProps = __decorate([
        inversify_1.injectable()
    ], SaveFileDialogProps);
    return SaveFileDialogProps;
}(FileDialogProps));
exports.SaveFileDialogProps = SaveFileDialogProps;
var FileDialog = /** @class */ (function (_super) {
    __extends(FileDialog, _super);
    function FileDialog(props, widget) {
        var _this = _super.call(this, props) || this;
        _this.props = props;
        _this.widget = widget;
        _this.treePanel = new widgets_1.Panel();
        _this.treePanel.addWidget(_this.widget);
        _this.toDispose.push(_this.treePanel);
        _this.toDispose.push(_this.model.onChanged(function () { return _this.update(); }));
        _this.toDispose.push(_this.model.onDidOpenFile(function () { return _this.accept(); }));
        _this.toDispose.push(_this.model.onSelectionChanged(function () { return _this.update(); }));
        var navigationPanel = document.createElement('div');
        navigationPanel.classList.add(exports.NAVIGATION_PANEL_CLASS);
        _this.contentNode.appendChild(navigationPanel);
        navigationPanel.appendChild(_this.back = browser_2.createIconButton('fa', 'fa-chevron-left'));
        _this.back.classList.add(exports.NAVIGATION_BACK_CLASS);
        _this.back.title = 'Navigate Back';
        navigationPanel.appendChild(_this.forward = browser_2.createIconButton('fa', 'fa-chevron-right'));
        _this.forward.classList.add(exports.NAVIGATION_FORWARD_CLASS);
        _this.forward.title = 'Navigate Forward';
        navigationPanel.appendChild(_this.home = browser_2.createIconButton('fa', 'fa-home'));
        _this.home.classList.add(exports.NAVIGATION_HOME_CLASS);
        _this.home.title = 'Go To Initial Location';
        _this.locationListRenderer = _this.createLocationListRenderer();
        _this.locationListRenderer.host.classList.add(exports.NAVIGATION_LOCATION_LIST_PANEL_CLASS);
        navigationPanel.appendChild(_this.locationListRenderer.host);
        _this.treeFiltersRenderer = _this.createFileTreeFiltersRenderer();
        return _this;
    }
    Object.defineProperty(FileDialog.prototype, "model", {
        get: function () {
            return this.widget.model;
        },
        enumerable: true,
        configurable: true
    });
    FileDialog.prototype.createLocationListRenderer = function () {
        return new location_1.LocationListRenderer(this.model);
    };
    FileDialog.prototype.createFileTreeFiltersRenderer = function () {
        if (this.props.filters) {
            return new file_dialog_tree_filters_renderer_1.FileDialogTreeFiltersRenderer(this.props.filters, this.widget.model.tree);
        }
        return undefined;
    };
    FileDialog.prototype.onUpdateRequest = function (msg) {
        _super.prototype.onUpdateRequest.call(this, msg);
        browser_2.setEnabled(this.back, this.model.canNavigateBackward());
        browser_2.setEnabled(this.forward, this.model.canNavigateForward());
        browser_2.setEnabled(this.home, !!this.model.initialLocation
            && !!this.model.location
            && this.model.initialLocation.toString() !== this.model.location.toString());
        this.locationListRenderer.render();
        if (this.treeFiltersRenderer) {
            this.treeFiltersRenderer.render();
        }
        this.widget.update();
    };
    FileDialog.prototype.appendFiltersPanel = function () {
        if (this.treeFiltersRenderer) {
            var filtersPanel = document.createElement('div');
            filtersPanel.classList.add(exports.FILTERS_PANEL_CLASS);
            this.contentNode.appendChild(filtersPanel);
            var titlePanel = document.createElement('div');
            titlePanel.innerHTML = 'Format:';
            titlePanel.classList.add(exports.FILTERS_LABEL_CLASS);
            filtersPanel.appendChild(titlePanel);
            this.treeFiltersRenderer.host.classList.add(exports.FILTERS_LIST_PANEL_CLASS);
            filtersPanel.appendChild(this.treeFiltersRenderer.host);
        }
    };
    FileDialog.prototype.onAfterAttach = function (msg) {
        var _this = this;
        browser_2.Widget.attach(this.treePanel, this.contentNode);
        this.toDisposeOnDetach.push(common_1.Disposable.create(function () {
            browser_2.Widget.detach(_this.treePanel);
            _this.locationListRenderer.dispose();
            if (_this.treeFiltersRenderer) {
                _this.treeFiltersRenderer.dispose();
            }
        }));
        this.appendFiltersPanel();
        this.appendCloseButton('Cancel');
        this.appendAcceptButton(this.getAcceptButtonLabel());
        this.addKeyListener(this.back, browser_1.Key.ENTER, function () { return _this.model.navigateBackward(); }, 'click');
        this.addKeyListener(this.forward, browser_1.Key.ENTER, function () { return _this.model.navigateForward(); }, 'click');
        this.addKeyListener(this.home, browser_1.Key.ENTER, function () {
            if (_this.model.initialLocation) {
                _this.model.location = _this.model.initialLocation;
            }
        }, 'click');
        _super.prototype.onAfterAttach.call(this, msg);
    };
    FileDialog.prototype.onActivateRequest = function (msg) {
        this.widget.activate();
    };
    FileDialog = __decorate([
        __param(0, inversify_1.inject(FileDialogProps)),
        __param(1, inversify_1.inject(file_dialog_widget_1.FileDialogWidget)),
        __metadata("design:paramtypes", [FileDialogProps,
            file_dialog_widget_1.FileDialogWidget])
    ], FileDialog);
    return FileDialog;
}(browser_2.AbstractDialog));
exports.FileDialog = FileDialog;
var OpenFileDialog = /** @class */ (function (_super) {
    __extends(OpenFileDialog, _super);
    function OpenFileDialog(props, widget) {
        var _this = _super.call(this, props, widget) || this;
        _this.props = props;
        _this.widget = widget;
        if (props.canSelectFiles !== undefined) {
            _this.widget.disableFileSelection = !props.canSelectFiles;
        }
        return _this;
    }
    OpenFileDialog.prototype.getAcceptButtonLabel = function () {
        return this.props.openLabel ? this.props.openLabel : 'Open';
    };
    OpenFileDialog.prototype.isValid = function (value) {
        if (value && !this.props.canSelectMany && value instanceof Array) {
            return 'You can select only one item';
        }
        return '';
    };
    Object.defineProperty(OpenFileDialog.prototype, "value", {
        get: function () {
            if (this.widget.model.selectedFileStatNodes.length === 1) {
                return this.widget.model.selectedFileStatNodes[0];
            }
            else {
                return this.widget.model.selectedFileStatNodes;
            }
        },
        enumerable: true,
        configurable: true
    });
    OpenFileDialog.prototype.accept = function () {
        return __awaiter(this, void 0, void 0, function () {
            var selection;
            return __generator(this, function (_a) {
                selection = this.value;
                if (!this.props.canSelectFolders
                    && !Array.isArray(selection)
                    && selection.fileStat.isDirectory) {
                    this.widget.model.openNode(selection);
                    return [2 /*return*/];
                }
                _super.prototype.accept.call(this);
                return [2 /*return*/];
            });
        });
    };
    OpenFileDialog = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(OpenFileDialogProps)),
        __param(1, inversify_1.inject(file_dialog_widget_1.FileDialogWidget)),
        __metadata("design:paramtypes", [OpenFileDialogProps,
            file_dialog_widget_1.FileDialogWidget])
    ], OpenFileDialog);
    return OpenFileDialog;
}(FileDialog));
exports.OpenFileDialog = OpenFileDialog;
var SaveFileDialog = /** @class */ (function (_super) {
    __extends(SaveFileDialog, _super);
    function SaveFileDialog(props, widget) {
        var _this = _super.call(this, props, widget) || this;
        _this.props = props;
        _this.widget = widget;
        widget.addClass(exports.SAVE_DIALOG_CLASS);
        return _this;
    }
    SaveFileDialog.prototype.getAcceptButtonLabel = function () {
        return this.props.saveLabel ? this.props.saveLabel : 'Save';
    };
    SaveFileDialog.prototype.onUpdateRequest = function (msg) {
        // Update file name field when changing a selection
        if (this.fileNameField) {
            if (this.widget.model.selectedFileStatNodes.length === 1) {
                var node = this.widget.model.selectedFileStatNodes[0];
                if (!node.fileStat.isDirectory) {
                    this.fileNameField.value = this.labelProvider.getName(node);
                }
            }
            else {
                this.fileNameField.value = '';
            }
        }
        // Continue updating the dialog
        _super.prototype.onUpdateRequest.call(this, msg);
    };
    SaveFileDialog.prototype.isValid = function (value) {
        if (this.fileNameField && this.fileNameField.value) {
            return '';
        }
        return false;
    };
    Object.defineProperty(SaveFileDialog.prototype, "value", {
        get: function () {
            if (this.fileNameField && this.widget.model.selectedFileStatNodes.length === 1) {
                var node = this.widget.model.selectedFileStatNodes[0];
                if (node.fileStat.isDirectory) {
                    return node.uri.resolve(this.fileNameField.value);
                }
                return node.uri.parent.resolve(this.fileNameField.value);
            }
            return undefined;
        },
        enumerable: true,
        configurable: true
    });
    SaveFileDialog.prototype.onAfterAttach = function (msg) {
        var _this = this;
        _super.prototype.onAfterAttach.call(this, msg);
        var fileNamePanel = document.createElement('div');
        fileNamePanel.classList.add(exports.FILENAME_PANEL_CLASS);
        this.contentNode.appendChild(fileNamePanel);
        var titlePanel = document.createElement('div');
        titlePanel.innerHTML = 'Name:';
        titlePanel.classList.add(exports.FILENAME_LABEL_CLASS);
        fileNamePanel.appendChild(titlePanel);
        this.fileNameField = document.createElement('input');
        this.fileNameField.type = 'text';
        this.fileNameField.classList.add('theia-input', exports.FILENAME_TEXTFIELD_CLASS);
        this.fileNameField.value = this.props.inputValue || '';
        fileNamePanel.appendChild(this.fileNameField);
        this.fileNameField.onkeyup = function () { return _this.validate(); };
    };
    __decorate([
        inversify_1.inject(browser_1.LabelProvider),
        __metadata("design:type", browser_1.LabelProvider)
    ], SaveFileDialog.prototype, "labelProvider", void 0);
    SaveFileDialog = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(SaveFileDialogProps)),
        __param(1, inversify_1.inject(file_dialog_widget_1.FileDialogWidget)),
        __metadata("design:paramtypes", [SaveFileDialogProps,
            file_dialog_widget_1.FileDialogWidget])
    ], SaveFileDialog);
    return SaveFileDialog;
}(FileDialog));
exports.SaveFileDialog = SaveFileDialog;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-tree/file-tree-container.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/file-tree-container.js ***!
  \************************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var file_tree_1 = __webpack_require__(/*! ./file-tree */ "../../packages/filesystem/lib/browser/file-tree/file-tree.js");
var file_tree_model_1 = __webpack_require__(/*! ./file-tree-model */ "../../packages/filesystem/lib/browser/file-tree/file-tree-model.js");
var file_tree_widget_1 = __webpack_require__(/*! ./file-tree-widget */ "../../packages/filesystem/lib/browser/file-tree/file-tree-widget.js");
function createFileTreeContainer(parent) {
    var child = browser_1.createTreeContainer(parent);
    child.unbind(browser_1.TreeImpl);
    child.bind(file_tree_1.FileTree).toSelf();
    child.rebind(browser_1.Tree).toService(file_tree_1.FileTree);
    child.unbind(browser_1.TreeModelImpl);
    child.bind(file_tree_model_1.FileTreeModel).toSelf();
    child.rebind(browser_1.TreeModel).toService(file_tree_model_1.FileTreeModel);
    child.unbind(browser_1.TreeWidget);
    child.bind(file_tree_widget_1.FileTreeWidget).toSelf();
    return child;
}
exports.createFileTreeContainer = createFileTreeContainer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree/file-tree-container'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-tree/file-tree-model.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/file-tree-model.js ***!
  \********************************************************************************************************/
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
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var common_1 = __webpack_require__(/*! ../../common */ "../../packages/filesystem/lib/common/index.js");
var filesystem_watcher_1 = __webpack_require__(/*! ../filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var file_tree_1 = __webpack_require__(/*! ./file-tree */ "../../packages/filesystem/lib/browser/file-tree/file-tree.js");
var label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/label-provider */ "../../packages/core/lib/browser/label-provider.js");
var FileTreeModel = /** @class */ (function (_super) {
    __extends(FileTreeModel, _super);
    function FileTreeModel() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    FileTreeModel.prototype.init = function () {
        var _this = this;
        _super.prototype.init.call(this);
        this.toDispose.push(this.watcher.onFilesChanged(function (changes) { return _this.onFilesChanged(changes); }));
        this.toDispose.push(this.watcher.onDidMove(function (move) { return _this.onDidMove(move); }));
    };
    Object.defineProperty(FileTreeModel.prototype, "location", {
        get: function () {
            var root = this.root;
            if (file_tree_1.FileStatNode.is(root)) {
                return root.uri;
            }
            return undefined;
        },
        set: function (uri) {
            var _this = this;
            if (uri) {
                this.fileSystem.getFileStat(uri.toString()).then(function (fileStat) { return __awaiter(_this, void 0, void 0, function () {
                    var node;
                    return __generator(this, function (_a) {
                        if (fileStat) {
                            node = file_tree_1.DirNode.createRoot(fileStat);
                            this.navigateTo(node);
                        }
                        return [2 /*return*/];
                    });
                }); });
            }
            else {
                this.navigateTo(undefined);
            }
        },
        enumerable: true,
        configurable: true
    });
    FileTreeModel.prototype.drives = function () {
        return __awaiter(this, void 0, void 0, function () {
            var drives, e_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 3]);
                        return [4 /*yield*/, this.fileSystem.getDrives()];
                    case 1:
                        drives = _a.sent();
                        return [2 /*return*/, drives.map(function (uri) { return new uri_1.default(uri); })];
                    case 2:
                        e_1 = _a.sent();
                        this.logger.error('Error when loading drives.', e_1);
                        return [2 /*return*/, []];
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    Object.defineProperty(FileTreeModel.prototype, "selectedFileStatNodes", {
        get: function () {
            return this.selectedNodes.filter(file_tree_1.FileStatNode.is);
        },
        enumerable: true,
        configurable: true
    });
    FileTreeModel.prototype.getNodesByUri = function (uri) {
        var node;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    node = this.getNode(uri.toString());
                    if (!node) return [3 /*break*/, 2];
                    return [4 /*yield*/, node];
                case 1:
                    _a.sent();
                    _a.label = 2;
                case 2: return [2 /*return*/];
            }
        });
    };
    /**
     * to workaround https://github.com/Axosoft/nsfw/issues/42
     */
    FileTreeModel.prototype.onDidMove = function (move) {
        if (filesystem_watcher_1.FileMoveEvent.isRename(move)) {
            return;
        }
        this.refreshAffectedNodes([
            move.sourceUri,
            move.targetUri
        ]);
    };
    FileTreeModel.prototype.onFilesChanged = function (changes) {
        if (!this.refreshAffectedNodes(this.getAffectedUris(changes)) && this.isRootAffected(changes)) {
            this.refresh();
        }
    };
    FileTreeModel.prototype.isRootAffected = function (changes) {
        var root = this.root;
        if (file_tree_1.FileStatNode.is(root)) {
            return changes.some(function (change) {
                return change.type < filesystem_watcher_1.FileChangeType.DELETED && change.uri.toString() === root.uri.toString();
            });
        }
        return false;
    };
    FileTreeModel.prototype.getAffectedUris = function (changes) {
        var _this = this;
        return changes.filter(function (change) { return !_this.isFileContentChanged(change); }).map(function (change) { return change.uri; });
    };
    FileTreeModel.prototype.isFileContentChanged = function (change) {
        return change.type === filesystem_watcher_1.FileChangeType.UPDATED && file_tree_1.FileNode.is(this.getNodesByUri(change.uri).next().value);
    };
    FileTreeModel.prototype.refreshAffectedNodes = function (uris) {
        var e_2, _a;
        var nodes = this.getAffectedNodes(uris);
        try {
            for (var _b = __values(nodes.values()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var node = _c.value;
                this.refresh(node);
            }
        }
        catch (e_2_1) { e_2 = { error: e_2_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_2) throw e_2.error; }
        }
        return nodes.size !== 0;
    };
    FileTreeModel.prototype.getAffectedNodes = function (uris) {
        var e_3, _a, e_4, _b;
        var nodes = new Map();
        try {
            for (var uris_1 = __values(uris), uris_1_1 = uris_1.next(); !uris_1_1.done; uris_1_1 = uris_1.next()) {
                var uri = uris_1_1.value;
                try {
                    for (var _c = (e_4 = void 0, __values(this.getNodesByUri(uri.parent))), _d = _c.next(); !_d.done; _d = _c.next()) {
                        var node = _d.value;
                        if (file_tree_1.DirNode.is(node) && node.expanded) {
                            nodes.set(node.id, node);
                        }
                    }
                }
                catch (e_4_1) { e_4 = { error: e_4_1 }; }
                finally {
                    try {
                        if (_d && !_d.done && (_b = _c.return)) _b.call(_c);
                    }
                    finally { if (e_4) throw e_4.error; }
                }
            }
        }
        catch (e_3_1) { e_3 = { error: e_3_1 }; }
        finally {
            try {
                if (uris_1_1 && !uris_1_1.done && (_a = uris_1.return)) _a.call(uris_1);
            }
            finally { if (e_3) throw e_3.error; }
        }
        return nodes;
    };
    FileTreeModel.prototype.copy = function (uri) {
        if (uri.scheme !== 'file') {
            return false;
        }
        var node = this.selectedFileStatNodes[0];
        if (!node) {
            return false;
        }
        var targetUri = node.uri.resolve(uri.path.base);
        /* Check if the folder is copied on itself */
        var sourcePath = uri.path.toString();
        var targetPath = node.uri.path.toString();
        if (sourcePath === targetPath) {
            return false;
        }
        this.fileSystem.copy(uri.toString(), targetUri.toString());
        return true;
    };
    /**
     * Move the given source file or directory to the given target directory.
     */
    FileTreeModel.prototype.move = function (source, target) {
        return __awaiter(this, void 0, void 0, function () {
            var sourceUri, name_1, targetUri, fileExistsInTarget, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        if (!(file_tree_1.DirNode.is(target) && file_tree_1.FileStatNode.is(source))) return [3 /*break*/, 5];
                        sourceUri = source.uri.toString();
                        if (target.uri.toString() === sourceUri) { /*  Folder on itself */
                            return [2 /*return*/];
                        }
                        name_1 = source.uri.displayName;
                        targetUri = target.uri.resolve(name_1).toString();
                        if (!(sourceUri !== targetUri)) return [3 /*break*/, 5];
                        return [4 /*yield*/, this.fileSystem.exists(targetUri)];
                    case 1:
                        fileExistsInTarget = _b.sent();
                        _a = !fileExistsInTarget;
                        if (_a) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.shouldReplace(name_1)];
                    case 2:
                        _a = (_b.sent());
                        _b.label = 3;
                    case 3:
                        if (!_a) return [3 /*break*/, 5];
                        return [4 /*yield*/, this.fileSystem.move(sourceUri, targetUri, { overwrite: true })];
                    case 4:
                        _b.sent();
                        _b.label = 5;
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    FileTreeModel.prototype.shouldReplace = function (fileName) {
        return __awaiter(this, void 0, void 0, function () {
            var dialog;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        dialog = new browser_1.ConfirmDialog({
                            title: 'Replace file',
                            msg: "File '" + fileName + "' already exists in the destination folder. Do you want to replace it?",
                            ok: 'Yes',
                            cancel: 'No'
                        });
                        return [4 /*yield*/, dialog.open()];
                    case 1: return [2 /*return*/, !!(_a.sent())];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(label_provider_1.LabelProvider),
        __metadata("design:type", label_provider_1.LabelProvider)
    ], FileTreeModel.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(common_1.FileSystem),
        __metadata("design:type", Object)
    ], FileTreeModel.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(filesystem_watcher_1.FileSystemWatcher),
        __metadata("design:type", filesystem_watcher_1.FileSystemWatcher)
    ], FileTreeModel.prototype, "watcher", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileTreeModel.prototype, "init", null);
    FileTreeModel = __decorate([
        inversify_1.injectable()
    ], FileTreeModel);
    return FileTreeModel;
}(browser_1.TreeModelImpl));
exports.FileTreeModel = FileTreeModel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree/file-tree-model'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-tree/file-tree-widget.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/file-tree-widget.js ***!
  \*********************************************************************************************************/
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
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var selection_1 = __webpack_require__(/*! @theia/core/lib/common/selection */ "../../packages/core/lib/common/selection.js");
var cancellation_1 = __webpack_require__(/*! @theia/core/lib/common/cancellation */ "../../packages/core/lib/common/cancellation.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var file_upload_service_1 = __webpack_require__(/*! ../file-upload-service */ "../../packages/filesystem/lib/browser/file-upload-service.js");
var file_tree_1 = __webpack_require__(/*! ./file-tree */ "../../packages/filesystem/lib/browser/file-tree/file-tree.js");
var file_tree_model_1 = __webpack_require__(/*! ./file-tree-model */ "../../packages/filesystem/lib/browser/file-tree/file-tree-model.js");
var icon_theme_service_1 = __webpack_require__(/*! @theia/core/lib/browser/icon-theme-service */ "../../packages/core/lib/browser/icon-theme-service.js");
exports.FILE_TREE_CLASS = 'theia-FileTree';
exports.FILE_STAT_NODE_CLASS = 'theia-FileStatNode';
exports.DIR_NODE_CLASS = 'theia-DirNode';
exports.FILE_STAT_ICON_CLASS = 'theia-FileStatIcon';
var FileTreeWidget = /** @class */ (function (_super) {
    __extends(FileTreeWidget, _super);
    function FileTreeWidget(props, model, contextMenuRenderer) {
        var _this = _super.call(this, props, model, contextMenuRenderer) || this;
        _this.props = props;
        _this.model = model;
        _this.toCancelNodeExpansion = new common_1.DisposableCollection();
        _this.addClass(exports.FILE_TREE_CLASS);
        _this.toDispose.push(_this.toCancelNodeExpansion);
        return _this;
    }
    FileTreeWidget.prototype.createNodeClassNames = function (node, props) {
        var classNames = _super.prototype.createNodeClassNames.call(this, node, props);
        if (file_tree_1.FileStatNode.is(node)) {
            classNames.push(exports.FILE_STAT_NODE_CLASS);
        }
        if (file_tree_1.DirNode.is(node)) {
            classNames.push(exports.DIR_NODE_CLASS);
        }
        return classNames;
    };
    FileTreeWidget.prototype.renderIcon = function (node, props) {
        var icon = this.toNodeIcon(node);
        if (icon) {
            return React.createElement("div", { className: icon + ' file-icon' });
        }
        // eslint-disable-next-line no-null/no-null
        return null;
    };
    FileTreeWidget.prototype.createContainerAttributes = function () {
        var _this = this;
        var attrs = _super.prototype.createContainerAttributes.call(this);
        return __assign({}, attrs, { onDragEnter: function (event) { return _this.handleDragEnterEvent(_this.model.root, event); }, onDragOver: function (event) { return _this.handleDragOverEvent(_this.model.root, event); }, onDragLeave: function (event) { return _this.handleDragLeaveEvent(_this.model.root, event); }, onDrop: function (event) { return _this.handleDropEvent(_this.model.root, event); } });
    };
    FileTreeWidget.prototype.createNodeAttributes = function (node, props) {
        var _this = this;
        var elementAttrs = _super.prototype.createNodeAttributes.call(this, node, props);
        return __assign({}, elementAttrs, { draggable: file_tree_1.FileStatNode.is(node), onDragStart: function (event) { return _this.handleDragStartEvent(node, event); }, onDragEnter: function (event) { return _this.handleDragEnterEvent(node, event); }, onDragOver: function (event) { return _this.handleDragOverEvent(node, event); }, onDragLeave: function (event) { return _this.handleDragLeaveEvent(node, event); }, onDrop: function (event) { return _this.handleDropEvent(node, event); }, title: this.getNodeTooltip(node) });
    };
    FileTreeWidget.prototype.getNodeTooltip = function (node) {
        var uri = selection_1.UriSelection.getUri(node);
        return uri ? uri.path.toString() : undefined;
    };
    FileTreeWidget.prototype.handleDragStartEvent = function (node, event) {
        event.stopPropagation();
        var selectedNodes;
        if (this.model.selectedNodes.find(function (selected) { return browser_1.TreeNode.equals(selected, node); })) {
            selectedNodes = __spread(this.model.selectedNodes);
        }
        else {
            selectedNodes = [node];
        }
        this.setSelectedTreeNodesAsData(event.dataTransfer, node, selectedNodes);
        if (event.dataTransfer) {
            var label = void 0;
            if (selectedNodes.length === 1) {
                label = this.toNodeName(node);
            }
            else {
                label = String(selectedNodes.length);
            }
            var dragImage_1 = document.createElement('div');
            dragImage_1.className = 'theia-file-tree-drag-image';
            dragImage_1.textContent = label;
            document.body.appendChild(dragImage_1);
            event.dataTransfer.setDragImage(dragImage_1, -10, -10);
            setTimeout(function () { return document.body.removeChild(dragImage_1); }, 0);
        }
    };
    FileTreeWidget.prototype.handleDragEnterEvent = function (node, event) {
        event.preventDefault();
        event.stopPropagation();
        this.toCancelNodeExpansion.dispose();
        var containing = file_tree_1.DirNode.getContainingDir(node);
        if (!!containing && !containing.selected) {
            this.model.selectNode(containing);
        }
    };
    FileTreeWidget.prototype.handleDragOverEvent = function (node, event) {
        var _this = this;
        event.preventDefault();
        event.stopPropagation();
        if (!this.toCancelNodeExpansion.disposed) {
            return;
        }
        var timer = setTimeout(function () {
            var containing = file_tree_1.DirNode.getContainingDir(node);
            if (!!containing && !containing.expanded) {
                _this.model.expandNode(containing);
            }
        }, 500);
        this.toCancelNodeExpansion.push(common_1.Disposable.create(function () { return clearTimeout(timer); }));
    };
    FileTreeWidget.prototype.handleDragLeaveEvent = function (node, event) {
        event.preventDefault();
        event.stopPropagation();
        this.toCancelNodeExpansion.dispose();
    };
    FileTreeWidget.prototype.handleDropEvent = function (node, event) {
        return __awaiter(this, void 0, void 0, function () {
            var containing, resources, resources_1, resources_1_1, treeNode, e_1_1, e_2;
            var e_1, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _b.trys.push([0, 12, , 13]);
                        event.preventDefault();
                        event.stopPropagation();
                        event.dataTransfer.dropEffect = 'copy'; // Explicitly show this is a copy.
                        containing = this.getDropTargetDirNode(node);
                        if (!containing) return [3 /*break*/, 11];
                        resources = this.getSelectedTreeNodesFromData(event.dataTransfer);
                        if (!(resources.length > 0)) return [3 /*break*/, 9];
                        _b.label = 1;
                    case 1:
                        _b.trys.push([1, 6, 7, 8]);
                        resources_1 = __values(resources), resources_1_1 = resources_1.next();
                        _b.label = 2;
                    case 2:
                        if (!!resources_1_1.done) return [3 /*break*/, 5];
                        treeNode = resources_1_1.value;
                        return [4 /*yield*/, this.model.move(treeNode, containing)];
                    case 3:
                        _b.sent();
                        _b.label = 4;
                    case 4:
                        resources_1_1 = resources_1.next();
                        return [3 /*break*/, 2];
                    case 5: return [3 /*break*/, 8];
                    case 6:
                        e_1_1 = _b.sent();
                        e_1 = { error: e_1_1 };
                        return [3 /*break*/, 8];
                    case 7:
                        try {
                            if (resources_1_1 && !resources_1_1.done && (_a = resources_1.return)) _a.call(resources_1);
                        }
                        finally { if (e_1) throw e_1.error; }
                        return [7 /*endfinally*/];
                    case 8: return [3 /*break*/, 11];
                    case 9: return [4 /*yield*/, this.uploadService.upload(containing.uri, { source: event.dataTransfer })];
                    case 10:
                        _b.sent();
                        _b.label = 11;
                    case 11: return [3 /*break*/, 13];
                    case 12:
                        e_2 = _b.sent();
                        if (!cancellation_1.isCancelled(e_2)) {
                            console.error(e_2);
                        }
                        return [3 /*break*/, 13];
                    case 13: return [2 /*return*/];
                }
            });
        });
    };
    FileTreeWidget.prototype.getDropTargetDirNode = function (node) {
        if (browser_1.CompositeTreeNode.is(node) && node.id === 'WorkspaceNodeId') {
            if (node.children.length === 1) {
                return file_tree_1.DirNode.getContainingDir(node.children[0]);
            }
            else if (node.children.length > 1) {
                // move file to the last root folder in multi-root scenario
                return file_tree_1.DirNode.getContainingDir(node.children[node.children.length - 1]);
            }
        }
        return file_tree_1.DirNode.getContainingDir(node);
    };
    FileTreeWidget.prototype.setTreeNodeAsData = function (data, node) {
        data.setData('tree-node', node.id);
    };
    FileTreeWidget.prototype.setSelectedTreeNodesAsData = function (data, sourceNode, relatedNodes) {
        this.setTreeNodeAsData(data, sourceNode);
        data.setData('selected-tree-nodes', JSON.stringify(relatedNodes.map(function (node) { return node.id; })));
    };
    FileTreeWidget.prototype.getTreeNodeFromData = function (data) {
        var id = data.getData('tree-node');
        return this.model.getNode(id);
    };
    FileTreeWidget.prototype.getSelectedTreeNodesFromData = function (data) {
        var _this = this;
        var resources = data.getData('selected-tree-nodes');
        if (!resources) {
            return [];
        }
        var ids = JSON.parse(resources);
        return ids.map(function (id) { return _this.model.getNode(id); }).filter(function (node) { return node !== undefined; });
    };
    Object.defineProperty(FileTreeWidget.prototype, "hidesExplorerArrows", {
        get: function () {
            var theme = this.iconThemeService.getDefinition(this.iconThemeService.current);
            return !!theme && !!theme.hidesExplorerArrows;
        },
        enumerable: true,
        configurable: true
    });
    FileTreeWidget.prototype.renderExpansionToggle = function (node, props) {
        if (this.hidesExplorerArrows) {
            // eslint-disable-next-line no-null/no-null
            return null;
        }
        return _super.prototype.renderExpansionToggle.call(this, node, props);
    };
    FileTreeWidget.prototype.getPaddingLeft = function (node, props) {
        if (this.hidesExplorerArrows) {
            // aditional left padding instead of top-level expansion toggle
            return _super.prototype.getPaddingLeft.call(this, node, props) + this.props.leftPadding;
        }
        return _super.prototype.getPaddingLeft.call(this, node, props);
    };
    FileTreeWidget.prototype.needsExpansionTogglePadding = function (node) {
        var theme = this.iconThemeService.getDefinition(this.iconThemeService.current);
        if (theme && (theme.hidesExplorerArrows || (theme.hasFileIcons && !theme.hasFolderIcons))) {
            return false;
        }
        return _super.prototype.needsExpansionTogglePadding.call(this, node);
    };
    __decorate([
        inversify_1.inject(file_upload_service_1.FileUploadService),
        __metadata("design:type", file_upload_service_1.FileUploadService)
    ], FileTreeWidget.prototype, "uploadService", void 0);
    __decorate([
        inversify_1.inject(icon_theme_service_1.IconThemeService),
        __metadata("design:type", icon_theme_service_1.IconThemeService)
    ], FileTreeWidget.prototype, "iconThemeService", void 0);
    FileTreeWidget = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_1.TreeProps)),
        __param(1, inversify_1.inject(file_tree_model_1.FileTreeModel)),
        __param(2, inversify_1.inject(browser_1.ContextMenuRenderer)),
        __metadata("design:paramtypes", [Object, file_tree_model_1.FileTreeModel, Object])
    ], FileTreeWidget);
    return FileTreeWidget;
}(browser_1.TreeWidget));
exports.FileTreeWidget = FileTreeWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree/file-tree-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-tree/index.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/index.js ***!
  \**********************************************************************************************/
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
__export(__webpack_require__(/*! ./file-tree */ "../../packages/filesystem/lib/browser/file-tree/file-tree.js"));
__export(__webpack_require__(/*! ./file-tree-model */ "../../packages/filesystem/lib/browser/file-tree/file-tree-model.js"));
__export(__webpack_require__(/*! ./file-tree-widget */ "../../packages/filesystem/lib/browser/file-tree/file-tree-widget.js"));
__export(__webpack_require__(/*! ./file-tree-container */ "../../packages/filesystem/lib/browser/file-tree/file-tree-container.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/location/index.js":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/location/index.js ***!
  \*********************************************************************************************/
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
__export(__webpack_require__(/*! ./location-renderer */ "../../packages/filesystem/lib/browser/location/location-renderer.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/location'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/location/location-renderer.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/location/location-renderer.js ***!
  \*********************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var react_renderer_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/react-renderer */ "../../packages/core/lib/browser/widgets/react-renderer.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var LocationListRenderer = /** @class */ (function (_super) {
    __extends(LocationListRenderer, _super);
    function LocationListRenderer(service, host) {
        var _this = _super.call(this, host) || this;
        _this.service = service;
        _this.handleLocationChanged = function (e) { return _this.onLocationChanged(e); };
        _this.doLoadDrives();
        return _this;
    }
    LocationListRenderer.prototype.render = function () {
        _super.prototype.render.call(this);
        var locationList = this.locationList;
        if (locationList) {
            var currentLocation = this.service.location;
            locationList.value = currentLocation ? currentLocation.toString() : '';
        }
    };
    LocationListRenderer.prototype.doRender = function () {
        var _this = this;
        var options = this.collectLocations().map(function (value) { return _this.renderLocation(value); });
        return React.createElement("select", { className: 'theia-select ' + LocationListRenderer.Styles.LOCATION_LIST_CLASS, onChange: this.handleLocationChanged }, options);
    };
    /**
     * Collects the available locations based on the currently selected, and appends the available drives to it.
     */
    LocationListRenderer.prototype.collectLocations = function () {
        var location = this.service.location;
        var locations = (!!location ? location.allLocations : []).map(function (uri) { return ({ uri: uri }); });
        if (this._drives) {
            var drives = this._drives.map(function (uri) { return ({ uri: uri, isDrive: true }); });
            // `URI.allLocations` returns with the URI without the trailing slash unlike `FileUri.create(fsPath)`.
            // to be able to compare file:///path/to/resource with file:///path/to/resource/.
            var toUriString_1 = function (uri) {
                var toString = uri.toString();
                return toString.endsWith('/') ? toString.slice(0, -1) : toString;
            };
            drives.forEach(function (drive) {
                var index = locations.findIndex(function (loc) { return toUriString_1(loc.uri) === toUriString_1(drive.uri); });
                // Ignore drives which are already discovered as a location based on the current model root URI.
                if (index === -1) {
                    // Make sure, it does not have the trailing slash.
                    locations.push({ uri: new uri_1.default(toUriString_1(drive.uri)), isDrive: true });
                }
                else {
                    // This is necessary for Windows to be able to show `/e:/` as a drive and `c:` as "non-drive" in the same way.
                    // `URI.path.toString()` Vs. `URI.displayName` behaves a bit differently on Windows.
                    // https://github.com/eclipse-theia/theia/pull/3038#issuecomment-425944189
                    locations[index].isDrive = true;
                }
            });
        }
        this.doLoadDrives();
        return locations;
    };
    /**
     * Asynchronously loads the drives (if not yet available) and triggers a UI update on success with the new values.
     */
    LocationListRenderer.prototype.doLoadDrives = function () {
        var _this = this;
        if (!this._drives) {
            this.service.drives().then(function (drives) {
                // If the `drives` are empty, something already went wrong.
                if (drives.length > 0) {
                    _this._drives = drives;
                    _this.render();
                }
            });
        }
    };
    LocationListRenderer.prototype.renderLocation = function (location) {
        var uri = location.uri, isDrive = location.isDrive;
        var value = uri.toString();
        return React.createElement("option", { value: value, key: uri.toString() }, isDrive ? uri.path.toString() : uri.displayName);
    };
    LocationListRenderer.prototype.onLocationChanged = function (e) {
        var locationList = this.locationList;
        if (locationList) {
            var value = locationList.value;
            var uri = new uri_1.default(value);
            this.service.location = uri;
        }
        e.preventDefault();
        e.stopPropagation();
    };
    Object.defineProperty(LocationListRenderer.prototype, "locationList", {
        get: function () {
            var locationList = this.host.getElementsByClassName(LocationListRenderer.Styles.LOCATION_LIST_CLASS)[0];
            if (locationList instanceof HTMLSelectElement) {
                return locationList;
            }
            return undefined;
        },
        enumerable: true,
        configurable: true
    });
    return LocationListRenderer;
}(react_renderer_1.ReactRenderer));
exports.LocationListRenderer = LocationListRenderer;
(function (LocationListRenderer) {
    var Styles;
    (function (Styles) {
        Styles.LOCATION_LIST_CLASS = 'theia-LocationList';
    })(Styles = LocationListRenderer.Styles || (LocationListRenderer.Styles = {}));
})(LocationListRenderer = exports.LocationListRenderer || (exports.LocationListRenderer = {}));
exports.LocationListRenderer = LocationListRenderer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/location/location-renderer'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=6.bundle.js.map