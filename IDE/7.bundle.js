(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[7],{

/***/ "../../packages/core/lib/common/selection-command-handler.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/common/selection-command-handler.js ***!
  \*************************************************************************************************/
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
var SelectionCommandHandler = /** @class */ (function () {
    function SelectionCommandHandler(selectionService, toSelection, options) {
        this.selectionService = selectionService;
        this.toSelection = toSelection;
        this.options = options;
    }
    SelectionCommandHandler.prototype.execute = function () {
        var _a;
        var args = [];
        for (var _i = 0; _i < arguments.length; _i++) {
            args[_i] = arguments[_i];
        }
        var selection = this.getSelection.apply(this, __spread(args));
        return selection ? (_a = this.options).execute.apply(_a, __spread([selection], args)) : undefined;
    };
    SelectionCommandHandler.prototype.isVisible = function () {
        var _a;
        var args = [];
        for (var _i = 0; _i < arguments.length; _i++) {
            args[_i] = arguments[_i];
        }
        var selection = this.getSelection.apply(this, __spread(args));
        return !!selection && (!this.options.isVisible || (_a = this.options).isVisible.apply(_a, __spread([selection], args)));
    };
    SelectionCommandHandler.prototype.isEnabled = function () {
        var _a;
        var args = [];
        for (var _i = 0; _i < arguments.length; _i++) {
            args[_i] = arguments[_i];
        }
        var selection = this.getSelection.apply(this, __spread(args));
        return !!selection && (!this.options.isEnabled || (_a = this.options).isEnabled.apply(_a, __spread([selection], args)));
    };
    SelectionCommandHandler.prototype.isMulti = function () {
        return this.options && !!this.options.multi;
    };
    SelectionCommandHandler.prototype.getSelection = function () {
        var args = [];
        for (var _i = 0; _i < arguments.length; _i++) {
            args[_i] = arguments[_i];
        }
        var givenSelection = args.length && this.toSelection(args[0]);
        if (givenSelection) {
            return this.isMulti() ? [givenSelection] : givenSelection;
        }
        var globalSelection = this.getSingleSelection(this.selectionService.selection);
        if (this.isMulti()) {
            return this.getMultiSelection(globalSelection);
        }
        return this.getSingleSelection(globalSelection);
    };
    SelectionCommandHandler.prototype.getSingleSelection = function (arg) {
        var e_1, _a;
        var selection = this.toSelection(arg);
        if (selection) {
            return selection;
        }
        if (Array.isArray(arg)) {
            try {
                for (var arg_1 = __values(arg), arg_1_1 = arg_1.next(); !arg_1_1.done; arg_1_1 = arg_1.next()) {
                    var element = arg_1_1.value;
                    selection = this.toSelection(element);
                    if (selection) {
                        return selection;
                    }
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (arg_1_1 && !arg_1_1.done && (_a = arg_1.return)) _a.call(arg_1);
                }
                finally { if (e_1) throw e_1.error; }
            }
        }
        return undefined;
    };
    SelectionCommandHandler.prototype.getMultiSelection = function (arg) {
        var e_2, _a;
        var selection = this.toSelection(arg);
        if (selection) {
            return [selection];
        }
        var result = [];
        if (Array.isArray(arg)) {
            try {
                for (var arg_2 = __values(arg), arg_2_1 = arg_2.next(); !arg_2_1.done; arg_2_1 = arg_2.next()) {
                    var element = arg_2_1.value;
                    selection = this.toSelection(element);
                    if (selection) {
                        result.push(selection);
                    }
                }
            }
            catch (e_2_1) { e_2 = { error: e_2_1 }; }
            finally {
                try {
                    if (arg_2_1 && !arg_2_1.done && (_a = arg_2.return)) _a.call(arg_2);
                }
                finally { if (e_2) throw e_2.error; }
            }
        }
        return result.length ? result : undefined;
    };
    return SelectionCommandHandler;
}());
exports.SelectionCommandHandler = SelectionCommandHandler;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/common/selection-command-handler'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-resource.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-resource.js ***!
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
var resource_1 = __webpack_require__(/*! @theia/core/lib/common/resource */ "../../packages/core/lib/common/resource.js");
var disposable_1 = __webpack_require__(/*! @theia/core/lib/common/disposable */ "../../packages/core/lib/common/disposable.js");
var event_1 = __webpack_require__(/*! @theia/core/lib/common/event */ "../../packages/core/lib/common/event.js");
var filesystem_1 = __webpack_require__(/*! ../common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var filesystem_watcher_1 = __webpack_require__(/*! ./filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var FileResourceVersion;
(function (FileResourceVersion) {
    function is(version) {
        return !!version && 'stat' in version && filesystem_1.FileStat.is(version['stat']);
    }
    FileResourceVersion.is = is;
})(FileResourceVersion = exports.FileResourceVersion || (exports.FileResourceVersion = {}));
var FileResource = /** @class */ (function () {
    function FileResource(uri, fileSystem, fileSystemWatcher) {
        this.uri = uri;
        this.fileSystem = fileSystem;
        this.fileSystemWatcher = fileSystemWatcher;
        this.toDispose = new disposable_1.DisposableCollection();
        this.onDidChangeContentsEmitter = new event_1.Emitter();
        this.onDidChangeContents = this.onDidChangeContentsEmitter.event;
        this.uriString = this.uri.toString();
        this.toDispose.push(this.onDidChangeContentsEmitter);
    }
    Object.defineProperty(FileResource.prototype, "version", {
        get: function () {
            return this._version;
        },
        enumerable: true,
        configurable: true
    });
    FileResource.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var stat, _a, _b, e_1;
            var _this = this;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, this.getFileStat()];
                    case 1:
                        stat = _c.sent();
                        if (stat && stat.isDirectory) {
                            throw new Error('The given uri is a directory: ' + this.uriString);
                        }
                        this.toDispose.push(this.fileSystemWatcher.onFilesChanged(function (event) {
                            if (filesystem_watcher_1.FileChangeEvent.isAffected(event, _this.uri)) {
                                _this.sync();
                            }
                        }));
                        this.toDispose.push(this.fileSystemWatcher.onDidDelete(function (event) {
                            if (event.uri.isEqualOrParent(_this.uri)) {
                                _this.sync();
                            }
                        }));
                        this.toDispose.push(this.fileSystemWatcher.onDidMove(function (event) {
                            if (event.sourceUri.isEqualOrParent(_this.uri) || event.targetUri.isEqualOrParent(_this.uri)) {
                                _this.sync();
                            }
                        }));
                        _c.label = 2;
                    case 2:
                        _c.trys.push([2, 4, , 5]);
                        _b = (_a = this.toDispose).push;
                        return [4 /*yield*/, this.fileSystemWatcher.watchFileChanges(this.uri)];
                    case 3:
                        _b.apply(_a, [_c.sent()]);
                        return [3 /*break*/, 5];
                    case 4:
                        e_1 = _c.sent();
                        console.error(e_1);
                        return [3 /*break*/, 5];
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    FileResource.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    FileResource.prototype.readContents = function (options) {
        return __awaiter(this, void 0, void 0, function () {
            var _a, stat, content, e_2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _b.trys.push([0, 2, , 3]);
                        return [4 /*yield*/, this.fileSystem.resolveContent(this.uriString, options)];
                    case 1:
                        _a = _b.sent(), stat = _a.stat, content = _a.content;
                        this._version = { stat: stat };
                        return [2 /*return*/, content];
                    case 2:
                        e_2 = _b.sent();
                        if (filesystem_1.FileSystemError.FileNotFound.is(e_2)) {
                            this._version = undefined;
                            throw resource_1.ResourceError.NotFound(__assign({}, e_2.toJson(), { data: {
                                    uri: this.uri
                                } }));
                        }
                        throw e_2;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    FileResource.prototype.saveContents = function (content, options) {
        return __awaiter(this, void 0, void 0, function () {
            var resolvedOptions, stat, e_3;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 3]);
                        resolvedOptions = options;
                        if (options && options.overwriteEncoding) {
                            resolvedOptions = __assign({}, options, { encoding: options.overwriteEncoding });
                            delete resolvedOptions.overwriteEncoding;
                        }
                        return [4 /*yield*/, this.doSaveContents(content, resolvedOptions)];
                    case 1:
                        stat = _a.sent();
                        this._version = { stat: stat };
                        return [3 /*break*/, 3];
                    case 2:
                        e_3 = _a.sent();
                        if (filesystem_1.FileSystemError.FileIsOutOfSync.is(e_3)) {
                            throw resource_1.ResourceError.OutOfSync(__assign({}, e_3.toJson(), { data: { uri: this.uri } }));
                        }
                        throw e_3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    FileResource.prototype.doSaveContents = function (content, options) {
        return __awaiter(this, void 0, void 0, function () {
            var version, stat, _a, e_4;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        version = options && options.version || this._version;
                        _a = FileResourceVersion.is(version) && version.stat;
                        if (_a) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.getFileStat()];
                    case 1:
                        _a = (_b.sent());
                        _b.label = 2;
                    case 2:
                        stat = _a;
                        if (!stat) return [3 /*break*/, 6];
                        _b.label = 3;
                    case 3:
                        _b.trys.push([3, 5, , 6]);
                        return [4 /*yield*/, this.fileSystem.setContent(stat, content, options)];
                    case 4: return [2 /*return*/, _b.sent()];
                    case 5:
                        e_4 = _b.sent();
                        if (!filesystem_1.FileSystemError.FileNotFound.is(e_4)) {
                            throw e_4;
                        }
                        return [3 /*break*/, 6];
                    case 6: return [2 /*return*/, this.fileSystem.createFile(this.uriString, __assign({ content: content }, options))];
                }
            });
        });
    };
    FileResource.prototype.saveContentChanges = function (changes, options) {
        return __awaiter(this, void 0, void 0, function () {
            var version, currentStat, stat, e_5;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        version = options && options.version || this._version;
                        currentStat = FileResourceVersion.is(version) && version.stat;
                        if (!currentStat) {
                            throw resource_1.ResourceError.NotFound({ message: 'has not been read yet', data: { uri: this.uri } });
                        }
                        _a.label = 1;
                    case 1:
                        _a.trys.push([1, 3, , 4]);
                        return [4 /*yield*/, this.fileSystem.updateContent(currentStat, changes, options)];
                    case 2:
                        stat = _a.sent();
                        this._version = { stat: stat };
                        return [3 /*break*/, 4];
                    case 3:
                        e_5 = _a.sent();
                        if (filesystem_1.FileSystemError.FileNotFound.is(e_5)) {
                            throw resource_1.ResourceError.NotFound(__assign({}, e_5.toJson(), { data: { uri: this.uri } }));
                        }
                        if (filesystem_1.FileSystemError.FileIsOutOfSync.is(e_5)) {
                            throw resource_1.ResourceError.OutOfSync(__assign({}, e_5.toJson(), { data: { uri: this.uri } }));
                        }
                        throw e_5;
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    FileResource.prototype.guessEncoding = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, this.fileSystem.guessEncoding(this.uriString)];
            });
        });
    };
    FileResource.prototype.sync = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.isInSync(this.version && this.version.stat)];
                    case 1:
                        if (_a.sent()) {
                            return [2 /*return*/];
                        }
                        this.onDidChangeContentsEmitter.fire(undefined);
                        return [2 /*return*/];
                }
            });
        });
    };
    FileResource.prototype.isInSync = function (current) {
        return __awaiter(this, void 0, void 0, function () {
            var stat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.getFileStat()];
                    case 1:
                        stat = _a.sent();
                        if (!current) {
                            return [2 /*return*/, !stat];
                        }
                        return [2 /*return*/, !!stat && current.lastModification >= stat.lastModification];
                }
            });
        });
    };
    FileResource.prototype.getFileStat = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.fileSystem.exists(this.uriString)];
                    case 1:
                        if (!(_a.sent())) {
                            return [2 /*return*/, undefined];
                        }
                        try {
                            return [2 /*return*/, this.fileSystem.getFileStat(this.uriString)];
                        }
                        catch (_b) {
                            return [2 /*return*/, undefined];
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    return FileResource;
}());
exports.FileResource = FileResource;
var FileResourceResolver = /** @class */ (function () {
    function FileResourceResolver() {
    }
    FileResourceResolver.prototype.resolve = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var resource;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (uri.scheme !== 'file') {
                            throw new Error('The given uri is not file uri: ' + uri);
                        }
                        resource = new FileResource(uri, this.fileSystem, this.fileSystemWatcher);
                        return [4 /*yield*/, resource.init()];
                    case 1:
                        _a.sent();
                        return [2 /*return*/, resource];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(filesystem_1.FileSystem),
        __metadata("design:type", Object)
    ], FileResourceResolver.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(filesystem_watcher_1.FileSystemWatcher),
        __metadata("design:type", filesystem_watcher_1.FileSystemWatcher)
    ], FileResourceResolver.prototype, "fileSystemWatcher", void 0);
    FileResourceResolver = __decorate([
        inversify_1.injectable()
    ], FileResourceResolver);
    return FileResourceResolver;
}());
exports.FileResourceResolver = FileResourceResolver;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-resource'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-selection.js":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-selection.js ***!
  \*********************************************************************************************/
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
var selection_command_handler_1 = __webpack_require__(/*! @theia/core/lib/common/selection-command-handler */ "../../packages/core/lib/common/selection-command-handler.js");
var filesystem_1 = __webpack_require__(/*! ../common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var FileSelection;
(function (FileSelection) {
    function is(arg) {
        return typeof arg === 'object' && ('fileStat' in arg) && filesystem_1.FileStat.is(arg['fileStat']);
    }
    FileSelection.is = is;
    var CommandHandler = /** @class */ (function (_super) {
        __extends(CommandHandler, _super);
        function CommandHandler(selectionService, options) {
            var _this = _super.call(this, selectionService, function (arg) { return FileSelection.is(arg) ? arg : undefined; }, options) || this;
            _this.selectionService = selectionService;
            _this.options = options;
            return _this;
        }
        return CommandHandler;
    }(selection_command_handler_1.SelectionCommandHandler));
    FileSelection.CommandHandler = CommandHandler;
})(FileSelection = exports.FileSelection || (exports.FileSelection = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-selection'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/filesystem-frontend-contribution.js":
/*!***************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/filesystem-frontend-contribution.js ***!
  \***************************************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var environment_1 = __webpack_require__(/*! @theia/application-package/lib/environment */ "../../dev-packages/application-package/lib/environment.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var filesystem_watcher_1 = __webpack_require__(/*! ./filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var mime_service_1 = __webpack_require__(/*! @theia/core/lib/browser/mime-service */ "../../packages/core/lib/browser/mime-service.js");
var tree_widget_selection_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-widget-selection */ "../../packages/core/lib/browser/tree/tree-widget-selection.js");
var filesystem_preferences_1 = __webpack_require__(/*! ./filesystem-preferences */ "../../packages/filesystem/lib/browser/filesystem-preferences.js");
var file_selection_1 = __webpack_require__(/*! ./file-selection */ "../../packages/filesystem/lib/browser/file-selection.js");
var file_upload_service_1 = __webpack_require__(/*! ./file-upload-service */ "../../packages/filesystem/lib/browser/file-upload-service.js");
var FileSystemCommands;
(function (FileSystemCommands) {
    FileSystemCommands.UPLOAD = {
        id: 'file.upload',
        category: 'File',
        label: 'Upload Files...'
    };
})(FileSystemCommands = exports.FileSystemCommands || (exports.FileSystemCommands = {}));
var FileSystemFrontendContribution = /** @class */ (function () {
    function FileSystemFrontendContribution() {
        this.pendingOperation = Promise.resolve();
        this.moveSnapshots = new Map();
        this.deletedSuffix = ' (deleted from disk)';
    }
    FileSystemFrontendContribution.prototype.initialize = function () {
        var _this = this;
        this.fileSystemWatcher.onFilesChanged(function (event) { return _this.run(function () { return _this.updateWidgets(event); }); });
        this.fileSystemWatcher.onWillMove(function (event) { return event.waitUntil(_this.runEach(function (uri, widget) { return _this.pushMove(uri, widget, event); })); });
        this.fileSystemWatcher.onDidFailMove(function (event) { return event.waitUntil(_this.runEach(function (uri, widget) { return _this.revertMove(uri, widget, event); })); });
        this.fileSystemWatcher.onDidMove(function (event) { return event.waitUntil(_this.runEach(function (uri, widget) { return _this.applyMove(uri, widget, event); })); });
    };
    FileSystemFrontendContribution.prototype.onStart = function (app) {
        var _this = this;
        this.updateAssociations();
        this.preferences.onPreferenceChanged(function (e) {
            if (e.preferenceName === 'files.associations') {
                _this.updateAssociations();
            }
        });
    };
    FileSystemFrontendContribution.prototype.registerCommands = function (commands) {
        var _this = this;
        commands.registerCommand(FileSystemCommands.UPLOAD, new file_selection_1.FileSelection.CommandHandler(this.selectionService, {
            multi: false,
            isEnabled: function (selection) { return _this.canUpload(selection); },
            isVisible: function (selection) { return _this.canUpload(selection); },
            execute: function (selection) { return _this.upload(selection); }
        }));
    };
    FileSystemFrontendContribution.prototype.canUpload = function (_a) {
        var fileStat = _a.fileStat;
        return !environment_1.environment.electron.is() && fileStat.isDirectory;
    };
    FileSystemFrontendContribution.prototype.upload = function (selection) {
        return __awaiter(this, void 0, void 0, function () {
            var source, e_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 4, , 5]);
                        source = tree_widget_selection_1.TreeWidgetSelection.getSource(this.selectionService.selection);
                        return [4 /*yield*/, this.uploadService.upload(selection.fileStat.uri)];
                    case 1:
                        _a.sent();
                        if (!(browser_1.ExpandableTreeNode.is(selection) && source)) return [3 /*break*/, 3];
                        return [4 /*yield*/, source.model.expandNode(selection)];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3: return [3 /*break*/, 5];
                    case 4:
                        e_1 = _a.sent();
                        if (!common_1.isCancelled(e_1)) {
                            console.error(e_1);
                        }
                        return [3 /*break*/, 5];
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    FileSystemFrontendContribution.prototype.run = function (operation) {
        var _this = this;
        return this.pendingOperation = this.pendingOperation.then(function () { return __awaiter(_this, void 0, void 0, function () {
            var e_2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 3]);
                        return [4 /*yield*/, operation()];
                    case 1:
                        _a.sent();
                        return [3 /*break*/, 3];
                    case 2:
                        e_2 = _a.sent();
                        console.error(e_2);
                        return [3 /*break*/, 3];
                    case 3: return [2 /*return*/];
                }
            });
        }); });
    };
    FileSystemFrontendContribution.prototype.runEach = function (participant) {
        var _this = this;
        return this.run(function () { return __awaiter(_this, void 0, void 0, function () {
            var promises, _a, _b, _c, resourceUri, widget;
            var e_3, _d;
            return __generator(this, function (_e) {
                switch (_e.label) {
                    case 0:
                        promises = [];
                        try {
                            for (_a = __values(browser_1.NavigatableWidget.get(this.shell.widgets)), _b = _a.next(); !_b.done; _b = _a.next()) {
                                _c = __read(_b.value, 2), resourceUri = _c[0], widget = _c[1];
                                promises.push(participant(resourceUri, widget));
                            }
                        }
                        catch (e_3_1) { e_3 = { error: e_3_1 }; }
                        finally {
                            try {
                                if (_b && !_b.done && (_d = _a.return)) _d.call(_a);
                            }
                            finally { if (e_3) throw e_3.error; }
                        }
                        return [4 /*yield*/, Promise.all(promises)];
                    case 1:
                        _e.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    };
    FileSystemFrontendContribution.prototype.popMoveSnapshot = function (resourceUri) {
        var snapshotKey = resourceUri.toString();
        var snapshot = this.moveSnapshots.get(snapshotKey);
        if (snapshot) {
            this.moveSnapshots.delete(snapshotKey);
        }
        return snapshot;
    };
    FileSystemFrontendContribution.prototype.applyMoveSnapshot = function (widget, snapshot) {
        if (!snapshot) {
            return undefined;
        }
        if (snapshot.dirty) {
            var saveable = browser_1.Saveable.get(widget);
            if (saveable && saveable.applySnapshot) {
                saveable.applySnapshot(snapshot.dirty);
            }
        }
        if (snapshot.view && browser_1.StatefulWidget.is(widget)) {
            widget.restoreState(snapshot.view);
        }
    };
    FileSystemFrontendContribution.prototype.pushMove = function (resourceUri, widget, event) {
        return __awaiter(this, void 0, void 0, function () {
            var newResourceUri, snapshot, saveable;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        newResourceUri = this.createMoveToUri(resourceUri, widget, event);
                        if (!newResourceUri) {
                            return [2 /*return*/];
                        }
                        snapshot = {};
                        saveable = browser_1.Saveable.get(widget);
                        if (browser_1.StatefulWidget.is(widget)) {
                            snapshot.view = widget.storeState();
                        }
                        if (!(saveable && saveable.dirty)) return [3 /*break*/, 2];
                        if (saveable.createSnapshot) {
                            snapshot.dirty = saveable.createSnapshot();
                        }
                        if (!saveable.revert) return [3 /*break*/, 2];
                        return [4 /*yield*/, saveable.revert({ soft: true })];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2:
                        this.moveSnapshots.set(newResourceUri.toString(), snapshot);
                        return [2 /*return*/];
                }
            });
        });
    };
    FileSystemFrontendContribution.prototype.revertMove = function (resourceUri, widget, event) {
        return __awaiter(this, void 0, void 0, function () {
            var newResourceUri, snapshot;
            return __generator(this, function (_a) {
                newResourceUri = this.createMoveToUri(resourceUri, widget, event);
                if (!newResourceUri) {
                    return [2 /*return*/];
                }
                snapshot = this.popMoveSnapshot(newResourceUri);
                this.applyMoveSnapshot(widget, snapshot);
                return [2 /*return*/];
            });
        });
    };
    FileSystemFrontendContribution.prototype.applyMove = function (resourceUri, widget, event) {
        return __awaiter(this, void 0, void 0, function () {
            var newResourceUri, snapshot, description, factoryId, options, newWidget, area, pending;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        newResourceUri = this.createMoveToUri(resourceUri, widget, event);
                        if (!newResourceUri) {
                            return [2 /*return*/];
                        }
                        snapshot = this.popMoveSnapshot(newResourceUri);
                        description = this.widgetManager.getDescription(widget);
                        if (!description) {
                            return [2 /*return*/];
                        }
                        factoryId = description.factoryId, options = description.options;
                        if (!browser_1.NavigatableWidgetOptions.is(options)) {
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, this.widgetManager.getOrCreateWidget(factoryId, __assign({}, options, { uri: newResourceUri.toString() }))];
                    case 1:
                        newWidget = _a.sent();
                        this.applyMoveSnapshot(newWidget, snapshot);
                        area = this.shell.getAreaFor(widget) || 'main';
                        pending = [this.shell.addWidget(newWidget, {
                                area: area, ref: widget
                            })];
                        if (this.shell.activeWidget === widget) {
                            pending.push(this.shell.activateWidget(newWidget.id));
                        }
                        else if (widget.isVisible) {
                            pending.push(this.shell.revealWidget(newWidget.id));
                        }
                        pending.push(this.shell.closeWidget(widget.id, { save: false }));
                        return [4 /*yield*/, Promise.all(pending)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileSystemFrontendContribution.prototype.createMoveToUri = function (resourceUri, widget, event) {
        var path = event.sourceUri.relative(resourceUri);
        var targetUri = path && event.targetUri.resolve(path);
        return targetUri && widget.createMoveToUri(targetUri);
    };
    FileSystemFrontendContribution.prototype.updateWidgets = function (event) {
        return __awaiter(this, void 0, void 0, function () {
            var relevantEvent;
            return __generator(this, function (_a) {
                relevantEvent = event.filter(function (_a) {
                    var type = _a.type;
                    return type !== filesystem_watcher_1.FileChangeType.UPDATED;
                });
                if (relevantEvent.length) {
                    return [2 /*return*/, this.doUpdateWidgets(relevantEvent)];
                }
                return [2 /*return*/];
            });
        });
    };
    FileSystemFrontendContribution.prototype.doUpdateWidgets = function (event) {
        return __awaiter(this, void 0, void 0, function () {
            var pending, dirty, toClose, _a, _b, _c, uri, widget, _d, _e, _f, uriString, widgets, widgets_1, widgets_1_1, widget;
            var e_4, _g, e_5, _h, e_6, _j;
            return __generator(this, function (_k) {
                switch (_k.label) {
                    case 0:
                        pending = [];
                        dirty = new Set();
                        toClose = new Map();
                        try {
                            for (_a = __values(browser_1.NavigatableWidget.get(this.shell.widgets)), _b = _a.next(); !_b.done; _b = _a.next()) {
                                _c = __read(_b.value, 2), uri = _c[0], widget = _c[1];
                                this.updateWidget(uri, widget, event, { dirty: dirty, toClose: toClose });
                            }
                        }
                        catch (e_4_1) { e_4 = { error: e_4_1 }; }
                        finally {
                            try {
                                if (_b && !_b.done && (_g = _a.return)) _g.call(_a);
                            }
                            finally { if (e_4) throw e_4.error; }
                        }
                        try {
                            for (_d = __values(toClose.entries()), _e = _d.next(); !_e.done; _e = _d.next()) {
                                _f = __read(_e.value, 2), uriString = _f[0], widgets = _f[1];
                                if (!dirty.has(uriString)) {
                                    try {
                                        for (widgets_1 = (e_6 = void 0, __values(widgets)), widgets_1_1 = widgets_1.next(); !widgets_1_1.done; widgets_1_1 = widgets_1.next()) {
                                            widget = widgets_1_1.value;
                                            widget.close();
                                            pending.push(browser_1.waitForClosed(widget));
                                        }
                                    }
                                    catch (e_6_1) { e_6 = { error: e_6_1 }; }
                                    finally {
                                        try {
                                            if (widgets_1_1 && !widgets_1_1.done && (_j = widgets_1.return)) _j.call(widgets_1);
                                        }
                                        finally { if (e_6) throw e_6.error; }
                                    }
                                }
                            }
                        }
                        catch (e_5_1) { e_5 = { error: e_5_1 }; }
                        finally {
                            try {
                                if (_e && !_e.done && (_h = _d.return)) _h.call(_d);
                            }
                            finally { if (e_5) throw e_5.error; }
                        }
                        return [4 /*yield*/, Promise.all(pending)];
                    case 1:
                        _k.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileSystemFrontendContribution.prototype.updateWidget = function (uri, widget, event, _a) {
        var dirty = _a.dirty, toClose = _a.toClose;
        var label = widget.title.label;
        var deleted = label.endsWith(this.deletedSuffix);
        if (filesystem_watcher_1.FileChangeEvent.isDeleted(event, uri)) {
            var uriString = uri.toString();
            if (browser_1.Saveable.isDirty(widget)) {
                if (!deleted) {
                    widget.title.label += this.deletedSuffix;
                }
                dirty.add(uriString);
            }
            var widgets = toClose.get(uriString) || [];
            widgets.push(widget);
            toClose.set(uriString, widgets);
        }
        else if (filesystem_watcher_1.FileChangeEvent.isAdded(event, uri)) {
            if (deleted) {
                widget.title.label = widget.title.label.substr(0, label.length - this.deletedSuffix.length);
            }
        }
    };
    FileSystemFrontendContribution.prototype.updateAssociations = function () {
        var fileAssociations = this.preferences['files.associations'];
        var mimeAssociations = Object.keys(fileAssociations).map(function (filepattern) { return ({ id: fileAssociations[filepattern], filepattern: filepattern }); });
        this.mimeService.setAssociations(mimeAssociations);
    };
    __decorate([
        inversify_1.inject(browser_1.ApplicationShell),
        __metadata("design:type", browser_1.ApplicationShell)
    ], FileSystemFrontendContribution.prototype, "shell", void 0);
    __decorate([
        inversify_1.inject(browser_1.WidgetManager),
        __metadata("design:type", browser_1.WidgetManager)
    ], FileSystemFrontendContribution.prototype, "widgetManager", void 0);
    __decorate([
        inversify_1.inject(filesystem_watcher_1.FileSystemWatcher),
        __metadata("design:type", filesystem_watcher_1.FileSystemWatcher)
    ], FileSystemFrontendContribution.prototype, "fileSystemWatcher", void 0);
    __decorate([
        inversify_1.inject(mime_service_1.MimeService),
        __metadata("design:type", mime_service_1.MimeService)
    ], FileSystemFrontendContribution.prototype, "mimeService", void 0);
    __decorate([
        inversify_1.inject(filesystem_preferences_1.FileSystemPreferences),
        __metadata("design:type", Object)
    ], FileSystemFrontendContribution.prototype, "preferences", void 0);
    __decorate([
        inversify_1.inject(common_1.SelectionService),
        __metadata("design:type", common_1.SelectionService)
    ], FileSystemFrontendContribution.prototype, "selectionService", void 0);
    __decorate([
        inversify_1.inject(file_upload_service_1.FileUploadService),
        __metadata("design:type", file_upload_service_1.FileUploadService)
    ], FileSystemFrontendContribution.prototype, "uploadService", void 0);
    FileSystemFrontendContribution = __decorate([
        inversify_1.injectable()
    ], FileSystemFrontendContribution);
    return FileSystemFrontendContribution;
}());
exports.FileSystemFrontendContribution = FileSystemFrontendContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/filesystem-frontend-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=7.bundle.js.map