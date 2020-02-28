(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[36],{

/***/ "../../packages/variable-resolver/lib/browser/common-variable-contribution.js":
/*!******************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/variable-resolver/lib/browser/common-variable-contribution.js ***!
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
var env_variables_1 = __webpack_require__(/*! @theia/core/lib/common/env-variables */ "../../packages/core/lib/common/env-variables/index.js");
var command_1 = __webpack_require__(/*! @theia/core/lib/common/command */ "../../packages/core/lib/common/command.js");
var preference_service_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences/preference-service */ "../../packages/core/lib/browser/preferences/preference-service.js");
var resource_context_key_1 = __webpack_require__(/*! @theia/core/lib/browser/resource-context-key */ "../../packages/core/lib/browser/resource-context-key.js");
var quick_input_service_1 = __webpack_require__(/*! @theia/core/lib/browser/quick-open/quick-input-service */ "../../packages/core/lib/browser/quick-open/quick-input-service.js");
var quick_pick_service_1 = __webpack_require__(/*! @theia/core/lib/common/quick-pick-service */ "../../packages/core/lib/common/quick-pick-service.js");
var CommonVariableContribution = /** @class */ (function () {
    function CommonVariableContribution() {
    }
    CommonVariableContribution.prototype.registerVariables = function (variables) {
        return __awaiter(this, void 0, void 0, function () {
            var execPath;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.env.getExecPath()];
                    case 1:
                        execPath = _a.sent();
                        variables.registerVariable({
                            name: 'execPath',
                            resolve: function () { return execPath; }
                        });
                        variables.registerVariable({
                            name: 'env',
                            resolve: function (_, envVariableName) { return __awaiter(_this, void 0, void 0, function () {
                                var envVariable, _a, envValue;
                                return __generator(this, function (_b) {
                                    switch (_b.label) {
                                        case 0:
                                            _a = envVariableName;
                                            if (!_a) return [3 /*break*/, 2];
                                            return [4 /*yield*/, this.env.getValue(envVariableName)];
                                        case 1:
                                            _a = (_b.sent());
                                            _b.label = 2;
                                        case 2:
                                            envVariable = _a;
                                            envValue = envVariable && envVariable.value;
                                            return [2 /*return*/, envValue || ''];
                                    }
                                });
                            }); }
                        });
                        variables.registerVariable({
                            name: 'config',
                            resolve: function (resourceUri, preferenceName) {
                                if (resourceUri === void 0) { resourceUri = _this.resourceContextKey.get(); }
                                if (!preferenceName) {
                                    return undefined;
                                }
                                return _this.preferences.get(preferenceName, undefined, resourceUri && resourceUri.toString());
                            }
                        });
                        variables.registerVariable({
                            name: 'command',
                            resolve: function (_, command) { return __awaiter(_this, void 0, void 0, function () { var _a; return __generator(this, function (_b) {
                                switch (_b.label) {
                                    case 0:
                                        // eslint-disable-next-line no-return-await
                                        _a = command;
                                        if (!_a) 
                                        // eslint-disable-next-line no-return-await
                                        return [3 /*break*/, 2];
                                        return [4 /*yield*/, this.commands.executeCommand(command)];
                                    case 1:
                                        _a = (_b.sent());
                                        _b.label = 2;
                                    case 2: 
                                    // eslint-disable-next-line no-return-await
                                    return [2 /*return*/, _a];
                                }
                            }); }); }
                        });
                        variables.registerVariable({
                            name: 'input',
                            resolve: function (resourceUri, variable, section) {
                                if (resourceUri === void 0) { resourceUri = _this.resourceContextKey.get(); }
                                return __awaiter(_this, void 0, void 0, function () {
                                    var configuration, inputs, input, elements, _a, _b, option;
                                    var e_1, _c;
                                    return __generator(this, function (_d) {
                                        if (!variable || !section) {
                                            return [2 /*return*/, undefined];
                                        }
                                        configuration = this.preferences.get(section, undefined, resourceUri && resourceUri.toString());
                                        inputs = !!configuration && 'inputs' in configuration ? configuration.inputs : undefined;
                                        input = Array.isArray(inputs) && inputs.find(function (item) { return !!item && item.id === variable; });
                                        if (!input) {
                                            return [2 /*return*/, undefined];
                                        }
                                        if (input.type === 'promptString') {
                                            if (typeof input.description !== 'string') {
                                                return [2 /*return*/, undefined];
                                            }
                                            return [2 /*return*/, this.quickInputService.open({
                                                    prompt: input.description,
                                                    value: input.default
                                                })];
                                        }
                                        if (input.type === 'pickString') {
                                            if (typeof input.description !== 'string' || !Array.isArray(input.options)) {
                                                return [2 /*return*/, undefined];
                                            }
                                            elements = [];
                                            try {
                                                for (_a = __values(input.options), _b = _a.next(); !_b.done; _b = _a.next()) {
                                                    option = _b.value;
                                                    if (typeof option !== 'string') {
                                                        return [2 /*return*/, undefined];
                                                    }
                                                    if (option === input.default) {
                                                        elements.unshift({
                                                            description: 'Default',
                                                            label: option,
                                                            value: option
                                                        });
                                                    }
                                                    else {
                                                        elements.push({
                                                            label: option,
                                                            value: option
                                                        });
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
                                            return [2 /*return*/, this.quickPickService.show(elements, { placeholder: input.description })];
                                        }
                                        if (input.type === 'command') {
                                            if (typeof input.command !== 'string') {
                                                return [2 /*return*/, undefined];
                                            }
                                            return [2 /*return*/, this.commands.executeCommand(input.command, input.args)];
                                        }
                                        return [2 /*return*/, undefined];
                                    });
                                });
                            }
                        });
                        return [2 /*return*/];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(env_variables_1.EnvVariablesServer),
        __metadata("design:type", Object)
    ], CommonVariableContribution.prototype, "env", void 0);
    __decorate([
        inversify_1.inject(command_1.CommandService),
        __metadata("design:type", Object)
    ], CommonVariableContribution.prototype, "commands", void 0);
    __decorate([
        inversify_1.inject(preference_service_1.PreferenceService),
        __metadata("design:type", Object)
    ], CommonVariableContribution.prototype, "preferences", void 0);
    __decorate([
        inversify_1.inject(resource_context_key_1.ResourceContextKey),
        __metadata("design:type", resource_context_key_1.ResourceContextKey)
    ], CommonVariableContribution.prototype, "resourceContextKey", void 0);
    __decorate([
        inversify_1.inject(quick_input_service_1.QuickInputService),
        __metadata("design:type", quick_input_service_1.QuickInputService)
    ], CommonVariableContribution.prototype, "quickInputService", void 0);
    __decorate([
        inversify_1.inject(quick_pick_service_1.QuickPickService),
        __metadata("design:type", Object)
    ], CommonVariableContribution.prototype, "quickPickService", void 0);
    CommonVariableContribution = __decorate([
        inversify_1.injectable()
    ], CommonVariableContribution);
    return CommonVariableContribution;
}());
exports.CommonVariableContribution = CommonVariableContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/variable-resolver/lib/browser/common-variable-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/variable-resolver/lib/browser/variable-resolver-frontend-contribution.js":
/*!*****************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/variable-resolver/lib/browser/variable-resolver-frontend-contribution.js ***!
  \*****************************************************************************************************************************/
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var variable_1 = __webpack_require__(/*! ./variable */ "../../packages/variable-resolver/lib/browser/variable.js");
var variable_quick_open_service_1 = __webpack_require__(/*! ./variable-quick-open-service */ "../../packages/variable-resolver/lib/browser/variable-quick-open-service.js");
exports.LIST_VARIABLES = {
    id: 'variable.list',
    label: 'Variable: List All'
};
var VariableResolverFrontendContribution = /** @class */ (function () {
    function VariableResolverFrontendContribution(contributionProvider, variableRegistry, variableQuickOpenService) {
        this.contributionProvider = contributionProvider;
        this.variableRegistry = variableRegistry;
        this.variableQuickOpenService = variableQuickOpenService;
    }
    VariableResolverFrontendContribution.prototype.onStart = function () {
        var _this = this;
        this.contributionProvider.getContributions().forEach(function (contrib) {
            return contrib.registerVariables(_this.variableRegistry);
        });
    };
    VariableResolverFrontendContribution.prototype.registerCommands = function (commands) {
        var _this = this;
        commands.registerCommand(exports.LIST_VARIABLES, {
            isEnabled: function () { return true; },
            execute: function () { return _this.variableQuickOpenService.open(); }
        });
    };
    VariableResolverFrontendContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(common_1.ContributionProvider)), __param(0, inversify_1.named(variable_1.VariableContribution)),
        __param(1, inversify_1.inject(variable_1.VariableRegistry)),
        __param(2, inversify_1.inject(variable_quick_open_service_1.VariableQuickOpenService)),
        __metadata("design:paramtypes", [Object, variable_1.VariableRegistry,
            variable_quick_open_service_1.VariableQuickOpenService])
    ], VariableResolverFrontendContribution);
    return VariableResolverFrontendContribution;
}());
exports.VariableResolverFrontendContribution = VariableResolverFrontendContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/variable-resolver/lib/browser/variable-resolver-frontend-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/variable-resolver/lib/browser/variable-resolver-frontend-module.js":
/*!***********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/variable-resolver/lib/browser/variable-resolver-frontend-module.js ***!
  \***********************************************************************************************************************/
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
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var variable_1 = __webpack_require__(/*! ./variable */ "../../packages/variable-resolver/lib/browser/variable.js");
var variable_quick_open_service_1 = __webpack_require__(/*! ./variable-quick-open-service */ "../../packages/variable-resolver/lib/browser/variable-quick-open-service.js");
var variable_resolver_frontend_contribution_1 = __webpack_require__(/*! ./variable-resolver-frontend-contribution */ "../../packages/variable-resolver/lib/browser/variable-resolver-frontend-contribution.js");
var variable_resolver_service_1 = __webpack_require__(/*! ./variable-resolver-service */ "../../packages/variable-resolver/lib/browser/variable-resolver-service.js");
var common_variable_contribution_1 = __webpack_require__(/*! ./common-variable-contribution */ "../../packages/variable-resolver/lib/browser/common-variable-contribution.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    var e_1, _a;
    bind(variable_1.VariableRegistry).toSelf().inSingletonScope();
    bind(variable_resolver_service_1.VariableResolverService).toSelf().inSingletonScope();
    core_1.bindContributionProvider(bind, variable_1.VariableContribution);
    bind(variable_resolver_frontend_contribution_1.VariableResolverFrontendContribution).toSelf().inSingletonScope();
    try {
        for (var _b = __values([browser_1.FrontendApplicationContribution, core_1.CommandContribution]), _c = _b.next(); !_c.done; _c = _b.next()) {
            var identifier = _c.value;
            bind(identifier).toService(variable_resolver_frontend_contribution_1.VariableResolverFrontendContribution);
        }
    }
    catch (e_1_1) { e_1 = { error: e_1_1 }; }
    finally {
        try {
            if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
        }
        finally { if (e_1) throw e_1.error; }
    }
    bind(variable_quick_open_service_1.VariableQuickOpenService).toSelf().inSingletonScope();
    bind(common_variable_contribution_1.CommonVariableContribution).toSelf().inSingletonScope();
    bind(variable_1.VariableContribution).toService(common_variable_contribution_1.CommonVariableContribution);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/variable-resolver/lib/browser/variable-resolver-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=36.bundle.js.map