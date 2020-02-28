(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[33],{

/***/ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/file-dialog.css":
/*!*************************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/filesystem/src/browser/style/file-dialog.css ***!
  \*************************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n /*\n  * Open and Save file dialogs\n  */\n\n.dialogContent .theia-FileDialog,\n.dialogContent .theia-SaveFileDialog,\n.dialogContent .theia-ResponsiveFileDialog {\n    height: 500px;\n    width: 500px;\n    border: 1px solid var(--theia-editorWidget-border);\n    background: var(--theia-dropdown-background);\n}\n\n\n@media only screen and (max-height: 700px) {\n    .dialogContent .theia-FileDialog,\n    .dialogContent .theia-SaveFileDialog,\n    .dialogContent .theia-ResponsiveFileDialog {\n        height: 300px;\n    }\n}\n\n.dialogContent .theia-NavigationPanel,\n.dialogContent .theia-FiltersPanel,\n.dialogContent .theia-FileNamePanel {\n    display: block;\n    position: relative;\n    overflow: hidden;\n}\n\n.dialogContent .theia-NavigationPanel,\n.dialogContent .theia-FiltersPanel {\n    min-height: 27px;\n}\n\n.dialogContent .theia-FileNamePanel {\n    height: 31px;\n}\n\n/*\n * Navigation panel items\n */\n\n.dialogContent .theia-NavigationBack,\n.dialogContent .theia-NavigationForward,\n.dialogContent .theia-NavigationHome {\n    position: absolute;\n    top: 0px;\n    line-height: 23px;\n    cursor: pointer;\n    outline: none;\n}\n\n.dialogContent .theia-NavigationBack:focus,\n.dialogContent .theia-NavigationForward:focus,\n.dialogContent .theia-NavigationHome:focus {\n    outline: none;\n    border: none;\n    box-shadow: none;\n}\n\n.dialogContent .theia-NavigationBack {\n    left: auto;\n}\n\n.dialogContent .theia-NavigationForward {\n    left: 21px;\n}\n\n.dialogContent .theia-NavigationHome {\n    left: 41px;\n}\n\n.dialogContent .theia-LocationListPanel {\n    position: absolute;\n    left: 72px;\n    top: 1px;\n}\n\n.dialogContent .theia-LocationList {\n    width: 427px;\n    height: 21px;\n}\n\n/*\n * Filters panel items\n */\n\n.dialogContent .theia-FiltersLabel {\n    position: absolute;\n    left: 1px;\n    top: 0px;\n    line-height: 27px;\n}\n\n.dialogContent .theia-FiltersListPanel {\n    position: absolute;\n    left: 72px;\n    top: 3px;\n}\n\n.dialogContent .theia-FileTreeFiltersList {\n    width: 427px;\n    height: 21px;    \n}\n\n/*\n * File name panel items\n */\n\n.dialogContent .theia-FileNameLabel {\n    position: absolute;\n    left: 1px;\n    top: 0px;\n    line-height: 23px;\n}\n\n.dialogContent .theia-FileNameTextField {\n    position: absolute;\n    left: 72px;\n    top: 0px;\n    width: 420px;\n}\n\n/*\n * Control panel items\n */\n\n.dialogContent .theia-ControlPanel {\n    display: flex;\n    flex-direction: row;\n    justify-content: flex-end;\n    margin-bottom: 0px;\n}\n\n.dialogContent .theia-ControlPanel > * {\n    margin-left: 4px;\n}", ""]);

// exports


/***/ }),

/***/ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/file-icons.css":
/*!************************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/filesystem/src/browser/style/file-icons.css ***!
  \************************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n .theia-file-icons-js {\n    /*\n    Here, the `line-height` ensures that FontAwesome and `file-icons-js` container has the same height.\n    Ideally, it would be 1 em, but since we set the max height above (and other places too) with 0.8\n    multiplier, we use 0.8 em here too.\n     */\n    line-height: 0.8em;\n}\n\n.theia-file-icons-js:before {\n    font-size: calc(var(--theia-content-font-size) * 0.8);\n}\n\n.theia-file-icons-js.file-icon {\n    width: var(--theia-icon-size);\n    height: var(--theia-icon-size);\n    display: flex;\n    justify-content: center;\n    align-items: center;\n}\n\n.fa-file:before,\n.fa-folder:before,\n.theia-file-icons-js:before {\n    text-align: center;\n    margin-right: 4px;\n}\n\n.theia-app-sides .theia-file-icons-js {\n    max-height: none;\n    line-height: inherit;\n}\n\n.theia-app-sides .theia-file-icons-js:before {\n    font-size: var(--theia-private-sidebar-icon-size);\n    margin-right: 0px;\n}", ""]);

