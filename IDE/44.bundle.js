(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[44],{

/***/ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-module.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/filesystem/lib/browser/file-dialog/file-dialog-module.js ***!
  \*************************************************************************************************************/
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var file_dialog_service_1 = __webpack_require__(/*! ./file-dialog-service */ "../../packages/filesystem/lib/browser/file-dialog/file-dialog-service.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    bind(file_dialog_service_1.DefaultFileDialogService).toSelf().inSingletonScope();
    bind(file_dialog_service_1.FileDialogService).toService(file_dialog_service_1.DefaultFileDialogService);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/filesystem/lib/browser/file-dialog/file-dialog-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=44.bundle.js.map