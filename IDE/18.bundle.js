(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[18],{

/***/ "../../packages/navigator/lib/browser/navigator-context-key-service.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-context-key-service.js ***!
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
var context_key_service_1 = __webpack_require__(/*! @theia/core/lib/browser/context-key-service */ "../../packages/core/lib/browser/context-key-service.js");
var NavigatorContextKeyService = /** @class */ (function () {
    function NavigatorContextKeyService() {
    }
    Object.defineProperty(NavigatorContextKeyService.prototype, "explorerViewletVisible", {
        get: function () {
            return this._explorerViewletVisible;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NavigatorContextKeyService.prototype, "explorerViewletFocus", {
        /** True if Explorer view has keyboard focus. */
        get: function () {
            return this._explorerViewletFocus;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NavigatorContextKeyService.prototype, "filesExplorerFocus", {
        /** True if File Explorer section has keyboard focus. */
        get: function () {
            return this._filesExplorerFocus;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NavigatorContextKeyService.prototype, "explorerResourceIsFolder", {
        get: function () {
            return this._explorerResourceIsFolder;
        },
        enumerable: true,
        configurable: true
    });
    NavigatorContextKeyService.prototype.init = function () {
        this._explorerViewletVisible = this.contextKeyService.createKey('explorerViewletVisible', false);
        this._explorerViewletFocus = this.contextKeyService.createKey('explorerViewletFocus', false);
        this._filesExplorerFocus = this.contextKeyService.createKey('filesExplorerFocus', false);
        this._explorerResourceIsFolder = this.contextKeyService.createKey('explorerResourceIsFolder', false);
    };
    __decorate([
        inversify_1.inject(context_key_service_1.ContextKeyService),
        __metadata("design:type", context_key_service_1.ContextKeyService)
    ], NavigatorContextKeyService.prototype, "contextKeyService", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], NavigatorContextKeyService.prototype, "init", null);
    NavigatorContextKeyService = __decorate([
        inversify_1.injectable()
    ], NavigatorContextKeyService);
    return NavigatorContextKeyService;
}());
exports.NavigatorContextKeyService = NavigatorContextKeyService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-context-key-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-contribution.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-contribution.js ***!
  \****************************************************************************************************/
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
var view_contribution_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/view-contribution */ "../../packages/core/lib/browser/shell/view-contribution.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var file_download_command_contribution_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/download/file-download-command-contribution */ "../../packages/filesystem/lib/browser/download/file-download-command-contribution.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var browser_3 = __webpack_require__(/*! @theia/workspace/lib/browser */ "../../packages/workspace/lib/browser/index.js");
var navigator_widget_1 = __webpack_require__(/*! ./navigator-widget */ "../../packages/navigator/lib/browser/navigator-widget.js");
var navigator_preferences_1 = __webpack_require__(/*! ./navigator-preferences */ "../../packages/navigator/lib/browser/navigator-preferences.js");
var navigator_keybinding_context_1 = __webpack_require__(/*! ./navigator-keybinding-context */ "../../packages/navigator/lib/browser/navigator-keybinding-context.js");
var navigator_filter_1 = __webpack_require__(/*! ./navigator-filter */ "../../packages/navigator/lib/browser/navigator-filter.js");
var navigator_tree_1 = __webpack_require__(/*! ./navigator-tree */ "../../packages/navigator/lib/browser/navigator-tree.js");
var navigator_context_key_service_1 = __webpack_require__(/*! ./navigator-context-key-service */ "../../packages/navigator/lib/browser/navigator-context-key-service.js");
var tab_bar_toolbar_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-toolbar */ "../../packages/core/lib/browser/shell/tab-bar-toolbar.js");
var filesystem_frontend_contribution_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/filesystem-frontend-contribution */ "../../packages/filesystem/lib/browser/filesystem-frontend-contribution.js");
var navigator_diff_1 = __webpack_require__(/*! ./navigator-diff */ "../../packages/navigator/lib/browser/navigator-diff.js");
var selection_1 = __webpack_require__(/*! @theia/core/lib/common/selection */ "../../packages/core/lib/common/selection.js");
var browser_4 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var FileNavigatorCommands;
(function (FileNavigatorCommands) {
    FileNavigatorCommands.REVEAL_IN_NAVIGATOR = {
        id: 'navigator.reveal',
        label: 'Reveal in Explorer'
    };
    FileNavigatorCommands.TOGGLE_HIDDEN_FILES = {
        id: 'navigator.toggle.hidden.files',
        label: 'Toggle Hidden Files'
    };
    FileNavigatorCommands.TOGGLE_AUTO_REVEAL = {
        id: 'navigator.toggle.autoReveal',
        category: 'File',
        label: 'Auto Reveal'
    };
    FileNavigatorCommands.REFRESH_NAVIGATOR = {
        id: 'navigator.refresh',
        category: 'File',
        label: 'Refresh in Explorer',
        iconClass: 'refresh'
    };
    FileNavigatorCommands.COLLAPSE_ALL = {
        id: 'navigator.collapse.all',
        category: 'File',
        label: 'Collapse Folders in Explorer',
        iconClass: 'theia-collapse-all-icon'
    };
    FileNavigatorCommands.ADD_ROOT_FOLDER = {
        id: 'navigator.addRootFolder'
    };
})(FileNavigatorCommands = exports.FileNavigatorCommands || (exports.FileNavigatorCommands = {}));
/**
 * Navigator `More Actions...` toolbar item groups.
 * Used in order to group items present in the toolbar.
 */
var NavigatorMoreToolbarGroups;
(function (NavigatorMoreToolbarGroups) {
    NavigatorMoreToolbarGroups.NEW_OPEN = '1_navigator_new_open';
    NavigatorMoreToolbarGroups.TOOLS = '2_navigator_tools';
    NavigatorMoreToolbarGroups.WORKSPACE = '3_navigator_workspace';
})(NavigatorMoreToolbarGroups = exports.NavigatorMoreToolbarGroups || (exports.NavigatorMoreToolbarGroups = {}));
exports.NAVIGATOR_CONTEXT_MENU = ['navigator-context-menu'];
/**
 * Navigator context menu default groups should be aligned
 * with VS Code default groups: https://code.visualstudio.com/api/references/contribution-points#contributes.menus
 */
var NavigatorContextMenu;
(function (NavigatorContextMenu) {
    NavigatorContextMenu.NAVIGATION = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['navigation']);
    /** @deprecated use NAVIGATION */
    NavigatorContextMenu.OPEN = NavigatorContextMenu.NAVIGATION;
    /** @deprecated use NAVIGATION */
    NavigatorContextMenu.NEW = NavigatorContextMenu.NAVIGATION;
    NavigatorContextMenu.WORKSPACE = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['2_workspace']);
    NavigatorContextMenu.COMPARE = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['3_compare']);
    /** @deprecated use COMPARE */
    NavigatorContextMenu.DIFF = NavigatorContextMenu.COMPARE;
    NavigatorContextMenu.SEARCH = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['4_search']);
    NavigatorContextMenu.CLIPBOARD = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['5_cutcopypaste']);
    NavigatorContextMenu.MODIFICATION = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['7_modification']);
    /** @deprecated use MODIFICATION */
    NavigatorContextMenu.MOVE = NavigatorContextMenu.MODIFICATION;
    /** @deprecated use MODIFICATION */
    NavigatorContextMenu.ACTIONS = NavigatorContextMenu.MODIFICATION;
    NavigatorContextMenu.OPEN_WITH = __spread(NavigatorContextMenu.NAVIGATION, ['open_with']);
})(NavigatorContextMenu = exports.NavigatorContextMenu || (exports.NavigatorContextMenu = {}));
var FileNavigatorContribution = /** @class */ (function (_super) {
    __extends(FileNavigatorContribution, _super);
    function FileNavigatorContribution(fileNavigatorPreferences, openerService, fileNavigatorFilter, workspaceService, workspacePreferences) {
        var _this = _super.call(this, {
            viewContainerId: navigator_widget_1.EXPLORER_VIEW_CONTAINER_ID,
            widgetId: navigator_widget_1.FILE_NAVIGATOR_ID,
            widgetName: 'Explorer',
            defaultWidgetOptions: {
                area: 'left',
                rank: 100
            },
            toggleCommandId: 'fileNavigator:toggle',
            toggleKeybinding: 'ctrlcmd+shift+e'
        }) || this;
        _this.fileNavigatorPreferences = fileNavigatorPreferences;
        _this.openerService = openerService;
        _this.fileNavigatorFilter = fileNavigatorFilter;
        _this.workspaceService = workspaceService;
        _this.workspacePreferences = workspacePreferences;
        /**
         * Register commands to the `More Actions...` navigator toolbar item.
         */
        _this.registerMoreToolbarItem = function (item) {
            var commandId = item.command;
            var id = 'navigator.tabbar.toolbar.' + commandId;
            var command = _this.commandRegistry.getCommand(commandId);
            _this.commandRegistry.registerCommand({ id: id, iconClass: command && command.iconClass }, {
                execute: function (w) {
                    var _a;
                    var args = [];
                    for (var _i = 1; _i < arguments.length; _i++) {
                        args[_i - 1] = arguments[_i];
                    }
                    return w instanceof navigator_widget_1.FileNavigatorWidget
                        && (_a = _this.commandRegistry).executeCommand.apply(_a, __spread([commandId], args));
                },
                isEnabled: function (w) {
                    var _a;
                    var args = [];
                    for (var _i = 1; _i < arguments.length; _i++) {
                        args[_i - 1] = arguments[_i];
                    }
                    return w instanceof navigator_widget_1.FileNavigatorWidget
                        && (_a = _this.commandRegistry).isEnabled.apply(_a, __spread([commandId], args));
                },
                isVisible: function (w) {
                    var _a;
                    var args = [];
                    for (var _i = 1; _i < arguments.length; _i++) {
                        args[_i - 1] = arguments[_i];
                    }
                    return w instanceof navigator_widget_1.FileNavigatorWidget
                        && (_a = _this.commandRegistry).isVisible.apply(_a, __spread([commandId], args));
                },
                isToggled: function (w) {
                    var _a;
                    var args = [];
                    for (var _i = 1; _i < arguments.length; _i++) {
                        args[_i - 1] = arguments[_i];
                    }
                    return w instanceof navigator_widget_1.FileNavigatorWidget
                        && (_a = _this.commandRegistry).isToggled.apply(_a, __spread([commandId], args));
                },
            });
            item.command = id;
            _this.tabbarToolbarRegistry.registerItem(item);
        };
        _this.toDisposeAddRemoveFolderActions = new common_1.DisposableCollection();
        return _this;
    }
    FileNavigatorContribution.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var updateFocusContextKeys;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.fileNavigatorPreferences.ready];
                    case 1:
                        _a.sent();
                        this.shell.currentChanged.connect(function () { return _this.onCurrentWidgetChangedHandler(); });
                        updateFocusContextKeys = function () {
                            var hasFocus = _this.shell.activeWidget instanceof navigator_widget_1.FileNavigatorWidget;
                            _this.contextKeyService.explorerViewletFocus.set(hasFocus);
                            _this.contextKeyService.filesExplorerFocus.set(hasFocus);
                        };
                        updateFocusContextKeys();
                        this.shell.activeChanged.connect(updateFocusContextKeys);
                        return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorContribution.prototype.onStart = function (app) {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                this.workspacePreferences.ready.then(function () {
                    _this.updateAddRemoveFolderActions(_this.menuRegistry);
                    _this.workspacePreferences.onPreferenceChanged(function (change) {
                        if (change.preferenceName === 'workspace.supportMultiRootWorkspace') {
                            _this.updateAddRemoveFolderActions(_this.menuRegistry);
                        }
                    });
                });
                return [2 /*return*/];
            });
        });
    };
    FileNavigatorContribution.prototype.initializeLayout = function (app) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.openView({ activate: true, reveal: true })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorContribution.prototype.registerCommands = function (registry) {
        var _this = this;
        _super.prototype.registerCommands.call(this, registry);
        registry.registerCommand(FileNavigatorCommands.REVEAL_IN_NAVIGATOR, {
            execute: function () { return _this.openView({ activate: true }).then(function () { return _this.selectWidgetFileNode(_this.shell.currentWidget); }); },
            isEnabled: function () { return browser_1.Navigatable.is(_this.shell.currentWidget); },
            isVisible: function () { return browser_1.Navigatable.is(_this.shell.currentWidget); }
        });
        registry.registerCommand(FileNavigatorCommands.TOGGLE_HIDDEN_FILES, {
            execute: function () {
                _this.fileNavigatorFilter.toggleHiddenFiles();
            },
            isEnabled: function () { return true; },
            isVisible: function () { return true; }
        });
        registry.registerCommand(FileNavigatorCommands.TOGGLE_AUTO_REVEAL, {
            isEnabled: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); },
            isVisible: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); },
            execute: function () {
                var autoReveal = !_this.fileNavigatorPreferences['explorer.autoReveal'];
                _this.preferenceService.set('explorer.autoReveal', autoReveal, browser_1.PreferenceScope.User);
                if (autoReveal) {
                    _this.selectWidgetFileNode(_this.shell.currentWidget);
                }
            },
            isToggled: function () { return _this.fileNavigatorPreferences['explorer.autoReveal']; }
        });
        registry.registerCommand(FileNavigatorCommands.COLLAPSE_ALL, {
            execute: function (widget) { return _this.withWidget(widget, function () { return _this.collapseFileNavigatorTree(); }); },
            isEnabled: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); },
            isVisible: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); }
        });
        registry.registerCommand(FileNavigatorCommands.REFRESH_NAVIGATOR, {
            execute: function (widget) { return _this.withWidget(widget, function () { return _this.refreshWorkspace(); }); },
            isEnabled: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); },
            isVisible: function (widget) { return _this.withWidget(widget, function () { return _this.workspaceService.opened; }); }
        });
        registry.registerCommand(FileNavigatorCommands.ADD_ROOT_FOLDER, {
            execute: function () {
                var args = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    args[_i] = arguments[_i];
                }
                return registry.executeCommand.apply(registry, __spread([browser_3.WorkspaceCommands.ADD_FOLDER.id], args));
            },
            isEnabled: function () {
                var args = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    args[_i] = arguments[_i];
                }
                return registry.isEnabled.apply(registry, __spread([browser_3.WorkspaceCommands.ADD_FOLDER.id], args));
            },
            isVisible: function () {
                var args = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    args[_i] = arguments[_i];
                }
                if (!registry.isVisible.apply(registry, __spread([browser_3.WorkspaceCommands.ADD_FOLDER.id], args))) {
                    return false;
                }
                var navigator = _this.tryGetWidget();
                var model = navigator && navigator.model;
                var uris = selection_1.UriSelection.getUris(model && model.selectedNodes);
                return _this.workspaceService.areWorkspaceRoots(uris);
            }
        });
        registry.registerCommand(navigator_diff_1.NavigatorDiffCommands.COMPARE_FIRST, {
            execute: function () {
                _this.navigatorDiff.addFirstComparisonFile();
            },
            isEnabled: function () { return true; },
            isVisible: function () { return true; }
        });
        registry.registerCommand(navigator_diff_1.NavigatorDiffCommands.COMPARE_SECOND, {
            execute: function () {
                _this.navigatorDiff.compareFiles();
            },
            isEnabled: function () { return _this.navigatorDiff.isFirstFileSelected; },
            isVisible: function () { return _this.navigatorDiff.isFirstFileSelected; }
        });
    };
    FileNavigatorContribution.prototype.withWidget = function (widget, cb) {
        if (widget === void 0) { widget = this.tryGetWidget(); }
        if (widget instanceof navigator_widget_1.FileNavigatorWidget && widget.id === navigator_widget_1.FILE_NAVIGATOR_ID) {
            return cb(widget);
        }
        return false;
    };
    FileNavigatorContribution.prototype.registerMenus = function (registry) {
        _super.prototype.registerMenus.call(this, registry);
        registry.registerMenuAction(browser_2.SHELL_TABBAR_CONTEXT_MENU, {
            commandId: FileNavigatorCommands.REVEAL_IN_NAVIGATOR.id,
            label: FileNavigatorCommands.REVEAL_IN_NAVIGATOR.label,
            order: '5'
        });
        registry.registerMenuAction(NavigatorContextMenu.NAVIGATION, {
            commandId: browser_1.CommonCommands.OPEN.id
        });
        registry.registerSubmenu(NavigatorContextMenu.OPEN_WITH, 'Open With');
        this.openerService.getOpeners().then(function (openers) {
            var e_1, _a;
            try {
                for (var openers_1 = __values(openers), openers_1_1 = openers_1.next(); !openers_1_1.done; openers_1_1 = openers_1.next()) {
                    var opener_1 = openers_1_1.value;
                    var openWithCommand = browser_3.WorkspaceCommands.FILE_OPEN_WITH(opener_1);
                    registry.registerMenuAction(NavigatorContextMenu.OPEN_WITH, {
                        commandId: openWithCommand.id,
                        label: opener_1.label,
                        icon: opener_1.iconClass
                    });
                }
            }
            catch (e_1_1) { e_1 = { error: e_1_1 }; }
            finally {
                try {
                    if (openers_1_1 && !openers_1_1.done && (_a = openers_1.return)) _a.call(openers_1);
                }
                finally { if (e_1) throw e_1.error; }
            }
        });
        // registry.registerMenuAction([CONTEXT_MENU_PATH, CUT_MENU_GROUP], {
        //     commandId: Commands.FILE_CUT
        // });
        registry.registerMenuAction(NavigatorContextMenu.CLIPBOARD, {
            commandId: browser_1.CommonCommands.COPY.id,
            order: 'a'
        });
        registry.registerMenuAction(NavigatorContextMenu.CLIPBOARD, {
            commandId: browser_1.CommonCommands.PASTE.id,
            order: 'b'
        });
        registry.registerMenuAction(NavigatorContextMenu.CLIPBOARD, {
            commandId: file_download_command_contribution_1.FileDownloadCommands.COPY_DOWNLOAD_LINK.id,
            order: 'z'
        });
        registry.registerMenuAction(NavigatorContextMenu.MODIFICATION, {
            commandId: browser_3.WorkspaceCommands.FILE_RENAME.id
        });
        registry.registerMenuAction(NavigatorContextMenu.MODIFICATION, {
            commandId: browser_3.WorkspaceCommands.FILE_DELETE.id
        });
        registry.registerMenuAction(NavigatorContextMenu.MODIFICATION, {
            commandId: browser_3.WorkspaceCommands.FILE_DUPLICATE.id
        });
        var downloadUploadMenu = __spread(exports.NAVIGATOR_CONTEXT_MENU, ['6_downloadupload']);
        registry.registerMenuAction(downloadUploadMenu, {
            commandId: filesystem_frontend_contribution_1.FileSystemCommands.UPLOAD.id,
            order: 'a'
        });
        registry.registerMenuAction(downloadUploadMenu, {
            commandId: file_download_command_contribution_1.FileDownloadCommands.DOWNLOAD.id,
            order: 'b'
        });
        registry.registerMenuAction(NavigatorContextMenu.NAVIGATION, {
            commandId: browser_3.WorkspaceCommands.NEW_FILE.id
        });
        registry.registerMenuAction(NavigatorContextMenu.NAVIGATION, {
            commandId: browser_3.WorkspaceCommands.NEW_FOLDER.id
        });
        registry.registerMenuAction(NavigatorContextMenu.COMPARE, {
            commandId: browser_3.WorkspaceCommands.FILE_COMPARE.id
        });
        registry.registerMenuAction(NavigatorContextMenu.MODIFICATION, {
            commandId: FileNavigatorCommands.COLLAPSE_ALL.id,
            label: 'Collapse All',
            order: 'z2'
        });
        registry.registerMenuAction(NavigatorContextMenu.COMPARE, {
            commandId: navigator_diff_1.NavigatorDiffCommands.COMPARE_FIRST.id,
            order: 'z'
        });
        registry.registerMenuAction(NavigatorContextMenu.COMPARE, {
            commandId: navigator_diff_1.NavigatorDiffCommands.COMPARE_SECOND.id,
            order: 'z'
        });
    };
    FileNavigatorContribution.prototype.registerKeybindings = function (registry) {
        _super.prototype.registerKeybindings.call(this, registry);
        registry.registerKeybinding({
            command: FileNavigatorCommands.REVEAL_IN_NAVIGATOR.id,
            keybinding: 'alt+r'
        });
        registry.registerKeybinding({
            command: browser_3.WorkspaceCommands.FILE_DELETE.id,
            keybinding: 'del',
            context: navigator_keybinding_context_1.NavigatorKeybindingContexts.navigatorActive
        });
        if (common_1.isOSX) {
            registry.registerKeybinding({
                command: browser_3.WorkspaceCommands.FILE_DELETE.id,
                keybinding: 'cmd+backspace',
                context: navigator_keybinding_context_1.NavigatorKeybindingContexts.navigatorActive
            });
        }
        registry.registerKeybinding({
            command: browser_3.WorkspaceCommands.FILE_RENAME.id,
            keybinding: 'f2',
            context: navigator_keybinding_context_1.NavigatorKeybindingContexts.navigatorActive
        });
        registry.registerKeybinding({
            command: FileNavigatorCommands.TOGGLE_HIDDEN_FILES.id,
            keybinding: 'ctrlcmd+i',
            context: navigator_keybinding_context_1.NavigatorKeybindingContexts.navigatorActive
        });
    };
    FileNavigatorContribution.prototype.registerToolbarItems = function (toolbarRegistry) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                toolbarRegistry.registerItem({
                    id: FileNavigatorCommands.REFRESH_NAVIGATOR.id,
                    command: FileNavigatorCommands.REFRESH_NAVIGATOR.id,
                    tooltip: 'Refresh Explorer',
                    priority: 0,
                });
                toolbarRegistry.registerItem({
                    id: FileNavigatorCommands.COLLAPSE_ALL.id,
                    command: FileNavigatorCommands.COLLAPSE_ALL.id,
                    tooltip: 'Collapse All',
                    priority: 1,
                });
                this.registerMoreToolbarItem({
                    id: browser_3.WorkspaceCommands.NEW_FILE.id,
                    command: browser_3.WorkspaceCommands.NEW_FILE.id,
                    tooltip: browser_3.WorkspaceCommands.NEW_FILE.label,
                    group: NavigatorMoreToolbarGroups.NEW_OPEN,
                });
                this.registerMoreToolbarItem({
                    id: browser_3.WorkspaceCommands.NEW_FOLDER.id,
                    command: browser_3.WorkspaceCommands.NEW_FOLDER.id,
                    tooltip: browser_3.WorkspaceCommands.NEW_FOLDER.label,
                    group: NavigatorMoreToolbarGroups.NEW_OPEN,
                });
                this.registerMoreToolbarItem({
                    id: FileNavigatorCommands.TOGGLE_AUTO_REVEAL.id,
                    command: FileNavigatorCommands.TOGGLE_AUTO_REVEAL.id,
                    tooltip: FileNavigatorCommands.TOGGLE_AUTO_REVEAL.label,
                    group: NavigatorMoreToolbarGroups.TOOLS,
                });
                this.registerMoreToolbarItem({
                    id: browser_3.WorkspaceCommands.ADD_FOLDER.id,
                    command: browser_3.WorkspaceCommands.ADD_FOLDER.id,
                    tooltip: browser_3.WorkspaceCommands.ADD_FOLDER.label,
                    group: NavigatorMoreToolbarGroups.WORKSPACE,
                });
                return [2 /*return*/];
            });
        });
    };
    /**
     * Reveals and selects node in the file navigator to which given widget is related.
     * Does nothing if given widget undefined or doesn't have related resource.
     *
     * @param widget widget file resource of which should be revealed and selected
     */
    FileNavigatorContribution.prototype.selectWidgetFileNode = function (widget) {
        return __awaiter(this, void 0, void 0, function () {
            var resourceUri, model, node;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!browser_1.Navigatable.is(widget)) return [3 /*break*/, 3];
                        resourceUri = widget.getResourceUri();
                        if (!resourceUri) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.widget];
                    case 1:
                        model = (_a.sent()).model;
                        return [4 /*yield*/, model.revealFile(resourceUri)];
                    case 2:
                        node = _a.sent();
                        if (browser_1.SelectableTreeNode.is(node)) {
                            model.selectNode(node);
                        }
                        _a.label = 3;
                    case 3: return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorContribution.prototype.onCurrentWidgetChangedHandler = function () {
        if (this.fileNavigatorPreferences['explorer.autoReveal']) {
            this.selectWidgetFileNode(this.shell.currentWidget);
        }
    };
    /**
     * Collapse file navigator nodes and set focus on first visible node
     * - single root workspace: collapse all nodes except root
     * - multiple root workspace: collapse all nodes, even roots
     */
    FileNavigatorContribution.prototype.collapseFileNavigatorTree = function () {
        return __awaiter(this, void 0, void 0, function () {
            var model, root, firstChild;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.widget];
                    case 1:
                        model = (_a.sent()).model;
                        root = model.root;
                        if (navigator_tree_1.WorkspaceNode.is(root) && root.children.length === 1) {
                            root = root.children[0];
                        }
                        root.children.forEach(function (child) { return browser_1.CompositeTreeNode.is(child) && model.collapseAll(child); });
                        firstChild = navigator_tree_1.WorkspaceNode.is(root) ? root.children[0] : root;
                        if (browser_1.SelectableTreeNode.is(firstChild)) {
                            model.selectNode(firstChild);
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    /**
     * force refresh workspace in navigator
     */
    FileNavigatorContribution.prototype.refreshWorkspace = function () {
        return __awaiter(this, void 0, void 0, function () {
            var model;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.widget];
                    case 1:
                        model = (_a.sent()).model;
                        return [4 /*yield*/, model.refresh()];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorContribution.prototype.updateAddRemoveFolderActions = function (registry) {
        this.toDisposeAddRemoveFolderActions.dispose();
        if (this.workspacePreferences['workspace.supportMultiRootWorkspace']) {
            this.toDisposeAddRemoveFolderActions.push(registry.registerMenuAction(NavigatorContextMenu.WORKSPACE, {
                commandId: FileNavigatorCommands.ADD_ROOT_FOLDER.id,
                label: browser_3.WorkspaceCommands.ADD_FOLDER.label
            }));
            this.toDisposeAddRemoveFolderActions.push(registry.registerMenuAction(NavigatorContextMenu.WORKSPACE, {
                commandId: browser_3.WorkspaceCommands.REMOVE_FOLDER.id
            }));
        }
    };
    __decorate([
        inversify_1.inject(common_1.CommandRegistry),
        __metadata("design:type", common_1.CommandRegistry)
    ], FileNavigatorContribution.prototype, "commandRegistry", void 0);
    __decorate([
        inversify_1.inject(tab_bar_toolbar_1.TabBarToolbarRegistry),
        __metadata("design:type", tab_bar_toolbar_1.TabBarToolbarRegistry)
    ], FileNavigatorContribution.prototype, "tabbarToolbarRegistry", void 0);
    __decorate([
        inversify_1.inject(navigator_context_key_service_1.NavigatorContextKeyService),
        __metadata("design:type", navigator_context_key_service_1.NavigatorContextKeyService)
    ], FileNavigatorContribution.prototype, "contextKeyService", void 0);
    __decorate([
        inversify_1.inject(common_1.MenuModelRegistry),
        __metadata("design:type", common_1.MenuModelRegistry)
    ], FileNavigatorContribution.prototype, "menuRegistry", void 0);
    __decorate([
        inversify_1.inject(navigator_diff_1.NavigatorDiff),
        __metadata("design:type", navigator_diff_1.NavigatorDiff)
    ], FileNavigatorContribution.prototype, "navigatorDiff", void 0);
    __decorate([
        inversify_1.inject(browser_4.PreferenceService),
        __metadata("design:type", Object)
    ], FileNavigatorContribution.prototype, "preferenceService", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], FileNavigatorContribution.prototype, "init", null);
    FileNavigatorContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(navigator_preferences_1.FileNavigatorPreferences)),
        __param(1, inversify_1.inject(browser_1.OpenerService)),
        __param(2, inversify_1.inject(navigator_filter_1.FileNavigatorFilter)),
        __param(3, inversify_1.inject(browser_3.WorkspaceService)),
        __param(4, inversify_1.inject(browser_3.WorkspacePreferences)),
        __metadata("design:paramtypes", [Object, Object, navigator_filter_1.FileNavigatorFilter,
            browser_3.WorkspaceService, Object])
    ], FileNavigatorContribution);
    return FileNavigatorContribution;
}(view_contribution_1.AbstractViewContribution));
exports.FileNavigatorContribution = FileNavigatorContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-diff.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-diff.js ***!
  \********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 David Saunders and others.
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
var filesystem_1 = __webpack_require__(/*! @theia/filesystem/lib/common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var opener_service_1 = __webpack_require__(/*! @theia/core/lib/browser/opener-service */ "../../packages/core/lib/browser/opener-service.js");
var message_service_1 = __webpack_require__(/*! @theia/core/lib/common/message-service */ "../../packages/core/lib/common/message-service.js");
var diff_uris_1 = __webpack_require__(/*! @theia/core/lib/browser/diff-uris */ "../../packages/core/lib/browser/diff-uris.js");
var NavigatorDiffCommands;
(function (NavigatorDiffCommands) {
    var COMPARE_CATEGORY = 'Compare';
    NavigatorDiffCommands.COMPARE_FIRST = {
        id: 'compare:first',
        category: COMPARE_CATEGORY,
        label: 'Select for Compare'
    };
    NavigatorDiffCommands.COMPARE_SECOND = {
        id: 'compare:second',
        category: COMPARE_CATEGORY,
        label: 'Compare with Selected'
    };
})(NavigatorDiffCommands = exports.NavigatorDiffCommands || (exports.NavigatorDiffCommands = {}));
var NavigatorDiff = /** @class */ (function () {
    function NavigatorDiff() {
        this._firstCompareFile = undefined;
    }
    Object.defineProperty(NavigatorDiff.prototype, "firstCompareFile", {
        get: function () {
            return this._firstCompareFile;
        },
        set: function (uri) {
            this._firstCompareFile = uri;
            this._isFirstFileSelected = true;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NavigatorDiff.prototype, "isFirstFileSelected", {
        get: function () {
            return this._isFirstFileSelected;
        },
        enumerable: true,
        configurable: true
    });
    NavigatorDiff.prototype.isDirectory = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var stat, e_1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        _a.trys.push([0, 2, , 3]);
                        return [4 /*yield*/, this.fileSystem.getFileStat(uri.path.toString())];
                    case 1:
                        stat = _a.sent();
                        if (!stat || stat.isDirectory) {
                            return [2 /*return*/, true];
                        }
                        return [3 /*break*/, 3];
                    case 2:
                        e_1 = _a.sent();
                        return [3 /*break*/, 3];
                    case 3: return [2 /*return*/, false];
                }
            });
        });
    };
    NavigatorDiff.prototype.getURISelection = function () {
        return __awaiter(this, void 0, void 0, function () {
            var uri;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        uri = common_1.UriSelection.getUri(this.selectionService.selection);
                        if (!uri) {
                            return [2 /*return*/, undefined];
                        }
                        return [4 /*yield*/, this.isDirectory(uri)];
                    case 1:
                        if (_a.sent()) {
                            return [2 /*return*/, undefined];
                        }
                        return [2 /*return*/, uri];
                }
            });
        });
    };
    /**
     * Adds the initial file for comparison
     * @see SelectionService
     * @see compareFiles
     * @returns Promise<boolean> indicating whether the uri is valid
     */
    NavigatorDiff.prototype.addFirstComparisonFile = function () {
        return __awaiter(this, void 0, void 0, function () {
            var uriSelected;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.getURISelection()];
                    case 1:
                        uriSelected = _a.sent();
                        if (uriSelected === undefined) {
                            return [2 /*return*/, false];
                        }
                        this.firstCompareFile = uriSelected;
                        return [2 /*return*/, true];
                }
            });
        });
    };
    /**
     * Compare selected files.  First file is selected through addFirstComparisonFile
     * @see SelectionService
     * @see addFirstComparisonFile
     * @returns Promise<boolean> indicating whether the comparison was completed successfully
     */
    NavigatorDiff.prototype.compareFiles = function () {
        return __awaiter(this, void 0, void 0, function () {
            var uriSelected, diffUri;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.getURISelection()];
                    case 1:
                        uriSelected = _a.sent();
                        if (this.firstCompareFile === undefined || uriSelected === undefined) {
                            return [2 /*return*/, false];
                        }
                        diffUri = diff_uris_1.DiffUris.encode(this.firstCompareFile, uriSelected);
                        opener_service_1.open(this.openerService, diffUri).catch(function (e) {
                            _this.notifications.error(e.message);
                        });
                        return [2 /*return*/, true];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(filesystem_1.FileSystem),
        __metadata("design:type", Object)
    ], NavigatorDiff.prototype, "fileSystem", void 0);
    __decorate([
        inversify_1.inject(opener_service_1.OpenerService),
        __metadata("design:type", Object)
    ], NavigatorDiff.prototype, "openerService", void 0);
    __decorate([
        inversify_1.inject(message_service_1.MessageService),
        __metadata("design:type", message_service_1.MessageService)
    ], NavigatorDiff.prototype, "notifications", void 0);
    __decorate([
        inversify_1.inject(common_1.SelectionService),
        __metadata("design:type", common_1.SelectionService)
    ], NavigatorDiff.prototype, "selectionService", void 0);
    NavigatorDiff = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], NavigatorDiff);
    return NavigatorDiff;
}());
exports.NavigatorDiff = NavigatorDiff;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-diff'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-filter.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-filter.js ***!
  \**********************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var minimatch_1 = __webpack_require__(/*! minimatch */ "../../node_modules/minimatch/minimatch.js");
