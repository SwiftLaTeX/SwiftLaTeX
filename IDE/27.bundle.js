(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[27],{

/***/ "../../node_modules/base64-arraybuffer/lib/base64-arraybuffer.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/base64-arraybuffer/lib/base64-arraybuffer.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {/*
 * base64-arraybuffer
 * https://github.com/niklasvh/base64-arraybuffer
 *
 * Copyright (c) 2012 Niklas von Hertzen
 * Licensed under the MIT license.
 */
(function(){
  "use strict";

  var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  // Use a lookup table to find the index.
  var lookup = new Uint8Array(256);
  for (var i = 0; i < chars.length; i++) {
    lookup[chars.charCodeAt(i)] = i;
  }

  exports.encode = function(arraybuffer) {
    var bytes = new Uint8Array(arraybuffer),
    i, len = bytes.length, base64 = "";

    for (i = 0; i < len; i+=3) {
      base64 += chars[bytes[i] >> 2];
      base64 += chars[((bytes[i] & 3) << 4) | (bytes[i + 1] >> 4)];
      base64 += chars[((bytes[i + 1] & 15) << 2) | (bytes[i + 2] >> 6)];
      base64 += chars[bytes[i + 2] & 63];
    }

    if ((len % 3) === 2) {
      base64 = base64.substring(0, base64.length - 1) + "=";
    } else if (len % 3 === 1) {
      base64 = base64.substring(0, base64.length - 2) + "==";
    }

    return base64;
  };

  exports.decode =  function(base64) {
    var bufferLength = base64.length * 0.75,
    len = base64.length, i, p = 0,
    encoded1, encoded2, encoded3, encoded4;

    if (base64[base64.length - 1] === "=") {
      bufferLength--;
      if (base64[base64.length - 2] === "=") {
        bufferLength--;
      }
    }

    var arraybuffer = new ArrayBuffer(bufferLength),
    bytes = new Uint8Array(arraybuffer);

    for (i = 0; i < len; i+=4) {
      encoded1 = lookup[base64.charCodeAt(i)];
      encoded2 = lookup[base64.charCodeAt(i+1)];
      encoded3 = lookup[base64.charCodeAt(i+2)];
      encoded4 = lookup[base64.charCodeAt(i+3)];

      bytes[p++] = (encoded1 << 2) | (encoded2 >> 4);
      bytes[p++] = ((encoded2 & 15) << 4) | (encoded3 >> 2);
      bytes[p++] = ((encoded3 & 3) << 6) | (encoded4 & 63);
    }

    return arraybuffer;
  };
})();

if (!global) global = {};
(global['theia'] = global['theia'] || {})['base64-arraybuffer/lib/base64-arraybuffer'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/css-loader/index.js!../../packages/preferences/src/browser/style/index.css":
/*!********************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/preferences/src/browser/style/index.css ***!
  \********************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2019 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n#preferences_container_widget .p-SplitPanel-handle {\n    border-right: var(--theia-border-width) solid var(--theia-editorGroup-border);\n}\n\n#preferences_container_widget .p-TabBar-tabIcon {\n    align-items: center;\n    display: flex;\n    line-height: var(--theia-content-line-height) !important;\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/preferences/lib/browser/abstract-resource-preference-provider.js":
/*!*********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/abstract-resource-preference-provider.js ***!
  \*********************************************************************************************************************/
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
/* eslint-disable @typescript-eslint/no-explicit-any */
/* eslint-disable no-null/no-null */
var jsoncparser = __webpack_require__(/*! jsonc-parser */ "../../node_modules/jsonc-parser/lib/esm/main.js");
var json_1 = __webpack_require__(/*! @phosphor/coreutils/lib/json */ "../../node_modules/@phosphor/coreutils/lib/json.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var message_service_1 = __webpack_require__(/*! @theia/core/lib/common/message-service */ "../../packages/core/lib/common/message-service.js");
var disposable_1 = __webpack_require__(/*! @theia/core/lib/common/disposable */ "../../packages/core/lib/common/disposable.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var preference_configurations_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-configurations */ "../../packages/core/lib/browser/preferences/preference-configurations.js");
var monaco_text_model_service_1 = __webpack_require__(/*! @theia/monaco/lib/browser/monaco-text-model-service */ "../../packages/monaco/lib/browser/monaco-text-model-service.js");
var monaco_workspace_1 = __webpack_require__(/*! @theia/monaco/lib/browser/monaco-workspace */ "../../packages/monaco/lib/browser/monaco-workspace.js");
var promise_util_1 = __webpack_require__(/*! @theia/core/lib/common/promise-util */ "../../packages/core/lib/common/promise-util.js");
var AbstractResourcePreferenceProvider = /** @class */ (function (_super) {
    __extends(AbstractResourcePreferenceProvider, _super);
    function AbstractResourcePreferenceProvider() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.preferences = {};
        _this.loading = new promise_util_1.Deferred();
        return _this;
    }
    AbstractResourcePreferenceProvider.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var uri, reference;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        uri = this.getUri();
                        this.toDispose.push(disposable_1.Disposable.create(function () { return _this.loading.reject(new Error("preference provider for '" + uri + "' was disposed")); }));
                        this._ready.resolve();
                        return [4 /*yield*/, this.textModelService.createModelReference(uri)];
                    case 1:
                        reference = _a.sent();
                        if (this.toDispose.disposed) {
                            reference.dispose();
                            return [2 /*return*/];
                        }
                        this.model = reference.object;
                        this.loading.resolve();
                        this.toDispose.push(reference);
                        this.toDispose.push(disposable_1.Disposable.create(function () { return _this.model = undefined; }));
                        this.readPreferences();
                        this.toDispose.push(this.model.onDidChangeContent(function () { return _this.readPreferences(); }));
                        this.toDispose.push(this.model.onDirtyChanged(function () { return _this.readPreferences(); }));
                        this.toDispose.push(this.model.onDidChangeValid(function () { return _this.readPreferences(); }));
                        this.toDispose.push(disposable_1.Disposable.create(function () { return _this.reset(); }));
                        return [2 /*return*/];
                }
            });
        });
    };
    Object.defineProperty(AbstractResourcePreferenceProvider.prototype, "valid", {
        get: function () {
            return this.model && this.model.valid || false;
        },
        enumerable: true,
        configurable: true
    });
    AbstractResourcePreferenceProvider.prototype.getConfigUri = function (resourceUri) {
        if (!resourceUri) {
            return this.getUri();
        }
        return this.valid && this.contains(resourceUri) ? this.getUri() : undefined;
    };
    AbstractResourcePreferenceProvider.prototype.contains = function (resourceUri) {
        if (!resourceUri) {
            return true;
        }
        var domain = this.getDomain();
        if (!domain) {
            return true;
        }
        var resourcePath = new uri_1.default(resourceUri).path;
        return domain.some(function (uri) { return new uri_1.default(uri).path.relativity(resourcePath) >= 0; });
    };
    AbstractResourcePreferenceProvider.prototype.getPreferences = function (resourceUri) {
        return this.valid && this.contains(resourceUri) ? this.preferences : {};
    };
    AbstractResourcePreferenceProvider.prototype.setPreference = function (key, value, resourceUri) {
        return __awaiter(this, void 0, void 0, function () {
            var path, content, textModel, editOperations, _a, insertSpaces, tabSize, defaultEOL, _b, _c, edit, start, end, e_1, message;
            var e_2, _d;
            return __generator(this, function (_e) {
                switch (_e.label) {
                    case 0: return [4 /*yield*/, this.loading.promise];
                    case 1:
                        _e.sent();
                        if (!this.model) {
                            return [2 /*return*/, false];
                        }
                        if (!this.contains(resourceUri)) {
                            return [2 /*return*/, false];
                        }
                        path = this.getPath(key);
                        if (!path) {
                            return [2 /*return*/, false];
                        }
                        _e.label = 2;
                    case 2:
                        _e.trys.push([2, 4, , 5]);
                        content = this.model.getText().trim();
                        if (!content && value === undefined) {
                            return [2 /*return*/, true];
                        }
                        textModel = this.model.textEditorModel;
                        editOperations = [];
                        if (path.length || value !== undefined) {
                            _a = textModel.getOptions(), insertSpaces = _a.insertSpaces, tabSize = _a.tabSize, defaultEOL = _a.defaultEOL;
                            try {
                                for (_b = __values(jsoncparser.modify(content, path, value, {
                                    formattingOptions: {
                                        insertSpaces: insertSpaces,
                                        tabSize: tabSize,
                                        eol: defaultEOL === monaco.editor.DefaultEndOfLine.LF ? '\n' : '\r\n'
                                    }
                                })), _c = _b.next(); !_c.done; _c = _b.next()) {
                                    edit = _c.value;
                                    start = textModel.getPositionAt(edit.offset);
                                    end = textModel.getPositionAt(edit.offset + edit.length);
                                    editOperations.push({
                                        range: monaco.Range.fromPositions(start, end),
                                        text: edit.content || null,
                                        forceMoveMarkers: false
                                    });
                                }
                            }
                            catch (e_2_1) { e_2 = { error: e_2_1 }; }
                            finally {
                                try {
                                    if (_c && !_c.done && (_d = _b.return)) _d.call(_b);
                                }
                                finally { if (e_2) throw e_2.error; }
                            }
                        }
                        else {
                            editOperations.push({
                                range: textModel.getFullModelRange(),
                                text: null,
                                forceMoveMarkers: false
                            });
                        }
                        return [4 /*yield*/, this.workspace.applyBackgroundEdit(this.model, editOperations)];
                    case 3:
                        _e.sent();
                        return [2 /*return*/, true];
                    case 4:
                        e_1 = _e.sent();
                        message = "Failed to update the value of '" + key + "' in '" + this.getUri() + "'.";
                        this.messageService.error(message + " Please check if it is corrupted.");
                        console.error("" + message, e_1);
                        return [2 /*return*/, false];
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    AbstractResourcePreferenceProvider.prototype.getPath = function (preferenceName) {
        return [preferenceName];
    };
    /**
     * It HAS to be sync to ensure that `setPreference` returns only when values are updated
     * or any other operation modifying the monaco model content.
     */
    AbstractResourcePreferenceProvider.prototype.readPreferences = function () {
        var model = this.model;
        if (!model || model.dirty) {
            return;
        }
        try {
            var preferences = void 0;
            if (model.valid) {
                var content = model.getText();
                preferences = this.getParsedContent(content);
            }
            else {
                preferences = {};
            }
            this.handlePreferenceChanges(preferences);
        }
        catch (e) {
            console.error("Failed to load preferences from '" + this.getUri() + "'.", e);
        }
    };
    AbstractResourcePreferenceProvider.prototype.getParsedContent = function (content) {
        var jsonData = this.parse(content);
        var preferences = {};
        if (typeof jsonData !== 'object') {
            return preferences;
        }
        // eslint-disable-next-line guard-for-in
        for (var preferenceName in jsonData) {
            var preferenceValue = jsonData[preferenceName];
            if (this.schemaProvider.testOverrideValue(preferenceName, preferenceValue)) {
                // eslint-disable-next-line guard-for-in
                for (var overriddenPreferenceName in preferenceValue) {
                    var overriddenValue = preferenceValue[overriddenPreferenceName];
                    preferences[preferenceName + "." + overriddenPreferenceName] = overriddenValue;
                }
            }
            else {
                preferences[preferenceName] = preferenceValue;
            }
        }
        return preferences;
    };
    AbstractResourcePreferenceProvider.prototype.parse = function (content) {
        content = content.trim();
        if (!content) {
            return undefined;
        }
        var strippedContent = jsoncparser.stripComments(content);
        return jsoncparser.parse(strippedContent);
    };
    AbstractResourcePreferenceProvider.prototype.handlePreferenceChanges = function (newPrefs) {
        var e_3, _a;
        var oldPrefs = Object.assign({}, this.preferences);
        this.preferences = newPrefs;
        var prefNames = new Set(__spread(Object.keys(oldPrefs), Object.keys(newPrefs)));
        var prefChanges = [];
        var uri = this.getUri();
        try {
            for (var _b = __values(prefNames.values()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var prefName = _c.value;
                var oldValue = oldPrefs[prefName];
                var newValue = newPrefs[prefName];
                var schemaProperties = this.schemaProvider.getCombinedSchema().properties[prefName];
                if (schemaProperties) {
                    var scope = schemaProperties.scope;
                    // do not emit the change event if the change is made out of the defined preference scope
                    if (!this.schemaProvider.isValidInScope(prefName, this.getScope())) {
                        console.warn("Preference " + prefName + " in " + uri + " can only be defined in scopes: " + browser_1.PreferenceScope.getScopeNames(scope).join(', ') + ".");
                        continue;
                    }
                }
                if (newValue === undefined && oldValue !== newValue
                    || oldValue === undefined && newValue !== oldValue // JSONExt.deepEqual() does not support handling `undefined`
                    || !json_1.JSONExt.deepEqual(oldValue, newValue)) {
                    prefChanges.push({
                        preferenceName: prefName, newValue: newValue, oldValue: oldValue, scope: this.getScope(), domain: this.getDomain()
                    });
                }
            }
        }
        catch (e_3_1) { e_3 = { error: e_3_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_3) throw e_3.error; }
        }
        if (prefChanges.length > 0) { // do not emit the change event if the pref value is not changed
            this.emitPreferencesChangedEvent(prefChanges);
        }
    };
    AbstractResourcePreferenceProvider.prototype.reset = function () {
        var e_4, _a;
        var preferences = this.preferences;
        this.preferences = {};
        var changes = [];
        try {
            for (var _b = __values(Object.keys(preferences)), _c = _b.next(); !_c.done; _c = _b.next()) {
                var prefName = _c.value;
                var value = preferences[prefName];
                if (value !== undefined) {
                    changes.push({
                        preferenceName: prefName, newValue: undefined, oldValue: value, scope: this.getScope(), domain: this.getDomain()
                    });
                }
            }
        }
        catch (e_4_1) { e_4 = { error: e_4_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_4) throw e_4.error; }
        }
        if (changes.length > 0) {
            this.emitPreferencesChangedEvent(changes);
        }
    };
    __decorate([
        inversify_1.inject(browser_1.PreferenceService),
        __metadata("design:type", Object)
    ], AbstractResourcePreferenceProvider.prototype, "preferenceService", void 0);
    __decorate([
        inversify_1.inject(message_service_1.MessageService),
        __metadata("design:type", message_service_1.MessageService)
    ], AbstractResourcePreferenceProvider.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceSchemaProvider),
        __metadata("design:type", browser_1.PreferenceSchemaProvider)
    ], AbstractResourcePreferenceProvider.prototype, "schemaProvider", void 0);
    __decorate([
        inversify_1.inject(preference_configurations_1.PreferenceConfigurations),
        __metadata("design:type", preference_configurations_1.PreferenceConfigurations)
    ], AbstractResourcePreferenceProvider.prototype, "configurations", void 0);
    __decorate([
        inversify_1.inject(monaco_text_model_service_1.MonacoTextModelService),
        __metadata("design:type", monaco_text_model_service_1.MonacoTextModelService)
    ], AbstractResourcePreferenceProvider.prototype, "textModelService", void 0);
    __decorate([
        inversify_1.inject(monaco_workspace_1.MonacoWorkspace),
        __metadata("design:type", monaco_workspace_1.MonacoWorkspace)
    ], AbstractResourcePreferenceProvider.prototype, "workspace", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], AbstractResourcePreferenceProvider.prototype, "init", null);
    AbstractResourcePreferenceProvider = __decorate([
        inversify_1.injectable()
    ], AbstractResourcePreferenceProvider);
    return AbstractResourcePreferenceProvider;
}(browser_1.PreferenceProvider));
exports.AbstractResourcePreferenceProvider = AbstractResourcePreferenceProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/abstract-resource-preference-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/folder-preference-provider.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/folder-preference-provider.js ***!
  \**********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Ericsson and others.
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
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var abstract_resource_preference_provider_1 = __webpack_require__(/*! ./abstract-resource-preference-provider */ "../../packages/preferences/lib/browser/abstract-resource-preference-provider.js");
var workspace_service_1 = __webpack_require__(/*! @theia/workspace/lib/browser/workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js");
exports.FolderPreferenceProviderFactory = Symbol('FolderPreferenceProviderFactory');
exports.FolderPreferenceProviderOptions = Symbol('FolderPreferenceProviderOptions');
var FolderPreferenceProvider = /** @class */ (function (_super) {
    __extends(FolderPreferenceProvider, _super);
    function FolderPreferenceProvider() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    Object.defineProperty(FolderPreferenceProvider.prototype, "folderUri", {
        get: function () {
            if (!this._folderUri) {
                this._folderUri = new uri_1.default(this.options.folder.uri);
            }
            return this._folderUri;
        },
        enumerable: true,
        configurable: true
    });
    FolderPreferenceProvider.prototype.getUri = function () {
        return this.options.configUri;
    };
    FolderPreferenceProvider.prototype.getScope = function () {
        if (!this.workspaceService.isMultiRootWorkspaceOpened) {
            // when FolderPreferenceProvider is used as a delegate of WorkspacePreferenceProvider in a one-folder workspace
            return browser_1.PreferenceScope.Workspace;
        }
        return browser_1.PreferenceScope.Folder;
    };
    FolderPreferenceProvider.prototype.getDomain = function () {
        return [this.folderUri.toString()];
    };
    __decorate([
        inversify_1.inject(workspace_service_1.WorkspaceService),
        __metadata("design:type", workspace_service_1.WorkspaceService)
    ], FolderPreferenceProvider.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(exports.FolderPreferenceProviderOptions),
        __metadata("design:type", Object)
    ], FolderPreferenceProvider.prototype, "options", void 0);
    FolderPreferenceProvider = __decorate([
        inversify_1.injectable()
    ], FolderPreferenceProvider);
    return FolderPreferenceProvider;
}(abstract_resource_preference_provider_1.AbstractResourcePreferenceProvider));
exports.FolderPreferenceProvider = FolderPreferenceProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/folder-preference-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/folders-preferences-provider.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/folders-preferences-provider.js ***!
  \************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Ericsson and others.
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var preference_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-provider */ "../../packages/core/lib/browser/preferences/preference-provider.js");
var workspace_service_1 = __webpack_require__(/*! @theia/workspace/lib/browser/workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js");
var preference_configurations_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-configurations */ "../../packages/core/lib/browser/preferences/preference-configurations.js");
var folder_preference_provider_1 = __webpack_require__(/*! ./folder-preference-provider */ "../../packages/preferences/lib/browser/folder-preference-provider.js");
var FoldersPreferencesProvider = /** @class */ (function (_super) {
    __extends(FoldersPreferencesProvider, _super);
    function FoldersPreferencesProvider() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.providers = new Map();
        return _this;
    }
    FoldersPreferencesProvider.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var readyPromises, _a, _b, provider;
            var e_1, _c;
            var _this = this;
            return __generator(this, function (_d) {
                switch (_d.label) {
                    case 0: return [4 /*yield*/, this.workspaceService.roots];
                    case 1:
                        _d.sent();
                        this.updateProviders();
                        this.workspaceService.onWorkspaceChanged(function () { return _this.updateProviders(); });
                        readyPromises = [];
                        try {
                            for (_a = __values(this.providers.values()), _b = _a.next(); !_b.done; _b = _a.next()) {
                                provider = _b.value;
                                readyPromises.push(provider.ready.catch(function (e) { return console.error(e); }));
                            }
                        }
                        catch (e_1_1) { e_1 = { error: e_1_1 }; }
                        finally {
                            try {
                                if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                            }
                            finally { if (e_1) throw e_1.error; }
                        }
                        Promise.all(readyPromises).then(function () { return _this._ready.resolve(); });
                        return [2 /*return*/];
                }
            });
        });
    };
    FoldersPreferencesProvider.prototype.updateProviders = function () {
        var e_2, _a, e_3, _b, e_4, _c, e_5, _d;
        var roots = this.workspaceService.tryGetRoots();
        var toDelete = new Set(this.providers.keys());
        try {
            for (var roots_1 = __values(roots), roots_1_1 = roots_1.next(); !roots_1_1.done; roots_1_1 = roots_1.next()) {
                var folder = roots_1_1.value;
                try {
                    for (var _e = (e_3 = void 0, __values(this.configurations.getPaths())), _f = _e.next(); !_f.done; _f = _e.next()) {
                        var configPath = _f.value;
                        try {
                            for (var _g = (e_4 = void 0, __values(__spread(this.configurations.getSectionNames(), [this.configurations.getConfigName()]))), _h = _g.next(); !_h.done; _h = _g.next()) {
                                var configName = _h.value;
                                var configUri = this.configurations.createUri(new uri_1.default(folder.uri), configPath, configName);
                                var key = configUri.toString();
                                toDelete.delete(key);
                                if (!this.providers.has(key)) {
                                    var provider = this.createProvider({ folder: folder, configUri: configUri });
                                    this.providers.set(key, provider);
                                }
                            }
                        }
                        catch (e_4_1) { e_4 = { error: e_4_1 }; }
                        finally {
                            try {
                                if (_h && !_h.done && (_c = _g.return)) _c.call(_g);
                            }
                            finally { if (e_4) throw e_4.error; }
                        }
                    }
                }
                catch (e_3_1) { e_3 = { error: e_3_1 }; }
                finally {
                    try {
                        if (_f && !_f.done && (_b = _e.return)) _b.call(_e);
                    }
                    finally { if (e_3) throw e_3.error; }
                }
            }
        }
        catch (e_2_1) { e_2 = { error: e_2_1 }; }
        finally {
            try {
                if (roots_1_1 && !roots_1_1.done && (_a = roots_1.return)) _a.call(roots_1);
            }
            finally { if (e_2) throw e_2.error; }
        }
        try {
            for (var toDelete_1 = __values(toDelete), toDelete_1_1 = toDelete_1.next(); !toDelete_1_1.done; toDelete_1_1 = toDelete_1.next()) {
                var key = toDelete_1_1.value;
                var provider = this.providers.get(key);
                if (provider) {
                    this.providers.delete(key);
                    provider.dispose();
                }
            }
        }
        catch (e_5_1) { e_5 = { error: e_5_1 }; }
        finally {
            try {
                if (toDelete_1_1 && !toDelete_1_1.done && (_d = toDelete_1.return)) _d.call(toDelete_1);
            }
            finally { if (e_5) throw e_5.error; }
        }
    };
    FoldersPreferencesProvider.prototype.getConfigUri = function (resourceUri) {
        var e_6, _a;
        try {
            for (var _b = __values(this.getFolderProviders(resourceUri)), _c = _b.next(); !_c.done; _c = _b.next()) {
                var provider = _c.value;
                var configUri = provider.getConfigUri(resourceUri);
                if (this.configurations.isConfigUri(configUri)) {
                    return configUri;
                }
            }
        }
        catch (e_6_1) { e_6 = { error: e_6_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_6) throw e_6.error; }
        }
        return undefined;
    };
    FoldersPreferencesProvider.prototype.getContainingConfigUri = function (resourceUri) {
        var e_7, _a;
        try {
            for (var _b = __values(this.getFolderProviders(resourceUri)), _c = _b.next(); !_c.done; _c = _b.next()) {
                var provider = _c.value;
                var configUri = provider.getConfigUri();
                if (this.configurations.isConfigUri(configUri) && provider.contains(resourceUri)) {
                    return configUri;
                }
            }
        }
        catch (e_7_1) { e_7 = { error: e_7_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_7) throw e_7.error; }
        }
        return undefined;
    };
    FoldersPreferencesProvider.prototype.getDomain = function () {
        return this.workspaceService.tryGetRoots().map(function (root) { return root.uri; });
    };
    FoldersPreferencesProvider.prototype.resolve = function (preferenceName, resourceUri) {
        var e_8, _a, e_9, _b;
        var result = {};
        var groups = this.groupProvidersByConfigName(resourceUri);
        try {
            for (var _c = __values(groups.values()), _d = _c.next(); !_d.done; _d = _c.next()) {
                var group = _d.value;
                try {
                    for (var group_1 = (e_9 = void 0, __values(group)), group_1_1 = group_1.next(); !group_1_1.done; group_1_1 = group_1.next()) {
                        var provider = group_1_1.value;
                        var _e = provider.resolve(preferenceName, resourceUri), value = _e.value, configUri = _e.configUri;
                        if (configUri && value !== undefined) {
                            result.configUri = configUri;
                            result.value = preference_provider_1.PreferenceProvider.merge(result.value, value);
                            break;
                        }
                    }
                }
                catch (e_9_1) { e_9 = { error: e_9_1 }; }
                finally {
                    try {
                        if (group_1_1 && !group_1_1.done && (_b = group_1.return)) _b.call(group_1);
                    }
                    finally { if (e_9) throw e_9.error; }
                }
            }
        }
        catch (e_8_1) { e_8 = { error: e_8_1 }; }
        finally {
            try {
                if (_d && !_d.done && (_a = _c.return)) _a.call(_c);
            }
            finally { if (e_8) throw e_8.error; }
        }
        return result;
    };
    FoldersPreferencesProvider.prototype.getPreferences = function (resourceUri) {
        var e_10, _a, e_11, _b;
        var result = {};
        var groups = this.groupProvidersByConfigName(resourceUri);
        try {
            for (var _c = __values(groups.values()), _d = _c.next(); !_d.done; _d = _c.next()) {
                var group = _d.value;
                try {
                    for (var group_2 = (e_11 = void 0, __values(group)), group_2_1 = group_2.next(); !group_2_1.done; group_2_1 = group_2.next()) {
                        var provider = group_2_1.value;
                        if (provider.getConfigUri(resourceUri)) {
                            var preferences = provider.getPreferences();
                            result = preference_provider_1.PreferenceProvider.merge(result, preferences);
                            break;
                        }
                    }
                }
                catch (e_11_1) { e_11 = { error: e_11_1 }; }
                finally {
                    try {
                        if (group_2_1 && !group_2_1.done && (_b = group_2.return)) _b.call(group_2);
                    }
                    finally { if (e_11) throw e_11.error; }
                }
            }
        }
        catch (e_10_1) { e_10 = { error: e_10_1 }; }
        finally {
            try {
                if (_d && !_d.done && (_a = _c.return)) _a.call(_c);
            }
            finally { if (e_10) throw e_10.error; }
        }
        return result;
    };
    FoldersPreferencesProvider.prototype.setPreference = function (preferenceName, value, resourceUri) {
        return __awaiter(this, void 0, void 0, function () {
            var sectionName, configName, providers, configPath, iterator, _loop_1, this_1, providers_1, providers_1_1, provider, next, provider;
            var e_12, _a;
            var _this = this;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        sectionName = preferenceName.split('.', 1)[0];
                        configName = this.configurations.isSectionName(sectionName) ? sectionName : this.configurations.getConfigName();
                        providers = this.getFolderProviders(resourceUri);
                        iterator = [];
                        _loop_1 = function (provider) {
                            if (configPath === undefined) {
                                var configUri = provider.getConfigUri(resourceUri);
                                if (configUri) {
                                    configPath = this_1.configurations.getPath(configUri);
                                }
                            }
                            if (this_1.configurations.getName(provider.getConfigUri()) === configName) {
                                iterator.push(function () {
                                    if (provider.getConfigUri(resourceUri)) {
                                        return provider;
                                    }
                                    iterator.push(function () {
                                        if (_this.configurations.getPath(provider.getConfigUri()) === configPath) {
                                            return provider;
                                        }
                                        iterator.push(function () { return provider; });
                                    });
                                });
                            }
                        };
                        this_1 = this;
                        try {
                            for (providers_1 = __values(providers), providers_1_1 = providers_1.next(); !providers_1_1.done; providers_1_1 = providers_1.next()) {
                                provider = providers_1_1.value;
                                _loop_1(provider);
                            }
                        }
                        catch (e_12_1) { e_12 = { error: e_12_1 }; }
                        finally {
                            try {
                                if (providers_1_1 && !providers_1_1.done && (_a = providers_1.return)) _a.call(providers_1);
                            }
                            finally { if (e_12) throw e_12.error; }
                        }
                        next = iterator.shift();
                        _b.label = 1;
                    case 1:
                        if (!next) return [3 /*break*/, 4];
                        provider = next();
                        if (!provider) return [3 /*break*/, 3];
                        return [4 /*yield*/, provider.setPreference(preferenceName, value, resourceUri)];
                    case 2:
                        if (_b.sent()) {
                            return [2 /*return*/, true];
                        }
                        _b.label = 3;
                    case 3:
                        next = iterator.shift();
                        return [3 /*break*/, 1];
                    case 4: return [2 /*return*/, false];
                }
            });
        });
    };
    FoldersPreferencesProvider.prototype.groupProvidersByConfigName = function (resourceUri) {
        var e_13, _a, e_14, _b;
        var groups = new Map();
        var providers = this.getFolderProviders(resourceUri);
        try {
            for (var _c = __values(__spread([this.configurations.getConfigName()], this.configurations.getSectionNames())), _d = _c.next(); !_d.done; _d = _c.next()) {
                var configName = _d.value;
                var group = [];
                try {
                    for (var providers_2 = (e_14 = void 0, __values(providers)), providers_2_1 = providers_2.next(); !providers_2_1.done; providers_2_1 = providers_2.next()) {
                        var provider = providers_2_1.value;
                        if (this.configurations.getName(provider.getConfigUri()) === configName) {
                            group.push(provider);
                        }
                    }
                }
                catch (e_14_1) { e_14 = { error: e_14_1 }; }
                finally {
                    try {
                        if (providers_2_1 && !providers_2_1.done && (_b = providers_2.return)) _b.call(providers_2);
                    }
                    finally { if (e_14) throw e_14.error; }
                }
                groups.set(configName, group);
            }
        }
        catch (e_13_1) { e_13 = { error: e_13_1 }; }
        finally {
            try {
                if (_d && !_d.done && (_a = _c.return)) _a.call(_c);
            }
            finally { if (e_13) throw e_13.error; }
        }
        return groups;
    };
    FoldersPreferencesProvider.prototype.getFolderProviders = function (resourceUri) {
        var e_15, _a;
        if (!resourceUri) {
            return [];
        }
        var resourcePath = new uri_1.default(resourceUri).path;
        var folder = { relativity: Number.MAX_SAFE_INTEGER };
        var providers = new Map();
        try {
            for (var _b = __values(this.providers.values()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var provider = _c.value;
                var uri = provider.folderUri.toString();
                var folderProviders = (providers.get(uri) || []);
                folderProviders.push(provider);
                providers.set(uri, folderProviders);
                var relativity = provider.folderUri.path.relativity(resourcePath);
                if (relativity >= 0 && folder.relativity > relativity) {
                    folder = { relativity: relativity, uri: uri };
                }
            }
        }
        catch (e_15_1) { e_15 = { error: e_15_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_15) throw e_15.error; }
        }
        return folder.uri && providers.get(folder.uri) || [];
    };
    FoldersPreferencesProvider.prototype.createProvider = function (options) {
        var _this = this;
        var provider = this.folderPreferenceProviderFactory(options);
        this.toDispose.push(provider);
        this.toDispose.push(provider.onDidPreferencesChanged(function (change) { return _this.onDidPreferencesChangedEmitter.fire(change); }));
        return provider;
    };
    __decorate([
        inversify_1.inject(workspace_service_1.WorkspaceService),
        __metadata("design:type", workspace_service_1.WorkspaceService)
    ], FoldersPreferencesProvider.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(folder_preference_provider_1.FolderPreferenceProviderFactory),
        __metadata("design:type", Function)
    ], FoldersPreferencesProvider.prototype, "folderPreferenceProviderFactory", void 0);
    __decorate([
        inversify_1.inject(preference_configurations_1.PreferenceConfigurations),
        __metadata("design:type", preference_configurations_1.PreferenceConfigurations)
    ], FoldersPreferencesProvider.prototype, "configurations", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], FoldersPreferencesProvider.prototype, "init", null);
    FoldersPreferencesProvider = __decorate([
        inversify_1.injectable()
    ], FoldersPreferencesProvider);
    return FoldersPreferencesProvider;
}(preference_provider_1.PreferenceProvider));
exports.FoldersPreferencesProvider = FoldersPreferencesProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/folders-preferences-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preference-bindings.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preference-bindings.js ***!
  \***************************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var preferences_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences */ "../../packages/core/lib/browser/preferences/index.js");
