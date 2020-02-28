(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[37],{

/***/ "../../packages/core/lib/browser/menu/browser-context-menu-renderer.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/menu/browser-context-menu-renderer.js ***!
  \***********************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var context_menu_renderer_1 = __webpack_require__(/*! ../context-menu-renderer */ "../../packages/core/lib/browser/context-menu-renderer.js");
var browser_menu_plugin_1 = __webpack_require__(/*! ./browser-menu-plugin */ "../../packages/core/lib/browser/menu/browser-menu-plugin.js");
var BrowserContextMenuRenderer = /** @class */ (function () {
    function BrowserContextMenuRenderer(menuFactory) {
        this.menuFactory = menuFactory;
    }
    BrowserContextMenuRenderer.prototype.render = function (arg, arg2, arg3) {
        var _a = context_menu_renderer_1.RenderContextMenuOptions.resolve(arg, arg2, arg3), menuPath = _a.menuPath, anchor = _a.anchor, args = _a.args, onHide = _a.onHide;
        var contextMenu = this.menuFactory.createContextMenu(menuPath, args);
        var _b = anchor instanceof MouseEvent ? { x: anchor.clientX, y: anchor.clientY } : anchor, x = _b.x, y = _b.y;
        if (onHide) {
            contextMenu.aboutToClose.connect(function () { return onHide(); });
        }
        contextMenu.open(x, y);
    };
    BrowserContextMenuRenderer = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_menu_plugin_1.BrowserMainMenuFactory)),
        __metadata("design:paramtypes", [browser_menu_plugin_1.BrowserMainMenuFactory])
    ], BrowserContextMenuRenderer);
    return BrowserContextMenuRenderer;
}());
exports.BrowserContextMenuRenderer = BrowserContextMenuRenderer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/menu/browser-context-menu-renderer'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/core/lib/browser/menu/browser-menu-module.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/menu/browser-menu-module.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var frontend_application_1 = __webpack_require__(/*! ../frontend-application */ "../../packages/core/lib/browser/frontend-application.js");
var context_menu_renderer_1 = __webpack_require__(/*! ../context-menu-renderer */ "../../packages/core/lib/browser/context-menu-renderer.js");
var browser_menu_plugin_1 = __webpack_require__(/*! ./browser-menu-plugin */ "../../packages/core/lib/browser/menu/browser-menu-plugin.js");
var browser_context_menu_renderer_1 = __webpack_require__(/*! ./browser-context-menu-renderer */ "../../packages/core/lib/browser/menu/browser-context-menu-renderer.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    bind(browser_menu_plugin_1.BrowserMainMenuFactory).toSelf().inSingletonScope();
    bind(context_menu_renderer_1.ContextMenuRenderer).to(browser_context_menu_renderer_1.BrowserContextMenuRenderer).inSingletonScope();
    bind(browser_menu_plugin_1.BrowserMenuBarContribution).toSelf().inSingletonScope();
    bind(frontend_application_1.FrontendApplicationContribution).toService(browser_menu_plugin_1.BrowserMenuBarContribution);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/menu/browser-menu-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=37.bundle.js.map