var event_1 = __webpack_require__(/*! @theia/core/lib/common/event */ "../../packages/core/lib/common/event.js");
var filesystem_preferences_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/filesystem-preferences */ "../../packages/filesystem/lib/browser/filesystem-preferences.js");
var navigator_preferences_1 = __webpack_require__(/*! ./navigator-preferences */ "../../packages/navigator/lib/browser/navigator-preferences.js");
/**
 * Filter for omitting elements from the navigator. For more details on the exclusion patterns,
 * one should check either the manual with `man 5 gitignore` or just [here](https://git-scm.com/docs/gitignore).
 */
var FileNavigatorFilter = /** @class */ (function () {
    function FileNavigatorFilter(preferences) {
        this.preferences = preferences;
        this.emitter = new event_1.Emitter();
    }
    FileNavigatorFilter.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                this.filterPredicate = this.createFilterPredicate(this.filesPreferences['files.exclude']);
                this.filesPreferences.onPreferenceChanged(function (event) { return _this.onFilesPreferenceChanged(event); });
                this.preferences.onPreferenceChanged(function (event) { return _this.onPreferenceChanged(event); });
                return [2 /*return*/];
            });
        });
    };
    FileNavigatorFilter.prototype.filter = function (items) {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, items];
                    case 1: return [2 /*return*/, (_a.sent()).filter(function (item) { return _this.filterItem(item); })];
                }
            });
        });
    };
    Object.defineProperty(FileNavigatorFilter.prototype, "onFilterChanged", {
        get: function () {
            return this.emitter.event;
        },
        enumerable: true,
        configurable: true
    });
    FileNavigatorFilter.prototype.filterItem = function (item) {
        return this.filterPredicate.filter(item);
    };
    FileNavigatorFilter.prototype.fireFilterChanged = function () {
        this.emitter.fire(undefined);
    };
    FileNavigatorFilter.prototype.onFilesPreferenceChanged = function (event) {
        var preferenceName = event.preferenceName, newValue = event.newValue;
        if (preferenceName === 'files.exclude') {
            this.filterPredicate = this.createFilterPredicate(newValue || {});
            this.fireFilterChanged();
        }
    };
    FileNavigatorFilter.prototype.onPreferenceChanged = function (event) {
    };
    FileNavigatorFilter.prototype.createFilterPredicate = function (exclusions) {
        return new FileNavigatorFilterPredicate(this.interceptExclusions(exclusions));
    };
    FileNavigatorFilter.prototype.toggleHiddenFiles = function () {
        this.showHiddenFiles = !this.showHiddenFiles;
        var filesExcludes = this.filesPreferences['files.exclude'];
        this.filterPredicate = this.createFilterPredicate(filesExcludes || {});
        this.fireFilterChanged();
    };
    FileNavigatorFilter.prototype.interceptExclusions = function (exclusions) {
        return __assign({}, exclusions, { '**/.*': this.showHiddenFiles });
    };
    __decorate([
        inversify_1.inject(filesystem_preferences_1.FileSystemPreferences),
        __metadata("design:type", Object)
    ], FileNavigatorFilter.prototype, "filesPreferences", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], FileNavigatorFilter.prototype, "init", null);
    FileNavigatorFilter = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(navigator_preferences_1.FileNavigatorPreferences)),
        __metadata("design:paramtypes", [Object])
    ], FileNavigatorFilter);
    return FileNavigatorFilter;
}());
exports.FileNavigatorFilter = FileNavigatorFilter;
(function (FileNavigatorFilter) {
    var Predicate;
    (function (Predicate) {
        /**
         * Wraps a bunch of predicates and returns with a new one that evaluates to `true` if
         * each of the wrapped predicates evaluates to `true`. Otherwise, `false`.
         */
        function and() {
            var predicates = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                predicates[_i] = arguments[_i];
            }
            return {
                filter: function (id) { return predicates.every(function (predicate) { return predicate.filter(id); }); }
            };
        }
        Predicate.and = and;
    })(Predicate = FileNavigatorFilter.Predicate || (FileNavigatorFilter.Predicate = {}));
})(FileNavigatorFilter = exports.FileNavigatorFilter || (exports.FileNavigatorFilter = {}));
exports.FileNavigatorFilter = FileNavigatorFilter;
/**
 * Concrete filter navigator filter predicate that is decoupled from the preferences.
 */