var preference_configurations_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-configurations */ "../../packages/core/lib/browser/preferences/preference-configurations.js");
var user_preference_provider_1 = __webpack_require__(/*! ./user-preference-provider */ "../../packages/preferences/lib/browser/user-preference-provider.js");
var workspace_preference_provider_1 = __webpack_require__(/*! ./workspace-preference-provider */ "../../packages/preferences/lib/browser/workspace-preference-provider.js");
var workspace_file_preference_provider_1 = __webpack_require__(/*! ./workspace-file-preference-provider */ "../../packages/preferences/lib/browser/workspace-file-preference-provider.js");
var folders_preferences_provider_1 = __webpack_require__(/*! ./folders-preferences-provider */ "../../packages/preferences/lib/browser/folders-preferences-provider.js");
var folder_preference_provider_1 = __webpack_require__(/*! ./folder-preference-provider */ "../../packages/preferences/lib/browser/folder-preference-provider.js");
function bindWorkspaceFilePreferenceProvider(bind) {
    bind(workspace_file_preference_provider_1.WorkspaceFilePreferenceProviderFactory).toFactory(function (ctx) { return function (options) {
        var child = new inversify_1.Container({ defaultScope: 'Singleton' });
        child.parent = ctx.container;
        child.bind(workspace_file_preference_provider_1.WorkspaceFilePreferenceProvider).toSelf();
        child.bind(workspace_file_preference_provider_1.WorkspaceFilePreferenceProviderOptions).toConstantValue(options);
        return child.get(workspace_file_preference_provider_1.WorkspaceFilePreferenceProvider);
    }; });
}
exports.bindWorkspaceFilePreferenceProvider = bindWorkspaceFilePreferenceProvider;
function bindFolderPreferenceProvider(bind) {
    bind(folder_preference_provider_1.FolderPreferenceProviderFactory).toFactory(function (ctx) {
        return function (options) {
            var child = new inversify_1.Container({ defaultScope: 'Singleton' });
            child.parent = ctx.container;
            child.bind(folder_preference_provider_1.FolderPreferenceProviderOptions).toConstantValue(options);
            var configurations = ctx.container.get(preference_configurations_1.PreferenceConfigurations);
            if (configurations.isConfigUri(options.configUri)) {
                child.bind(folder_preference_provider_1.FolderPreferenceProvider).toSelf();
                return child.get(folder_preference_provider_1.FolderPreferenceProvider);
            }
            var sectionName = configurations.getName(options.configUri);
            return child.getNamed(folder_preference_provider_1.FolderPreferenceProvider, sectionName);
        };
    });
}
exports.bindFolderPreferenceProvider = bindFolderPreferenceProvider;
function bindPreferenceProviders(bind, unbind) {
    unbind(preferences_1.PreferenceProvider);
    bind(preferences_1.PreferenceProvider).to(user_preference_provider_1.UserPreferenceProvider).inSingletonScope().whenTargetNamed(preferences_1.PreferenceScope.User);
    bind(preferences_1.PreferenceProvider).to(workspace_preference_provider_1.WorkspacePreferenceProvider).inSingletonScope().whenTargetNamed(preferences_1.PreferenceScope.Workspace);
    bind(preferences_1.PreferenceProvider).to(folders_preferences_provider_1.FoldersPreferencesProvider).inSingletonScope().whenTargetNamed(preferences_1.PreferenceScope.Folder);
    bindFolderPreferenceProvider(bind);
    bindWorkspaceFilePreferenceProvider(bind);
}
exports.bindPreferenceProviders = bindPreferenceProviders;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preference-bindings'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preference-editor-widget.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preference-editor-widget.js ***!
  \********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Ericsson and others.
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
var widgets_1 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
var properties_1 = __webpack_require__(/*! @phosphor/properties */ "../../node_modules/@phosphor/properties/lib/index.js");
var widgets_2 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
var commands_1 = __webpack_require__(/*! @phosphor/commands */ "../../node_modules/@phosphor/commands/lib/index.js");
var virtualdom_1 = __webpack_require__(/*! @phosphor/virtualdom */ "../../node_modules/@phosphor/virtualdom/lib/index.js");
var browser_1 = __webpack_require__(/*! @theia/editor/lib/browser */ "../../packages/editor/lib/browser/index.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var common_1 = __webpack_require__(/*! @theia/filesystem/lib/common */ "../../packages/filesystem/lib/common/index.js");
var PreferencesEditorWidgetTitle = /** @class */ (function (_super) {
    __extends(PreferencesEditorWidgetTitle, _super);
    function PreferencesEditorWidgetTitle() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return PreferencesEditorWidgetTitle;
}(widgets_1.Title));
exports.PreferencesEditorWidgetTitle = PreferencesEditorWidgetTitle;
var PreferencesEditorWidget = /** @class */ (function (_super) {
    __extends(PreferencesEditorWidget, _super);
    function PreferencesEditorWidget() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    Object.defineProperty(PreferencesEditorWidget.prototype, "title", {
        get: function () {
            return new properties_1.AttachedProperty({
                name: 'title',
                create: function (owner) { return new PreferencesEditorWidgetTitle({ owner: owner }); },
            }).get(this);
        },
        enumerable: true,
        configurable: true
    });
    return PreferencesEditorWidget;
}(browser_1.EditorWidget));
exports.PreferencesEditorWidget = PreferencesEditorWidget;
// TODO: put into DI context
var PreferenceEditorTabHeaderRenderer = /** @class */ (function (_super) {
    __extends(PreferenceEditorTabHeaderRenderer, _super);
    function PreferenceEditorTabHeaderRenderer(workspaceService, fileSystem, foldersPreferenceProvider) {
        var _this = _super.call(this) || this;
        _this.workspaceService = workspaceService;
        _this.fileSystem = fileSystem;
        _this.foldersPreferenceProvider = foldersPreferenceProvider;
        return _this;
    }
    PreferenceEditorTabHeaderRenderer.prototype.renderTab = function (data) {
        var title = data.title;
        title.closable = false;
        var key = this.createTabKey(data);
        var style = this.createTabStyle(data);
        var className = this.createTabClass(data);
        return virtualdom_1.h.li({ key: key, className: className, title: title.caption, style: style }, this.renderIcon(data), this.renderLabel(data), this.renderCloseIcon(data));
    };
    PreferenceEditorTabHeaderRenderer.prototype.renderLabel = function (data) {
        var _this = this;
        var clickableTitle = data.title.owner.title;
        if (clickableTitle.clickableText) {
            return virtualdom_1.h.div(virtualdom_1.h.span({ className: 'p-TabBar-tabLabel' }, data.title.label), virtualdom_1.h.span({
                className: 'p-TabBar-tabLabel p-TabBar-tab-secondary-label',
                title: clickableTitle.clickableTextTooltip,
                onclick: function (event) {
                    var editorUri = data.title.owner.editor.uri;
                    _this.refreshContextMenu(editorUri.parent.parent.toString(), clickableTitle.clickableTextCallback || (function () { }))
                        .then(function (menu) { return menu.open(event.x, event.y); });
                }
            }, clickableTitle.clickableText));
        }
        return _super.prototype.renderLabel.call(this, data);
    };
    PreferenceEditorTabHeaderRenderer.prototype.refreshContextMenu = function (activeMenuId, menuItemAction) {
        return __awaiter(this, void 0, void 0, function () {
            var commands, menu, roots, _loop_1, this_1, roots_1, roots_1_1, root, e_1_1;
            var e_1, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        commands = new commands_1.CommandRegistry();
                        menu = new widgets_2.Menu({ commands: commands });
                        roots = this.workspaceService.tryGetRoots().map(function (r) { return r.uri; });
                        _loop_1 = function (root) {
                            var commandId, rootUri, isActive_1;
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0: return [4 /*yield*/, this_1.canAccessSettings(root)];
                                    case 1:
                                        if (_a.sent()) {
                                            commandId = "switch_folder_pref_editor_to_" + root;
                                            if (!commands.hasCommand(commandId)) {
                                                rootUri = new uri_1.default(root);
                                                isActive_1 = rootUri.toString() === activeMenuId;
                                                commands.addCommand(commandId, {
                                                    label: rootUri.displayName,
                                                    iconClass: isActive_1 ? 'fa fa-check' : '',
                                                    execute: function () {
                                                        if (!isActive_1) {
                                                            menuItemAction(root);
                                                        }
                                                    }
                                                });
                                            }
                                            menu.addItem({
                                                type: 'command',
                                                command: commandId
                                            });
                                        }
                                        return [2 /*return*/];
                                }
                            });
                        };
                        this_1 = this;
                        _b.label = 1;
                    case 1:
                        _b.trys.push([1, 6, 7, 8]);
                        roots_1 = __values(roots), roots_1_1 = roots_1.next();
                        _b.label = 2;
                    case 2:
                        if (!!roots_1_1.done) return [3 /*break*/, 5];
                        root = roots_1_1.value;
                        return [5 /*yield**/, _loop_1(root)];
                    case 3:
                        _b.sent();
                        _b.label = 4;
                    case 4:
                        roots_1_1 = roots_1.next();
                        return [3 /*break*/, 2];
                    case 5: return [3 /*break*/, 8];
                    case 6:
                        e_1_1 = _b.sent();
                        e_1 = { error: e_1_1 };
                        return [3 /*break*/, 8];
                    case 7:
                        try {
                            if (roots_1_1 && !roots_1_1.done && (_a = roots_1.return)) _a.call(roots_1);
                        }
                        finally { if (e_1) throw e_1.error; }
                        return [7 /*endfinally*/];
                    case 8: return [2 /*return*/, menu];
                }
            });
        });
    };
    PreferenceEditorTabHeaderRenderer.prototype.canAccessSettings = function (folderUriStr) {
        return __awaiter(this, void 0, void 0, function () {
            var settingsUri;
            return __generator(this, function (_a) {
                settingsUri = this.foldersPreferenceProvider.getConfigUri(folderUriStr);
                if (settingsUri) {
                    return [2 /*return*/, this.fileSystem.access(settingsUri.toString(), common_1.FileAccess.Constants.R_OK)];
                }
                return [2 /*return*/, this.fileSystem.access(folderUriStr, common_1.FileAccess.Constants.W_OK)];
            });
        });
    };
    return PreferenceEditorTabHeaderRenderer;
}(widgets_2.TabBar.Renderer));
exports.PreferenceEditorTabHeaderRenderer = PreferenceEditorTabHeaderRenderer;
// TODO put into DI context
var PreferenceEditorContainerTabBarRenderer = /** @class */ (function (_super) {
    __extends(PreferenceEditorContainerTabBarRenderer, _super);
    function PreferenceEditorContainerTabBarRenderer(workspaceService, fileSystem, foldersPreferenceProvider) {
        var _this = _super.call(this) || this;
        _this.workspaceService = workspaceService;
        _this.fileSystem = fileSystem;
        _this.foldersPreferenceProvider = foldersPreferenceProvider;
        return _this;
    }
    PreferenceEditorContainerTabBarRenderer.prototype.createTabBar = function () {
        var bar = new widgets_2.TabBar({ renderer: new PreferenceEditorTabHeaderRenderer(this.workspaceService, this.fileSystem, this.foldersPreferenceProvider) });
        bar.addClass('p-DockPanel-tabBar');
        return bar;
    };
    return PreferenceEditorContainerTabBarRenderer;
}(widgets_2.DockPanel.Renderer));
exports.PreferenceEditorContainerTabBarRenderer = PreferenceEditorContainerTabBarRenderer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preference-editor-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preference-frontend-module.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preference-frontend-module.js ***!
  \**********************************************************************************************************/
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
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/preferences/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var preferences_contribution_1 = __webpack_require__(/*! ./preferences-contribution */ "../../packages/preferences/lib/browser/preferences-contribution.js");
var preference_tree_container_1 = __webpack_require__(/*! ./preference-tree-container */ "../../packages/preferences/lib/browser/preference-tree-container.js");
var preferences_menu_factory_1 = __webpack_require__(/*! ./preferences-menu-factory */ "../../packages/preferences/lib/browser/preferences-menu-factory.js");
var preferences_frontend_application_contribution_1 = __webpack_require__(/*! ./preferences-frontend-application-contribution */ "../../packages/preferences/lib/browser/preferences-frontend-application-contribution.js");
var preferences_tree_widget_1 = __webpack_require__(/*! ./preferences-tree-widget */ "../../packages/preferences/lib/browser/preferences-tree-widget.js");
var preference_bindings_1 = __webpack_require__(/*! ./preference-bindings */ "../../packages/preferences/lib/browser/preference-bindings.js");
__webpack_require__(/*! ./preferences-monaco-contribution */ "../../packages/preferences/lib/browser/preferences-monaco-contribution.js");
exports.PreferencesWidgetFactory = Symbol('PreferencesWidgetFactory');
function bindPreferences(bind, unbind) {
    preference_bindings_1.bindPreferenceProviders(bind, unbind);
    browser_1.bindViewContribution(bind, preferences_contribution_1.PreferencesContribution);
    bind(preferences_tree_widget_1.PreferencesContainer).toSelf();
    bind(browser_1.WidgetFactory).toDynamicValue(function (_a) {
        var container = _a.container;
        return ({
            id: preferences_tree_widget_1.PreferencesContainer.ID,
            createWidget: function () { return container.get(preferences_tree_widget_1.PreferencesContainer); }
        });
    });
    bind(exports.PreferencesWidgetFactory).toDynamicValue(function (_a) {
        var container = _a.container;
        return ({
            id: preferences_tree_widget_1.PreferencesTreeWidget.ID,
            createWidget: function () { return preference_tree_container_1.createPreferencesTreeWidget(container); }
        });
    }).inSingletonScope();
    bind(browser_1.WidgetFactory).toService(exports.PreferencesWidgetFactory);
    bind(preferences_tree_widget_1.PreferencesEditorsContainer).toSelf();
    bind(browser_1.WidgetFactory).toDynamicValue(function (_a) {
        var container = _a.container;
        return ({
            id: preferences_tree_widget_1.PreferencesEditorsContainer.ID,
            createWidget: function () { return container.get(preferences_tree_widget_1.PreferencesEditorsContainer); }
        });
    });
    bind(preferences_menu_factory_1.PreferencesMenuFactory).toSelf();
    bind(browser_1.FrontendApplicationContribution).to(preferences_frontend_application_contribution_1.PreferencesFrontendApplicationContribution).inSingletonScope();
}
exports.bindPreferences = bindPreferences;
exports.default = new inversify_1.ContainerModule(function (bind, unbind, isBound, rebind) {
    bindPreferences(bind, unbind);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preference-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preference-tree-container.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preference-tree-container.js ***!
  \*********************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var preferences_decorator_1 = __webpack_require__(/*! ./preferences-decorator */ "../../packages/preferences/lib/browser/preferences-decorator.js");
var preferences_decorator_service_1 = __webpack_require__(/*! ./preferences-decorator-service */ "../../packages/preferences/lib/browser/preferences-decorator-service.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var preferences_tree_widget_1 = __webpack_require__(/*! ./preferences-tree-widget */ "../../packages/preferences/lib/browser/preferences-tree-widget.js");
function createPreferencesTreeWidget(parent) {
    var child = browser_1.createTreeContainer(parent);
    child.bind(preferences_tree_widget_1.PreferencesTreeWidget).toSelf();
    child.rebind(browser_1.TreeProps).toConstantValue(__assign({}, browser_1.defaultTreeProps, { search: true }));
    child.rebind(browser_1.TreeWidget).toService(preferences_tree_widget_1.PreferencesTreeWidget);
    bindPreferencesDecorator(child);
    return child.get(preferences_tree_widget_1.PreferencesTreeWidget);
}
exports.createPreferencesTreeWidget = createPreferencesTreeWidget;
function bindPreferencesDecorator(parent) {
    parent.bind(preferences_decorator_1.PreferencesDecorator).toSelf().inSingletonScope();
    parent.bind(preferences_decorator_service_1.PreferencesDecoratorService).toSelf().inSingletonScope();
    parent.rebind(browser_1.TreeDecoratorService).toService(preferences_decorator_service_1.PreferencesDecoratorService);
}

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preference-tree-container'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-contribution.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-contribution.js ***!
  \********************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var workspace_preference_provider_1 = __webpack_require__(/*! ./workspace-preference-provider */ "../../packages/preferences/lib/browser/workspace-preference-provider.js");
var common_1 = __webpack_require__(/*! @theia/filesystem/lib/common */ "../../packages/filesystem/lib/common/index.js");
var browser_2 = __webpack_require__(/*! @theia/userstorage/lib/browser */ "../../packages/userstorage/lib/browser/index.js");
var preferences_tree_widget_1 = __webpack_require__(/*! ./preferences-tree-widget */ "../../packages/preferences/lib/browser/preferences-tree-widget.js");
var browser_3 = __webpack_require__(/*! @theia/editor/lib/browser */ "../../packages/editor/lib/browser/index.js");
var browser_4 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var os_1 = __webpack_require__(/*! @theia/core/lib/common/os */ "../../packages/core/lib/common/os.js");
var PreferencesContribution = /** @class */ (function (_super) {
    __extends(PreferencesContribution, _super);
    function PreferencesContribution() {
        return _super.call(this, {
            widgetId: preferences_tree_widget_1.PreferencesContainer.ID,
            widgetName: 'Preferences',
            defaultWidgetOptions: { area: 'main' }
        }) || this;
    }
    PreferencesContribution.prototype.registerCommands = function (commands) {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                commands.registerCommand(browser_1.CommonCommands.OPEN_PREFERENCES, {
                    isEnabled: function () { return true; },
                    execute: function (preferenceScope) {
                        if (preferenceScope === void 0) { preferenceScope = browser_1.PreferenceScope.User; }
                        return _this.openPreferences(preferenceScope);
                    }
                });
                return [2 /*return*/];
            });
        });
    };
    PreferencesContribution.prototype.registerMenus = function (menus) {
        menus.registerMenuAction(browser_1.CommonMenus.FILE_SETTINGS_SUBMENU_OPEN, {
            commandId: browser_1.CommonCommands.OPEN_PREFERENCES.id,
            order: 'a10'
        });
    };
    PreferencesContribution.prototype.registerKeybindings = function (keybindings) {
        if (os_1.isOSX && !browser_4.isFirefox) {
            keybindings.registerKeybinding({
                command: browser_1.CommonCommands.OPEN_PREFERENCES.id,
                keybinding: 'cmd+,'
            });
        }
        keybindings.registerKeybinding({
            command: browser_1.CommonCommands.OPEN_PREFERENCES.id,
            keybinding: 'ctrl+,'
        });
    };
    PreferencesContribution.prototype.openPreferences = function (preferenceScope) {
        return __awaiter(this, void 0, void 0, function () {
            var wsUri, _a, widget;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        wsUri = this.workspacePreferenceProvider.getConfigUri();
                        _a = wsUri;
                        if (!_a) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.filesystem.exists(wsUri.toString())];
                    case 1:
                        _a = !(_b.sent());
                        _b.label = 2;
                    case 2:
                        if (!_a) return [3 /*break*/, 4];
                        return [4 /*yield*/, this.filesystem.createFile(wsUri.toString())];
                    case 3:
                        _b.sent();
                        _b.label = 4;
                    case 4: return [4 /*yield*/, this.widget];
                    case 5:
                        widget = _b.sent();
                        widget.preferenceScope = preferenceScope;
                        _super.prototype.openView.call(this, { activate: true });
                        widget.activatePreferenceEditor(preferenceScope);
                        return [2 /*return*/];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(browser_2.UserStorageService),
        __metadata("design:type", Object)
    ], PreferencesContribution.prototype, "userStorageService", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceProvider), inversify_1.named(browser_1.PreferenceScope.Workspace),
        __metadata("design:type", workspace_preference_provider_1.WorkspacePreferenceProvider)
    ], PreferencesContribution.prototype, "workspacePreferenceProvider", void 0);
    __decorate([
        inversify_1.inject(common_1.FileSystem),
        __metadata("design:type", Object)
    ], PreferencesContribution.prototype, "filesystem", void 0);
    __decorate([
        inversify_1.inject(browser_3.EditorManager),
        __metadata("design:type", browser_3.EditorManager)
    ], PreferencesContribution.prototype, "editorManager", void 0);
    PreferencesContribution = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], PreferencesContribution);
    return PreferencesContribution;
}(browser_1.AbstractViewContribution));
exports.PreferencesContribution = PreferencesContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-decorator-service.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-decorator-service.js ***!
  \*************************************************************************************************************/
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
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
};
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var tree_decorator_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-decorator */ "../../packages/core/lib/browser/tree/tree-decorator.js");
var preferences_decorator_1 = __webpack_require__(/*! ./preferences-decorator */ "../../packages/preferences/lib/browser/preferences-decorator.js");
var PreferencesDecoratorService = /** @class */ (function (_super) {
    __extends(PreferencesDecoratorService, _super);
    function PreferencesDecoratorService(preferencesTreeDecorator) {
        var _this = _super.call(this, [preferencesTreeDecorator]) || this;
        _this.preferencesTreeDecorator = preferencesTreeDecorator;
        return _this;
    }
    PreferencesDecoratorService = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(preferences_decorator_1.PreferencesDecorator)),
        __metadata("design:paramtypes", [preferences_decorator_1.PreferencesDecorator])
    ], PreferencesDecoratorService);
    return PreferencesDecoratorService;
}(tree_decorator_1.AbstractTreeDecoratorService));
exports.PreferencesDecoratorService = PreferencesDecoratorService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-decorator-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-decorator.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-decorator.js ***!
  \*****************************************************************************************************/
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
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var strings_1 = __webpack_require__(/*! @theia/core/lib/common/strings */ "../../packages/core/lib/common/strings.js");
var PreferencesDecorator = /** @class */ (function () {
    function PreferencesDecorator(preferencesService) {
        var _this = this;
        this.preferencesService = preferencesService;
        this.id = 'theia-preferences-decorator';
        this.emitter = new core_1.Emitter();
        this.preferencesDecorations = new Map();
        this.preferencesService.onPreferenceChanged(function () {
            _this.fireDidChangeDecorations(_this.preferences);
        });
    }
    Object.defineProperty(PreferencesDecorator.prototype, "onDidChangeDecorations", {
        get: function () {
            return this.emitter.event;
        },
        enumerable: true,
        configurable: true
    });
    PreferencesDecorator.prototype.fireDidChangeDecorations = function (preferences) {
        var _this = this;
        if (!this.preferences) {
            this.preferences = preferences;
        }
        this.preferencesDecorations = new Map(preferences.map(function (m) {
            var preferenceName = Object.keys(m)[0];
            var preferenceValue = m[preferenceName];
            var storedValue = _this.preferencesService.get(preferenceName, undefined, _this.activeFolderUri);
            var description = _this.getDescription(preferenceValue);
            return [preferenceName, {
                    tooltip: _this.buildTooltip(preferenceValue),
                    captionSuffixes: [
                        {
                            data: ": " + _this.getPreferenceDisplayValue(storedValue, preferenceValue.defaultValue)
                        },
                        {
                            data: ' ' + description,
                            fontData: { color: 'var(--theia-descriptionForeground)' }
                        }
                    ]
                }];
        }));
        this.emitter.fire(function () { return _this.preferencesDecorations; });
    };
    PreferencesDecorator.prototype.decorations = function (tree) {
        return this.preferencesDecorations;
    };
    PreferencesDecorator.prototype.setActiveFolder = function (folder) {
        this.activeFolderUri = folder;
        this.fireDidChangeDecorations(this.preferences);
    };
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    PreferencesDecorator.prototype.getPreferenceDisplayValue = function (storedValue, defaultValue) {
        if (storedValue !== undefined) {
            if (typeof storedValue === 'string') {
                return strings_1.escapeInvisibleChars(storedValue);
            }
            return storedValue;
        }
        return defaultValue;
    };
    PreferencesDecorator.prototype.buildTooltip = function (data) {
        var tooltips = '';
        if (data.description) {
            tooltips = data.description;
        }
        if (data.defaultValue) {
            tooltips += "\nDefault: " + JSON.stringify(data.defaultValue);
        }
        else if (data.default !== undefined) {
            tooltips += "\nDefault: " + JSON.stringify(data.default);
        }
        if (data.minimum) {
            tooltips += "\nMin: " + data.minimum;
        }
        if (data.enum) {
            tooltips += "\nAccepted Values: " + data.enum.join(', ');
        }
        return tooltips;
    };
    /**
     * Get the description for the preference for display purposes.
     * @param value {PreferenceDataProperty} the preference data property.
     * @returns the description if available.
     */
    PreferencesDecorator.prototype.getDescription = function (value) {
        /**
         * Format the string for consistency and display purposes.
         * Formatting includes:
         * - capitalizing the string.
         * - ensuring it ends in punctuation (`.`).
         * @param str {string} the string to format.
         * @returns the formatted string.
         */
        function format(str) {
            if (str.endsWith('.')) {
                return str.charAt(0).toUpperCase() + str.slice(1);
            }
            return str.charAt(0).toUpperCase() + str.slice(1) + ".";
        }
        if (value.description) {
            return format(value.description);
        }
        else if (value.markdownDescription) {
            return format(value.markdownDescription);
        }
        return '';
    };
    PreferencesDecorator = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_1.PreferenceService)),
        __metadata("design:paramtypes", [Object])
    ], PreferencesDecorator);
    return PreferencesDecorator;
}());
exports.PreferencesDecorator = PreferencesDecorator;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-decorator'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-frontend-application-contribution.js":
/*!*****************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-frontend-application-contribution.js ***!
  \*****************************************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var json_schema_store_1 = __webpack_require__(/*! @theia/core/lib/browser/json-schema-store */ "../../packages/core/lib/browser/json-schema-store.js");
