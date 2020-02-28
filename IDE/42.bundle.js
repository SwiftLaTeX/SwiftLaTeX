(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[42],{

/***/ "../../packages/core/lib/browser/keyboard/browser-keyboard-module.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/core/lib/browser/keyboard/browser-keyboard-module.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var command_1 = __webpack_require__(/*! ../../common/command */ "../../packages/core/lib/common/command.js");
var keyboard_layout_provider_1 = __webpack_require__(/*! ../../common/keyboard/keyboard-layout-provider */ "../../packages/core/lib/common/keyboard/keyboard-layout-provider.js");
var browser_keyboard_layout_provider_1 = __webpack_require__(/*! ./browser-keyboard-layout-provider */ "../../packages/core/lib/browser/keyboard/browser-keyboard-layout-provider.js");
var browser_keyboard_frontend_contribution_1 = __webpack_require__(/*! ./browser-keyboard-frontend-contribution */ "../../packages/core/lib/browser/keyboard/browser-keyboard-frontend-contribution.js");
exports.default = new inversify_1.ContainerModule(function (bind, unbind, isBound, rebind) {
    bind(browser_keyboard_layout_provider_1.BrowserKeyboardLayoutProvider).toSelf().inSingletonScope();
    bind(keyboard_layout_provider_1.KeyboardLayoutProvider).toService(browser_keyboard_layout_provider_1.BrowserKeyboardLayoutProvider);
    bind(keyboard_layout_provider_1.KeyboardLayoutChangeNotifier).toService(browser_keyboard_layout_provider_1.BrowserKeyboardLayoutProvider);
    bind(keyboard_layout_provider_1.KeyValidator).toService(browser_keyboard_layout_provider_1.BrowserKeyboardLayoutProvider);
    bind(browser_keyboard_frontend_contribution_1.BrowserKeyboardFrontendContribution).toSelf().inSingletonScope();
    bind(command_1.CommandContribution).toService(browser_keyboard_frontend_contribution_1.BrowserKeyboardFrontendContribution);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/core/lib/browser/keyboard/browser-keyboard-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=42.bundle.js.map