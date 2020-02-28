(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[32],{

/***/ "../../node_modules/css-loader/index.js!../../packages/navigator/src/browser/style/index.css":
/*!******************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/navigator/src/browser/style/index.css ***!
  \******************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

var escape = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/url/escape.js */ "../../node_modules/css-loader/lib/url/escape.js");
exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n.theia-Files {\n    height: 100%;\n}\n\n.theia-navigator-container {\n    font-size: var(--theia-ui-font-size1);\n    padding: 5px;\n    position: relative;\n}\n\n.theia-navigator-container .open-workspace-button-container {\n    margin: auto;\n    margin-top: 5px;\n    display: flex;\n    justify-content: center;\n    align-self: center;\n}\n\n.theia-navigator-container .center {\n    text-align: center;\n}\n\n.p-Widget .open-workspace-button {\n    padding: 4px 12px;\n    width: calc(100% - var(--theia-ui-padding)*4);\n    margin-left: 0;\n}\n\n.navigator-tab-icon {\n    -webkit-mask: url(" + escape(__webpack_require__(/*! ./files.svg */ "../../packages/navigator/src/browser/style/files.svg")) + ");\n    mask: url(" + escape(__webpack_require__(/*! ./files.svg */ "../../packages/navigator/src/browser/style/files.svg")) + ");\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-container.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-container.js ***!
  \*************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var browser_2 = __webpack_require__(/*! @theia/filesystem/lib/browser */ "../../packages/filesystem/lib/browser/index.js");
var contribution_provider_1 = __webpack_require__(/*! @theia/core/lib/common/contribution-provider */ "../../packages/core/lib/common/contribution-provider.js");
var navigator_tree_1 = __webpack_require__(/*! ./navigator-tree */ "../../packages/navigator/lib/browser/navigator-tree.js");
var navigator_model_1 = __webpack_require__(/*! ./navigator-model */ "../../packages/navigator/lib/browser/navigator-model.js");
var navigator_widget_1 = __webpack_require__(/*! ./navigator-widget */ "../../packages/navigator/lib/browser/navigator-widget.js");
var navigator_contribution_1 = __webpack_require__(/*! ./navigator-contribution */ "../../packages/navigator/lib/browser/navigator-contribution.js");
var navigator_decorator_service_1 = __webpack_require__(/*! ./navigator-decorator-service */ "../../packages/navigator/lib/browser/navigator-decorator-service.js");
exports.FILE_NAVIGATOR_PROPS = __assign({}, browser_1.defaultTreeProps, { contextMenuPath: navigator_contribution_1.NAVIGATOR_CONTEXT_MENU, multiSelect: true, search: true, globalSelection: true });
function createFileNavigatorContainer(parent) {
    var child = browser_2.createFileTreeContainer(parent);
    child.unbind(browser_2.FileTree);
    child.bind(navigator_tree_1.FileNavigatorTree).toSelf();
    child.rebind(browser_1.Tree).toService(navigator_tree_1.FileNavigatorTree);
    child.unbind(browser_2.FileTreeModel);
    child.bind(navigator_model_1.FileNavigatorModel).toSelf();
    child.rebind(browser_1.TreeModel).toService(navigator_model_1.FileNavigatorModel);
    child.unbind(browser_2.FileTreeWidget);
    child.bind(navigator_widget_1.FileNavigatorWidget).toSelf();
    child.rebind(browser_1.TreeProps).toConstantValue(exports.FILE_NAVIGATOR_PROPS);
    child.bind(navigator_decorator_service_1.NavigatorDecoratorService).toSelf().inSingletonScope();
    child.rebind(browser_1.TreeDecoratorService).toService(navigator_decorator_service_1.NavigatorDecoratorService);
    contribution_provider_1.bindContributionProvider(child, navigator_decorator_service_1.NavigatorTreeDecorator);
    return child;
}
exports.createFileNavigatorContainer = createFileNavigatorContainer;
function createFileNavigatorWidget(parent) {
    return createFileNavigatorContainer(parent).get(navigator_widget_1.FileNavigatorWidget);
}
exports.createFileNavigatorWidget = createFileNavigatorWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-container'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-decorator-service.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-decorator-service.js ***!
  \*********************************************************************************************************/
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
var contribution_provider_1 = __webpack_require__(/*! @theia/core/lib/common/contribution-provider */ "../../packages/core/lib/common/contribution-provider.js");
var tree_decorator_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-decorator */ "../../packages/core/lib/browser/tree/tree-decorator.js");
/**
 * Symbol for all decorators that would like to contribute into the navigator.
 */
exports.NavigatorTreeDecorator = Symbol('NavigatorTreeDecorator');
/**
 * Decorator service for the navigator.
 */
var NavigatorDecoratorService = /** @class */ (function (_super) {
    __extends(NavigatorDecoratorService, _super);
    function NavigatorDecoratorService(contributions) {
        var _this = _super.call(this, contributions.getContributions()) || this;
        _this.contributions = contributions;
        return _this;
    }
    NavigatorDecoratorService = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(contribution_provider_1.ContributionProvider)), __param(0, inversify_1.named(exports.NavigatorTreeDecorator)),
        __metadata("design:paramtypes", [Object])
    ], NavigatorDecoratorService);
    return NavigatorDecoratorService;
}(tree_decorator_1.AbstractTreeDecoratorService));
exports.NavigatorDecoratorService = NavigatorDecoratorService;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-decorator-service'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-frontend-module.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-frontend-module.js ***!
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
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/navigator/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var navigator_widget_1 = __webpack_require__(/*! ./navigator-widget */ "../../packages/navigator/lib/browser/navigator-widget.js");
var navigator_keybinding_context_1 = __webpack_require__(/*! ./navigator-keybinding-context */ "../../packages/navigator/lib/browser/navigator-keybinding-context.js");
var navigator_contribution_1 = __webpack_require__(/*! ./navigator-contribution */ "../../packages/navigator/lib/browser/navigator-contribution.js");
var navigator_container_1 = __webpack_require__(/*! ./navigator-container */ "../../packages/navigator/lib/browser/navigator-container.js");
var widget_manager_1 = __webpack_require__(/*! @theia/core/lib/browser/widget-manager */ "../../packages/core/lib/browser/widget-manager.js");
var navigator_preferences_1 = __webpack_require__(/*! ./navigator-preferences */ "../../packages/navigator/lib/browser/navigator-preferences.js");
var navigator_filter_1 = __webpack_require__(/*! ./navigator-filter */ "../../packages/navigator/lib/browser/navigator-filter.js");
var navigator_context_key_service_1 = __webpack_require__(/*! ./navigator-context-key-service */ "../../packages/navigator/lib/browser/navigator-context-key-service.js");
var tab_bar_toolbar_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-toolbar */ "../../packages/core/lib/browser/shell/tab-bar-toolbar.js");
var navigator_diff_1 = __webpack_require__(/*! ./navigator-diff */ "../../packages/navigator/lib/browser/navigator-diff.js");
var navigator_layout_migrations_1 = __webpack_require__(/*! ./navigator-layout-migrations */ "../../packages/navigator/lib/browser/navigator-layout-migrations.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    navigator_preferences_1.bindFileNavigatorPreferences(bind);
    bind(navigator_filter_1.FileNavigatorFilter).toSelf().inSingletonScope();
    bind(navigator_context_key_service_1.NavigatorContextKeyService).toSelf().inSingletonScope();
    browser_1.bindViewContribution(bind, navigator_contribution_1.FileNavigatorContribution);
    bind(browser_1.FrontendApplicationContribution).toService(navigator_contribution_1.FileNavigatorContribution);
    bind(tab_bar_toolbar_1.TabBarToolbarContribution).toService(navigator_contribution_1.FileNavigatorContribution);
    bind(browser_1.KeybindingContext).to(navigator_keybinding_context_1.NavigatorActiveContext).inSingletonScope();
    bind(navigator_widget_1.FileNavigatorWidget).toDynamicValue(function (ctx) {
        return navigator_container_1.createFileNavigatorWidget(ctx.container);
    });
    bind(widget_manager_1.WidgetFactory).toDynamicValue(function (_a) {
        var container = _a.container;
        return ({
            id: navigator_widget_1.FILE_NAVIGATOR_ID,
            createWidget: function () { return container.get(navigator_widget_1.FileNavigatorWidget); }
        });
    }).inSingletonScope();
    bind(widget_manager_1.WidgetFactory).toDynamicValue(function (_a) {
        var container = _a.container;
        return ({
            id: navigator_widget_1.EXPLORER_VIEW_CONTAINER_ID,
            createWidget: function () { return __awaiter(_this, void 0, void 0, function () {
                var viewContainer, widget;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            viewContainer = container.get(browser_1.ViewContainer.Factory)({ id: navigator_widget_1.EXPLORER_VIEW_CONTAINER_ID });
                            viewContainer.setTitleOptions(navigator_widget_1.EXPLORER_VIEW_CONTAINER_TITLE_OPTIONS);
                            return [4 /*yield*/, container.get(widget_manager_1.WidgetManager).getOrCreateWidget(navigator_widget_1.FILE_NAVIGATOR_ID)];
                        case 1:
                            widget = _a.sent();
                            viewContainer.addWidget(widget, {
                                canHide: false,
                                initiallyCollapsed: false
                            });
                            return [2 /*return*/, viewContainer];
                    }
                });
            }); }
        });
    });
    bind(browser_1.ApplicationShellLayoutMigration).to(navigator_layout_migrations_1.NavigatorLayoutVersion3Migration).inSingletonScope();
    bind(navigator_diff_1.NavigatorDiff).toSelf().inSingletonScope();
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/lib/browser/navigator-layout-migrations.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/lib/browser/navigator-layout-migrations.js ***!
  \*********************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var navigator_widget_1 = __webpack_require__(/*! ./navigator-widget */ "../../packages/navigator/lib/browser/navigator-widget.js");