var user_preference_provider_1 = __webpack_require__(/*! ./user-preference-provider */ "../../packages/preferences/lib/browser/user-preference-provider.js");
var PreferencesFrontendApplicationContribution = /** @class */ (function () {
    function PreferencesFrontendApplicationContribution() {
    }
    PreferencesFrontendApplicationContribution.prototype.onStart = function () {
        var _this = this;
        var serializeSchema = function () { return JSON.stringify(_this.schemaProvider.getCombinedSchema()); };
        var uri = new uri_1.default('vscode://schemas/settings/user');
        this.inmemoryResources.add(uri, serializeSchema());
        this.jsonSchemaStore.registerSchema({
            fileMatch: ['settings.json', user_preference_provider_1.USER_PREFERENCE_URI.toString()],
            url: uri.toString()
        });
        this.schemaProvider.onDidPreferenceSchemaChanged(function () {
            return _this.inmemoryResources.update(uri, serializeSchema());
        });
    };
    __decorate([
        inversify_1.inject(json_schema_store_1.JsonSchemaStore),
        __metadata("design:type", json_schema_store_1.JsonSchemaStore)
    ], PreferencesFrontendApplicationContribution.prototype, "jsonSchemaStore", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceSchemaProvider),
        __metadata("design:type", browser_1.PreferenceSchemaProvider)
    ], PreferencesFrontendApplicationContribution.prototype, "schemaProvider", void 0);
    __decorate([
        inversify_1.inject(core_1.InMemoryResources),
        __metadata("design:type", core_1.InMemoryResources)
    ], PreferencesFrontendApplicationContribution.prototype, "inmemoryResources", void 0);
    PreferencesFrontendApplicationContribution = __decorate([
        inversify_1.injectable()
    ], PreferencesFrontendApplicationContribution);
    return PreferencesFrontendApplicationContribution;
}());
exports.PreferencesFrontendApplicationContribution = PreferencesFrontendApplicationContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-frontend-application-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-menu-factory.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-menu-factory.js ***!
  \********************************************************************************************************/
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
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var widgets_1 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
var commands_1 = __webpack_require__(/*! @phosphor/commands */ "../../node_modules/@phosphor/commands/lib/index.js");
var strings_1 = __webpack_require__(/*! @theia/core/lib/common/strings */ "../../packages/core/lib/common/strings.js");
var PreferencesMenuFactory = /** @class */ (function () {
    function PreferencesMenuFactory() {
    }
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    PreferencesMenuFactory.prototype.createPreferenceContextMenu = function (id, savedPreference, property, execute) {
        var commands = new commands_1.CommandRegistry();
        var menu = new widgets_1.Menu({ commands: commands });
        if (property) {
            var enumConst = property.enum;
            if (enumConst) {
                enumConst.map(strings_1.escapeInvisibleChars)
                    .forEach(function (enumValue) {
                    var commandId = id + '-' + enumValue;
                    if (!commands.hasCommand(commandId)) {
                        commands.addCommand(commandId, {
                            label: enumValue,
                            iconClass: strings_1.escapeInvisibleChars(savedPreference) === enumValue || !savedPreference && property.defaultValue === enumValue ? 'fa fa-check' : '',
                            execute: function () { return execute(id, strings_1.unescapeInvisibleChars(enumValue)); }
                        });
                        menu.addItem({
                            type: 'command',
                            command: commandId
                        });
                    }
                });
            }
            else if (property.type && property.type === 'boolean') {
                var commandTrue = id + '-true';
                commands.addCommand(commandTrue, {
                    label: 'true',
                    iconClass: savedPreference === true || savedPreference === 'true' || savedPreference === undefined && property.defaultValue === true ? 'fa fa-check' : '',
                    execute: function () { return execute(id, true); }
                });
                menu.addItem({
                    type: 'command',
                    command: commandTrue
                });
                var commandFalse = id + '-false';
                commands.addCommand(commandFalse, {
                    label: 'false',
                    iconClass: savedPreference === false || savedPreference === 'false' || savedPreference === undefined && property.defaultValue === false ? 'fa fa-check' : '',
                    execute: function () { return execute(id, false); }
                });
                menu.addItem({
                    type: 'command',
                    command: commandFalse
                });
            }
            else {
                var commandId = id + '-stringValue';
                commands.addCommand(commandId, {
                    label: 'Add Value',
                    execute: function () { return execute(id, property.defaultValue ? property.defaultValue : ''); }
                });
                menu.addItem({
                    type: 'command',
                    command: commandId
                });
            }
        }
        return menu;
    };
    PreferencesMenuFactory = __decorate([
        inversify_1.injectable()
    ], PreferencesMenuFactory);
    return PreferencesMenuFactory;
}());
exports.PreferencesMenuFactory = PreferencesMenuFactory;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-menu-factory'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-monaco-contribution.js":
/*!***************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-monaco-contribution.js ***!
  \***************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {/********************************************************************************
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
monaco.languages.register({
    id: 'jsonc',
    'aliases': [
        'JSON with Comments'
    ],
    'filenames': [
        'settings.json'
    ]
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-monaco-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/preferences-tree-widget.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/preferences-tree-widget.js ***!
  \*******************************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var preferences_menu_factory_1 = __webpack_require__(/*! ./preferences-menu-factory */ "../../packages/preferences/lib/browser/preferences-menu-factory.js");
