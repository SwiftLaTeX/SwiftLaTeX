(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[19],{

/***/ "../../packages/userstorage/lib/browser/user-storage-frontend-module.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/user-storage-frontend-module.js ***!
  \************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 Ericsson and others.
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var user_storage_resource_1 = __webpack_require__(/*! ./user-storage-resource */ "../../packages/userstorage/lib/browser/user-storage-resource.js");
var user_storage_service_filesystem_1 = __webpack_require__(/*! ./user-storage-service-filesystem */ "../../packages/userstorage/lib/browser/user-storage-service-filesystem.js");
var user_storage_service_1 = __webpack_require__(/*! ./user-storage-service */ "../../packages/userstorage/lib/browser/user-storage-service.js");
function bindUserStorage(bind) {
    bind(common_1.ResourceResolver).to(user_storage_resource_1.UserStorageResolver).inSingletonScope();
    bind(user_storage_service_1.UserStorageService).to(user_storage_service_filesystem_1.UserStorageServiceFilesystemImpl).inSingletonScope();
}
exports.bindUserStorage = bindUserStorage;
exports.default = new inversify_1.ContainerModule(function (bind) {
    bindUserStorage(bind);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser/user-storage-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/userstorage/lib/browser/user-storage-resource.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/user-storage-resource.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 Ericsson and others.
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
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var user_storage_service_1 = __webpack_require__(/*! ./user-storage-service */ "../../packages/userstorage/lib/browser/user-storage-service.js");
var user_storage_uri_1 = __webpack_require__(/*! ./user-storage-uri */ "../../packages/userstorage/lib/browser/user-storage-uri.js");
var UserStorageResource = /** @class */ (function () {
    function UserStorageResource(uri, service) {
        var _this = this;
        this.uri = uri;
        this.service = service;
        this.onDidChangeContentsEmitter = new common_1.Emitter();
        this.toDispose = new common_1.DisposableCollection();
        this.toDispose.push(this.service.onUserStorageChanged(function (e) {
            var e_1, _a;
            try {
                for (var _b = __values(e.uris), _c = _b.next(); !_c.done; _c = _b.next()) {
                    var changedUri = _c.value;
                    if (changedUri.toString() === _this.uri.toString()) {
                        _this.onDidChangeContentsEmitter.fire(undefined);
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
        }));
        this.toDispose.push(this.onDidChangeContentsEmitter);
    }
    UserStorageResource.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    UserStorageResource.prototype.readContents = function (options) {
        return this.service.readContents(this.uri);
    };
    UserStorageResource.prototype.saveContents = function (content) {
        return this.service.saveContents(this.uri, content);
    };
    Object.defineProperty(UserStorageResource.prototype, "onDidChangeContents", {
        get: function () {
            return this.onDidChangeContentsEmitter.event;
        },
        enumerable: true,
        configurable: true
    });
    return UserStorageResource;
}());
exports.UserStorageResource = UserStorageResource;
var UserStorageResolver = /** @class */ (function () {
    function UserStorageResolver(service) {
        this.service = service;
    }
    UserStorageResolver.prototype.resolve = function (uri) {
        if (uri.scheme !== user_storage_uri_1.UserStorageUri.SCHEME) {
            throw new Error('The given uri is not a user storage uri: ' + uri);
        }
        return new UserStorageResource(uri, this.service);
    };
    UserStorageResolver = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(user_storage_service_1.UserStorageService)),
        __metadata("design:paramtypes", [Object])
    ], UserStorageResolver);
    return UserStorageResolver;
}());
exports.UserStorageResolver = UserStorageResolver;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser/user-storage-resource'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/userstorage/lib/browser/user-storage-service-filesystem.js":
/*!***************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/user-storage-service-filesystem.js ***!
  \***************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 Ericsson and others.
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
Object.defineProperty(exports, "__esModule", { value: true });
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var filesystem_watcher_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var env_variables_1 = __webpack_require__(/*! @theia/core/lib/common/env-variables */ "../../packages/core/lib/common/env-variables/index.js");
var common_2 = __webpack_require__(/*! @theia/filesystem/lib/common */ "../../packages/filesystem/lib/common/index.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var user_storage_uri_1 = __webpack_require__(/*! ./user-storage-uri */ "../../packages/userstorage/lib/browser/user-storage-uri.js");
var UserStorageServiceFilesystemImpl = /** @class */ (function () {
    function UserStorageServiceFilesystemImpl(fileSystem, watcher, logger, envServer) {
        var _this = this;
        this.fileSystem = fileSystem;
        this.watcher = watcher;
        this.logger = logger;
        this.envServer = envServer;
        this.toDispose = new common_1.DisposableCollection();
        this.onUserStorageChangedEmitter = new common_1.Emitter();
        this.userStorageFolder = this.envServer.getConfigDirUri().then(function (configDirUri) {
            var userDataFolderUri = new uri_1.default(configDirUri);
            watcher.watchFileChanges(userDataFolderUri).then(function (disposable) {
                return _this.toDispose.push(disposable);
            });
            _this.toDispose.push(_this.watcher.onFilesChanged(function (changes) { return _this.onDidFilesChanged(changes); }));
            return userDataFolderUri;
        });
        this.toDispose.push(this.onUserStorageChangedEmitter);
    }
    UserStorageServiceFilesystemImpl_1 = UserStorageServiceFilesystemImpl;
    UserStorageServiceFilesystemImpl.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    UserStorageServiceFilesystemImpl.prototype.onDidFilesChanged = function (event) {
        var _this = this;
        var uris = [];
        this.userStorageFolder.then(function (folder) {
            var e_1, _a;
            if (folder) {
                try {
                    for (var event_1 = __values(event), event_1_1 = event_1.next(); !event_1_1.done; event_1_1 = event_1.next()) {
                        var change = event_1_1.value;
                        if (folder.isEqualOrParent(change.uri)) {
                            var userStorageUri = UserStorageServiceFilesystemImpl_1.toUserStorageUri(folder, change.uri);
                            uris.push(userStorageUri);
                        }
                    }
                }
                catch (e_1_1) { e_1 = { error: e_1_1 }; }
                finally {
                    try {
                        if (event_1_1 && !event_1_1.done && (_a = event_1.return)) _a.call(event_1);
                    }
                    finally { if (e_1) throw e_1.error; }
                }
                if (uris.length > 0) {
                    _this.onUserStorageChangedEmitter.fire({ uris: uris });
                }
            }
        });
    };
    UserStorageServiceFilesystemImpl.prototype.readContents = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var folderUri, filesystemUri, exists;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.userStorageFolder];
                    case 1:
                        folderUri = _a.sent();
                        if (!folderUri) return [3 /*break*/, 3];
                        filesystemUri = UserStorageServiceFilesystemImpl_1.toFilesystemURI(folderUri, uri);
                        return [4 /*yield*/, this.fileSystem.exists(filesystemUri.toString())];
                    case 2:
                        exists = _a.sent();
                        if (exists) {
                            return [2 /*return*/, this.fileSystem.resolveContent(filesystemUri.toString()).then(function (_a) {
                                    var stat = _a.stat, content = _a.content;
                                    return content;
                                })];
                        }
                        _a.label = 3;
                    case 3: return [2 /*return*/, ''];
                }
            });
        });
    };
    UserStorageServiceFilesystemImpl.prototype.saveContents = function (uri, content) {
        return __awaiter(this, void 0, void 0, function () {
            var folderUri, filesystemUri, fileStat;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.userStorageFolder];
                    case 1:
                        folderUri = _a.sent();
                        if (!folderUri) {
                            return [2 /*return*/];
                        }
                        filesystemUri = UserStorageServiceFilesystemImpl_1.toFilesystemURI(folderUri, uri);
                        return [4 /*yield*/, this.fileSystem.getFileStat(filesystemUri.toString())];
                    case 2:
                        fileStat = _a.sent();
                        if (fileStat) {
                            this.fileSystem.setContent(fileStat, content).then(function () { return Promise.resolve(); });
                        }
                        else {
                            this.fileSystem.createFile(filesystemUri.toString(), { content: content });
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    Object.defineProperty(UserStorageServiceFilesystemImpl.prototype, "onUserStorageChanged", {
        get: function () {
            return this.onUserStorageChangedEmitter.event;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Creates a new user storage URI from the filesystem URI.
     * @param userStorageFolderUri User storage folder URI
     * @param fsPath The filesystem URI
     */
    UserStorageServiceFilesystemImpl.toUserStorageUri = function (userStorageFolderUri, rawUri) {
        var userStorageRelativePath = this.getRelativeUserStoragePath(userStorageFolderUri, rawUri);
        return new uri_1.default('').withScheme(user_storage_uri_1.UserStorageUri.SCHEME).withPath(userStorageRelativePath).withFragment(rawUri.fragment).withQuery(rawUri.query);
    };
    /**
     * Returns the path relative to the user storage filesystem uri i.e if the user storage root is
     * 'file://home/user/.theia' and the fileUri is 'file://home/user.theia/keymaps.json' it will return 'keymaps.json'
     * @param userStorageFolderUri User storage folder URI
     * @param fileUri User storage
     */
    UserStorageServiceFilesystemImpl.getRelativeUserStoragePath = function (userStorageFolderUri, fileUri) {
        /* + 1 so that it removes the beginning slash  i.e return keymaps.json and not /keymaps.json */
        return fileUri.toString().slice(userStorageFolderUri.toString().length + 1);
    };
    /**
     * Returns the associated filesystem URI relative to the user storage folder passed as argument.
     * @param userStorageFolderUri User storage folder URI
     * @param userStorageUri User storage URI to be converted in filesystem URI
     */
    UserStorageServiceFilesystemImpl.toFilesystemURI = function (userStorageFolderUri, userStorageUri) {
        return userStorageFolderUri.withPath(userStorageFolderUri.path.join(userStorageUri.path.toString()));
    };
    var UserStorageServiceFilesystemImpl_1;
    UserStorageServiceFilesystemImpl = UserStorageServiceFilesystemImpl_1 = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(common_2.FileSystem)),
        __param(1, inversify_1.inject(filesystem_watcher_1.FileSystemWatcher)),
        __param(2, inversify_1.inject(common_1.ILogger)),
        __param(3, inversify_1.inject(env_variables_1.EnvVariablesServer)),
        __metadata("design:paramtypes", [Object, filesystem_watcher_1.FileSystemWatcher, Object, Object])
    ], UserStorageServiceFilesystemImpl);
    return UserStorageServiceFilesystemImpl;
}());
exports.UserStorageServiceFilesystemImpl = UserStorageServiceFilesystemImpl;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser/user-storage-service-filesystem'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/userstorage/lib/browser/user-storage-service.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/user-storage-service.js ***!
  \****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
Object.defineProperty(exports, "__esModule", { value: true });
exports.UserStorageService = Symbol('UserStorageService');

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser/user-storage-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/userstorage/lib/browser/user-storage-uri.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/user-storage-uri.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017 Ericsson and others.
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
var UserStorageUri;
(function (UserStorageUri) {
    UserStorageUri.SCHEME = 'user_storage';
})(UserStorageUri = exports.UserStorageUri || (exports.UserStorageUri = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser/user-storage-uri'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=19.bundle.js.map