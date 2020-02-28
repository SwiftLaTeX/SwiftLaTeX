(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[14],{

/***/ "../../packages/languages/lib/browser/langtool-connection-provider.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/langtool-connection-provider.js ***!
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var lib_1 = __webpack_require__(/*! vscode-ws-jsonrpc/lib */ "../../node_modules/vscode-ws-jsonrpc/lib/index.js");
var web_socket_channel_1 = __webpack_require__(/*! @theia/core/lib/common/messaging/web-socket-channel */ "../../packages/core/lib/common/messaging/web-socket-channel.js");
var reconnecting_websocket_1 = __webpack_require__(/*! reconnecting-websocket */ "../../node_modules/reconnecting-websocket/dist/reconnecting-websocket-mjs.js");
var LANGTOOL_URL = 'wss://spell.swiftlatex.com';
var LangToolWebSocketConnectionProvider = /** @class */ (function () {
    function LangToolWebSocketConnectionProvider() {
        var _this = this;
        this.channelIdSeq = 0;
        this.channels = new Map();
        var socket = this.createWebSocket(LANGTOOL_URL + window.location.pathname);
        socket.onerror = console.error;
        socket.onclose = function (_a) {
            var e_1, _b;
            var code = _a.code, reason = _a.reason;
            try {
                for (var _c = __values(__spread(_this.channels.values())), _d = _c.next(); !_d.done; _d = _c.next()) {
                    var channel = _d.value;
                    channel.close(code, reason);
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (_d && !_d.done && (_b = _c.return)) _b.call(_c);
                }
                finally { if (e_1) throw e_1.error; }
            }
        };
        socket.onmessage = function (_a) {
            var data = _a.data;
            var message = JSON.parse(data);
            var channel = _this.channels.get(message.id);
            if (channel) {
                channel.handleMessage(message);
            }
            else {
                console.error('The ws channel does not exist', message.id);
            }
        };
        this.socket = socket;
    }
    /**
     * Install a connection handler for the given path.
     */
    LangToolWebSocketConnectionProvider.prototype.listen = function (handler, options) {
        var _this = this;
        this.openChannel(handler.path, function (channel) {
            var connection = lib_1.createWebSocketConnection(channel, _this.createLogger());
            connection.onDispose(function () { return channel.close(); });
            handler.onConnection(connection);
        }, options);
    };
    LangToolWebSocketConnectionProvider.prototype.openChannel = function (path, handler, options) {
        var _this = this;
        if (this.socket.readyState === WebSocket.OPEN) {
            this.doOpenChannel(path, handler, options);
        }
        else {
            var openChannel_1 = function () {
                _this.socket.removeEventListener('open', openChannel_1);
                _this.openChannel(path, handler, options);
            };
            this.socket.addEventListener('open', openChannel_1);
        }
    };
    LangToolWebSocketConnectionProvider.prototype.doOpenChannel = function (path, handler, options) {
        var _this = this;
        var id = this.channelIdSeq++;
        var channel = this.createChannel(id);
        this.channels.set(id, channel);
        channel.onClose(function () {
            if (_this.channels.delete(channel.id)) {
                var reconnecting = __assign({ reconnecting: true }, options).reconnecting;
                if (reconnecting) {
                    _this.openChannel(path, handler, options);
                }
            }
            else {
                console.error('The ws channel does not exist', channel.id);
            }
        });
        channel.onOpen(function () { return handler(channel); });
        channel.open(path);
    };
    LangToolWebSocketConnectionProvider.prototype.createChannel = function (id) {
        var _this = this;
        return new web_socket_channel_1.WebSocketChannel(id, function (content) {
            if (_this.socket.readyState < WebSocket.CLOSING) {
                _this.socket.send(content);
            }
        });
    };
    LangToolWebSocketConnectionProvider.prototype.createLogger = function () {
        return new lib_1.ConsoleLogger();
    };
    /**
     * Creates a web socket for the given url
     */
    LangToolWebSocketConnectionProvider.prototype.createWebSocket = function (url) {
        return new reconnecting_websocket_1.default(url, undefined, {
            maxReconnectionDelay: 10000,
            minReconnectionDelay: 1000,
            reconnectionDelayGrowFactor: 1.3,
            connectionTimeout: 10000,
            maxRetries: Infinity,
            debug: false
        });
    };
    LangToolWebSocketConnectionProvider = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], LangToolWebSocketConnectionProvider);
    return LangToolWebSocketConnectionProvider;
}());
exports.LangToolWebSocketConnectionProvider = LangToolWebSocketConnectionProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/langtool-connection-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-client-contribution.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-client-contribution.js ***!
  \**********************************************************************************************************/
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var language_client_services_1 = __webpack_require__(/*! ./language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var language_client_factory_1 = __webpack_require__(/*! ./language-client-factory */ "../../packages/languages/lib/browser/language-client-factory.js");
var browser_2 = __webpack_require__(/*! @theia/workspace/lib/browser */ "../../packages/workspace/lib/browser/index.js");
var promise_util_1 = __webpack_require__(/*! @theia/core/lib/common/promise-util */ "../../packages/core/lib/common/promise-util.js");
var langtool_connection_provider_1 = __webpack_require__(/*! ./langtool-connection-provider */ "../../packages/languages/lib/browser/langtool-connection-provider.js");
exports.LanguageClientContribution = Symbol('LanguageClientContribution');
var BaseLanguageClientContribution = /** @class */ (function () {
    function BaseLanguageClientContribution(workspace, languages, languageClientFactory) {
        this.workspace = workspace;
        this.languages = languages;
        this.languageClientFactory = languageClientFactory;
        this.deferredConnection = new promise_util_1.Deferred();
        this.toDeactivate = new common_1.DisposableCollection();
        this.stop = Promise.resolve();
        this.waitForReady();
    }
    Object.defineProperty(BaseLanguageClientContribution.prototype, "languageClient", {
        get: function () {
            return this._languageClient ? Promise.resolve(this._languageClient) : this.ready;
        },
        enumerable: true,
        configurable: true
    });
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    BaseLanguageClientContribution.prototype.waitForActivation = function (app) {
        var _this = this;
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        var activationPromises = [];
        var workspaceContains = this.workspaceContains;
        if (workspaceContains.length !== 0) {
            activationPromises.push(this.waitForItemInWorkspace());
        }
        var documentSelector = this.documentSelector;
        if (documentSelector) {
            activationPromises.push(this.waitForOpenTextDocument(documentSelector));
        }
        if (activationPromises.length !== 0) {
            return Promise.all([
                this.workspace.ready,
                Promise.race(activationPromises.map(function (p) { return new Promise(function (resolve) { return __awaiter(_this, void 0, void 0, function () {
                    var e_1;
                    return __generator(this, function (_a) {
                        switch (_a.label) {
                            case 0:
                                _a.trys.push([0, 2, , 3]);
                                return [4 /*yield*/, p];
                            case 1:
                                _a.sent();
                                resolve();
                                return [3 /*break*/, 3];
                            case 2:
                                e_1 = _a.sent();
                                console.error(e_1);
                                return [3 /*break*/, 3];
                            case 3: return [2 /*return*/];
                        }
                    });
                }); }); }))
            ]);
        }
        return this.workspace.ready;
    };
    BaseLanguageClientContribution.prototype.activate = function () {
        var _this = this;
        if (this.toDeactivate.disposed) {
            if (!this._languageClient) {
                this._languageClient = this.createLanguageClient(function () { return _this.deferredConnection.promise; });
                this._languageClient.onDidChangeState(function (_a) {
                    var newState = _a.newState;
                    _this.state = newState;
                });
            }
            var toStop = new common_1.DisposableCollection(common_1.Disposable.create(function () { })); // mark as not disposed
            this.toDeactivate.push(toStop);
            this.doActivate(toStop);
        }
        return this.toDeactivate;
    };
    BaseLanguageClientContribution.prototype.deactivate = function () {
        this.toDeactivate.dispose();
    };
    BaseLanguageClientContribution.prototype.doActivate = function (toStop) {
        return __awaiter(this, void 0, void 0, function () {
            var startParameters, sessionId_1, e_2;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 4, , 5]);
                        // make sure that the previous client is stopped to avoid duplicate commands and language services
                        return [4 /*yield*/, this.stop];
                    case 1:
                        // make sure that the previous client is stopped to avoid duplicate commands and language services
                        _a.sent();
                        if (toStop.disposed) {
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, this.getStartParameters()];
                    case 2:
                        startParameters = _a.sent();
                        if (toStop.disposed) {
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, this.languageContributionService.create(this.id, startParameters)];
                    case 3:
                        sessionId_1 = _a.sent();
                        if (toStop.disposed) {
                            this.languageContributionService.destroy(sessionId_1);
                            return [2 /*return*/];
                        }
                        toStop.push(common_1.Disposable.create(function () { return _this.languageContributionService.destroy(sessionId_1); }));
                        this.langtoolConnectionProvider.listen({
                            path: language_client_services_1.LanguageContribution.getPath(this, sessionId_1),
                            onConnection: function (messageConnection) {
                                _this.deferredConnection.resolve(messageConnection);
                                messageConnection.onDispose(function () { return _this.deferredConnection = new promise_util_1.Deferred(); });
                                if (toStop.disposed) {
                                    messageConnection.dispose();
                                    return;
                                }
                                toStop.push(common_1.Disposable.create(function () { return _this.stop = (function () { return __awaiter(_this, void 0, void 0, function () {
                                    var _a;
                                    return __generator(this, function (_b) {
                                        switch (_b.label) {
                                            case 0:
                                                _b.trys.push([0, 3, , 4]);
                                                // avoid calling stop if start failed
                                                return [4 /*yield*/, this._languageClient.onReady()];
                                            case 1:
                                                // avoid calling stop if start failed
                                                _b.sent();
                                                // remove all listeners
                                                return [4 /*yield*/, this._languageClient.stop()];
                                            case 2:
                                                // remove all listeners
                                                _b.sent();
                                                return [3 /*break*/, 4];
                                            case 3:
                                                _a = _b.sent();
                                                try {
                                                    // if start or stop failed make sure the the connection is closed
                                                    messageConnection.dispose();
                                                }
                                                catch ( /* no-op */_c) { /* no-op */ }
                                                return [3 /*break*/, 4];
                                            case 4: return [2 /*return*/];
                                        }
                                    });
                                }); })(); }));
                                toStop.push(messageConnection.onClose(function () { return _this.forceRestart(); }));
                                _this._languageClient.start();
                                // it should be called after `start` that `onReady` promise gets reinitialized
                                _this.onWillStart(_this._languageClient, toStop);
                            }
                        }, { reconnecting: false });
                        return [3 /*break*/, 5];
                    case 4:
                        e_2 = _a.sent();
                        console.error(e_2);
                        if (!toStop.disposed) {
                            this.forceRestart();
                        }
                        return [3 /*break*/, 5];
                    case 5: return [2 /*return*/];
                }
            });
        });
    };
    Object.defineProperty(BaseLanguageClientContribution.prototype, "running", {
        get: function () {
            return !this.toDeactivate.disposed && this.state === language_client_services_1.State.Running;
        },
        enumerable: true,
        configurable: true
    });
    BaseLanguageClientContribution.prototype.restart = function () {
        if (!this.running) {
            return;
        }
        this.forceRestart();
    };
    BaseLanguageClientContribution.prototype.forceRestart = function () {
        this.deactivate();
        this.activate();
    };
    BaseLanguageClientContribution.prototype.onWillStart = function (languageClient, toStop) {
        var _this = this;
        languageClient.onReady().then(function () { return _this.onReady(languageClient, toStop); });
    };
    BaseLanguageClientContribution.prototype.onReady = function (languageClient, toStop) {
        this._languageClient = languageClient;
        this.resolveReady(this._languageClient);
        this.waitForReady();
    };
    BaseLanguageClientContribution.prototype.waitForReady = function () {
        var _this = this;
        this.ready = new Promise(function (resolve) {
            return _this.resolveReady = resolve;
        });
    };
    BaseLanguageClientContribution.prototype.createLanguageClient = function (connection) {
        var clientOptions = this.createOptions();
        return this.languageClientFactory.get(this, clientOptions, connection);
    };
    BaseLanguageClientContribution.prototype.createOptions = function () {
        var _this = this;
        var _a = this, id = _a.id, documentSelector = _a.documentSelector, fileEvents = _a.fileEvents, configurationSection = _a.configurationSection, initializationOptions = _a.initializationOptions;
        return {
            documentSelector: documentSelector,
            synchronize: { fileEvents: fileEvents, configurationSection: configurationSection },
            initializationFailedHandler: function (err) { return _this.handleInitializationFailed(err); },
            diagnosticCollectionName: id,
            initializationOptions: initializationOptions
        };
    };
    BaseLanguageClientContribution.prototype.handleInitializationFailed = function (err) {
        var _this = this;
        this.deactivate();
        var detail = err instanceof Error ? ": " + err.message : '.';
        this.messageService.error("Failed to start " + this.name + " language server" + detail, 'Retry').then(function (result) {
            if (result) {
                _this.activate();
            }
        });
        return false;
    };
    BaseLanguageClientContribution.prototype.getStartParameters = function () {
        return undefined;
    };
    Object.defineProperty(BaseLanguageClientContribution.prototype, "initializationOptions", {
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        get: function () {
            return undefined;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BaseLanguageClientContribution.prototype, "configurationSection", {
        get: function () {
            return undefined;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BaseLanguageClientContribution.prototype, "workspaceContains", {
        get: function () {
            return [];
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BaseLanguageClientContribution.prototype, "documentSelector", {
        get: function () {
            return [this.id];
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BaseLanguageClientContribution.prototype, "fileEvents", {
        get: function () {
            return this._fileEvents = this._fileEvents || this.createFileEvents();
        },
        enumerable: true,
        configurable: true
    });
    BaseLanguageClientContribution.prototype.createFileEvents = function () {
        var e_3, _a;
        var watchers = [];
        if (this.workspace.createFileSystemWatcher) {
            try {
                for (var _b = __values(this.globPatterns), _c = _b.next(); !_c.done; _c = _b.next()) {
                    var globPattern = _c.value;
                    watchers.push(this.workspace.createFileSystemWatcher(globPattern));
                }
            }
            catch (e_3_1) { e_3 = { error: e_3_1 }; }
            finally {
                try {
                    if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
                }
                finally { if (e_3) throw e_3.error; }
            }
        }
        return watchers;
    };
    Object.defineProperty(BaseLanguageClientContribution.prototype, "globPatterns", {
        get: function () {
            return [];
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Check to see if one of the paths is in the current workspace.
     */
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    BaseLanguageClientContribution.prototype.waitForItemInWorkspace = function () {
        return __awaiter(this, void 0, void 0, function () {
            var doesContain;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.workspaceService.containsSome(this.workspaceContains)];
                    case 1:
                        doesContain = _a.sent();
                        if (!doesContain) {
                            return [2 /*return*/, new Promise(function (resolve) { })];
                        }
                        return [2 /*return*/, doesContain];
                }
            });
        });
    };
    // FIXME move it to the workspace
    BaseLanguageClientContribution.prototype.waitForOpenTextDocument = function (selector) {
        var _this = this;
        var document = this.workspace.textDocuments.filter(function (doc) {
            return _this.languages.match(selector, doc);
        })[0];
        if (document !== undefined) {
            return Promise.resolve(document);
        }
        return new Promise(function (resolve) {
            var disposable = _this.workspace.onDidOpenTextDocument(function (doc) {
                if (_this.languages.match(selector, doc)) {
                    disposable.dispose();
                    resolve(doc);
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(core_1.MessageService),
        __metadata("design:type", core_1.MessageService)
    ], BaseLanguageClientContribution.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(core_1.CommandRegistry),
        __metadata("design:type", core_1.CommandRegistry)
    ], BaseLanguageClientContribution.prototype, "registry", void 0);
    __decorate([
        inversify_1.inject(browser_2.WorkspaceService),
        __metadata("design:type", browser_2.WorkspaceService)
    ], BaseLanguageClientContribution.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(language_client_services_1.LanguageContribution.Service),
        __metadata("design:type", Object)
    ], BaseLanguageClientContribution.prototype, "languageContributionService", void 0);
    __decorate([
        inversify_1.inject(browser_1.WebSocketConnectionProvider),
        __metadata("design:type", browser_1.WebSocketConnectionProvider)
    ], BaseLanguageClientContribution.prototype, "connectionProvider", void 0);
    __decorate([
        inversify_1.inject(langtool_connection_provider_1.LangToolWebSocketConnectionProvider),
        __metadata("design:type", langtool_connection_provider_1.LangToolWebSocketConnectionProvider)
    ], BaseLanguageClientContribution.prototype, "langtoolConnectionProvider", void 0);
    BaseLanguageClientContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(language_client_services_1.Workspace)),
        __param(1, inversify_1.inject(language_client_services_1.Languages)),
        __param(2, inversify_1.inject(language_client_factory_1.LanguageClientFactory)),
        __metadata("design:paramtypes", [Object, Object, language_client_factory_1.LanguageClientFactory])
    ], BaseLanguageClientContribution);
    return BaseLanguageClientContribution;
}());
exports.BaseLanguageClientContribution = BaseLanguageClientContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-client-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-client-factory.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-client-factory.js ***!
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var monaco_languageclient_1 = __webpack_require__(/*! monaco-languageclient */ "../../node_modules/monaco-languageclient/lib/index.js");
var language_client_services_1 = __webpack_require__(/*! ./language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var typehierarchy_feature_1 = __webpack_require__(/*! ./typehierarchy/typehierarchy-feature */ "../../packages/languages/lib/browser/typehierarchy/typehierarchy-feature.js");
var LanguageClientFactory = /** @class */ (function () {
    function LanguageClientFactory(workspace, languages, window) {
        this.workspace = workspace;
        this.languages = languages;
        this.window = window;
        language_client_services_1.Services.install({
            workspace: workspace,
            languages: languages,
            window: window,
            commands: {
                registerCommand: this.registerCommand.bind(this)
            }
        });
    }
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    LanguageClientFactory.prototype.registerCommand = function (id, callback, thisArg) {
        var execute = callback.bind(thisArg);
        return this.registry.registerCommand({ id: id }, { execute: execute });
    };
    LanguageClientFactory.prototype.get = function (contribution, clientOptions, connectionProvider) {
        var _this = this;
        if (clientOptions.revealOutputChannelOn === undefined) {
            clientOptions.revealOutputChannelOn = monaco_languageclient_1.RevealOutputChannelOn.Never;
        }
        if (!clientOptions.errorHandler) {
            clientOptions.errorHandler = {
                // ignore connection errors
                error: function () { return monaco_languageclient_1.ErrorAction.Continue; },
                closed: function () { return monaco_languageclient_1.CloseAction.DoNotRestart; }
            };
        }
        var initializationFailedHandler = clientOptions.initializationFailedHandler;
        clientOptions.initializationFailedHandler = function (e) { return !!initializationFailedHandler && initializationFailedHandler(e); };
        var client = new language_client_services_1.MonacoLanguageClient({
            id: contribution.id,
            name: contribution.name,
            clientOptions: clientOptions,
            connectionProvider: {
                get: function (errorHandler, closeHandler) { return __awaiter(_this, void 0, void 0, function () {
                    var connection, _a;
                    return __generator(this, function (_b) {
                        switch (_b.label) {
                            case 0:
                                if (!(typeof connectionProvider === 'function')) return [3 /*break*/, 2];
                                return [4 /*yield*/, connectionProvider()];
                            case 1:
                                _a = _b.sent();
                                return [3 /*break*/, 3];
                            case 2:
                                _a = connectionProvider;
                                _b.label = 3;
                            case 3:
                                connection = _a;
                                return [2 /*return*/, language_client_services_1.createConnection(connection, errorHandler, closeHandler)];
                        }
                    });
                }); }
            }
        });
        client.registerFeature(new typehierarchy_feature_1.TypeHierarchyFeature(client));
        return this.patch4085(client);
    };
    /**
     * see https://github.com/eclipse-theia/theia/issues/4085
     * remove when monaco-languageclient is upgraded to latest vscode-languageclient
     */
    LanguageClientFactory.prototype.patch4085 = function (client) {
        var e_1, _a;
        var features = client['_dynamicFeatures'];
        var _loop_1 = function (feature) {
            if (feature.dispose) {
                var dispose_1 = feature.dispose.bind(feature);
                feature.dispose = function () {
                    dispose_1();
                    if (feature._listener) {
                        feature._listener = undefined;
                    }
                };
            }
        };
        try {
            for (var _b = __values(features.values()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var feature = _c.value;
                _loop_1(feature);
            }
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_1) throw e_1.error; }
        }
        return client;
    };
    __decorate([
        inversify_1.inject(common_1.CommandRegistry),
        __metadata("design:type", common_1.CommandRegistry)
    ], LanguageClientFactory.prototype, "registry", void 0);
    LanguageClientFactory = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(language_client_services_1.Workspace)),
        __param(1, inversify_1.inject(language_client_services_1.Languages)),
        __param(2, inversify_1.inject(language_client_services_1.Window)),
        __metadata("design:paramtypes", [Object, Object, Object])
    ], LanguageClientFactory);
    return LanguageClientFactory;
}());
exports.LanguageClientFactory = LanguageClientFactory;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-client-factory'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-client-provider-impl.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-client-provider-impl.js ***!
  \***********************************************************************************************************/
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
var language_client_contribution_1 = __webpack_require__(/*! ./language-client-contribution */ "../../packages/languages/lib/browser/language-client-contribution.js");
var LanguageClientProviderImpl = /** @class */ (function () {
    function LanguageClientProviderImpl() {
    }
    LanguageClientProviderImpl.prototype.getLanguageClient = function (languageId) {
        return __awaiter(this, void 0, void 0, function () {
            var contribution;
            return __generator(this, function (_a) {
                contribution = this.getLanguageContribution(languageId);
                if (contribution) {
                    return [2 /*return*/, contribution.languageClient];
                }
                return [2 /*return*/];
            });
        });
    };
    LanguageClientProviderImpl.prototype.getLanguageContribution = function (languageId) {
        var e_1, _a;
        var contributions = this.contributions.getContributions();
        if (contributions) {
            try {
                for (var contributions_1 = __values(contributions), contributions_1_1 = contributions_1.next(); !contributions_1_1.done; contributions_1_1 = contributions_1.next()) {
                    var contribution = contributions_1_1.value;
                    if (contribution.id === languageId) {
                        return contribution;
                    }
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (contributions_1_1 && !contributions_1_1.done && (_a = contributions_1.return)) _a.call(contributions_1);
                }
                finally { if (e_1) throw e_1.error; }
            }
        }
        return undefined;
    };
    __decorate([
        inversify_1.inject(common_1.ContributionProvider), inversify_1.named(language_client_contribution_1.LanguageClientContribution),
        __metadata("design:type", Object)
    ], LanguageClientProviderImpl.prototype, "contributions", void 0);
    LanguageClientProviderImpl = __decorate([
        inversify_1.injectable()
    ], LanguageClientProviderImpl);
    return LanguageClientProviderImpl;
}());
exports.LanguageClientProviderImpl = LanguageClientProviderImpl;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-client-provider-impl'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-client-provider.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-client-provider.js ***!
  \******************************************************************************************************/
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
exports.LanguageClientProvider = Symbol('LanguageClientProvider');

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-client-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-client-services.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-client-services.js ***!
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
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
__export(__webpack_require__(/*! monaco-languageclient */ "../../node_modules/monaco-languageclient/lib/index.js"));
__export(__webpack_require__(/*! ../common */ "../../packages/languages/lib/common/index.js"));
exports.Languages = Symbol('Languages');
exports.Workspace = Symbol('Workspace');
exports.Commands = Symbol('Commands');
exports.Window = Symbol('Window');
exports.IConnectionProvider = Symbol('IConnectionProvider');
exports.ILanguageClient = Symbol('ILanguageClient');

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-client-services'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-dummy-service.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-dummy-service.js ***!
  \****************************************************************************************************/
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var LanguageDummyService = /** @class */ (function () {
    function LanguageDummyService() {
        this.nextId = 1;
    }
    LanguageDummyService.prototype.create = function (contributionId, parameters) {
        return __awaiter(this, void 0, void 0, function () {
            var id, sessionId;
            return __generator(this, function (_a) {
                id = this.nextId;
                this.nextId++;
                sessionId = String(id);
                console.log('Creating a dummy language service ' + sessionId);
                return [2 /*return*/, sessionId];
            });
        });
    };
    LanguageDummyService.prototype.destroy = function (sessionId) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                console.log('Deleting a dummy language service ' + sessionId);
                return [2 /*return*/];
            });
        });
    };
    LanguageDummyService = __decorate([
        inversify_1.injectable()
    ], LanguageDummyService);
    return LanguageDummyService;
}());
exports.LanguageDummyService = LanguageDummyService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-dummy-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/language-resource-context-key.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/language-resource-context-key.js ***!
  \***********************************************************************************************************/
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
var resource_context_key_1 = __webpack_require__(/*! @theia/core/lib/browser/resource-context-key */ "../../packages/core/lib/browser/resource-context-key.js");
var language_client_services_1 = __webpack_require__(/*! ./language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var LanguageResourceContextKey = /** @class */ (function (_super) {
    __extends(LanguageResourceContextKey, _super);
    function LanguageResourceContextKey() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    LanguageResourceContextKey.prototype.getLanguageId = function (uri) {
        var e_1, _a;
        var languages = this.languages.languages;
        if (uri && languages) {
            try {
                for (var languages_1 = __values(languages), languages_1_1 = languages_1.next(); !languages_1_1.done; languages_1_1 = languages_1.next()) {
                    var language = languages_1_1.value;
                    if (language.extensions.has(uri.path.ext)) {
                        return language.id;
                    }
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (languages_1_1 && !languages_1_1.done && (_a = languages_1.return)) _a.call(languages_1);
                }
                finally { if (e_1) throw e_1.error; }
            }
        }
        return undefined;
    };
    __decorate([
        inversify_1.inject(language_client_services_1.Languages),
        __metadata("design:type", Object)
    ], LanguageResourceContextKey.prototype, "languages", void 0);
    LanguageResourceContextKey = __decorate([
        inversify_1.injectable()
    ], LanguageResourceContextKey);
    return LanguageResourceContextKey;
}(resource_context_key_1.ResourceContextKey));
exports.LanguageResourceContextKey = LanguageResourceContextKey;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/language-resource-context-key'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/languages-frontend-contribution.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/languages-frontend-contribution.js ***!
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var language_client_contribution_1 = __webpack_require__(/*! ./language-client-contribution */ "../../packages/languages/lib/browser/language-client-contribution.js");
var LanguagesFrontendContribution = /** @class */ (function () {
    function LanguagesFrontendContribution() {
    }
    LanguagesFrontendContribution.prototype.onStart = function (app) {
        var e_1, _a;
        var schema = {
            type: 'object',
            properties: {}
        };
        var _loop_1 = function (contribution) {
            contribution.waitForActivation(app).then(function () {
                return contribution.activate(app);
            });
            schema.properties[contribution.id + ".trace.server"] = {
                type: 'string',
                enum: [
                    'off',
                    'messages',
                    'verbose'
                ],
                default: 'off',
                description: "Enable/disable tracing communications with the " + contribution.name + " language server"
            };
        };
        try {
            for (var _b = __values(this.contributions.getContributions()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var contribution = _c.value;
                _loop_1(contribution);
            }
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_1) throw e_1.error; }
        }
        this.preferenceSchema.setSchema(schema);
    };
    LanguagesFrontendContribution.prototype.registerCommands = function (commands) {
        var e_2, _a;
        var _this = this;
        var _loop_2 = function (contribution) {
            commands.registerCommand({
                id: contribution.id + ".server.start",
                label: contribution.name + ": Start Language Server"
            }, {
                execute: function () { return contribution.activate(_this.app); },
                isEnabled: function () { return !contribution.running; },
                isVisible: function () { return !contribution.running; },
            });
            commands.registerCommand({
                id: contribution.id + ".server.stop",
                label: contribution.name + ": Stop Language Server"
            }, {
                execute: function () { return contribution.deactivate(); },
                isEnabled: function () { return contribution.running; },
                isVisible: function () { return contribution.running; },
            });
            commands.registerCommand({
                id: contribution.id + ".server.restart",
                label: contribution.name + ": Restart Language Server"
            }, {
                execute: function () { return contribution.restart(); },
                isEnabled: function () { return contribution.running; },
                isVisible: function () { return contribution.running; },
            });
        };
        try {
            for (var _b = __values(this.contributions.getContributions()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var contribution = _c.value;
                _loop_2(contribution);
            }
        }
        catch (e_2_1) { e_2 = { error: e_2_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_2) throw e_2.error; }
        }
    };
    __decorate([
        inversify_1.inject(browser_1.FrontendApplication),
        __metadata("design:type", browser_1.FrontendApplication)
    ], LanguagesFrontendContribution.prototype, "app", void 0);
    __decorate([
        inversify_1.inject(browser_1.PreferenceSchemaProvider),
        __metadata("design:type", browser_1.PreferenceSchemaProvider)
    ], LanguagesFrontendContribution.prototype, "preferenceSchema", void 0);
    __decorate([
        inversify_1.inject(common_1.ContributionProvider), inversify_1.named(language_client_contribution_1.LanguageClientContribution),
        __metadata("design:type", Object)
    ], LanguagesFrontendContribution.prototype, "contributions", void 0);
    LanguagesFrontendContribution = __decorate([
        inversify_1.injectable()
    ], LanguagesFrontendContribution);
    return LanguagesFrontendContribution;
}());
exports.LanguagesFrontendContribution = LanguagesFrontendContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/languages-frontend-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/languages-frontend-module.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/languages-frontend-module.js ***!
  \*******************************************************************************************************/
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
var resource_context_key_1 = __webpack_require__(/*! @theia/core/lib/browser/resource-context-key */ "../../packages/core/lib/browser/resource-context-key.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var language_client_services_1 = __webpack_require__(/*! ./language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var window_impl_1 = __webpack_require__(/*! ./window-impl */ "../../packages/languages/lib/browser/window-impl.js");
var language_client_factory_1 = __webpack_require__(/*! ./language-client-factory */ "../../packages/languages/lib/browser/language-client-factory.js");
var languages_frontend_contribution_1 = __webpack_require__(/*! ./languages-frontend-contribution */ "../../packages/languages/lib/browser/languages-frontend-contribution.js");
var language_client_contribution_1 = __webpack_require__(/*! ./language-client-contribution */ "../../packages/languages/lib/browser/language-client-contribution.js");
var workspace_symbols_1 = __webpack_require__(/*! ./workspace-symbols */ "../../packages/languages/lib/browser/workspace-symbols.js");
var language_client_provider_1 = __webpack_require__(/*! ./language-client-provider */ "../../packages/languages/lib/browser/language-client-provider.js");
var language_client_provider_impl_1 = __webpack_require__(/*! ./language-client-provider-impl */ "../../packages/languages/lib/browser/language-client-provider-impl.js");
var common_2 = __webpack_require__(/*! ../common */ "../../packages/languages/lib/common/index.js");
var language_resource_context_key_1 = __webpack_require__(/*! ./language-resource-context-key */ "../../packages/languages/lib/browser/language-resource-context-key.js");
var language_dummy_service_1 = __webpack_require__(/*! ./language-dummy-service */ "../../packages/languages/lib/browser/language-dummy-service.js");
var langtool_connection_provider_1 = __webpack_require__(/*! ./langtool-connection-provider */ "../../packages/languages/lib/browser/langtool-connection-provider.js");
exports.default = new inversify_1.ContainerModule(function (bind, unbind, isBound, rebind) {
    var e_1, _a;
    bind(language_client_services_1.Window).to(window_impl_1.WindowImpl).inSingletonScope();
    bind(language_client_factory_1.LanguageClientFactory).toSelf().inSingletonScope();
    // bind(LanguageContribution.Service).toDynamicValue(({ container }) =>
    //     WebSocketConnectionProvider.createProxy(container, LanguageContribution.servicePath)
    // ).inSingletonScope();
    bind(common_2.LanguageContribution.Service).to(language_dummy_service_1.LanguageDummyService).inSingletonScope();
    common_1.bindContributionProvider(bind, language_client_contribution_1.LanguageClientContribution);
    bind(languages_frontend_contribution_1.LanguagesFrontendContribution).toSelf().inSingletonScope();
    bind(browser_1.FrontendApplicationContribution).toService(languages_frontend_contribution_1.LanguagesFrontendContribution);
    bind(common_1.CommandContribution).toService(languages_frontend_contribution_1.LanguagesFrontendContribution);
    bind(workspace_symbols_1.WorkspaceSymbolCommand).toSelf().inSingletonScope();
    try {
        for (var _b = __values([common_1.CommandContribution, browser_1.KeybindingContribution, browser_1.QuickOpenContribution]), _c = _b.next(); !_c.done; _c = _b.next()) {
            var identifier = _c.value;
            bind(identifier).toService(workspace_symbols_1.WorkspaceSymbolCommand);
        }
    }
    catch (e_1_1) { e_1 = { error: e_1_1 }; }
    finally {
        try {
            if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
        }
        finally { if (e_1) throw e_1.error; }
    }
    bind(language_client_provider_impl_1.LanguageClientProviderImpl).toSelf().inSingletonScope();
    bind(language_client_provider_1.LanguageClientProvider).toService(language_client_provider_impl_1.LanguageClientProviderImpl);
    bind(language_resource_context_key_1.LanguageResourceContextKey).toSelf().inSingletonScope();
    rebind(resource_context_key_1.ResourceContextKey).to(language_resource_context_key_1.LanguageResourceContextKey).inSingletonScope();
    bind(langtool_connection_provider_1.LangToolWebSocketConnectionProvider).toSelf().inSingletonScope();
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/languages-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/typehierarchy/typehierarchy-feature.js":
/*!*****************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/typehierarchy/typehierarchy-feature.js ***!
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
var uuid_1 = __webpack_require__(/*! uuid */ "../../node_modules/uuid/index.js");
var language_client_services_1 = __webpack_require__(/*! ../language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var typehierarchy_protocol_1 = __webpack_require__(/*! ./typehierarchy-protocol */ "../../packages/languages/lib/browser/typehierarchy/typehierarchy-protocol.js");
// NOTE: This module can be removed, or at least can be simplified once the type hierarchy will become the part of the LSP.
// https://github.com/Microsoft/language-server-protocol/issues/582
// https://github.com/Microsoft/vscode-languageserver-node/pull/346#discussion_r221659062
/**
 * Text document feature for handling super- and subtype hierarchies through the LSP.
 */
var TypeHierarchyFeature = /** @class */ (function (_super) {
    __extends(TypeHierarchyFeature, _super);
    function TypeHierarchyFeature(client) {
        var _this = _super.call(this, client, typehierarchy_protocol_1.TypeHierarchyRequest.type) || this;
        _this.client = client;
        return _this;
    }
    TypeHierarchyFeature.prototype.fillClientCapabilities = function (capabilities) {
        if (!capabilities.textDocument) {
            capabilities.textDocument = {};
        }
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        capabilities.textDocument.typeHierarchy = {
            dynamicRegistration: true
        };
    };
    TypeHierarchyFeature.prototype.initialize = function (capabilities, documentSelector) {
        if (!documentSelector) {
            return;
        }
        var capabilitiesExt = capabilities;
        if (capabilitiesExt.typeHierarchyProvider) {
            var id = uuid_1.v4();
            this.register(this.messages, {
                id: id,
                registerOptions: Object.assign({}, { documentSelector: documentSelector }, capabilitiesExt.typeHierarchyProvider)
            });
        }
    };
    TypeHierarchyFeature.prototype.registerLanguageProvider = function () {
        return language_client_services_1.Disposable.create(function () { });
    };
    return TypeHierarchyFeature;
}(language_client_services_1.TextDocumentFeature));
exports.TypeHierarchyFeature = TypeHierarchyFeature;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/typehierarchy/typehierarchy-feature'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/typehierarchy/typehierarchy-protocol.js":
/*!******************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/typehierarchy/typehierarchy-protocol.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var language_client_services_1 = __webpack_require__(/*! ../language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var TypeHierarchyDirection;
(function (TypeHierarchyDirection) {
    /**
     * Flag for retrieving/resolving the subtypes.
     */
    TypeHierarchyDirection.Children = 0;
    /**
     * Flag to use when retrieving/resolving the supertypes.
     */
    TypeHierarchyDirection.Parents = 1;
    /**
     * Flag for resolving both the super- and subtypes.
     */
    TypeHierarchyDirection.Both = 2;
})(TypeHierarchyDirection = exports.TypeHierarchyDirection || (exports.TypeHierarchyDirection = {}));
/**
 * The `textDocument/typeHierarchy` request is sent from the client to the server to retrieve the type hierarchy
 * items from a given position of a text document. Can resolve the parentage information on demand.
 * If no item can be retrieved for a given text document position, returns with `null`.
 */