var preferences_decorator_1 = __webpack_require__(/*! ./preferences-decorator */ "../../packages/preferences/lib/browser/preferences-decorator.js");
var algorithm_1 = __webpack_require__(/*! @phosphor/algorithm */ "../../node_modules/@phosphor/algorithm/lib/index.js");
var widgets_1 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var user_preference_provider_1 = __webpack_require__(/*! ./user-preference-provider */ "../../packages/preferences/lib/browser/user-preference-provider.js");
var workspace_preference_provider_1 = __webpack_require__(/*! ./workspace-preference-provider */ "../../packages/preferences/lib/browser/workspace-preference-provider.js");
var preference_editor_widget_1 = __webpack_require__(/*! ./preference-editor-widget */ "../../packages/preferences/lib/browser/preference-editor-widget.js");
var browser_2 = __webpack_require__(/*! @theia/editor/lib/browser */ "../../packages/editor/lib/browser/index.js");
// import { JSONC_LANGUAGE_ID } from '@theia/json/lib/common';
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var promise_util_1 = __webpack_require__(/*! @theia/core/lib/common/promise-util */ "../../packages/core/lib/common/promise-util.js");
var common_1 = __webpack_require__(/*! @theia/filesystem/lib/common */ "../../packages/filesystem/lib/common/index.js");
var browser_3 = __webpack_require__(/*! @theia/userstorage/lib/browser */ "../../packages/userstorage/lib/browser/index.js");
var workspace_service_1 = __webpack_require__(/*! @theia/workspace/lib/browser/workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var folders_preferences_provider_1 = __webpack_require__(/*! ./folders-preferences-provider */ "../../packages/preferences/lib/browser/folders-preferences-provider.js");
var preference_configurations_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-configurations */ "../../packages/core/lib/browser/preferences/preference-configurations.js");
var env_variables_1 = __webpack_require__(/*! @theia/core/lib/common/env-variables */ "../../packages/core/lib/common/env-variables/index.js");
var PreferencesContainer = /** @class */ (function (_super) {
    __extends(PreferencesContainer, _super);
    function PreferencesContainer() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.editors = [];
        _this.deferredEditors = new promise_util_1.Deferred();
        _this.onDirtyChangedEmitter = new core_1.Emitter();
        _this.onDirtyChanged = _this.onDirtyChangedEmitter.event;
        _this.onDidChangeTrackableWidgetsEmitter = new core_1.Emitter();
        _this.onDidChangeTrackableWidgets = _this.onDidChangeTrackableWidgetsEmitter.event;
        _this.toDispose = new core_1.DisposableCollection();
        _this._preferenceScope = browser_1.PreferenceScope.User;
        return _this;
    }
    PreferencesContainer_1 = PreferencesContainer;
    PreferencesContainer.prototype.init = function () {
        this.id = PreferencesContainer_1.ID;
        this.title.label = 'Preferences';
        this.title.caption = this.title.label;
        this.title.closable = true;
        this.title.iconClass = 'fa fa-sliders';
        this.toDispose.pushAll([this.onDirtyChangedEmitter, this.onDidChangeTrackableWidgetsEmitter]);
    };
    PreferencesContainer.prototype.dispose = function () {
        if (this.isDisposed) {
            return;
        }
        _super.prototype.dispose.call(this);
        this.toDispose.dispose();
    };
    Object.defineProperty(PreferencesContainer.prototype, "autoSave", {
        get: function () {
            return this.editors.some(function (editor) { return editor.saveable.autoSave === 'on'; }) ? 'on' : 'off';
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PreferencesContainer.prototype, "dirty", {
        get: function () {
            return this.editors.some(function (editor) { return editor.saveable.dirty; });
        },
        enumerable: true,
        configurable: true
    });
    PreferencesContainer.prototype.save = function () {
        this.editors.forEach(function (editor) { return editor.saveable.save(); });
    };
    PreferencesContainer.prototype.getTrackableWidgets = function () {
        return this.deferredEditors.promise;
    };
    Object.defineProperty(PreferencesContainer.prototype, "preferenceScope", {
        get: function () {
            return this._preferenceScope;
        },
        set: function (preferenceScope) {
            this._preferenceScope = preferenceScope;
        },
        enumerable: true,
        configurable: true
    });
    PreferencesContainer.prototype.onAfterAttach = function (msg) {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, treePanel;
            var _this = this;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        if (this.widgets.length > 0) {
                            return [2 /*return*/];
                        }
                        _a = this;
                        return [4 /*yield*/, this.widgetManager.getOrCreateWidget(PreferencesTreeWidget.ID)];
                    case 1:
                        _a.treeWidget = _c.sent();
                        this.treeWidget.onPreferenceSelected(function (value) {
                            var preferenceName = Object.keys(value)[0];
                            var preferenceValue = value[preferenceName];
                            if (_this.dirty) {
                                _this.messageService.warn('Preferences editor(s) has/have unsaved changes');
                            }
                            else if (_this.currentEditor) {
                                _this.preferenceService.set(preferenceName, preferenceValue, _this.currentEditor.scope, _this.currentEditor.editor.uri.toString());
                            }
                        });
                        _b = this;
                        return [4 /*yield*/, this.widgetManager.getOrCreateWidget(PreferencesEditorsContainer.ID)];
                    case 2:
                        _b.editorsContainer = _c.sent();
                        this.toDispose.push(this.editorsContainer);
                        this.editorsContainer.activatePreferenceEditor(this.preferenceScope);
                        this.toDispose.push(this.editorsContainer.onInit(function () {
                            _this.handleEditorsChanged();
                            _this.deferredEditors.resolve(_this.editors);
                        }));
                        this.toDispose.push(this.editorsContainer.onEditorChanged(function (editor) {
                            if (_this.currentEditor && _this.currentEditor.editor.uri.toString() !== editor.editor.uri.toString()) {
                                _this.currentEditor.saveable.save();
                            }
                            if (editor) {
                                _this.preferenceScope = editor.scope || browser_1.PreferenceScope.User;
                            }
                            else {
                                _this.preferenceScope = browser_1.PreferenceScope.User;
                            }
                            _this.currentEditor = editor;
                        }));
                        this.toDispose.push(this.editorsContainer.onFolderPreferenceEditorUriChanged(function (uriStr) {
                            if (_this.treeWidget) {
                                _this.treeWidget.setActiveFolder(uriStr);
                            }
                            _this.handleEditorsChanged();
                        }));
                        this.toDispose.push(this.workspaceService.onWorkspaceLocationChanged(function (workspaceFile) { return __awaiter(_this, void 0, void 0, function () {
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0: return [4 /*yield*/, this.editorsContainer.refreshWorkspacePreferenceEditor()];
                                    case 1:
                                        _a.sent();
                                        return [4 /*yield*/, this.refreshFoldersPreferencesEditor()];
                                    case 2:
                                        _a.sent();
                                        this.handleEditorsChanged();
                                        return [2 /*return*/];
                                }
                            });
                        }); }));
                        this.toDispose.push(this.workspaceService.onWorkspaceChanged(function (roots) { return __awaiter(_this, void 0, void 0, function () {
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0: return [4 /*yield*/, this.refreshFoldersPreferencesEditor()];
                                    case 1:
                                        _a.sent();
                                        return [2 /*return*/];
                                }
                            });
                        }); }));
                        treePanel = new widgets_1.BoxPanel();
                        treePanel.addWidget(this.treeWidget);
                        this.addWidget(treePanel);
                        this.addWidget(this.editorsContainer);
                        this.treeWidget.activate();
                        _super.prototype.onAfterAttach.call(this, msg);
                        return [2 /*return*/];
                }
            });
        });
    };
    PreferencesContainer.prototype.onActivateRequest = function (msg) {
        if (this.currentEditor) {
            this.currentEditor.activate();
        }
        _super.prototype.onActivateRequest.call(this, msg);
    };
    PreferencesContainer.prototype.onCloseRequest = function (msg) {
        if (this.treeWidget) {
            this.treeWidget.close();
        }
        this.editorsContainer.close();
        _super.prototype.onCloseRequest.call(this, msg);
        this.dispose();
    };
    PreferencesContainer.prototype.activatePreferenceEditor = function (preferenceScope) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.deferredEditors.promise];
                    case 1:
                        _a.sent();
                        this.doActivatePreferenceEditor(preferenceScope);
                        return [2 /*return*/];
                }
            });
        });
    };
    PreferencesContainer.prototype.doActivatePreferenceEditor = function (preferenceScope) {
        this.preferenceScope = preferenceScope;
        if (this.editorsContainer) {
            this.editorsContainer.activatePreferenceEditor(preferenceScope);
        }
    };
    PreferencesContainer.prototype.handleEditorsChanged = function () {
        var _this = this;
        var currentEditors = algorithm_1.toArray(this.editorsContainer.widgets());
        currentEditors.forEach(function (editor) {
            if (editor instanceof browser_2.EditorWidget && _this.editors.findIndex(function (e) { return e === editor; }) < 0) {
                var editorWidget = editor;
                _this.editors.push(editorWidget);
                var savable = editorWidget.saveable;
                savable.onDirtyChanged(function () {
                    _this.onDirtyChangedEmitter.fire(undefined);
                });
            }
        });
        var _loop_1 = function (i) {
            if (currentEditors.findIndex(function (e) { return e === _this.editors[i]; }) < 0) {
                this_1.editors.splice(i, 1);
            }
        };
        var this_1 = this;
        for (var i = this.editors.length - 1; i >= 0; i--) {
            _loop_1(i);
        }
        this.onDidChangeTrackableWidgetsEmitter.fire(this.editors);
        this.doActivatePreferenceEditor(this.preferenceScope);
    };
    PreferencesContainer.prototype.refreshFoldersPreferencesEditor = function () {
        return __awaiter(this, void 0, void 0, function () {
            var roots, firstRoot;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        roots = this.workspaceService.tryGetRoots();
                        if (!(roots.length === 0)) return [3 /*break*/, 1];
                        this.editorsContainer.closeFoldersPreferenceEditorWidget();
                        return [3 /*break*/, 3];
                    case 1:
                        if (!!roots.some(function (r) { return r.uri === _this.editorsContainer.activeFolder; })) return [3 /*break*/, 3];
                        firstRoot = roots[0];
                        return [4 /*yield*/, this.editorsContainer.refreshFoldersPreferencesEditorWidget(firstRoot ? firstRoot.uri : undefined)];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    var PreferencesContainer_1;
    PreferencesContainer.ID = 'preferences_container_widget';
    __decorate([
        inversify_1.inject(browser_1.WidgetManager),
        __metadata("design:type", browser_1.WidgetManager)
    ], PreferencesContainer.prototype, "widgetManager", void 0);
    __decorate([
        inversify_1.inject(browser_1.ApplicationShell),
        __metadata("design:type", browser_1.ApplicationShell)
    ], PreferencesContainer.prototype, "shell", void 0);
    __decorate([
        inversify_1.inject(core_1.MessageService),
        __metadata("design:type", core_1.MessageService)
    ], PreferencesContainer.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceService),
        __metadata("design:type", Object)
    ], PreferencesContainer.prototype, "preferenceService", void 0);
    __decorate([
        inversify_1.inject(workspace_service_1.WorkspaceService),
        __metadata("design:type", workspace_service_1.WorkspaceService)
    ], PreferencesContainer.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], PreferencesContainer.prototype, "init", null);
    PreferencesContainer = PreferencesContainer_1 = __decorate([
        inversify_1.injectable()
    ], PreferencesContainer);
    return PreferencesContainer;
}(widgets_1.SplitPanel));
exports.PreferencesContainer = PreferencesContainer;
var PreferencesEditorsContainer = /** @class */ (function (_super) {
    __extends(PreferencesEditorsContainer, _super);
    function PreferencesEditorsContainer(workspaceService, fileSystem, foldersPreferenceProvider) {
        var _this = _super.call(this, { renderer: new preference_editor_widget_1.PreferenceEditorContainerTabBarRenderer(workspaceService, fileSystem, foldersPreferenceProvider) }) || this;
        _this.workspaceService = workspaceService;
        _this.fileSystem = fileSystem;
        _this.foldersPreferenceProvider = foldersPreferenceProvider;
        _this.onInitEmitter = new core_1.Emitter();
        _this.onInit = _this.onInitEmitter.event;
        _this.onEditorChangedEmitter = new core_1.Emitter();
        _this.onEditorChanged = _this.onEditorChangedEmitter.event;
        _this.onFolderPreferenceEditorUriChangedEmitter = new core_1.Emitter();
        _this.onFolderPreferenceEditorUriChanged = _this.onFolderPreferenceEditorUriChangedEmitter.event;
        _this.toDispose = new core_1.DisposableCollection(_this.onEditorChangedEmitter, _this.onInitEmitter);
        _this.toDisposeOnDetach = new core_1.DisposableCollection();
        return _this;
    }
    PreferencesEditorsContainer.prototype.dispose = function () {
        this.toDispose.dispose();
        _super.prototype.dispose.call(this);
    };
    PreferencesEditorsContainer.prototype.onCloseRequest = function (msg) {
        algorithm_1.toArray(this.widgets()).forEach(function (widget) { return widget.close(); });
        _super.prototype.onCloseRequest.call(this, msg);
    };
    PreferencesEditorsContainer.prototype.onUpdateRequest = function (msg) {
        var editor = this.selectedWidgets().next();
        if (editor) {
            this.onEditorChangedEmitter.fire(editor);
        }
        _super.prototype.onUpdateRequest.call(this, msg);
    };
    PreferencesEditorsContainer.prototype.onBeforeDetach = function () {
        this.toDisposeOnDetach.dispose();
    };
    PreferencesEditorsContainer.prototype.onAfterAttach = function (msg) {
        return __awaiter(this, void 0, void 0, function () {
            var _a;
            var _this = this;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = this;
                        return [4 /*yield*/, this.getUserPreferenceEditorWidget()];
                    case 1:
                        _a.userPreferenceEditorWidget = _b.sent();
                        this.addWidget(this.userPreferenceEditorWidget);
                        return [4 /*yield*/, this.refreshWorkspacePreferenceEditor()];
                    case 2:
                        _b.sent();
                        return [4 /*yield*/, this.refreshFoldersPreferencesEditorWidget(undefined)];
                    case 3:
                        _b.sent();
                        _super.prototype.onAfterAttach.call(this, msg);
                        this.onInitEmitter.fire(undefined);
                        this.toDisposeOnDetach.push(this.labelProvider.onDidChange(function () {
                            // Listen to changes made by the label provider and apply updates to the preference editors.
                            var icon = _this.labelProvider.getIcon(new uri_1.default('settings.json'));
                            _this.userPreferenceEditorWidget.title.iconClass = icon;
                            if (_this.workspacePreferenceEditorWidget) {
                                // Explicitly update the workspace preference title to `Workspace` for single and multi-root workspaces.
                                _this.workspacePreferenceEditorWidget.title.label = 'Workspace';
                                _this.workspacePreferenceEditorWidget.title.iconClass = icon;
                            }
                        }));
                        return [2 /*return*/];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.getUserPreferenceEditorWidget = function () {
        return __awaiter(this, void 0, void 0, function () {
            var userPreferenceUri, userPreferences, _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        userPreferenceUri = this.userPreferenceProvider.getConfigUri();
                        return [4 /*yield*/, this.editorManager.getOrCreateByUri(userPreferenceUri)];
                    case 1:
                        userPreferences = _c.sent();
                        userPreferences.title.label = 'User';
                        userPreferences.title.iconClass = this.labelProvider.getIcon(new uri_1.default('settings.json'));
                        _a = userPreferences.title;
                        _b = "User Preferences: ";
                        return [4 /*yield*/, this.getPreferenceEditorCaption(userPreferenceUri)];
                    case 2:
                        _a.caption = _b + (_c.sent());
                        userPreferences.scope = browser_1.PreferenceScope.User;
                        return [2 /*return*/, userPreferences];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.refreshWorkspacePreferenceEditor = function () {
        return __awaiter(this, void 0, void 0, function () {
            var newWorkspacePreferenceEditorWidget;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.getWorkspacePreferenceEditorWidget()];
                    case 1:
                        newWorkspacePreferenceEditorWidget = _a.sent();
                        if (newWorkspacePreferenceEditorWidget) {
                            this.addWidget(newWorkspacePreferenceEditorWidget, { ref: this.workspacePreferenceEditorWidget || this.userPreferenceEditorWidget });
                            if (this.workspacePreferenceEditorWidget) {
                                this.workspacePreferenceEditorWidget.close();
                                this.workspacePreferenceEditorWidget.dispose();
                            }
                            this.workspacePreferenceEditorWidget = newWorkspacePreferenceEditorWidget;
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.getWorkspacePreferenceEditorWidget = function () {
        return __awaiter(this, void 0, void 0, function () {
            var workspacePreferenceUri, workspacePreferences, _a, _b, _c;
            return __generator(this, function (_d) {
                switch (_d.label) {
                    case 0:
                        workspacePreferenceUri = this.workspacePreferenceProvider.getConfigUri();
                        _a = workspacePreferenceUri;
                        if (!_a) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.editorManager.getOrCreateByUri(workspacePreferenceUri)];
                    case 1:
                        _a = (_d.sent());
                        _d.label = 2;
                    case 2:
                        workspacePreferences = _a;
                        if (!workspacePreferences) return [3 /*break*/, 4];
                        workspacePreferences.title.label = 'Workspace';
                        _b = workspacePreferences.title;
                        _c = "Workspace Preferences: ";
                        return [4 /*yield*/, this.getPreferenceEditorCaption(workspacePreferenceUri)];
                    case 3:
                        _b.caption = _c + (_d.sent());
                        workspacePreferences.title.iconClass = this.labelProvider.getIcon(new uri_1.default('settings.json'));
                        // workspacePreferences.editor.setLanguage('jsonc');
                        workspacePreferences.scope = browser_1.PreferenceScope.Workspace;
                        _d.label = 4;
                    case 4: return [2 /*return*/, workspacePreferences];
                }
            });
        });
    };
    Object.defineProperty(PreferencesEditorsContainer.prototype, "activeFolder", {
        get: function () {
            if (this.foldersPreferenceEditorWidget) {
                return this.foldersPreferenceEditorWidget.editor.uri.parent.parent.toString();
            }
        },
        enumerable: true,
        configurable: true
    });
    PreferencesEditorsContainer.prototype.refreshFoldersPreferencesEditorWidget = function (currentFolderUri) {
        return __awaiter(this, void 0, void 0, function () {
            var folders, newFolderUri, newFoldersPreferenceEditorWidget;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        folders = this.workspaceService.tryGetRoots().map(function (r) { return r.uri; });
                        newFolderUri = currentFolderUri || folders[0];
                        return [4 /*yield*/, this.getFoldersPreferencesEditor(newFolderUri)];
                    case 1:
                        newFoldersPreferenceEditorWidget = _a.sent();
                        if (newFoldersPreferenceEditorWidget && // new widget is created
                            // the FolderPreferencesEditor is not available, OR the existing FolderPreferencesEditor is displaying the content of a different file
                            (!this.foldersPreferenceEditorWidget || this.foldersPreferenceEditorWidget.editor.uri.parent.parent.toString() !== newFolderUri)) {
                            this.addWidget(newFoldersPreferenceEditorWidget, { ref: this.foldersPreferenceEditorWidget || this.workspacePreferenceEditorWidget || this.userPreferenceEditorWidget });
                            this.closeFoldersPreferenceEditorWidget();
                            this.foldersPreferenceEditorWidget = newFoldersPreferenceEditorWidget;
                            this.onFolderPreferenceEditorUriChangedEmitter.fire(newFoldersPreferenceEditorWidget.editor.uri.toString());
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.closeFoldersPreferenceEditorWidget = function () {
        if (this.foldersPreferenceEditorWidget) {
            this.foldersPreferenceEditorWidget.close();
            this.foldersPreferenceEditorWidget.dispose();
            this.foldersPreferenceEditorWidget = undefined;
        }
    };
    PreferencesEditorsContainer.prototype.getFoldersPreferencesEditor = function (folderUri) {
        return __awaiter(this, void 0, void 0, function () {
            var settingsUri, foldersPreferences_1, _a, _b, _c;
            var _this = this;
            return __generator(this, function (_d) {
                switch (_d.label) {
                    case 0:
                        if (!this.workspaceService.saved) return [3 /*break*/, 6];
                        return [4 /*yield*/, this.getFolderSettingsUri(folderUri)];
                    case 1:
                        settingsUri = _d.sent();
                        _a = settingsUri;
                        if (!_a) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.editorManager.getOrCreateByUri(settingsUri)];
                    case 2:
                        _a = (_d.sent());
                        _d.label = 3;
                    case 3:
                        foldersPreferences_1 = _a;
                        if (!foldersPreferences_1) return [3 /*break*/, 5];
                        foldersPreferences_1.title.label = 'Folder';
                        _b = foldersPreferences_1.title;
                        _c = "Folder Preferences: ";
                        return [4 /*yield*/, this.getPreferenceEditorCaption(settingsUri)];
                    case 4:
                        _b.caption = _c + (_d.sent());
                        foldersPreferences_1.title.clickableText = new uri_1.default(folderUri).displayName;
                        foldersPreferences_1.title.clickableTextTooltip = 'Click to manage preferences in another folder';
                        foldersPreferences_1.title.clickableTextCallback = function (folderUriStr) { return __awaiter(_this, void 0, void 0, function () {
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0: return [4 /*yield*/, foldersPreferences_1.saveable.save()];
                                    case 1:
                                        _a.sent();
                                        return [4 /*yield*/, this.refreshFoldersPreferencesEditorWidget(folderUriStr)];
                                    case 2:
                                        _a.sent();
                                        this.activatePreferenceEditor(browser_1.PreferenceScope.Folder);
                                        return [2 /*return*/];
                                }
                            });
                        }); };
                        foldersPreferences_1.scope = browser_1.PreferenceScope.Folder;
                        _d.label = 5;
                    case 5: return [2 /*return*/, foldersPreferences_1];
                    case 6: return [2 /*return*/];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.getFolderSettingsUri = function (folderUri) {
        return __awaiter(this, void 0, void 0, function () {
            var configUri;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        configUri = this.foldersPreferenceProvider.getConfigUri(folderUri);
                        if (!!configUri) return [3 /*break*/, 2];
                        configUri = this.foldersPreferenceProvider.getContainingConfigUri(folderUri);
                        if (!configUri) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.fileSystem.createFile(configUri.toString())];
                    case 1:
                        _a.sent();
                        _a.label = 2;
                    case 2: return [2 /*return*/, configUri];
                }
            });
        });
    };
    PreferencesEditorsContainer.prototype.activatePreferenceEditor = function (preferenceScope) {
        var e_1, _a;
        try {
            for (var _b = __values(algorithm_1.toArray(this.widgets())), _c = _b.next(); !_c.done; _c = _b.next()) {
                var widget = _c.value;
                var preferenceEditor = widget;
                if (preferenceEditor.scope === preferenceScope) {
                    this.activateWidget(widget);
                    break;
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
    };
    PreferencesEditorsContainer.prototype.getPreferenceEditorCaption = function (preferenceUri) {
        return __awaiter(this, void 0, void 0, function () {
            var homeStat, homeUri, uri, configDirUri;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.fileSystem.getCurrentUserHome()];
                    case 1:
                        homeStat = _a.sent();
                        homeUri = homeStat ? new uri_1.default(homeStat.uri) : undefined;
                        uri = preferenceUri;
                        if (!(preferenceUri.scheme === browser_3.UserStorageUri.SCHEME && homeUri)) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.envServer.getConfigDirUri()];
                    case 2:
                        configDirUri = _a.sent();
                        uri = new uri_1.default(configDirUri).resolve(preferenceUri.path);
                        _a.label = 3;
                    case 3: return [2 /*return*/, homeUri
                            ? common_1.FileSystemUtils.tildifyPath(uri.path.toString(), homeUri.path.toString())
                            : uri.path.toString()];
                }
            });
        });
    };
    PreferencesEditorsContainer.ID = 'preferences_editors_container';
    __decorate([
        inversify_1.inject(browser_2.EditorManager),
        __metadata("design:type", browser_2.EditorManager)
    ], PreferencesEditorsContainer.prototype, "editorManager", void 0);
    __decorate([
        inversify_1.inject(browser_1.LabelProvider),
        __metadata("design:type", browser_1.LabelProvider)
    ], PreferencesEditorsContainer.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceProvider), inversify_1.named(browser_1.PreferenceScope.User),
        __metadata("design:type", user_preference_provider_1.UserPreferenceProvider)
    ], PreferencesEditorsContainer.prototype, "userPreferenceProvider", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceProvider), inversify_1.named(browser_1.PreferenceScope.Workspace),
        __metadata("design:type", workspace_preference_provider_1.WorkspacePreferenceProvider)
    ], PreferencesEditorsContainer.prototype, "workspacePreferenceProvider", void 0);
    __decorate([
        inversify_1.inject(env_variables_1.EnvVariablesServer),
        __metadata("design:type", Object)
    ], PreferencesEditorsContainer.prototype, "envServer", void 0);
    PreferencesEditorsContainer = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(workspace_service_1.WorkspaceService)),
        __param(1, inversify_1.inject(common_1.FileSystem)),
        __param(2, inversify_1.inject(browser_1.PreferenceProvider)), __param(2, inversify_1.named(browser_1.PreferenceScope.Folder)),
        __metadata("design:paramtypes", [workspace_service_1.WorkspaceService, Object, folders_preferences_provider_1.FoldersPreferencesProvider])
    ], PreferencesEditorsContainer);
    return PreferencesEditorsContainer;
}(widgets_1.DockPanel));
exports.PreferencesEditorsContainer = PreferencesEditorsContainer;
var PreferencesTreeWidget = /** @class */ (function (_super) {
    __extends(PreferencesTreeWidget, _super);
    function PreferencesTreeWidget(model, treeProps, contextMenuRenderer, preferenceSchemaProvider) {
        var _this = _super.call(this, treeProps, model, contextMenuRenderer) || this;
        _this.model = model;
        _this.treeProps = treeProps;
        _this.contextMenuRenderer = contextMenuRenderer;
        _this.preferenceSchemaProvider = preferenceSchemaProvider;
        _this.preferencesGroupNames = new Set();
        _this.onPreferenceSelectedEmitter = new core_1.Emitter();
        _this.onPreferenceSelected = _this.onPreferenceSelectedEmitter.event;
        _this.toDispose.push(_this.onPreferenceSelectedEmitter);
        _this.id = PreferencesTreeWidget_1.ID;
        return _this;
    }
    PreferencesTreeWidget_1 = PreferencesTreeWidget;
    PreferencesTreeWidget.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
    };
    PreferencesTreeWidget.prototype.onAfterAttach = function (msg) {
        var _this = this;
        this.initializeModel();
        this.toDisposeOnDetach.push(this.preferenceSchemaProvider.onDidPreferenceSchemaChanged(function () {
            _this.initializeModel();
        }));
        _super.prototype.onAfterAttach.call(this, msg);
    };
    PreferencesTreeWidget.prototype.handleContextMenuEvent = function (node, event) {
        _super.prototype.handleContextMenuEvent.call(this, node, event);
        if (node.expanded === undefined) {
            this.openContextMenu(node, event.nativeEvent.x, event.nativeEvent.y);
        }
    };
    PreferencesTreeWidget.prototype.handleClickEvent = function (node, event) {
        _super.prototype.handleClickEvent.call(this, node, event);
        if (node.expanded === undefined) {
            this.openContextMenu(node, event.nativeEvent.x, event.nativeEvent.y);
        }
    };
    PreferencesTreeWidget.prototype.handleEnter = function (event) {
        _super.prototype.handleEnter.call(this, event);
        var node = this.model.selectedNodes[0];
        if (node.expanded === undefined) {
            if (node) {
                var nodeElement = document.getElementById(node.id);
                if (nodeElement) {
                    var position = nodeElement.getBoundingClientRect();
                    this.openContextMenu(this.model.selectedNodes[0], position.left, position.bottom);
                }
            }
        }
    };
    PreferencesTreeWidget.prototype.openContextMenu = function (node, positionX, positionY) {
        var _this = this;
        if (node && browser_1.SelectableTreeNode.is(node)) {
            var contextMenu = this.preferencesMenuFactory.createPreferenceContextMenu(node.id, this.preferenceService.get(node.id, undefined, this.activeFolderUri), this.properties[node.id], function (property, value) {
                var _a;
                _this.onPreferenceSelectedEmitter.fire((_a = {}, _a[property] = value, _a));
            });
            contextMenu.aboutToClose.connect(function () {
                _this.activate();
            });
            contextMenu.activeItem = contextMenu.items[0];
            contextMenu.open(positionX, positionY);
        }
    };
    PreferencesTreeWidget.prototype.initializeModel = function () {
        var e_2, _a;
        var _this = this;
        this.properties = this.preferenceSchemaProvider.getCombinedSchema().properties;
        for (var property in this.properties) {
            if (property) {
                // Compute preference group name and accept those which have the proper format.
                var group = property.substring(0, property.indexOf('.'));
                if (property.split('.').length > 1) {
                    this.preferencesGroupNames.add(group);
                }
            }
        }
        var preferencesGroups = [];
        var nodes = [];
        var groupNames = Array.from(this.preferencesGroupNames).sort(function (a, b) { return _this.sort(a, b); });
        var root = {
            id: 'root-node-id',
            name: 'Apply the preference to selected preferences file',
            parent: undefined,
            visible: true,
            children: preferencesGroups,
            expanded: true,
        };
        var _loop_2 = function (group) {
            var propertyNodes = [];
            var properties = [];
            // Add a preference property if it is currently part of the group name.
            // Properties which satisfy the condition `isSectionName` should not be added.
            for (var property in this_2.properties) {
                if (property.split('.', 1)[0] === group &&
                    !this_2.preferenceConfigs.isSectionName(property)) {
                    properties.push(property);
                }
            }
            // Build the group name node (used to categorize common preferences together).
            var preferencesGroup = {
                id: group + '-id',
                name: group.toLocaleUpperCase().substring(0, 1) + group.substring(1) + ' (' + properties.length + ')',
                visible: true,
                parent: root,
                children: propertyNodes,
                expanded: false,
                selected: false
            };
            properties.sort(function (a, b) { return _this.sort(a, b); }).forEach(function (property) {
                var _a;
                var node = {
                    id: property,
                    name: property.substring(property.indexOf('.') + 1),
                    parent: preferencesGroup,
                    visible: true,
                    selected: false
                };
                propertyNodes.push(node);
                nodes.push((_a = {}, _a[property] = _this.properties[property], _a));
            });
            preferencesGroups.push(preferencesGroup);
        };
        var this_2 = this;
        try {
            for (var groupNames_1 = __values(groupNames), groupNames_1_1 = groupNames_1.next(); !groupNames_1_1.done; groupNames_1_1 = groupNames_1.next()) {
                var group = groupNames_1_1.value;
                _loop_2(group);
            }
        }
        catch (e_2_1) { e_2 = { error: e_2_1 }; }
        finally {
            try {
                if (groupNames_1_1 && !groupNames_1_1.done && (_a = groupNames_1.return)) _a.call(groupNames_1);
            }
            finally { if (e_2) throw e_2.error; }
        }
        this.decorator.fireDidChangeDecorations(nodes);
        this.model.root = root;
    };
    PreferencesTreeWidget.prototype.setActiveFolder = function (folder) {
        this.activeFolderUri = folder;
        this.decorator.setActiveFolder(folder);
    };
    /**
     * Sort two string.
     *
     * @param a the first string.
     * @param b the second string.
     */
    PreferencesTreeWidget.prototype.sort = function (a, b) {
        return a.localeCompare(b, undefined, { ignorePunctuation: true });
    };
    var PreferencesTreeWidget_1;
    PreferencesTreeWidget.ID = 'preferences_tree_widget';
    __decorate([
        inversify_1.inject(preferences_menu_factory_1.PreferencesMenuFactory),
        __metadata("design:type", preferences_menu_factory_1.PreferencesMenuFactory)
    ], PreferencesTreeWidget.prototype, "preferencesMenuFactory", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceService),
        __metadata("design:type", Object)
    ], PreferencesTreeWidget.prototype, "preferenceService", void 0);
    __decorate([
        inversify_1.inject(preferences_decorator_1.PreferencesDecorator),
        __metadata("design:type", preferences_decorator_1.PreferencesDecorator)
    ], PreferencesTreeWidget.prototype, "decorator", void 0);
    __decorate([
        inversify_1.inject(preference_configurations_1.PreferenceConfigurations),
        __metadata("design:type", preference_configurations_1.PreferenceConfigurations)
    ], PreferencesTreeWidget.prototype, "preferenceConfigs", void 0);
    PreferencesTreeWidget = PreferencesTreeWidget_1 = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_1.TreeModel)),
        __param(1, inversify_1.inject(browser_1.TreeProps)),
        __param(2, inversify_1.inject(browser_1.ContextMenuRenderer)),
        __param(3, inversify_1.inject(browser_1.PreferenceSchemaProvider)),
        __metadata("design:paramtypes", [Object, Object, Object, browser_1.PreferenceSchemaProvider])
    ], PreferencesTreeWidget);
    return PreferencesTreeWidget;
}(browser_1.TreeWidget));
exports.PreferencesTreeWidget = PreferencesTreeWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/preferences-tree-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/user-preference-provider.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/user-preference-provider.js ***!
  \********************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var abstract_resource_preference_provider_1 = __webpack_require__(/*! ./abstract-resource-preference-provider */ "../../packages/preferences/lib/browser/abstract-resource-preference-provider.js");
