(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[3],{

/***/ "../../packages/filesystem/lib/browser/s3storagesystem.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/s3storagesystem.js ***!
  \**********************************************************************************************/
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
/* eslint-disable @typescript-eslint/tslint/config */
/********************************************************************************
 * Copyright (C) 2019 Gerald Weber and Elliott Wen.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 ********************************************************************************/
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var S3 = __webpack_require__(/*! aws-sdk/clients/s3 */ "../../node_modules/aws-sdk/clients/s3.js");
var JSZIP = __webpack_require__(/*! jszip */ "../../node_modules/jszip/lib/index.js");
var path = __webpack_require__(/*! path */ "../../node_modules/path-browserify/index.js");
var filesystem_watcher_protocol_1 = __webpack_require__(/*! ../common/filesystem-watcher-protocol */ "../../packages/filesystem/lib/common/filesystem-watcher-protocol.js");
/* eslint-disable @typescript-eslint/no-explicit-any */
/*
 * Minio can not end a file with '/', we need a marker, this client is designed only for Minio
 * For s3, a conflicted solution is needed to tell whether a file is a dir or file.
 * For S3, a more work is needed. Do not use it in S3 right now. A possible solution is to use ETag
 * for directory distinguish
 */
var ROOT_MARKER = '__D1r__Ro0t__';
function GenerateRandomID() {
    // From http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
        var r = (Math.random() * 16) | 0;
        var v = c === 'x' ? r : (r & 0x3) | 0x8;
        return v.toString(16);
    });
}
/**
 * Configuration options for file system.
 */
var S3StorageSystemOptions = /** @class */ (function () {
    function S3StorageSystemOptions() {
        // The name of this file system. You can have multiple IndexedDB file systems operating
        // at once, but each must have a different name.
        this.bucket = 'swiftlatex';
        this.prefix = '';
        // The Key, Secret, Token
        this.apiKey = '';
        this.apiSecret = '';
        this.sessionToken = '';
        this.endpoint = '';
        this.expiry = 0;
        // The size of the inode cache. Defaults to 100. A size of 0 or below disables caching.
    }
    return S3StorageSystemOptions;
}());
exports.S3StorageSystemOptions = S3StorageSystemOptions;
/**
 * FileStat
 */
var S3Object = /** @class */ (function () {
    function S3Object() {
        this.modifiedTime = new Date();
        this.ETag = '';
        this.size = 0;
        this.meta = {};
        this.data = undefined;
        /* For upper application layer */
        this.key = '';
        this.uri = '';
        this.isDir = false;
    }
    return S3Object;
}());
exports.S3Object = S3Object;
/**
 * Configuration options for file system. All APIs do not accpet parameter ends with '/'
 */
