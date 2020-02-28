(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[22],{

/***/ "../../packages/core/lib/browser/menu/browser-menu-plugin.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/menu/browser-menu-plugin.js ***!
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
var widgets_1 = __webpack_require__(/*! @phosphor/widgets */ "../../node_modules/@phosphor/widgets/lib/index.js");
var commands_1 = __webpack_require__(/*! @phosphor/commands */ "../../node_modules/@phosphor/commands/lib/index.js");
var common_1 = __webpack_require__(/*! ../../common */ "../../packages/core/lib/common/index.js");
var keybinding_1 = __webpack_require__(/*! ../keybinding */ "../../packages/core/lib/browser/keybinding.js");
var context_key_service_1 = __webpack_require__(/*! ../context-key-service */ "../../packages/core/lib/browser/context-key-service.js");
var context_menu_context_1 = __webpack_require__(/*! ./context-menu-context */ "../../packages/core/lib/browser/menu/context-menu-context.js");
var widgets_2 = __webpack_require__(/*! ../widgets */ "../../packages/core/lib/browser/widgets/index.js");
var shell_1 = __webpack_require__(/*! ../shell */ "../../packages/core/lib/browser/shell/index.js");
var MenuBarWidget = /** @class */ (function (_super) {
    __extends(MenuBarWidget, _super);
    function MenuBarWidget() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return MenuBarWidget;
}(widgets_1.MenuBar));
exports.MenuBarWidget = MenuBarWidget;
var BrowserMainMenuFactory = /** @class */ (function () {
    function BrowserMainMenuFactory(commandRegistry, keybindingRegistry, menuProvider) {
        this.commandRegistry = commandRegistry;
        this.keybindingRegistry = keybindingRegistry;
        this.menuProvider = menuProvider;
    }
    BrowserMainMenuFactory.prototype.createMenuBar = function () {
        var _this = this;
        var menuBar = new DynamicMenuBarWidget();
        menuBar.id = 'theia:menubar';
        this.fillMenuBar(menuBar);
        var listener = this.keybindingRegistry.onKeybindingsChanged(function () {
            menuBar.clearMenus();
            _this.fillMenuBar(menuBar);
        });
        menuBar.disposed.connect(function () { return listener.dispose(); });
        return menuBar;
    };
    BrowserMainMenuFactory.prototype.fillMenuBar = function (menuBar) {
        var e_1, _a;
        var menuModel = this.menuProvider.getMenu(common_1.MAIN_MENU_BAR);
        var phosphorCommands = this.createPhosphorCommands(menuModel);
        // for the main menu we want all items to be visible.
        phosphorCommands.isVisible = function () { return true; };
        try {
            for (var _b = __values(menuModel.children), _c = _b.next(); !_c.done; _c = _b.next()) {
                var menu = _c.value;
                if (menu instanceof common_1.CompositeMenuNode) {
                    var menuWidget = new DynamicMenuWidget(menu, { commands: phosphorCommands }, this.contextKeyService, this.context);
                    menuBar.addMenu(menuWidget);
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
    BrowserMainMenuFactory.prototype.createContextMenu = function (path, args) {
        var menuModel = this.menuProvider.getMenu(path);
        var phosphorCommands = this.createPhosphorCommands(menuModel, args);
        var contextMenu = new DynamicMenuWidget(menuModel, { commands: phosphorCommands }, this.contextKeyService, this.context);
        return contextMenu;
    };
    BrowserMainMenuFactory.prototype.createPhosphorCommands = function (menu, args) {
        if (args === void 0) { args = []; }
        var commands = new commands_1.CommandRegistry();
        this.addPhosphorCommands(commands, menu, args);
        return commands;
    };
    BrowserMainMenuFactory.prototype.addPhosphorCommands = function (commands, menu, args) {
        var e_2, _a;
        try {
            for (var _b = __values(menu.children), _c = _b.next(); !_c.done; _c = _b.next()) {
                var child = _c.value;
                if (child instanceof common_1.ActionMenuNode) {
                    this.addPhosphorCommand(commands, child, args);
                    if (child.altNode) {
                        this.addPhosphorCommand(commands, child.altNode, args);
                    }
                }
                else if (child instanceof common_1.CompositeMenuNode) {
                    this.addPhosphorCommands(commands, child, args);
                }
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
    BrowserMainMenuFactory.prototype.addPhosphorCommand = function (commands, menu, args) {
        var _this = this;
        var command = this.commandRegistry.getCommand(menu.action.commandId);
        if (!command) {
            return;
        }
        if (commands.hasCommand(command.id)) {
            // several menu items can be registered for the same command in different contexts
            return;
        }
        commands.addCommand(command.id, {
            execute: function () {
                var _a;
                return (_a = _this.commandRegistry).executeCommand.apply(_a, __spread([command.id], args));
            },
            label: menu.label,
            icon: menu.icon,
            isEnabled: function () {
                var _a;
                return (_a = _this.commandRegistry).isEnabled.apply(_a, __spread([command.id], args));
            },
            isVisible: function () {
                var _a;
                return (_a = _this.commandRegistry).isVisible.apply(_a, __spread([command.id], args));
            },
            isToggled: function () {
                var _a;
                return (_a = _this.commandRegistry).isToggled.apply(_a, __spread([command.id], args));
            }
        });
        var bindings = this.keybindingRegistry.getKeybindingsForCommand(command.id);
        /* Only consider the first keybinding. */
        if (bindings.length > 0) {
            var binding = bindings[0];
            var keys = this.keybindingRegistry.acceleratorFor(binding);
            commands.addKeyBinding({
                command: command.id,
                keys: keys,
                selector: '.p-Widget' // We have the Phosphor.JS dependency anyway.
            });
        }
    };
    __decorate([
        inversify_1.inject(common_1.ILogger),
        __metadata("design:type", Object)
    ], BrowserMainMenuFactory.prototype, "logger", void 0);
    __decorate([
        inversify_1.inject(context_key_service_1.ContextKeyService),
        __metadata("design:type", context_key_service_1.ContextKeyService)
    ], BrowserMainMenuFactory.prototype, "contextKeyService", void 0);
    __decorate([
        inversify_1.inject(context_menu_context_1.ContextMenuContext),
        __metadata("design:type", context_menu_context_1.ContextMenuContext)
    ], BrowserMainMenuFactory.prototype, "context", void 0);
    BrowserMainMenuFactory = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(common_1.CommandRegistry)),
        __param(1, inversify_1.inject(keybinding_1.KeybindingRegistry)),
        __param(2, inversify_1.inject(common_1.MenuModelRegistry)),
        __metadata("design:paramtypes", [common_1.CommandRegistry,
            keybinding_1.KeybindingRegistry,
            common_1.MenuModelRegistry])
    ], BrowserMainMenuFactory);
    return BrowserMainMenuFactory;
}());
exports.BrowserMainMenuFactory = BrowserMainMenuFactory;
var DynamicMenuBarWidget = /** @class */ (function (_super) {
    __extends(DynamicMenuBarWidget, _super);
    function DynamicMenuBarWidget() {
        var _this = _super.call(this) || this;
        // HACK we need to hook in on private method _openChildMenu. Don't do this at home!
        DynamicMenuBarWidget.prototype['_openChildMenu'] = function () {
            if (_this.activeMenu instanceof DynamicMenuWidget) {
                _this.activeMenu.aboutToShow();
            }
            _super.prototype['_openChildMenu'].call(_this);
        };
        return _this;
    }
    DynamicMenuBarWidget.prototype.activateMenu = function (label) {
        var labels = [];
        for (var _i = 1; _i < arguments.length; _i++) {
            labels[_i - 1] = arguments[_i];
        }
        return __awaiter(this, void 0, void 0, function () {
            var menu, menuPath, current, _loop_1, labels_1, labels_1_1, itemLabel, e_3_1;
            var e_3, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        menu = this.menus.find(function (m) { return m.title.label === label; });
                        if (!menu) {
                            throw new Error("could not find '" + label + "' menu");
                        }
                        this.activeMenu = menu;
                        this.openActiveMenu();
                        return [4 /*yield*/, widgets_2.waitForRevealed(menu)];
                    case 1:
                        _b.sent();
                        menuPath = [label];
                        current = menu;
                        _loop_1 = function (itemLabel) {
                            var item;
                            return __generator(this, function (_a) {
                                switch (_a.label) {
                                    case 0:
                                        item = current.items.find(function (i) { return i.label === itemLabel; });
                                        if (!item || !item.submenu) {
                                            throw new Error("could not find '" + label + "' submenu in " + menuPath.map(function (l) { return "'" + l + "'"; }).join(' -> ') + " menu");
                                        }
                                        current.activeItem = item;
                                        current.triggerActiveItem();
                                        current = item.submenu;
                                        return [4 /*yield*/, widgets_2.waitForRevealed(current)];
                                    case 1:
                                        _a.sent();
                                        return [2 /*return*/];
                                }
                            });
                        };
                        _b.label = 2;
                    case 2:
                        _b.trys.push([2, 7, 8, 9]);
                        labels_1 = __values(labels), labels_1_1 = labels_1.next();
                        _b.label = 3;
                    case 3:
                        if (!!labels_1_1.done) return [3 /*break*/, 6];
                        itemLabel = labels_1_1.value;
                        return [5 /*yield**/, _loop_1(itemLabel)];
                    case 4:
                        _b.sent();
                        _b.label = 5;
                    case 5:
                        labels_1_1 = labels_1.next();
                        return [3 /*break*/, 3];
                    case 6: return [3 /*break*/, 9];
                    case 7:
                        e_3_1 = _b.sent();
                        e_3 = { error: e_3_1 };
                        return [3 /*break*/, 9];
                    case 8:
                        try {
                            if (labels_1_1 && !labels_1_1.done && (_a = labels_1.return)) _a.call(labels_1);
                        }
                        finally { if (e_3) throw e_3.error; }
                        return [7 /*endfinally*/];
                    case 9: return [2 /*return*/, current];
                }
            });
        });
    };
    DynamicMenuBarWidget.prototype.triggerMenuItem = function (label) {
        var labels = [];
        for (var _i = 1; _i < arguments.length; _i++) {
            labels[_i - 1] = arguments[_i];
        }
        return __awaiter(this, void 0, void 0, function () {
            var menuPath, menu, item;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!labels.length) {
                            throw new Error('menu item label is not specified');
                        }
                        menuPath = __spread([label], labels.slice(0, labels.length - 1));
                        return [4 /*yield*/, this.activateMenu.apply(this, __spread([menuPath[0]], menuPath.slice(1)))];
                    case 1:
                        menu = _a.sent();
                        item = menu.items.find(function (i) { return i.label === labels[labels.length - 1]; });
                        if (!item) {
                            throw new Error("could not find '" + label + "' item in " + menuPath.map(function (l) { return "'" + l + "'"; }).join(' -> ') + " menu");
                        }
                        menu.activeItem = item;
                        menu.triggerActiveItem();
                        return [2 /*return*/, item];
                }
            });
        });
    };
    return DynamicMenuBarWidget;
}(MenuBarWidget));
/**
 * A menu widget that would recompute its items on update
 */