var browser_1 = __webpack_require__(/*! @theia/userstorage/lib/browser */ "../../packages/userstorage/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
exports.USER_PREFERENCE_URI = new uri_1.default().withScheme(browser_1.UserStorageUri.SCHEME).withPath('settings.json');
var UserPreferenceProvider = /** @class */ (function (_super) {
    __extends(UserPreferenceProvider, _super);
    function UserPreferenceProvider() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    UserPreferenceProvider.prototype.getUri = function () {
        return exports.USER_PREFERENCE_URI;
    };
    UserPreferenceProvider.prototype.getScope = function () {
        return browser_2.PreferenceScope.User;
    };
    UserPreferenceProvider = __decorate([
        inversify_1.injectable()
    ], UserPreferenceProvider);
    return UserPreferenceProvider;
}(abstract_resource_preference_provider_1.AbstractResourcePreferenceProvider));
exports.UserPreferenceProvider = UserPreferenceProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/user-preference-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/workspace-file-preference-provider.js":
/*!******************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/workspace-file-preference-provider.js ***!
  \******************************************************************************************************************/
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
var preferences_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences */ "../../packages/core/lib/browser/preferences/index.js");
var workspace_service_1 = __webpack_require__(/*! @theia/workspace/lib/browser/workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js");
var abstract_resource_preference_provider_1 = __webpack_require__(/*! ./abstract-resource-preference-provider */ "../../packages/preferences/lib/browser/abstract-resource-preference-provider.js");
var WorkspaceFilePreferenceProviderOptions = /** @class */ (function () {
    function WorkspaceFilePreferenceProviderOptions() {
    }
    WorkspaceFilePreferenceProviderOptions = __decorate([
        inversify_1.injectable()
    ], WorkspaceFilePreferenceProviderOptions);
    return WorkspaceFilePreferenceProviderOptions;
}());
exports.WorkspaceFilePreferenceProviderOptions = WorkspaceFilePreferenceProviderOptions;
exports.WorkspaceFilePreferenceProviderFactory = Symbol('WorkspaceFilePreferenceProviderFactory');
var WorkspaceFilePreferenceProvider = /** @class */ (function (_super) {
    __extends(WorkspaceFilePreferenceProvider, _super);
    function WorkspaceFilePreferenceProvider() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    WorkspaceFilePreferenceProvider.prototype.getUri = function () {
        return this.options.workspaceUri;
    };
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    WorkspaceFilePreferenceProvider.prototype.parse = function (content) {
        var data = _super.prototype.parse.call(this, content);
        if (workspace_service_1.WorkspaceData.is(data)) {
            return data.settings || {};
        }
        return {};
    };
    WorkspaceFilePreferenceProvider.prototype.getPath = function (preferenceName) {
        return ['settings', preferenceName];
    };
    WorkspaceFilePreferenceProvider.prototype.getScope = function () {
        return preferences_1.PreferenceScope.Workspace;
    };
    WorkspaceFilePreferenceProvider.prototype.getDomain = function () {
        // workspace file is treated as part of the workspace
        return this.workspaceService.tryGetRoots().map(function (r) { return r.uri; }).concat([this.options.workspaceUri.toString()]);
    };
    __decorate([
        inversify_1.inject(workspace_service_1.WorkspaceService),
        __metadata("design:type", workspace_service_1.WorkspaceService)
    ], WorkspaceFilePreferenceProvider.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(WorkspaceFilePreferenceProviderOptions),
        __metadata("design:type", WorkspaceFilePreferenceProviderOptions)
    ], WorkspaceFilePreferenceProvider.prototype, "options", void 0);
    WorkspaceFilePreferenceProvider = __decorate([
        inversify_1.injectable()
    ], WorkspaceFilePreferenceProvider);
    return WorkspaceFilePreferenceProvider;
}(abstract_resource_preference_provider_1.AbstractResourcePreferenceProvider));
exports.WorkspaceFilePreferenceProvider = WorkspaceFilePreferenceProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/workspace-file-preference-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/lib/browser/workspace-preference-provider.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/lib/browser/workspace-preference-provider.js ***!
  \*************************************************************************************************************/
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var disposable_1 = __webpack_require__(/*! @theia/core/lib/common/disposable */ "../../packages/core/lib/common/disposable.js");
var preferences_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences */ "../../packages/core/lib/browser/preferences/index.js");
var workspace_service_1 = __webpack_require__(/*! @theia/workspace/lib/browser/workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js");
var workspace_file_preference_provider_1 = __webpack_require__(/*! ./workspace-file-preference-provider */ "../../packages/preferences/lib/browser/workspace-file-preference-provider.js");
var WorkspacePreferenceProvider = /** @class */ (function (_super) {
    __extends(WorkspacePreferenceProvider, _super);
    function WorkspacePreferenceProvider() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.toDisposeOnEnsureDelegateUpToDate = new disposable_1.DisposableCollection();
        return _this;
    }
    WorkspacePreferenceProvider.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                this._ready.resolve();
                this.ensureDelegateUpToDate();
                this.workspaceService.onWorkspaceLocationChanged(function () { return _this.ensureDelegateUpToDate(); });
                return [2 /*return*/];
            });
        });
    };
    WorkspacePreferenceProvider.prototype.getConfigUri = function (resourceUri) {
        if (resourceUri === void 0) { resourceUri = this.ensureResourceUri(); }
        var delegate = this.delegate;
        return delegate && delegate.getConfigUri(resourceUri);
    };
    Object.defineProperty(WorkspacePreferenceProvider.prototype, "delegate", {
        get: function () {
            if (!this._delegate) {
                this.ensureDelegateUpToDate();
            }
            return this._delegate;
        },
        enumerable: true,
        configurable: true
    });
    WorkspacePreferenceProvider.prototype.ensureDelegateUpToDate = function () {
        var _this = this;
        var delegate = this.createDelegate();
        if (this._delegate !== delegate) {
            this.toDisposeOnEnsureDelegateUpToDate.dispose();
            this.toDispose.push(this.toDisposeOnEnsureDelegateUpToDate);
            this._delegate = delegate;
            if (delegate instanceof workspace_file_preference_provider_1.WorkspaceFilePreferenceProvider) {
                this.toDisposeOnEnsureDelegateUpToDate.pushAll([
                    delegate,
                    delegate.onDidPreferencesChanged(function (changes) { return _this.onDidPreferencesChangedEmitter.fire(changes); })
                ]);
            }
        }
    };
    WorkspacePreferenceProvider.prototype.createDelegate = function () {
        var workspace = this.workspaceService.workspace;
        if (!workspace) {
            return undefined;
        }
        if (!this.workspaceService.isMultiRootWorkspaceOpened) {
            return this.folderPreferenceProvider;
        }
        return this.workspaceFileProviderFactory({
            workspaceUri: new uri_1.default(workspace.uri)
        });
    };
    WorkspacePreferenceProvider.prototype.get = function (preferenceName, resourceUri) {
        if (resourceUri === void 0) { resourceUri = this.ensureResourceUri(); }
        var delegate = this.delegate;
        return delegate ? delegate.get(preferenceName, resourceUri) : undefined;
    };
    WorkspacePreferenceProvider.prototype.resolve = function (preferenceName, resourceUri) {
        if (resourceUri === void 0) { resourceUri = this.ensureResourceUri(); }
        var delegate = this.delegate;
        return delegate ? delegate.resolve(preferenceName, resourceUri) : {};
    };
    WorkspacePreferenceProvider.prototype.getPreferences = function (resourceUri) {
        if (resourceUri === void 0) { resourceUri = this.ensureResourceUri(); }
        var delegate = this.delegate;
        return delegate ? delegate.getPreferences(resourceUri) : {};
    };
    WorkspacePreferenceProvider.prototype.setPreference = function (preferenceName, value, resourceUri) {
        if (resourceUri === void 0) { resourceUri = this.ensureResourceUri(); }
        return __awaiter(this, void 0, void 0, function () {
            var delegate;
            return __generator(this, function (_a) {
                delegate = this.delegate;
                if (delegate) {
                    return [2 /*return*/, delegate.setPreference(preferenceName, value, resourceUri)];
                }
                return [2 /*return*/, false];
            });
        });
    };
    WorkspacePreferenceProvider.prototype.ensureResourceUri = function () {
        if (this.workspaceService.workspace && !this.workspaceService.isMultiRootWorkspaceOpened) {
            return this.workspaceService.workspace.uri;
        }
        return undefined;
    };
    __decorate([
        inversify_1.inject(workspace_service_1.WorkspaceService),
        __metadata("design:type", workspace_service_1.WorkspaceService)
    ], WorkspacePreferenceProvider.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(workspace_file_preference_provider_1.WorkspaceFilePreferenceProviderFactory),
        __metadata("design:type", Function)
    ], WorkspacePreferenceProvider.prototype, "workspaceFileProviderFactory", void 0);
    __decorate([
        inversify_1.inject(preferences_1.PreferenceProvider), inversify_1.named(preferences_1.PreferenceScope.Folder),
        __metadata("design:type", preferences_1.PreferenceProvider)
    ], WorkspacePreferenceProvider.prototype, "folderPreferenceProvider", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], WorkspacePreferenceProvider.prototype, "init", null);
    WorkspacePreferenceProvider = __decorate([
        inversify_1.injectable()
    ], WorkspacePreferenceProvider);
    return WorkspacePreferenceProvider;
}(preferences_1.PreferenceProvider));
exports.WorkspacePreferenceProvider = WorkspacePreferenceProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/preferences/lib/browser/workspace-preference-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/preferences/src/browser/style/index.css":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/preferences/src/browser/style/index.css ***!
  \********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/preferences/src/browser/style/index.css");

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

/***/ "../../packages/userstorage/lib/browser/index.js":
/*!*************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/userstorage/lib/browser/index.js ***!
  \*************************************************************************************/
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
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
__export(__webpack_require__(/*! ./user-storage-service */ "../../packages/userstorage/lib/browser/user-storage-service.js"));
__export(__webpack_require__(/*! ./user-storage-resource */ "../../packages/userstorage/lib/browser/user-storage-resource.js"));
__export(__webpack_require__(/*! ./user-storage-uri */ "../../packages/userstorage/lib/browser/user-storage-uri.js"));
__export(__webpack_require__(/*! ./user-storage-service-filesystem */ "../../packages/userstorage/lib/browser/user-storage-service-filesystem.js"));
__export(__webpack_require__(/*! ./user-storage-frontend-module */ "../../packages/userstorage/lib/browser/user-storage-frontend-module.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/userstorage/lib/browser'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=27.bundle.js.map