var S3StorageSystem = /** @class */ (function () {
    function S3StorageSystem() {
        this.s3 = undefined;
        this.opts = new S3StorageSystemOptions();
        this.s3lock = false;
        this._init_fs().catch(function (e) {
            throw e;
        });
    }
    S3StorageSystem.prototype._init_fs = function () {
        return __awaiter(this, void 0, void 0, function () {
            var rootnode, empty_list, obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._headObject(ROOT_MARKER)];
                    case 1:
                        rootnode = _a.sent();
                        if (!!rootnode) return [3 /*break*/, 3];
                        empty_list = {};
                        obj = new S3Object();
                        obj.data = JSON.stringify(empty_list);
                        return [4 /*yield*/, this._putObject(ROOT_MARKER, obj)];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    /* Query a path to its node id */
    S3StorageSystem.prototype._resolvePath = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var parent_1, parent_nodeid, dirListObj, jsonList, node;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p || p === '.') {
                            return [2 /*return*/, undefined];
                        }
                        if (!(p === '/')) return [3 /*break*/, 1];
                        return [2 /*return*/, { node_id: ROOT_MARKER, is_dir: true }];
                    case 1: return [4 /*yield*/, this._resolvePath(path.dirname(p))];
                    case 2:
                        parent_1 = _a.sent();
                        if (!parent_1) {
                            return [2 /*return*/, undefined];
                        }
                        parent_nodeid = parent_1.node_id;
                        return [4 /*yield*/, this._getObject(parent_nodeid)];
                    case 3:
                        dirListObj = _a.sent();
                        if (!dirListObj) {
                            return [2 /*return*/, undefined];
                        }
                        jsonList = JSON.parse(dirListObj.data.toString());
                        node = jsonList[path.basename(p)];
                        return [2 /*return*/, node];
                    case 4: return [2 /*return*/, undefined];
                }
            });
        });
    };
    S3StorageSystem.prototype.ensureDirExist = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var s3stat, parentDir;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        /* To prevent endless recursive */
                        if (p === '/' || p === '.' || p === '') {
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        s3stat = _a.sent();
                        if (s3stat) {
                            return [2 /*return*/];
                        }
                        parentDir = path.dirname(p);
                        return [4 /*yield*/, this.ensureDirExist(parentDir)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, this.mkdir(p)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    /* Stat a file or dir, one API turnaround*/
    S3StorageSystem.prototype.stat = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var node, obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        node = _a.sent();
                        if (!node) {
                            return [2 /*return*/, undefined];
                        }
                        return [4 /*yield*/, this._headObject(node.node_id)];
                    case 2:
                        obj = _a.sent();
                        if (obj) {
                            obj.uri = p;
                            obj.isDir = node.is_dir;
                        }
                        return [2 /*return*/, obj];
                }
            });
        });
    };
    /* Read dir, one api turnaround , a little bit hack to reduce the turnaround */
    S3StorageSystem.prototype.readdir = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var node, obj, jsonList, objs, name_1, robj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        node = _a.sent();
                        if (!node || !node.is_dir) {
                            throw Error("Directory " + p + " no exists");
                        }
                        return [4 /*yield*/, this._getObject(node.node_id)];
                    case 2:
                        obj = _a.sent();
                        if (!obj) {
                            throw Error("Url " + p + " does not exists due to filesystem corruption");
                        }
                        jsonList = JSON.parse(obj.data.toString());
                        objs = [];
                        for (name_1 in jsonList) {
                            if (jsonList.hasOwnProperty(name_1)) {
                                robj = new S3Object();
                                robj.uri = (p === '/' ? '/' : p + '/') + name_1;
                                robj.isDir = jsonList[name_1].is_dir;
                                robj.key = jsonList[name_1].node_id;
                                objs.push(robj);
                            }
                        }
                        return [2 /*return*/, objs];
                }
            });
        });
    };
    S3StorageSystem.prototype.mkdir = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var parent_node, parent_obj, jsonList, basename, new_id, new_obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p || p === '/') {
                            throw Error("Directory " + p + " already exists");
                        }
                        return [4 /*yield*/, this._resolvePath(path.dirname(p))];
                    case 1:
                        parent_node = _a.sent();
                        if (!parent_node || !parent_node.is_dir) {
                            throw Error("Directory " + p + " cannot be made because its parent is missing");
                        }
                        return [4 /*yield*/, this._getObject(parent_node.node_id)];
                    case 2:
                        parent_obj = _a.sent();
                        if (!parent_obj) {
                            throw Error("Directory " + p + " cannot be made due to filesystem corruption");
                        }
                        jsonList = JSON.parse(parent_obj.data.toString());
                        basename = path.basename(p);
                        if (basename in jsonList) {
                            throw Error("Directory " + p + " already exists");
                        }
                        new_id = GenerateRandomID();
                        new_obj = new S3Object();
                        new_obj.data = '{}';
                        return [4 /*yield*/, this._putObject(new_id, new_obj)];
                    case 3:
                        _a.sent();
                        /* Update the parent */
                        jsonList[basename] = { node_id: new_id, is_dir: true };
                        parent_obj.data = JSON.stringify(jsonList);
                        return [4 /*yield*/, this._putObject(parent_node.node_id, parent_obj)];
                    case 4:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 1);
                        return [2 /*return*/];
                }
            });
        });
    };
    /* Two turn around */
    S3StorageSystem.prototype.writeFile = function (p, data) {
        return __awaiter(this, void 0, void 0, function () {
            var parent_node, parent_obj, jsonList, basename, old_node, old_id, new_obj, new_id, new_obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p || p === '/') {
                            throw Error("Directory " + p + " not writable");
                        }
                        return [4 /*yield*/, this._resolvePath(path.dirname(p))];
                    case 1:
                        parent_node = _a.sent();
                        if (!parent_node || !parent_node.is_dir) {
                            throw Error("File " + p + " cannot be write because its parent is missing");
                        }
                        return [4 /*yield*/, this._getObject(parent_node.node_id)];
                    case 2:
                        parent_obj = _a.sent();
                        if (!parent_obj) {
                            throw Error("Directory " + p + " cannot be made due to filesystem corruption");
                        }
                        jsonList = JSON.parse(parent_obj.data.toString());
                        basename = path.basename(p);
                        if (!(basename in jsonList)) return [3 /*break*/, 4];
                        old_node = jsonList[basename];
                        if (old_node.is_dir) {
                            throw Error(" " + p + " is a directory");
                        }
                        old_id = jsonList[basename].node_id;
                        new_obj = new S3Object();
                        new_obj.data = data;
                        return [4 /*yield*/, this._putObject(old_id, new_obj)];
                    case 3:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 0);
                        return [3 /*break*/, 7];
                    case 4:
                        new_id = GenerateRandomID();
                        new_obj = new S3Object();
                        new_obj.data = data;
                        return [4 /*yield*/, this._putObject(new_id, new_obj)];
                    case 5:
                        _a.sent();
                        /* Update the parent */
                        jsonList[basename] = { node_id: new_id, is_dir: false };
                        parent_obj.data = JSON.stringify(jsonList);
                        return [4 /*yield*/, this._putObject(parent_node.node_id, parent_obj)];
                    case 6:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 1);
                        _a.label = 7;
                    case 7: return [2 /*return*/];
                }
            });
        });
    };
    /* Delete file three turnaround, very rarely used, so no optimization */
    S3StorageSystem.prototype.delete = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var parent_node, parent_obj, jsonList, basename;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p || p === '/') {
                            throw Error("Directory " + p + " not deletable");
                        }
                        return [4 /*yield*/, this._resolvePath(path.dirname(p))];
                    case 1:
                        parent_node = _a.sent();
                        if (!parent_node || !parent_node.is_dir) {
                            throw Error("File " + p + " cannot be deleted because its parent is missing");
                        }
                        return [4 /*yield*/, this._getObject(parent_node.node_id)];
                    case 2:
                        parent_obj = _a.sent();
                        if (!parent_obj) {
                            throw Error("Directory " + p + " cannot be made due to filesystem corruption");
                        }
                        jsonList = JSON.parse(parent_obj.data.toString());
                        basename = path.basename(p);
                        if (!(basename in jsonList)) return [3 /*break*/, 4];
                        delete jsonList[basename];
                        parent_obj.data = JSON.stringify(jsonList);
                        return [4 /*yield*/, this._putObject(parent_node.node_id, parent_obj)];
                    case 3:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 2);
                        return [3 /*break*/, 5];
                    case 4: throw Error("Stuff " + p + " does not exist");
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype.zipDir = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var walkResults, zipobj, j, s3obj, buf;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._walkdir(p)];
                    case 1:
                        walkResults = _a.sent();
                        zipobj = new JSZIP();
                        j = 0;
                        _a.label = 2;
                    case 2:
                        if (!(j < walkResults.length)) return [3 /*break*/, 5];
                        return [4 /*yield*/, this._getObject(walkResults[j].node_id)];
                    case 3:
                        s3obj = _a.sent();
                        if (!s3obj) {
                            return [3 /*break*/, 4];
                        }
                        zipobj.file(walkResults[j].uri, s3obj.data);
                        _a.label = 4;
                    case 4:
                        j++;
                        return [3 /*break*/, 2];
                    case 5: return [4 /*yield*/, zipobj.generateAsync({ type: 'uint8array' })];
                    case 6:
                        buf = _a.sent();
                        return [2 /*return*/, buf];
                }
            });
        });
    };
    S3StorageSystem.prototype.copyFile = function (p, newPath) {
        return __awaiter(this, void 0, void 0, function () {
            var self_node, dstParentPath, dstParentNode, dstParentObj, dstJsonList, dstBasename, new_object_id;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        self_node = _a.sent();
                        if (!self_node) {
                            throw Error("Stuff " + p + " does not exist");
                        }
                        if (self_node.is_dir) {
                            throw Error("Stuff " + p + " is a directory");
                        }
                        dstParentPath = path.dirname(newPath);
                        return [4 /*yield*/, this._resolvePath(dstParentPath)];
                    case 2:
                        dstParentNode = _a.sent();
                        if (!dstParentNode) {
                            throw Error("Directory " + dstParentPath + " does no exists");
                        }
                        return [4 /*yield*/, this._getObject(dstParentNode.node_id)];
                    case 3:
                        dstParentObj = _a.sent();
                        if (!dstParentObj) {
                            throw Error("Directory " + dstParentPath + " does no exists due to filesystem corruption");
                        }
                        dstJsonList = JSON.parse(dstParentObj.data.toString());
                        dstBasename = path.basename(newPath);
                        if (dstBasename in dstJsonList && dstJsonList[dstBasename].is_dir !== self_node.is_dir) {
                            throw Error('Cannot overwrite a dir with a file');
                        }
                        new_object_id = GenerateRandomID();
                        return [4 /*yield*/, this._copyObject(self_node.node_id, new_object_id)];
                    case 4:
                        _a.sent();
                        dstJsonList[dstBasename] = { node_id: new_object_id, is_dir: false };
                        dstParentObj.data = JSON.stringify(dstJsonList);
                        return [4 /*yield*/, this._putObject(dstParentNode.node_id, dstParentObj)];
                    case 5:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(newPath, 1);
                        return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype.copyFolder = function (p, newPath) {
        return __awaiter(this, void 0, void 0, function () {
            var self_node, dstParentPath, dstParentNode, dstParentObj, dstJsonList, dstBasename, walk, j, uri, dst;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        self_node = _a.sent();
                        if (!self_node) {
                            throw Error("Stuff " + p + " does not exist");
                        }
                        if (!self_node.is_dir) {
                            throw Error("Stuff " + p + " is a file");
                        }
                        dstParentPath = path.dirname(newPath);
                        return [4 /*yield*/, this._resolvePath(dstParentPath)];
                    case 2:
                        dstParentNode = _a.sent();
                        if (!dstParentNode) {
                            throw Error("Directory " + dstParentPath + " does no exists");
                        }
                        return [4 /*yield*/, this._getObject(dstParentNode.node_id)];
                    case 3:
                        dstParentObj = _a.sent();
                        if (!dstParentObj) {
                            throw Error("Directory " + dstParentPath + " does no exists due to filesystem corruption");
                        }
                        dstJsonList = JSON.parse(dstParentObj.data.toString());
                        dstBasename = path.basename(newPath);
                        if (dstBasename in dstJsonList) {
                            if (dstJsonList[dstBasename].is_dir !== self_node.is_dir) {
                                throw Error('Cannot overwrite a dir with a file');
                            }
                        }
                        return [4 /*yield*/, this._walkdir(p)];
                    case 4:
                        walk = _a.sent();
                        j = 0;
                        _a.label = 5;
                    case 5:
                        if (!(j < walk.length)) return [3 /*break*/, 9];
                        uri = walk[j].uri;
                        dst = newPath + uri.substring(p.length);
                        return [4 /*yield*/, this.ensureDirExist(path.dirname(dst))];
                    case 6:
                        _a.sent();
                        return [4 /*yield*/, this.copyFile(uri, dst)];
                    case 7:
                        _a.sent();
                        _a.label = 8;
                    case 8:
                        j++;
                        return [3 /*break*/, 5];
                    case 9: return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype._walkdir = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var results, objs, j, obj, inner_results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p) {
                            return [2 /*return*/, []];
                        }
                        results = [];
                        return [4 /*yield*/, this.readdir(p)];
                    case 1:
                        objs = _a.sent();
                        j = 0;
                        _a.label = 2;
                    case 2:
                        if (!(j < objs.length)) return [3 /*break*/, 6];
                        obj = objs[j];
                        if (!obj.isDir) return [3 /*break*/, 4];
                        return [4 /*yield*/, this._walkdir(obj.uri)];
                    case 3:
                        inner_results = _a.sent();
                        results = results.concat(inner_results);
                        return [3 /*break*/, 5];
                    case 4:
                        results.push({ uri: obj.uri, node_id: obj.key });
                        _a.label = 5;
                    case 5:
                        j++;
                        return [3 /*break*/, 2];
                    case 6: return [2 /*return*/, results];
                }
            });
        });
    };
    S3StorageSystem.prototype.rename = function (p, newPath) {
        return __awaiter(this, void 0, void 0, function () {
            var srcParentPath, srcParentNode, srcParentObj, srcJsonList, srcBasename, dstParentPath, dstBasename, dstParentNode, dstParentObj, dstJsonList;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!p || p === '/') {
                            throw Error('cannot rename/copy root folder /');
                        }
                        if (p === newPath) {
                            throw Error('Src and Dst are the same');
                        }
                        srcParentPath = path.dirname(p);
                        return [4 /*yield*/, this._resolvePath(srcParentPath)];
                    case 1:
                        srcParentNode = _a.sent();
                        if (!srcParentNode) {
                            throw Error("File or Directory " + p + " does not exists");
                        }
                        return [4 /*yield*/, this._getObject(srcParentNode.node_id)];
                    case 2:
                        srcParentObj = _a.sent();
                        if (!srcParentObj) {
                            throw Error("File or Directory " + p + " does not exists");
                        }
                        srcJsonList = JSON.parse(srcParentObj.data.toString());
                        srcBasename = path.basename(p);
                        if (!(srcBasename in srcJsonList)) {
                            throw Error("File or Directory " + p + " does not exists");
                        }
                        dstParentPath = path.dirname(newPath);
                        dstBasename = path.basename(newPath);
                        if (!(srcParentPath === dstParentPath)) return [3 /*break*/, 4];
                        /* Fast routine */
                        if (dstBasename in srcJsonList && srcJsonList[dstBasename].is_dir !== srcJsonList[srcBasename].is_dir) {
                            throw Error('Cannot overwrite a dir with a file');
                        }
                        srcJsonList[dstBasename] = srcJsonList[srcBasename];
                        delete srcJsonList[srcBasename];
                        srcParentObj.data = JSON.stringify(srcJsonList);
                        return [4 /*yield*/, this._putObject(srcParentNode.node_id, srcParentObj)];
                    case 3:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 2);
                        this.fileSystemWatcherServer.kernel_notify(newPath, 1);
                        return [2 /*return*/];
                    case 4: return [4 /*yield*/, this._resolvePath(dstParentPath)];
                    case 5:
                        dstParentNode = _a.sent();
                        if (!dstParentNode) {
                            throw Error("Directory " + dstParentPath + " does no exists");
                        }
                        return [4 /*yield*/, this._getObject(dstParentNode.node_id)];
                    case 6:
                        dstParentObj = _a.sent();
                        if (!dstParentObj) {
                            throw Error("Directory " + dstParentPath + " does no exists due to filesystem corruption");
                        }
                        dstJsonList = JSON.parse(dstParentObj.data.toString());
                        if (dstBasename in dstJsonList && dstJsonList[dstBasename].is_dir !== srcJsonList[srcBasename].is_dir) {
                            throw Error('Cannot overwrite a dir with a file');
                        }
                        dstJsonList[dstBasename] = srcJsonList[srcBasename];
                        /* Write */
                        delete srcJsonList[srcBasename];
                        srcParentObj.data = JSON.stringify(srcJsonList);
                        return [4 /*yield*/, this._putObject(srcParentNode.node_id, srcParentObj)];
                    case 7:
                        _a.sent();
                        /* Update Dst */
                        dstParentObj.data = JSON.stringify(dstJsonList);
                        return [4 /*yield*/, this._putObject(dstParentNode.node_id, dstParentObj)];
                    case 8:
                        _a.sent();
                        this.fileSystemWatcherServer.kernel_notify(p, 2);
                        this.fileSystemWatcherServer.kernel_notify(newPath, 1);
                        return [2 /*return*/];
                }
            });
        });
    };
    /* Optimized for fast read, two turn around */
    S3StorageSystem.prototype.readFile = function (p) {
        return __awaiter(this, void 0, void 0, function () {
            var self_node, self_obj;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._resolvePath(p)];
                    case 1:
                        self_node = _a.sent();
                        if (!self_node) {
                            throw Error("File " + p + " not exists");
                        }
                        if (self_node.is_dir) {
                            throw Error("File " + p + " is directory");
                        }
                        return [4 /*yield*/, this._getObject(self_node.node_id)];
                    case 2:
                        self_obj = _a.sent();
                        if (!self_obj) {
                            throw Error("File " + p + " cannot be made read to filesystem corruption");
                        }
                        return [2 /*return*/, self_obj.data];
                }
            });
        });
    };
    /* Check whether it is key available */
    S3StorageSystem.prototype._headObject = function (key) {
        return __awaiter(this, void 0, void 0, function () {
            var headResult, filestat, err_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._lock()];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2:
                        _a.trys.push([2, 4, 5, 6]);
                        return [4 /*yield*/, this.s3.headObject({
                                Bucket: this.opts.bucket,
                                Key: this.opts.prefix + key,
                            }).promise()];
                    case 3:
                        headResult = _a.sent();
                        filestat = new S3Object();
                        filestat.meta = headResult.Metadata || {};
                        filestat.ETag = headResult.ETag;
                        filestat.size = headResult.ContentLength;
                        filestat.modifiedTime = headResult.LastModified;
                        filestat.key = key;
                        return [2 /*return*/, filestat];
                    case 4:
                        err_1 = _a.sent();
                        if (err_1.code === 'NotFound') {
                            return [2 /*return*/, undefined];
                        }
                        throw err_1;
                    case 5:
                        this._unlock();
                        return [7 /*endfinally*/];
                    case 6: return [2 /*return*/];
                }
            });
        });
    };
    /* Get object */
    S3StorageSystem.prototype._getObject = function (key) {
        return __awaiter(this, void 0, void 0, function () {
            var getResult, obj, err_2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._lock()];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2:
                        _a.trys.push([2, 4, 5, 6]);
                        return [4 /*yield*/, this.s3.getObject({
                                Bucket: this.opts.bucket,
                                Key: this.opts.prefix + key,
                                ResponseCacheControl: 'max-age=0'
                            }).promise()];
                    case 3:
                        getResult = _a.sent();
                        obj = new S3Object();
                        obj.size = getResult.ContentLength;
                        obj.ETag = getResult.ETag;
                        obj.data = getResult.Body;
                        obj.modifiedTime = getResult.LastModified;
                        obj.meta = getResult.Metadata || {};
                        obj.key = key;
                        return [2 /*return*/, obj];
                    case 4:
                        err_2 = _a.sent();
                        if (err_2.code === 'NotFound') {
                            return [2 /*return*/, undefined];
                        }
                        throw err_2;
                    case 5:
                        this._unlock();
                        return [7 /*endfinally*/];
                    case 6: return [2 /*return*/];
                }
            });
        });
    };
    /* Put object */
    S3StorageSystem.prototype._putObject = function (key, obj) {
        return __awaiter(this, void 0, void 0, function () {
            var s3CompatMeta, metaKey, e_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._lock()];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2:
                        _a.trys.push([2, 4, 5, 6]);
                        s3CompatMeta = {};
                        if (obj.meta) {
                            for (metaKey in obj.meta) {
                                if (obj.meta.hasOwnProperty(metaKey)) {
                                    s3CompatMeta['x-amz-meta-' + metaKey] = obj.meta[metaKey];
                                }
                            }
                        }
                        return [4 /*yield*/, this.s3.putObject({
                                Body: obj.data,
                                Bucket: this.opts.bucket,
                                Key: this.opts.prefix + key,
                                Metadata: s3CompatMeta,
                            }).promise()];
                    case 3:
                        _a.sent();
                        return [3 /*break*/, 6];
                    case 4:
                        e_1 = _a.sent();
                        throw e_1;
                    case 5:
                        this._unlock();
                        return [7 /*endfinally*/];
                    case 6: return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype._copyObject = function (srcKey, dstKey) {
        return __awaiter(this, void 0, void 0, function () {
            var e_2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this._lock()];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2:
                        _a.trys.push([2, 4, 5, 6]);
                        return [4 /*yield*/, this.s3.copyObject({
                                Bucket: this.opts.bucket,
                                CopySource: this.opts.bucket + '/' + this.opts.prefix + srcKey,
                                Key: this.opts.prefix + dstKey,
                            }).promise()];
                    case 3:
                        _a.sent();
                        return [3 /*break*/, 6];
                    case 4:
                        e_2 = _a.sent();
                        throw e_2;
                    case 5:
                        this._unlock();
                        return [7 /*endfinally*/];
                    case 6: return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype._lock = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!this.s3lock) return [3 /*break*/, 2];
                        return [4 /*yield*/, new Promise(function (r) { return setTimeout(r, 10); })];
                    case 1:
                        _a.sent();
                        return [3 /*break*/, 0];
                    case 2:
                        this.s3lock = true;
                        return [4 /*yield*/, this._prepareLocked()];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    S3StorageSystem.prototype._unlock = function () {
        this.s3lock = false;
    };
    /* Assume Lock */
    S3StorageSystem.prototype._prepareLocked = function () {
        return __awaiter(this, void 0, void 0, function () {
            var creds, json_creds;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!(Date.now() > this.opts.expiry)) return [3 /*break*/, 3];
                        /* Need to renew */
                        this.s3 = undefined;
                        return [4 /*yield*/, fetch('/key')];
                    case 1:
                        creds = _a.sent();
                        return [4 /*yield*/, creds.json()];
                    case 2:
                        json_creds = _a.sent();
                        this.opts.apiKey = json_creds['_APIKEY'];
                        this.opts.apiSecret = json_creds['_APISECRET'];
                        this.opts.sessionToken = json_creds['_SESSIONTOKEN'];
                        this.opts.prefix = json_creds['_PREFIX'];
                        this.opts.bucket = json_creds['_BUCKET'];
                        this.opts.expiry = Date.now() + 60 * 2 * 1000;
                        this.opts.endpoint = json_creds['_ENDPOINT'];
                        if (!this.opts.apiKey) {
                            window.alert('Failed to connect to SwiftLaTeX');
                            throw Error('Failed to obtain an api key');
                        }
                        if (!this.opts.prefix || !this.opts.prefix.endsWith('/')) {
                            throw Error('Prefix should not be empty or endwith a slash.');
                        }
                        /* Have a new client */
                        this.s3 = new S3({
                            accessKeyId: this.opts.apiKey,
                            secretAccessKey: this.opts.apiSecret,
                            sessionToken: this.opts.sessionToken,
                            endpoint: this.opts.endpoint,
                            s3ForcePathStyle: true,
                            signatureVersion: 'v4',
                        });
                        _a.label = 3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(filesystem_watcher_protocol_1.FileSystemWatcherServer),
        __metadata("design:type", Object)
    ], S3StorageSystem.prototype, "fileSystemWatcherServer", void 0);
    S3StorageSystem = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], S3StorageSystem);
    return S3StorageSystem;
}());
exports.S3StorageSystem = S3StorageSystem;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/s3storagesystem'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ 0:
/*!********************!*\
  !*** fs (ignored) ***!
  \********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ }),

/***/ 1:
/*!**********************!*\
  !*** util (ignored) ***!
  \**********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ }),

/***/ 2:
/*!**********************!*\
  !*** util (ignored) ***!
  \**********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ })

}]);
//# sourceMappingURL=3.bundle.js.map