var TypeHierarchyRequest;
(function (TypeHierarchyRequest) {
    TypeHierarchyRequest.type = new language_client_services_1.RequestType('textDocument/typeHierarchy');
})(TypeHierarchyRequest = exports.TypeHierarchyRequest || (exports.TypeHierarchyRequest = {}));
/**
 * The `typeHierarchy/resolve` request is sent from the client to the server to resolve a type hierarchy
 * item by resolving sub- and supertype information.
 */
var ResolveTypeHierarchyRequest;
(function (ResolveTypeHierarchyRequest) {
    ResolveTypeHierarchyRequest.type = new language_client_services_1.RequestType('typeHierarchy/resolve');
})(ResolveTypeHierarchyRequest = exports.ResolveTypeHierarchyRequest || (exports.ResolveTypeHierarchyRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/typehierarchy/typehierarchy-protocol'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/window-impl.js":
/*!*****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/window-impl.js ***!
  \*****************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var services_1 = __webpack_require__(/*! monaco-languageclient/lib/services */ "../../node_modules/monaco-languageclient/lib/services.js");
var output_channel_1 = __webpack_require__(/*! @theia/output/lib/common/output-channel */ "../../packages/output/lib/common/output-channel.js");
var output_contribution_1 = __webpack_require__(/*! @theia/output/lib/browser/output-contribution */ "../../packages/output/lib/browser/output-contribution.js");
var WindowImpl = /** @class */ (function () {
    function WindowImpl() {
    }
    WindowImpl.prototype.showMessage = function (type, message) {
        var _a, _b, _c, _d;
        var actions = [];
        for (var _i = 2; _i < arguments.length; _i++) {
            actions[_i - 2] = arguments[_i];
        }
        var originalActions = new Map((actions || []).map(function (action) { return [action.title, action]; }));
        var actionTitles = (actions || []).map(function (action) { return action.title; });
        var mapActionType = function (result) {
            if (!!result) {
                return originalActions.get(result);
            }
            return undefined;
        };
        if (type === services_1.MessageType.Error) {
            return (_a = this.messageService).error.apply(_a, __spread([message], actionTitles)).then(mapActionType);
        }
        if (type === services_1.MessageType.Warning) {
            return (_b = this.messageService).warn.apply(_b, __spread([message], actionTitles)).then(mapActionType);
        }
        if (type === services_1.MessageType.Info) {
            return (_c = this.messageService).info.apply(_c, __spread([message], actionTitles)).then(mapActionType);
        }
        if (type === services_1.MessageType.Log) {
            return (_d = this.messageService).log.apply(_d, __spread([message], actionTitles)).then(mapActionType);
        }
        return Promise.resolve(undefined);
    };
    WindowImpl.prototype.createOutputChannel = function (name) {
        var _this = this;
        var outputChannel = this.outputChannelManager.getChannel(name);
        return {
            append: outputChannel.append.bind(outputChannel),
            appendLine: outputChannel.appendLine.bind(outputChannel),
            show: function (preserveFocus) { return __awaiter(_this, void 0, void 0, function () {
                var options, activate, reveal;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            options = Object.assign({
                                preserveFocus: false,
                            }, { preserveFocus: preserveFocus });
                            activate = !options.preserveFocus;
                            reveal = options.preserveFocus;
                            return [4 /*yield*/, this.outputContribution.openView({ activate: activate, reveal: reveal })];
                        case 1:
                            _a.sent();
                            outputChannel.setVisibility(true);
                            return [2 /*return*/];
                    }
                });
            }); },
            dispose: function () {
                _this.outputChannelManager.deleteChannel(outputChannel.name);
            }
        };
    };
    __decorate([
        inversify_1.inject(common_1.MessageService),
        __metadata("design:type", common_1.MessageService)
    ], WindowImpl.prototype, "messageService", void 0);
    __decorate([
        inversify_1.inject(output_channel_1.OutputChannelManager),
        __metadata("design:type", output_channel_1.OutputChannelManager)
    ], WindowImpl.prototype, "outputChannelManager", void 0);
    __decorate([
        inversify_1.inject(output_contribution_1.OutputContribution),
        __metadata("design:type", output_contribution_1.OutputContribution)
    ], WindowImpl.prototype, "outputContribution", void 0);
    WindowImpl = __decorate([
        inversify_1.injectable()
    ], WindowImpl);
    return WindowImpl;
}());
exports.WindowImpl = WindowImpl;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/window-impl'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/browser/workspace-symbols.js":
/*!***********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/browser/workspace-symbols.js ***!
  \***********************************************************************************************/
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
var environment_1 = __webpack_require__(/*! @theia/application-package/lib/environment */ "../../dev-packages/application-package/lib/environment.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var language_client_services_1 = __webpack_require__(/*! ./language-client-services */ "../../packages/languages/lib/browser/language-client-services.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var vscode_languageserver_types_1 = __webpack_require__(/*! vscode-languageserver-types */ "../../node_modules/vscode-languageserver-types/lib/esm/main.js");
var WorkspaceSymbolCommand = /** @class */ (function () {
    function WorkspaceSymbolCommand(languages, openerService, quickOpenService, selectionService) {
        this.languages = languages;
        this.openerService = openerService;
        this.quickOpenService = quickOpenService;
        this.selectionService = selectionService;
        this.prefix = '#';
        this.description = 'Go to Symbol in Workspace';
        this.command = {
            id: 'languages.workspace.symbol',
            label: 'Open Workspace Symbol...'
        };
        this.cancellationSource = new core_1.CancellationTokenSource();
    }
    WorkspaceSymbolCommand.prototype.isEnabled = function () {
        return this.languages.workspaceSymbolProviders !== undefined;
    };
    WorkspaceSymbolCommand.prototype.execute = function () {
        this.quickOpenService.open(this.prefix);
    };
    WorkspaceSymbolCommand.prototype.getModel = function () {
        return this;
    };
    WorkspaceSymbolCommand.prototype.getOptions = function () {
        var _this = this;
        return {
            fuzzyMatchLabel: true,
            showItemsWithoutHighlight: true,
            onClose: function () {
                _this.cancellationSource.cancel();
            }
        };
    };
    WorkspaceSymbolCommand.prototype.registerCommands = function (commands) {
        commands.registerCommand(this.command, this);
    };
    WorkspaceSymbolCommand.prototype.isElectron = function () {
        return environment_1.environment.electron.is();
    };
    WorkspaceSymbolCommand.prototype.registerKeybindings = function (keybindings) {
        keybindings.registerKeybinding({
            command: this.command.id,
            keybinding: this.isElectron() ? 'ctrlcmd+t' : 'ctrlcmd+o',
        });
    };
    WorkspaceSymbolCommand.prototype.registerQuickOpenHandlers = function (handlers) {
        handlers.registerHandler(this);
    };
    WorkspaceSymbolCommand.prototype.onType = function (lookFor, acceptor) {
        return __awaiter(this, void 0, void 0, function () {
            var newCancellationSource_1, param_1, items_1, workspaceProviderPromises, _loop_1, _a, _b, provider;
            var e_1, _c;
            var _this = this;
            return __generator(this, function (_d) {
                if (this.languages.workspaceSymbolProviders) {
                    this.cancellationSource.cancel();
                    newCancellationSource_1 = new core_1.CancellationTokenSource();
                    this.cancellationSource = newCancellationSource_1;
                    param_1 = {
                        query: lookFor
                    };
                    items_1 = [];
                    workspaceProviderPromises = [];
                    _loop_1 = function (provider) {
                        workspaceProviderPromises.push((function () { return __awaiter(_this, void 0, void 0, function () {
                            var symbols, symbols_1, symbols_1_1, symbol;
                            var e_2, _a;
                            return __generator(this, function (_b) {
                                switch (_b.label) {
                                    case 0: return [4 /*yield*/, provider.provideWorkspaceSymbols(param_1, newCancellationSource_1.token)];
                                    case 1:
                                        symbols = _b.sent();
                                        if (symbols && !newCancellationSource_1.token.isCancellationRequested) {
                                            try {
                                                for (symbols_1 = __values(symbols), symbols_1_1 = symbols_1.next(); !symbols_1_1.done; symbols_1_1 = symbols_1.next()) {
                                                    symbol = symbols_1_1.value;
                                                    items_1.push(this.createItem(symbol, provider, newCancellationSource_1.token));
                                                }
                                            }
                                            catch (e_2_1) { e_2 = { error: e_2_1 }; }
                                            finally {
                                                try {
                                                    if (symbols_1_1 && !symbols_1_1.done && (_a = symbols_1.return)) _a.call(symbols_1);
                                                }
                                                finally { if (e_2) throw e_2.error; }
                                            }
                                            acceptor(items_1);
                                        }
                                        return [2 /*return*/, symbols];
                                }
                            });
                        }); })());
                    };
                    try {
                        for (_a = __values(this.languages.workspaceSymbolProviders), _b = _a.next(); !_b.done; _b = _a.next()) {
                            provider = _b.value;
                            _loop_1(provider);
                        }
                    }
                    catch (e_1_1) { e_1 = { error: e_1_1 }; }
                    finally {
                        try {
                            if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                        }
                        finally { if (e_1) throw e_1.error; }
                    }
                    Promise.all(workspaceProviderPromises.map(function (p) { return p.then(function (sym) { return sym; }, function (_) { return undefined; }); })).then(function (symbols) {
                        var filteredSymbols = symbols.filter(function (el) { return el && el.length !== 0; });
                        if (filteredSymbols.length === 0) {
                            items_1.push(new browser_1.QuickOpenItem({
                                label: lookFor.length === 0 ? 'Type to search for symbols' : 'No symbols matching',
                                run: function () { return false; }
                            }));
                            acceptor(items_1);
                        }
                    }).catch();
                }
                return [2 /*return*/];
            });
        });
    };
    WorkspaceSymbolCommand.prototype.createItem = function (sym, provider, token) {
        var _this = this;
        var uri = new uri_1.default(sym.location.uri);
        var kind = SymbolKind[sym.kind];
        var icon = (kind) ? SymbolKind[sym.kind].toLowerCase() : 'unknown';
        var parent = sym.containerName;
        if (parent) {
            parent += ' - ';
        }
        parent = (parent || '') + uri.displayName;
        return new SimpleOpenItem(sym.name, icon, parent, uri.toString(), function () {
            if (provider.resolveWorkspaceSymbol) {
                provider.resolveWorkspaceSymbol(sym, token).then(function (resolvedSymbol) {
                    if (resolvedSymbol) {
                        _this.openURL(uri, resolvedSymbol.location.range.start, resolvedSymbol.location.range.end);
                    }
                    else {
                        // the symbol didn't resolve -> use given symbol
                        _this.openURL(uri, sym.location.range.start, sym.location.range.end);
                    }
                });
            }
            else {
                // resolveWorkspaceSymbol wasn't specified
                _this.openURL(uri, sym.location.range.start, sym.location.range.end);
            }
        });
    };
    WorkspaceSymbolCommand.prototype.openURL = function (uri, start, end) {
        this.openerService.getOpener(uri).then(function (opener) { return opener.open(uri, {
            selection: vscode_languageserver_types_1.Range.create(start, end)
        }); });
    };
    WorkspaceSymbolCommand = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(language_client_services_1.Languages)),
        __param(1, inversify_1.inject(browser_1.OpenerService)),
        __param(2, inversify_1.inject(browser_1.PrefixQuickOpenService)),
        __param(3, inversify_1.inject(core_1.SelectionService)),
        __metadata("design:paramtypes", [Object, Object, browser_1.PrefixQuickOpenService,
            core_1.SelectionService])
    ], WorkspaceSymbolCommand);
    return WorkspaceSymbolCommand;
}());
exports.WorkspaceSymbolCommand = WorkspaceSymbolCommand;
var SimpleOpenItem = /** @class */ (function (_super) {
    __extends(SimpleOpenItem, _super);
    function SimpleOpenItem(label, icon, parent, toolTip, onOpen, onSelect) {
        var _this = _super.call(this) || this;
        _this.label = label;
        _this.icon = icon;
        _this.parent = parent;
        _this.toolTip = toolTip;
        _this.onOpen = onOpen;
        _this.onSelect = onSelect;
        return _this;
    }
    SimpleOpenItem.prototype.getLabel = function () {
        return this.label;
    };
    SimpleOpenItem.prototype.isHidden = function () {
        return false;
    };
    SimpleOpenItem.prototype.getTooltip = function () {
        return this.toolTip;
    };
    SimpleOpenItem.prototype.getDescription = function () {
        return this.parent;
    };
    SimpleOpenItem.prototype.getIconClass = function () {
        return this.icon;
    };
    SimpleOpenItem.prototype.run = function (mode) {
        if (mode !== browser_1.QuickOpenMode.OPEN) {
            if (!this.onSelect) {
                return false;
            }
            this.onSelect();
            return true;
        }
        this.onOpen();
        return true;
    };
    return SimpleOpenItem;
}(browser_1.QuickOpenItem));
var SymbolKind;
(function (SymbolKind) {
    SymbolKind[SymbolKind["File"] = 1] = "File";
    SymbolKind[SymbolKind["Module"] = 2] = "Module";
    SymbolKind[SymbolKind["Namespace"] = 3] = "Namespace";
    SymbolKind[SymbolKind["Package"] = 4] = "Package";
    SymbolKind[SymbolKind["Class"] = 5] = "Class";
    SymbolKind[SymbolKind["Method"] = 6] = "Method";
    SymbolKind[SymbolKind["Property"] = 7] = "Property";
    SymbolKind[SymbolKind["Field"] = 8] = "Field";
    SymbolKind[SymbolKind["Constructor"] = 9] = "Constructor";
    SymbolKind[SymbolKind["Enum"] = 10] = "Enum";
    SymbolKind[SymbolKind["Interface"] = 11] = "Interface";
    SymbolKind[SymbolKind["Function"] = 12] = "Function";
    SymbolKind[SymbolKind["Variable"] = 13] = "Variable";
    SymbolKind[SymbolKind["Constant"] = 14] = "Constant";
    SymbolKind[SymbolKind["String"] = 15] = "String";
    SymbolKind[SymbolKind["Number"] = 16] = "Number";
    SymbolKind[SymbolKind["Boolean"] = 17] = "Boolean";
    SymbolKind[SymbolKind["Array"] = 18] = "Array";
    SymbolKind[SymbolKind["Object"] = 19] = "Object";
    SymbolKind[SymbolKind["Key"] = 20] = "Key";
    SymbolKind[SymbolKind["Null"] = 21] = "Null";
    SymbolKind[SymbolKind["EnumMember"] = 22] = "EnumMember";
    SymbolKind[SymbolKind["Struct"] = 23] = "Struct";
    SymbolKind[SymbolKind["Event"] = 24] = "Event";
    SymbolKind[SymbolKind["Operator"] = 25] = "Operator";
    SymbolKind[SymbolKind["TypeParameter"] = 26] = "TypeParameter";
})(SymbolKind || (SymbolKind = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/browser/workspace-symbols'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/common/index.js":
/*!**********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/common/index.js ***!
  \**********************************************************************************/
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
__export(__webpack_require__(/*! ./language-contribution */ "../../packages/languages/lib/common/language-contribution.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/common'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/languages/lib/common/language-contribution.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/languages/lib/common/language-contribution.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var LanguageContribution;
(function (LanguageContribution) {
    LanguageContribution.servicePath = '/services/languages';
    function getPath(contribution, sessionId) {
        if (sessionId === void 0) { sessionId = ':id'; }
        return LanguageContribution.servicePath + '/' + contribution.id + '/' + sessionId;
    }
    LanguageContribution.getPath = getPath;
    LanguageContribution.Service = Symbol('LanguageContributionService');
})(LanguageContribution = exports.LanguageContribution || (exports.LanguageContribution = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/languages/lib/common/language-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/workspace/lib/browser/index.js":
/*!***********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/workspace/lib/browser/index.js ***!
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
__export(__webpack_require__(/*! ./workspace-commands */ "../../packages/workspace/lib/browser/workspace-commands.js"));
__export(__webpack_require__(/*! ./workspace-service */ "../../packages/workspace/lib/browser/workspace-service.js"));
__export(__webpack_require__(/*! ./workspace-frontend-contribution */ "../../packages/workspace/lib/browser/workspace-frontend-contribution.js"));
__export(__webpack_require__(/*! ./workspace-frontend-module */ "../../packages/workspace/lib/browser/workspace-frontend-module.js"));
__export(__webpack_require__(/*! ./workspace-preferences */ "../../packages/workspace/lib/browser/workspace-preferences.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/workspace/lib/browser'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=14.bundle.js.map