var FileNavigatorFilterPredicate = /** @class */ (function () {
    function FileNavigatorFilterPredicate(exclusions) {
        var _a;
        var _this = this;
        var patterns = Object.keys(exclusions).map(function (pattern) { return ({ pattern: pattern, enabled: exclusions[pattern] }); }).filter(function (object) { return object.enabled; }).map(function (object) { return object.pattern; });
        this.delegate = (_a = FileNavigatorFilter.Predicate).and.apply(_a, __spread(patterns.map(function (pattern) { return _this.createDelegate(pattern); })));
    }
    FileNavigatorFilterPredicate.prototype.filter = function (item) {
        return this.delegate.filter(item);
    };
    FileNavigatorFilterPredicate.prototype.createDelegate = function (pattern) {
        var delegate = new minimatch_1.Minimatch(pattern, { matchBase: true });
        return {
            filter: function (item) { return !delegate.match(item.id); }
        };
    };
    return FileNavigatorFilterPredicate;
}());
exports.FileNavigatorFilterPredicate = FileNavigatorFilterPredicate;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-filter'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-keybinding-context.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-keybinding-context.js ***!
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var navigator_widget_1 = __webpack_require__(/*! ./navigator-widget */ "../../packages/navigator/lib/browser/navigator-widget.js");
var NavigatorKeybindingContexts;
(function (NavigatorKeybindingContexts) {
    NavigatorKeybindingContexts.navigatorActive = 'navigatorActive';
})(NavigatorKeybindingContexts = exports.NavigatorKeybindingContexts || (exports.NavigatorKeybindingContexts = {}));
var NavigatorActiveContext = /** @class */ (function () {
    function NavigatorActiveContext() {
        this.id = NavigatorKeybindingContexts.navigatorActive;
    }
    NavigatorActiveContext.prototype.isEnabled = function () {
        return this.applicationShell.activeWidget instanceof navigator_widget_1.FileNavigatorWidget;
    };
    __decorate([
        inversify_1.inject(browser_1.ApplicationShell),
        __metadata("design:type", browser_1.ApplicationShell)
    ], NavigatorActiveContext.prototype, "applicationShell", void 0);
    NavigatorActiveContext = __decorate([
        inversify_1.injectable()
    ], NavigatorActiveContext);
    return NavigatorActiveContext;
}());
exports.NavigatorActiveContext = NavigatorActiveContext;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-keybinding-context'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-model.js":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-model.js ***!
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/filesystem/lib/browser */ "../../packages/filesystem/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var navigator_tree_1 = __webpack_require__(/*! ./navigator-tree */ "../../packages/navigator/lib/browser/navigator-tree.js");
var browser_3 = __webpack_require__(/*! @theia/workspace/lib/browser */ "../../packages/workspace/lib/browser/index.js");
var frontend_application_state_1 = __webpack_require__(/*! @theia/core/lib/browser/frontend-application-state */ "../../packages/core/lib/browser/frontend-application-state.js");
var FileNavigatorModel = /** @class */ (function (_super) {
    __extends(FileNavigatorModel, _super);
    function FileNavigatorModel() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    FileNavigatorModel.prototype.init = function () {
        _super.prototype.init.call(this);
        this.initializeRoot();
    };
    FileNavigatorModel.prototype.initializeRoot = function () {
        return __awaiter(this, void 0, void 0, function () {
            var root, child;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            this.applicationState.reachedState('initialized_layout'),
                            this.workspaceService.roots
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, this.updateRoot()];
                    case 2:
                        _a.sent();
                        if (this.toDispose.disposed) {
                            return [2 /*return*/];
                        }
                        this.toDispose.push(this.workspaceService.onWorkspaceChanged(function () { return _this.updateRoot(); }));
                        this.toDispose.push(this.workspaceService.onWorkspaceLocationChanged(function () { return _this.updateRoot(); }));
                        if (this.selectedNodes.length) {
                            return [2 /*return*/];
                        }
                        root = this.root;
                        if (browser_2.CompositeTreeNode.is(root) && root.children.length === 1) {
                            child = root.children[0];
                            if (browser_2.SelectableTreeNode.is(child) && !child.selected && browser_2.ExpandableTreeNode.is(child)) {
                                this.selectNode(child);
                                this.expandNode(child);
                            }
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorModel.prototype.previewNode = function (node) {
        if (browser_1.FileNode.is(node)) {
            browser_2.open(this.openerService, node.uri, { mode: 'reveal', preview: true });
        }
    };
    FileNavigatorModel.prototype.doOpenNode = function (node) {
        if (browser_1.FileNode.is(node)) {
            browser_2.open(this.openerService, node.uri);
        }
        else {
            _super.prototype.doOpenNode.call(this, node);
        }
    };
    FileNavigatorModel.prototype.getNodesByUri = function (uri) {
        var workspace, _a, _b, root, id, node, e_1_1;
        var e_1, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0:
                    workspace = this.root;
                    if (!navigator_tree_1.WorkspaceNode.is(workspace)) return [3 /*break*/, 8];
                    _d.label = 1;
                case 1:
                    _d.trys.push([1, 6, 7, 8]);
                    _a = __values(workspace.children), _b = _a.next();
                    _d.label = 2;
                case 2:
                    if (!!_b.done) return [3 /*break*/, 5];
                    root = _b.value;
                    id = this.tree.createId(root, uri);
                    node = this.getNode(id);
                    if (!node) return [3 /*break*/, 4];
                    return [4 /*yield*/, node];
                case 3:
                    _d.sent();
                    _d.label = 4;
                case 4:
                    _b = _a.next();
                    return [3 /*break*/, 2];
                case 5: return [3 /*break*/, 8];
                case 6:
                    e_1_1 = _d.sent();
                    e_1 = { error: e_1_1 };
                    return [3 /*break*/, 8];
                case 7:
                    try {
                        if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                    }
                    finally { if (e_1) throw e_1.error; }
                    return [7 /*endfinally*/];
                case 8: return [2 /*return*/];
            }
        });
    };
    FileNavigatorModel.prototype.updateRoot = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = this;
                        return [4 /*yield*/, this.createRoot()];
                    case 1:
                        _a.root = _b.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    FileNavigatorModel.prototype.createRoot = function () {
        return __awaiter(this, void 0, void 0, function () {
            var stat, isMulti, workspaceNode, roots, roots_1, roots_1_1, root, _a, _b, e_2_1;
            var e_2, _c;
            return __generator(this, function (_d) {
                switch (_d.label) {
                    case 0:
                        if (!this.workspaceService.opened) return [3 /*break*/, 10];
                        stat = this.workspaceService.workspace;
                        isMulti = (stat) ? !stat.isDirectory : false;
                        workspaceNode = isMulti
                            ? this.createMultipleRootNode()
                            : navigator_tree_1.WorkspaceNode.createRoot();
                        return [4 /*yield*/, this.workspaceService.roots];
                    case 1:
                        roots = _d.sent();
                        _d.label = 2;
                    case 2:
                        _d.trys.push([2, 7, 8, 9]);
                        roots_1 = __values(roots), roots_1_1 = roots_1.next();
                        _d.label = 3;
                    case 3:
                        if (!!roots_1_1.done) return [3 /*break*/, 6];
                        root = roots_1_1.value;
                        _b = (_a = workspaceNode.children).push;
                        return [4 /*yield*/, this.tree.createWorkspaceRoot(root, workspaceNode)];
                    case 4:
                        _b.apply(_a, [_d.sent()]);
                        _d.label = 5;
                    case 5:
                        roots_1_1 = roots_1.next();
                        return [3 /*break*/, 3];
                    case 6: return [3 /*break*/, 9];
                    case 7:
                        e_2_1 = _d.sent();
                        e_2 = { error: e_2_1 };
                        return [3 /*break*/, 9];
                    case 8:
                        try {
                            if (roots_1_1 && !roots_1_1.done && (_c = roots_1.return)) _c.call(roots_1);
                        }
                        finally { if (e_2) throw e_2.error; }
                        return [7 /*endfinally*/];
                    case 9: return [2 /*return*/, workspaceNode];
                    case 10: return [2 /*return*/];
                }
            });
        });
    };
    /**
     * Create multiple root node used to display
     * the multiple root workspace name.
     *
     * @returns `WorkspaceNode`
     */
    FileNavigatorModel.prototype.createMultipleRootNode = function () {
        var workspace = this.workspaceService.workspace;
        var name = workspace
            ? new uri_1.default(workspace.uri).path.name
            : 'untitled';
        name += ' (Workspace)';
        return navigator_tree_1.WorkspaceNode.createRoot(name);
    };
    /**
     * Move the given source file or directory to the given target directory.
     */
    FileNavigatorModel.prototype.move = function (source, target) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (source.parent && navigator_tree_1.WorkspaceRootNode.is(source)) {
                            // do not support moving a root folder
                            return [2 /*return*/];
                        }
                        return [4 /*yield*/, _super.prototype.move.call(this, source, target)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    /**
     * Reveals node in the navigator by given file uri.
     *
     * @param uri uri to file which should be revealed in the navigator
     * @returns file tree node if the file with given uri was revealed, undefined otherwise
     */
    FileNavigatorModel.prototype.revealFile = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var node;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!uri.path.isAbsolute) {
                            return [2 /*return*/, undefined];
                        }
                        node = this.getNodeClosestToRootByUri(uri);
                        if (!navigator_tree_1.WorkspaceRootNode.is(node)) return [3 /*break*/, 4];
                        if (!browser_2.ExpandableTreeNode.is(node)) return [3 /*break*/, 3];
                        if (!!node.expanded) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.expandNode(node)];
                    case 1:
                        node = _a.sent();
                        _a.label = 2;
                    case 2: return [2 /*return*/, node];
                    case 3: 
                    // shouldn't happen, root node is always directory, i.e. expandable
                    return [2 /*return*/, undefined];
                    case 4:
                        // fail stop condition
                        if (uri.path.isRoot) {
                            // file system root is reached but workspace root wasn't found, it means that
                            // given uri is not in workspace root folder or points to not existing file.
                            return [2 /*return*/, undefined];
                        }
                        return [4 /*yield*/, this.revealFile(uri.parent)];
                    case 5:
                        if (!_a.sent()) return [3 /*break*/, 8];
                        if (node === undefined) {
                            // get node if it wasn't mounted into navigator tree before expansion
                            node = this.getNodeClosestToRootByUri(uri);
                        }
                        if (!(browser_2.ExpandableTreeNode.is(node) && !node.expanded)) return [3 /*break*/, 7];
                        return [4 /*yield*/, this.expandNode(node)];
                    case 6:
                        node = _a.sent();
                        _a.label = 7;
                    case 7: return [2 /*return*/, node];
                    case 8: return [2 /*return*/, undefined];
                }
            });
        });
    };
    FileNavigatorModel.prototype.getNodeClosestToRootByUri = function (uri) {
        var nodes = __spread(this.getNodesByUri(uri));
        return nodes.length > 0
            ? nodes.reduce(function (node1, node2) {
                return node1.id.length >= node2.id.length ? node1 : node2;
            }) : undefined;
    };
    __decorate([
        inversify_1.inject(browser_2.OpenerService),
        __metadata("design:type", Object)
    ], FileNavigatorModel.prototype, "openerService", void 0);
    __decorate([
        inversify_1.inject(navigator_tree_1.FileNavigatorTree),
        __metadata("design:type", navigator_tree_1.FileNavigatorTree)
    ], FileNavigatorModel.prototype, "tree", void 0);
    __decorate([
        inversify_1.inject(browser_3.WorkspaceService),
        __metadata("design:type", browser_3.WorkspaceService)
    ], FileNavigatorModel.prototype, "workspaceService", void 0);
    __decorate([
        inversify_1.inject(frontend_application_state_1.FrontendApplicationStateService),
        __metadata("design:type", frontend_application_state_1.FrontendApplicationStateService)
    ], FileNavigatorModel.prototype, "applicationState", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileNavigatorModel.prototype, "init", null);
    FileNavigatorModel = __decorate([
        inversify_1.injectable()
    ], FileNavigatorModel);
    return FileNavigatorModel;
}(browser_1.FileTreeModel));
exports.FileNavigatorModel = FileNavigatorModel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-model'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-preferences.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-preferences.js ***!
  \***************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