var DynamicMenuWidget = /** @class */ (function (_super) {
    __extends(DynamicMenuWidget, _super);
    function DynamicMenuWidget(menu, options, contextKeyService, context) {
        var _this = _super.call(this, options) || this;
        _this.menu = menu;
        _this.options = options;
        _this.contextKeyService = contextKeyService;
        _this.context = context;
        if (menu.label) {
            _this.title.label = menu.label;
        }
        if (menu.iconClass) {
            _this.title.iconClass = menu.iconClass;
        }
        _this.updateSubMenus(_this, _this.menu, _this.options.commands);
        return _this;
    }
    DynamicMenuWidget.prototype.aboutToShow = function () {
        this.clearItems();
        this.updateSubMenus(this, this.menu, this.options.commands);
    };
    DynamicMenuWidget.prototype.open = function (x, y, options) {
        var _this = this;
        // we want to restore the focus after the menu closes.
        var previouslyActive = window.document.activeElement;
        var cb = function () {
            previouslyActive.focus({ preventScroll: true });
            _this.aboutToClose.disconnect(cb);
        };
        this.aboutToClose.connect(cb);
        _super.prototype.open.call(this, x, y, options);
    };
    DynamicMenuWidget.prototype.updateSubMenus = function (parent, menu, commands) {
        var e_4, _a;
        var items = this.buildSubMenus([], menu, commands);
        try {
            for (var items_1 = __values(items), items_1_1 = items_1.next(); !items_1_1.done; items_1_1 = items_1.next()) {
                var item = items_1_1.value;
                parent.addItem(item);
            }
        }
        catch (e_4_1) { e_4 = { error: e_4_1 }; }
        finally {
            try {
                if (items_1_1 && !items_1_1.done && (_a = items_1.return)) _a.call(items_1);
            }
            finally { if (e_4) throw e_4.error; }
        }
    };
    DynamicMenuWidget.prototype.buildSubMenus = function (items, menu, commands) {
        var e_5, _a;
        try {
            for (var _b = __values(menu.children), _c = _b.next(); !_c.done; _c = _b.next()) {
                var item = _c.value;
                if (item instanceof common_1.CompositeMenuNode) {
                    if (item.children.length > 0) {
                        // do not render empty nodes
                        if (item.isSubmenu) { // submenu node
                            var submenu = new DynamicMenuWidget(item, this.options, this.contextKeyService, this.context);
                            if (submenu.items.length === 0) {
                                continue;
                            }
                            items.push({
                                type: 'submenu',
                                submenu: submenu,
                            });
                        }
                        else { // group node
                            var submenu = this.buildSubMenus([], item, commands);
                            if (submenu.length === 0) {
                                continue;
                            }
                            if (items.length > 0) {
                                // do not put a separator above the first group
                                items.push({
                                    type: 'separator'
                                });
                            }
                            // render children
                            items.push.apply(items, __spread(submenu));
                        }
                    }
                }
                else if (item instanceof common_1.ActionMenuNode) {
                    var node = item.altNode && this.context.altPressed ? item.altNode : item;
                    var when = node.action.when;
                    if (!(commands.isVisible(node.action.commandId) && (!when || this.contextKeyService.match(when)))) {
                        continue;
                    }
                    items.push({
                        command: node.action.commandId,
                        type: 'command'
                    });
                }
            }
        }
        catch (e_5_1) { e_5 = { error: e_5_1 }; }
        finally {
            try {
                if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
            }
            finally { if (e_5) throw e_5.error; }
        }
        return items;
    };
    return DynamicMenuWidget;
}(widgets_1.Menu));
var BrowserMenuBarContribution = /** @class */ (function () {
    function BrowserMenuBarContribution(factory) {
        this.factory = factory;
    }
    BrowserMenuBarContribution.prototype.onStart = function (app) {
        var logo = this.createLogo();
        app.shell.addWidget(logo, { area: 'top' });
        var menu = this.factory.createMenuBar();
        app.shell.addWidget(menu, { area: 'top' });
    };
    Object.defineProperty(BrowserMenuBarContribution.prototype, "menuBar", {
        get: function () {
            return this.shell.topPanel.widgets.find(function (w) { return w instanceof MenuBarWidget; });
        },
        enumerable: true,
        configurable: true
    });
    BrowserMenuBarContribution.prototype.createLogo = function () {
        var logo = new widgets_1.Widget();
        logo.id = 'theia:icon';
        logo.addClass('theia-icon');
        return logo;
    };
    __decorate([
        inversify_1.inject(shell_1.ApplicationShell),
        __metadata("design:type", shell_1.ApplicationShell)
    ], BrowserMenuBarContribution.prototype, "shell", void 0);
    BrowserMenuBarContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(BrowserMainMenuFactory)),
        __metadata("design:paramtypes", [BrowserMainMenuFactory])
    ], BrowserMenuBarContribution);
    return BrowserMenuBarContribution;
}());
exports.BrowserMenuBarContribution = BrowserMenuBarContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/menu/browser-menu-plugin'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=22.bundle.js.map