// exports


/***/ }),

/***/ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/index.css":
/*!*******************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/filesystem/src/browser/style/index.css ***!
  \*******************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports
exports.i(__webpack_require__(/*! -!../../../../../node_modules/css-loader!./file-dialog.css */ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/file-dialog.css"), "");
exports.i(__webpack_require__(/*! -!../../../../../node_modules/css-loader!./file-icons.css */ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/file-icons.css"), "");

// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n.theia-file-tree-drag-image {\n  position: absolute;\n  /*\n    make sure you don't see it flashing\n     */\n  top: -1000px;\n  font-size: var(--theia-ui-font-size1);\n  display: inline-block;\n  padding: 1px calc(var(--theia-ui-padding)*2);\n  border-radius: 10px;\n\n  background: var(--theia-list-activeSelectionBackground);\n  color: var(--theia-list-activeSelectionForeground);\n  outline: 1px solid var(--theia-contrastActiveBorder); \n  outline-offset: -1px;\n}", ""]);

// exports


/***/ }),

/***/ "../../packages/filesystem/lib/browser/file-tree/file-tree-label-provider.js":
/*!*****************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-tree/file-tree-label-provider.js ***!
  \*****************************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/label-provider */ "../../packages/core/lib/browser/label-provider.js");
var file_tree_1 = __webpack_require__(/*! ./file-tree */ "../../packages/filesystem/lib/browser/file-tree/file-tree.js");
var tree_label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-label-provider */ "../../packages/core/lib/browser/tree/tree-label-provider.js");
var FileTreeLabelProvider = /** @class */ (function () {
    function FileTreeLabelProvider() {
    }
    FileTreeLabelProvider.prototype.canHandle = function (element) {
        return file_tree_1.FileStatNode.is(element) ?
            this.treeLabelProvider.canHandle(element) + 1 :
            0;
    };
    FileTreeLabelProvider.prototype.getIcon = function (node) {
        return this.labelProvider.getIcon(node.fileStat);
    };
    FileTreeLabelProvider.prototype.getName = function (node) {
        return this.labelProvider.getName(node.fileStat);
    };
    FileTreeLabelProvider.prototype.getDescription = function (node) {
        return this.labelProvider.getLongName(node.fileStat);
    };
    FileTreeLabelProvider.prototype.affects = function (node, event) {
        return event.affects(node.fileStat);
    };
    __decorate([
        inversify_1.inject(label_provider_1.LabelProvider),
        __metadata("design:type", label_provider_1.LabelProvider)
    ], FileTreeLabelProvider.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(tree_label_provider_1.TreeLabelProvider),
        __metadata("design:type", tree_label_provider_1.TreeLabelProvider)
    ], FileTreeLabelProvider.prototype, "treeLabelProvider", void 0);
    FileTreeLabelProvider = __decorate([
        inversify_1.injectable()
    ], FileTreeLabelProvider);
    return FileTreeLabelProvider;
}());
exports.FileTreeLabelProvider = FileTreeLabelProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-tree/file-tree-label-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/filesystem-browserfs-watcher.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/filesystem-browserfs-watcher.js ***!
  \***********************************************************************************************************/
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
var WorkspaceBrowserFileSystemWatcher = /** @class */ (function () {
    function WorkspaceBrowserFileSystemWatcher() {
        /**
         * Start file watching for the given param.
         * Resolve when watching is started.
         * Return a watcher id.
         */
        this.files2Watch = new Map();
    }
    WorkspaceBrowserFileSystemWatcher.prototype.watchFileChanges = function (uri, options) {
        return __awaiter(this, void 0, void 0, function () {
            var currentItemCount;
            return __generator(this, function (_a) {
                if (!this.files2Watch.has(uri)) {
                    currentItemCount = this.files2Watch.size + 1;
                    console.log("Start watching " + uri + " with ID " + currentItemCount);
                    this.files2Watch.set(uri, currentItemCount);
                    return [2 /*return*/, currentItemCount];
                }
                else {
                    console.log("Already watching " + uri);
                    return [2 /*return*/, this.files2Watch.get(uri)];
                }
                return [2 /*return*/];
            });
        });
    };
    /**
     * Stop file watching for the given id.
     * Resolve when watching is stopped.
     */
    WorkspaceBrowserFileSystemWatcher.prototype.unwatchFileChanges = function (watcher) {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, key;
            var e_1, _c;
            return __generator(this, function (_d) {
                try {
                    for (_a = __values(this.files2Watch.keys()), _b = _a.next(); !_b.done; _b = _a.next()) {
                        key = _b.value;
                        if (this.files2Watch.get(key) === watcher) {
                            console.log("Unwatch " + key + " with ID " + watcher);
                            this.files2Watch.delete(key);
                            break;
                        }
                    }
                }
                catch (e_1_1) { e_1 = { error: e_1_1 }; }
                finally {
                    try {
                        if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                    }
                    finally { if (e_1) throw e_1.error; }
                }
                return [2 /*return*/];
            });
        });
    };
    WorkspaceBrowserFileSystemWatcher.prototype.kernel_notify = function (path, changeType) {
        if (changeType === void 0) { changeType = 0; }
        return __awaiter(this, void 0, void 0, function () {
            var changes, _a, _b, key, thatClient;
            var e_2, _c;
            return __generator(this, function (_d) {
                // console.log(`Notify coming ${path}`);
                if (!this.client) {
                    return [2 /*return*/];
                }
                if (!path.startsWith('file://')) {
                    path = 'file://' + path;
                }
                changes = [];
                try {
                    for (_a = __values(this.files2Watch.keys()), _b = _a.next(); !_b.done; _b = _a.next()) {
                        key = _b.value;
                        if (path.startsWith(key)) {
                            if (changeType !== 0) {
                                /* Notify Unconditionally */
                                changes.push({ uri: key, type: changeType });
                            }
                            else {
                                /* Only Notify Non-Root One, Notify root will cause full refresh */
                                if (key !== 'file:///') {
                                    changes.push({ uri: key, type: changeType });
                                }
                            }
                            // console.log(key);
                        }
                    }
                }
                catch (e_2_1) { e_2 = { error: e_2_1 }; }
                finally {
                    try {
                        if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                    }
                    finally { if (e_2) throw e_2.error; }
                }
                changes.push({ uri: path, type: changeType });
                thatClient = this.client;
                setTimeout(function () {
                    thatClient.onDidFilesChanged({
                        changes: changes
                    });
                }, 100);
                return [2 /*return*/];
            });
        });
    };
    WorkspaceBrowserFileSystemWatcher.prototype.setClient = function (client) {
        this.client = client;
    };
    WorkspaceBrowserFileSystemWatcher.prototype.dispose = function () {
        this.files2Watch.clear();
    };
    WorkspaceBrowserFileSystemWatcher = __decorate([
        inversify_1.injectable()
    ], WorkspaceBrowserFileSystemWatcher);
    return WorkspaceBrowserFileSystemWatcher;
}());
exports.WorkspaceBrowserFileSystemWatcher = WorkspaceBrowserFileSystemWatcher;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/filesystem-browserfs-watcher'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/lib/browser/filesystem-frontend-module.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/filesystem-frontend-module.js ***!
  \*********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2017-2018 TypeFox and others.
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
var _this = this;
Object.defineProperty(exports, "__esModule", { value: true });
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/filesystem/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var common_2 = __webpack_require__(/*! ../common */ "../../packages/filesystem/lib/common/index.js");
var filesystem_watcher_protocol_1 = __webpack_require__(/*! ../common/filesystem-watcher-protocol */ "../../packages/filesystem/lib/common/filesystem-watcher-protocol.js");
var filesystem_browserfs_watcher_1 = __webpack_require__(/*! ./filesystem-browserfs-watcher */ "../../packages/filesystem/lib/browser/filesystem-browserfs-watcher.js");
// import {
//     fileSystemWatcherPath, FileSystemWatcherServer,
//     FileSystemWatcherServerProxy, ReconnectingFileSystemWatcherServer
// } from '../common/filesystem-watcher-protocol';
var file_resource_1 = __webpack_require__(/*! ./file-resource */ "../../packages/filesystem/lib/browser/file-resource.js");
var filesystem_preferences_1 = __webpack_require__(/*! ./filesystem-preferences */ "../../packages/filesystem/lib/browser/filesystem-preferences.js");
var filesystem_watcher_1 = __webpack_require__(/*! ./filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var filesystem_frontend_contribution_1 = __webpack_require__(/*! ./filesystem-frontend-contribution */ "../../packages/filesystem/lib/browser/filesystem-frontend-contribution.js");
// import { FileSystemProxyFactory } from './filesystem-proxy-factory';
var file_upload_service_1 = __webpack_require__(/*! ./file-upload-service */ "../../packages/filesystem/lib/browser/file-upload-service.js");
var file_tree_label_provider_1 = __webpack_require__(/*! ./file-tree/file-tree-label-provider */ "../../packages/filesystem/lib/browser/file-tree/file-tree-label-provider.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var filesystem_browserfs_1 = __webpack_require__(/*! ./filesystem-browserfs */ "../../packages/filesystem/lib/browser/filesystem-browserfs.js");
var s3storagesystem_1 = __webpack_require__(/*! ./s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    filesystem_preferences_1.bindFileSystemPreferences(bind);
    // bind(FileSystemWatcherServerProxy).toDynamicValue(ctx =>
    //     WebSocketConnectionProvider.createProxy(ctx.container, fileSystemWatcherPath)
    // );
    bind(filesystem_watcher_protocol_1.FileSystemWatcherServer).to(filesystem_browserfs_watcher_1.WorkspaceBrowserFileSystemWatcher).inSingletonScope();
    bind(filesystem_watcher_1.FileSystemWatcher).toSelf().inSingletonScope();
    bind(common_2.FileShouldOverwrite).toDynamicValue(function (context) { return function (file, stat) { return __awaiter(_this, void 0, void 0, function () {
        var labelProvider, dialog;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    labelProvider = context.container.get(browser_1.LabelProvider);
                    dialog = new browser_1.ConfirmDialog({
                        title: "The file '" + labelProvider.getName(new uri_1.default(file.uri)) + "' has been changed on the file system.",
                        msg: "Do you want to overwrite the changes made to '" + labelProvider.getLongName(new uri_1.default(file.uri)) + "' on the file system?",
                        ok: 'Yes',
                        cancel: 'No'
                    });
                    return [4 /*yield*/, dialog.open()];
                case 1: return [2 /*return*/, !!(_a.sent())];
            }
        });
    }); }; }).inSingletonScope();
    // bind(FileSystemProxyFactory).toSelf();
    // bind(FileSystem).toDynamicValue(ctx => {
    //     const proxyFactory = ctx.container.get(FileSystemProxyFactory);
    //     return WebSocketConnectionProvider.createProxy(ctx.container, fileSystemPath, proxyFactory);
    // }).inSingletonScope();
    bind(s3storagesystem_1.S3StorageSystem).toSelf().inSingletonScope();
    bind(common_2.FileSystem).to(filesystem_browserfs_1.BrowserFileSystem).inSingletonScope();
    bindFileResource(bind);
    bind(file_upload_service_1.FileUploadService).toSelf().inSingletonScope();
    bind(filesystem_frontend_contribution_1.FileSystemFrontendContribution).toSelf().inSingletonScope();
    bind(common_1.CommandContribution).toService(filesystem_frontend_contribution_1.FileSystemFrontendContribution);
    bind(browser_1.FrontendApplicationContribution).toService(filesystem_frontend_contribution_1.FileSystemFrontendContribution);
    bind(file_tree_label_provider_1.FileTreeLabelProvider).toSelf().inSingletonScope();
    bind(browser_1.LabelProviderContribution).toService(file_tree_label_provider_1.FileTreeLabelProvider);
});
function bindFileResource(bind) {
    bind(file_resource_1.FileResourceResolver).toSelf().inSingletonScope();
    bind(common_1.ResourceResolver).toService(file_resource_1.FileResourceResolver);
}
exports.bindFileResource = bindFileResource;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/filesystem-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/filesystem/src/browser/style/index.css":
/*!*******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/src/browser/style/index.css ***!
  \*******************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/filesystem/src/browser/style/index.css");

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
//# sourceMappingURL=33.bundle.js.map