var NavigatorLayoutVersion3Migration = /** @class */ (function () {
    function NavigatorLayoutVersion3Migration() {
        this.layoutVersion = 3.0;
    }
    NavigatorLayoutVersion3Migration.prototype.onWillInflateWidget = function (desc, _a) {
        var parent = _a.parent;
        if (desc.constructionOptions.factoryId === navigator_widget_1.FILE_NAVIGATOR_ID && !parent) {
            return {
                constructionOptions: {
                    factoryId: navigator_widget_1.EXPLORER_VIEW_CONTAINER_ID
                },
                innerWidgetState: {
                    parts: [
                        {
                            widget: {
                                constructionOptions: {
                                    factoryId: navigator_widget_1.FILE_NAVIGATOR_ID
                                },
                                innerWidgetState: desc.innerWidgetState
                            },
                            partId: {
                                factoryId: navigator_widget_1.FILE_NAVIGATOR_ID
                            },
                            collapsed: false,
                            hidden: false
                        }
                    ],
                    title: navigator_widget_1.EXPLORER_VIEW_CONTAINER_TITLE_OPTIONS
                }
            };
        }
        return undefined;
    };
    NavigatorLayoutVersion3Migration = __decorate([
        inversify_1.injectable()
    ], NavigatorLayoutVersion3Migration);
    return NavigatorLayoutVersion3Migration;
}());
exports.NavigatorLayoutVersion3Migration = NavigatorLayoutVersion3Migration;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/navigator/lib/browser/navigator-layout-migrations'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/navigator/src/browser/style/files.svg":
/*!******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/src/browser/style/files.svg ***!
  \******************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports) {

module.exports = "data:image/svg+xml;base64,PCEtLUNvcHlyaWdodCAoYykgTWljcm9zb2Z0IENvcnBvcmF0aW9uLiBBbGwgcmlnaHRzIHJlc2VydmVkLi0tPgo8IS0tQ29weXJpZ2h0IChDKSAyMDE5IFR5cGVGb3ggYW5kIG90aGVycy4tLT4KPCEtLUxpY2Vuc2VkIHVuZGVyIHRoZSBNSVQgTGljZW5zZS4gU2VlIExpY2Vuc2UudHh0IGluIHRoZSBwcm9qZWN0IHJvb3QgZm9yIGxpY2Vuc2UgaW5mb3JtYXRpb24uLS0+CjxzdmcgZmlsbD0iI0Y2RjZGNiIgaGVpZ2h0PSIyOCIgdmlld0JveD0iMCAwIDI4IDI4IiB3aWR0aD0iMjgiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PHBhdGggZD0ibTE0Ljk2NSA3aC04LjkxNjQycy0yLjA0ODU4LjA3OC0yLjA0ODU4IDJ2MTVzMCAyIDIuMDQ4NTggMmwxMS4yNjcyMi0uMDA0YzIuMDQ4Ni4wMDQgMi4wNDg2LTEuOTk2IDIuMDQ4Ni0xLjk5NnYtMTEuNDkxem0tMS43NDY0IDJ2NWg0LjA5NzJ2MTBoLTExLjI2NzIydi0xNXptNS42NDI4LTZoLTguNjk5M3MtMi4wNjQ5My4wMTYtMi4wODAzIDJoOC4yMDk3di40NTRsNC4wMjY1IDQuNTQ2aDEuMDk1djEyYzIuMDQ4NSAwIDIuMDQ4NS0xLjk5NSAyLjA0ODUtMS45OTV2LTExLjM1N3oiIGZpbGw9IiNGNkY2RjYiLz48L3N2Zz4K"

/***/ }),

/***/ "../../packages/navigator/src/browser/style/index.css":
/*!******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/navigator/src/browser/style/index.css ***!
  \******************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/navigator/src/browser/style/index.css");

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
//# sourceMappingURL=32.bundle.js.map