exports.FileNavigatorConfigSchema = {
    'type': 'object',
    properties: {
        'explorer.autoReveal': {
            type: 'boolean',
            description: 'Selects file under editing in the explorer.',
            default: true
        }
    }
};
exports.FileNavigatorPreferences = Symbol('NavigatorPreferences');
function createNavigatorPreferences(preferences) {
    return browser_1.createPreferenceProxy(preferences, exports.FileNavigatorConfigSchema);
}
exports.createNavigatorPreferences = createNavigatorPreferences;
function bindFileNavigatorPreferences(bind) {
    bind(exports.FileNavigatorPreferences).toDynamicValue(function (ctx) {
        var preferences = ctx.container.get(browser_1.PreferenceService);
        return createNavigatorPreferences(preferences);
    });
    bind(browser_1.PreferenceContribution).toConstantValue({ schema: exports.FileNavigatorConfigSchema });
}
exports.bindFileNavigatorPreferences = bindFileNavigatorPreferences;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-preferences'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-tree.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-tree.js ***!
  \********************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/filesystem/lib/browser */ "../../packages/filesystem/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var navigator_filter_1 = __webpack_require__(/*! ./navigator-filter */ "../../packages/navigator/lib/browser/navigator-filter.js");
var FileNavigatorTree = /** @class */ (function (_super) {
    __extends(FileNavigatorTree, _super);
    function FileNavigatorTree() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    FileNavigatorTree.prototype.init = function () {
        var _this = this;
        this.toDispose.push(this.filter.onFilterChanged(function () { return _this.refresh(); }));
    };
    FileNavigatorTree.prototype.resolveChildren = function (parent) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                if (WorkspaceNode.is(parent)) {
                    return [2 /*return*/, parent.children];
                }
                return [2 /*return*/, this.filter.filter(_super.prototype.resolveChildren.call(this, parent))];
            });
        });
    };
    FileNavigatorTree.prototype.toNodeId = function (uri, parent) {
        var workspaceRootNode = WorkspaceRootNode.find(parent);
        if (workspaceRootNode) {
            return this.createId(workspaceRootNode, uri);
        }
        return _super.prototype.toNodeId.call(this, uri, parent);
    };
    FileNavigatorTree.prototype.createId = function (root, uri) {
        var id = _super.prototype.toNodeId.call(this, uri, root);
        return id === root.id ? id : root.id + ":" + id;
    };
    FileNavigatorTree.prototype.createWorkspaceRoot = function (rootFolder, workspaceNode) {
        return __awaiter(this, void 0, void 0, function () {
            var node;
            return __generator(this, function (_a) {
                node = this.toNode(rootFolder, workspaceNode);
                Object.assign(node, {
                    visible: workspaceNode.name !== WorkspaceNode.name,
                });
                return [2 /*return*/, node];
            });
        });
    };
    __decorate([
        inversify_1.inject(navigator_filter_1.FileNavigatorFilter),
        __metadata("design:type", navigator_filter_1.FileNavigatorFilter)
    ], FileNavigatorTree.prototype, "filter", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileNavigatorTree.prototype, "init", null);
    FileNavigatorTree = __decorate([
        inversify_1.injectable()
    ], FileNavigatorTree);
    return FileNavigatorTree;
}(browser_1.FileTree));
exports.FileNavigatorTree = FileNavigatorTree;
var WorkspaceNode;
(function (WorkspaceNode) {
    WorkspaceNode.id = 'WorkspaceNodeId';
    WorkspaceNode.name = 'WorkspaceNode';
    function is(node) {
        return browser_2.CompositeTreeNode.is(node) && node.id === WorkspaceNode.id;
    }
    WorkspaceNode.is = is;
    function createRoot(multiRootName) {
        return {
            id: WorkspaceNode.id,
            name: multiRootName || WorkspaceNode.name,
            parent: undefined,
            children: [],
            visible: false,
            selected: false
        };
    }
    WorkspaceNode.createRoot = createRoot;
})(WorkspaceNode = exports.WorkspaceNode || (exports.WorkspaceNode = {}));
var WorkspaceRootNode;
(function (WorkspaceRootNode) {
    function is(node) {
        return browser_1.DirNode.is(node) && WorkspaceNode.is(node.parent);
    }
    WorkspaceRootNode.is = is;
    function find(node) {
        if (node) {
            if (is(node)) {
                return node;
            }
            return find(node.parent);
        }
    }
    WorkspaceRootNode.find = find;
})(WorkspaceRootNode = exports.WorkspaceRootNode || (exports.WorkspaceRootNode = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-tree'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-widget.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-widget.js ***!
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var browser_3 = __webpack_require__(/*! @theia/filesystem/lib/browser */ "../../packages/filesystem/lib/browser/index.js");
var browser_4 = __webpack_require__(/*! @theia/workspace/lib/browser */ "../../packages/workspace/lib/browser/index.js");
var application_shell_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/application-shell */ "../../packages/core/lib/browser/shell/application-shell.js");
var navigator_tree_1 = __webpack_require__(/*! ./navigator-tree */ "../../packages/navigator/lib/browser/navigator-tree.js");
var navigator_model_1 = __webpack_require__(/*! ./navigator-model */ "../../packages/navigator/lib/browser/navigator-model.js");
var filesystem_1 = __webpack_require__(/*! @theia/filesystem/lib/common/filesystem */ "../../packages/filesystem/lib/common/filesystem.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var navigator_context_key_service_1 = __webpack_require__(/*! ./navigator-context-key-service */ "../../packages/navigator/lib/browser/navigator-context-key-service.js");
exports.FILE_NAVIGATOR_ID = 'files';
exports.EXPLORER_VIEW_CONTAINER_ID = 'explorer-view-container';
exports.EXPLORER_VIEW_CONTAINER_TITLE_OPTIONS = {
    label: 'Explorer',
    iconClass: 'navigator-tab-icon',
    closeable: true
};
exports.LABEL = 'No folder opened';
exports.CLASS = 'theia-Files';
var FileNavigatorWidget = /** @class */ (function (_super) {
    __extends(FileNavigatorWidget, _super);
    function FileNavigatorWidget(props, model, contextMenuRenderer, commandService, selectionService, workspaceService, shell, fileSystem) {
        var _this = _super.call(this, props, model, contextMenuRenderer) || this;
        _this.props = props;
        _this.model = model;
        _this.commandService = commandService;
        _this.selectionService = selectionService;
        _this.workspaceService = workspaceService;
        _this.shell = shell;
        _this.fileSystem = fileSystem;
        _this.canOpenWorkspaceFileAndFolder = core_1.isOSX || !core_1.environment.electron.is();
        _this.openWorkspace = function () { return _this.doOpenWorkspace(); };
        _this.openFolder = function () { return _this.doOpenFolder(); };
        _this.keyUpHandler = function (e) {
            if (browser_1.Key.ENTER.keyCode === e.keyCode) {
                e.target.click();
            }
        };
        _this.id = exports.FILE_NAVIGATOR_ID;
        _this.addClass(exports.CLASS);
        return _this;
    }
    FileNavigatorWidget.prototype.init = function () {
        var _this = this;
        _super.prototype.init.call(this);
        this.updateSelectionContextKeys();
        this.toDispose.pushAll([
            this.model.onSelectionChanged(function () {
                return _this.updateSelectionContextKeys();
            }),
            this.model.onExpansionChanged(function (node) {
                if (node.expanded && node.children.length === 1) {
                    var child = node.children[0];
                    if (browser_2.ExpandableTreeNode.is(child) && !child.expanded) {
                        _this.model.expandNode(child);
                    }
                }
            })
        ]);
    };
    FileNavigatorWidget.prototype.doUpdateRows = function () {
        _super.prototype.doUpdateRows.call(this);
        this.title.label = exports.LABEL;
        if (navigator_tree_1.WorkspaceNode.is(this.model.root)) {
            if (this.model.root.name === navigator_tree_1.WorkspaceNode.name) {
                var rootNode = this.model.root.children[0];
                if (navigator_tree_1.WorkspaceRootNode.is(rootNode)) {
                    this.title.label = this.toNodeName(rootNode);
                    this.title.caption = this.labelProvider.getLongName(rootNode.uri);
                }
            }
            else {
                this.title.label = this.toNodeName(this.model.root);
                this.title.caption = this.title.label;
            }
        }
        else {
            this.title.caption = this.title.label;
        }
    };
    FileNavigatorWidget.prototype.enableDndOnMainPanel = function () {
        var _this = this;
        var mainPanelNode = this.shell.mainPanel.node;
        this.addEventListener(mainPanelNode, 'drop', function (_a) {
            var dataTransfer = _a.dataTransfer;
            return __awaiter(_this, void 0, void 0, function () {
                var treeNodes;
                var _this = this;
                return __generator(this, function (_b) {
                    treeNodes = dataTransfer && this.getSelectedTreeNodesFromData(dataTransfer) || [];
                    treeNodes.filter(browser_3.FileNode.is).forEach(function (treeNode) { return _this.commandService.executeCommand(browser_1.CommonCommands.OPEN.id, treeNode.uri); });
                    return [2 /*return*/];
                });
            });
        });
        var handler = function (e) {
            if (e.dataTransfer) {
                e.dataTransfer.dropEffect = 'link';
                e.preventDefault();
            }
        };
        this.addEventListener(mainPanelNode, 'dragover', handler);
        this.addEventListener(mainPanelNode, 'dragenter', handler);
    };
    FileNavigatorWidget.prototype.getContainerTreeNode = function () {
        var root = this.model.root;
        if (this.workspaceService.isMultiRootWorkspaceOpened) {
            return root;
        }
        if (navigator_tree_1.WorkspaceNode.is(root)) {
            return root.children[0];
        }
        return undefined;
    };
    FileNavigatorWidget.prototype.deflateForStorage = function (node) {
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        var copy = __assign({}, node);
        if (copy.uri) {
            copy.uri = copy.uri.toString();
        }
        return _super.prototype.deflateForStorage.call(this, copy);
    };
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    FileNavigatorWidget.prototype.inflateFromStorage = function (node, parent) {
        if (node.uri) {
            node.uri = new uri_1.default(node.uri);
        }
        return _super.prototype.inflateFromStorage.call(this, node, parent);
    };
    FileNavigatorWidget.prototype.renderTree = function (model) {
        return _super.prototype.renderTree.call(this, model) || this.renderOpenWorkspaceDiv();
    };
    FileNavigatorWidget.prototype.onAfterAttach = function (msg) {
        var _this = this;
        _super.prototype.onAfterAttach.call(this, msg);
        this.addClipboardListener(this.node, 'copy', function (e) { return _this.handleCopy(e); });
        this.addClipboardListener(this.node, 'paste', function (e) { return _this.handlePaste(e); });
        this.enableDndOnMainPanel();
    };
    FileNavigatorWidget.prototype.handleCopy = function (event) {
        var uris = this.model.selectedFileStatNodes.map(function (node) { return node.uri.toString(); });
        if (uris.length > 0 && event.clipboardData) {
            event.clipboardData.setData('text/plain', uris.join('\n'));
            event.preventDefault();
        }
    };
    FileNavigatorWidget.prototype.handlePaste = function (event) {
        var e_1, _a;
        if (event.clipboardData) {
            var raw = event.clipboardData.getData('text/plain');
            if (!raw) {
                return;
            }
            try {
                for (var _b = __values(raw.split('\n')), _c = _b.next(); !_c.done; _c = _b.next()) {
                    var file = _c.value;
                    var uri = new uri_1.default(file);
                    if (this.model.copy(uri)) {
                        event.preventDefault();
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
        }
    };
    FileNavigatorWidget.prototype.doOpenWorkspace = function () {
        this.commandService.executeCommand(browser_4.WorkspaceCommands.OPEN_WORKSPACE.id);
    };
    FileNavigatorWidget.prototype.doOpenFolder = function () {
        this.commandService.executeCommand(browser_4.WorkspaceCommands.OPEN_FOLDER.id);
    };
    /**
     * Instead of rendering the file resources from the workspace, we render a placeholder
     * button when the workspace root is not yet set.
     */
    FileNavigatorWidget.prototype.renderOpenWorkspaceDiv = function () {
        var openButton;
        if (this.canOpenWorkspaceFileAndFolder) {
            openButton = React.createElement("button", { className: 'theia-button open-workspace-button', title: 'Select a folder or a workspace-file to open as your workspace', onClick: this.openWorkspace, onKeyUp: this.keyUpHandler }, "Open Workspace");
        }
        else {
            openButton = React.createElement("button", { className: 'theia-button open-workspace-button', title: 'Select a folder as your workspace root', onClick: this.openFolder, onKeyUp: this.keyUpHandler }, "Open Folder");
        }
        return React.createElement("div", { className: 'theia-navigator-container' },
            React.createElement("div", { className: 'center' }, "You have not yet opened a workspace."),
            React.createElement("div", { className: 'open-workspace-button-container' }, openButton));
    };
    FileNavigatorWidget.prototype.handleClickEvent = function (node, event) {
        var modifierKeyCombined = core_1.isOSX ? (event.shiftKey || event.metaKey) : (event.shiftKey || event.ctrlKey);
        if (!modifierKeyCombined && node && this.corePreferences['workbench.list.openMode'] === 'singleClick') {
            this.model.previewNode(node);
        }
        _super.prototype.handleClickEvent.call(this, node, event);
    };
    FileNavigatorWidget.prototype.onAfterShow = function (msg) {
        _super.prototype.onAfterShow.call(this, msg);
        this.contextKeyService.explorerViewletVisible.set(true);
    };
    FileNavigatorWidget.prototype.onAfterHide = function (msg) {
        _super.prototype.onAfterHide.call(this, msg);
        this.contextKeyService.explorerViewletVisible.set(false);
    };
    FileNavigatorWidget.prototype.updateSelectionContextKeys = function () {
        this.contextKeyService.explorerResourceIsFolder.set(browser_3.DirNode.is(this.model.selectedNodes[0]));
    };
    __decorate([
        inversify_1.inject(browser_1.CorePreferences),
        __metadata("design:type", Object)
    ], FileNavigatorWidget.prototype, "corePreferences", void 0);
    __decorate([
        inversify_1.inject(navigator_context_key_service_1.NavigatorContextKeyService),
        __metadata("design:type", navigator_context_key_service_1.NavigatorContextKeyService)
    ], FileNavigatorWidget.prototype, "contextKeyService", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], FileNavigatorWidget.prototype, "init", null);
    FileNavigatorWidget = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_2.TreeProps)),
        __param(1, inversify_1.inject(navigator_model_1.FileNavigatorModel)),
        __param(2, inversify_1.inject(browser_2.ContextMenuRenderer)),
        __param(3, inversify_1.inject(common_1.CommandService)),
        __param(4, inversify_1.inject(common_1.SelectionService)),
        __param(5, inversify_1.inject(browser_4.WorkspaceService)),
        __param(6, inversify_1.inject(application_shell_1.ApplicationShell)),
        __param(7, inversify_1.inject(filesystem_1.FileSystem)),
        __metadata("design:paramtypes", [Object, navigator_model_1.FileNavigatorModel, Object, Object, common_1.SelectionService,
            browser_4.WorkspaceService,
            application_shell_1.ApplicationShell, Object])
    ], FileNavigatorWidget);
    return FileNavigatorWidget;
}(browser_3.FileTreeWidget));
exports.FileNavigatorWidget = FileNavigatorWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=18.bundle.js.map