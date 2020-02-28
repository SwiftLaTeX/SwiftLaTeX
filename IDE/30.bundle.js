(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[30],{

/***/ "../../node_modules/css-loader/index.js!../../packages/messages/src/browser/style/index.css":
/*!*****************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/messages/src/browser/style/index.css ***!
  \*****************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports
exports.i(__webpack_require__(/*! -!../../../../../node_modules/css-loader!./notifications.css */ "../../node_modules/css-loader/index.js!../../packages/messages/src/browser/style/notifications.css"), "");

// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n", ""]);

// exports


/***/ }),

/***/ "../../node_modules/css-loader/index.js!../../packages/messages/src/browser/style/notifications.css":
/*!*************************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/messages/src/browser/style/notifications.css ***!
  \*************************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2019 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n/* Container */\n\n.theia-notifications-overlay {\n    height: 0px;\n}\n\n.theia-notifications-container {\n    position: absolute;\n    bottom: 36px;\n    right: 16px;\n    width: 500px;\n    user-select: none;\n    z-index: 1111;\n}\n.theia-notifications-container.closed {\n    display: none;\n}\n.theia-notifications-container > * {\n    position: relative;\n}\n\n/* Toasts */\n\n.theia-notifications-container.theia-notification-toasts .theia-notification-list-item {\n    background-color: var(--theia-notifications-background);\n    box-shadow: 0px 0px 4px 0px var(--theia-widget-shadow);\n    border: 1px solid var(--theia-notificationToast-border);\n    margin-top: 6px;\n}\n\n/* Center */\n\n.theia-notifications-container.theia-notification-center {\n    background-color: var(--theia-notifications-background);\n    border: 1px solid var(--theia-notificationCenter-border);\n    box-shadow: 0px 0px 6px 0px var(--theia-widget-shadow);\n}\n\n/* Center > Header */\n\n.theia-notification-center-header {\n    color: var(--theia-notificationCenterHeader-foreground);\n    background-color: var(--theia-notificationCenterHeader-background);\n    display: flex;\n    flex-direction: row;\n    flex-wrap: nowrap;\n    justify-content: flex-end;\n    min-height: 30px;\n    align-items: center;\n}\n\n.theia-notification-center-header-title {\n    font-size: calc(var(--theia-ui-font-size1) / 1.1);\n    font-family: var(--theia-ui-font-family);\n    margin: 8px;\n    flex-grow: 2;\n}\n\n.theia-notification-center-header-actions {\n    margin: 8px;\n}\n\n/* List */\n\n.theia-notification-list-scroll-container {\n    max-height: 300px;\n    overflow: auto;\n}\n\n.theia-notification-list {\n    display: flex;\n    flex-direction: column-reverse;\n    flex-wrap: nowrap;\n}\n\n\n/* List > Item */\n\n.theia-notification-list-item {\n    width: 100%;\n    cursor: pointer;\n    flex-grow: 1;\n    display: flex;\n    flex-direction: column;\n    flex-wrap: nowrap;\n    justify-content: space-between;\n}\n.theia-notification-list-item:hover {\n    background-color: var(--theia-list-hoverBackground);\n}\n.theia-notification-list > .theia-notification-list-item:not(:last-child) {\n    border-top: 1px var(--theia-notifications-border) solid;\n}\n\n.theia-notification-list-item-content {\n    margin: 6px;\n    flex-grow: 3;\n}\n.theia-notification-list-item-content.collapsed .theia-notification-list-item-content-bottom {\n    display: none;\n}\n.theia-notification-list-item-content.collapsed .theia-notification-message {\n    overflow: hidden;\n    text-overflow: ellipsis;\n    white-space: nowrap;\n}\n\n.theia-notification-list-item-content-main {\n    display: flex;\n    flex-direction: row;\n    flex-wrap: nowrap;\n    justify-content: space-between;\n    padding: 8px 4px 0px 4px;\n}\n\n.theia-notification-message {\n    font-size: var(--theia-ui-font-size1);\n    font-family: var(--theia-ui-font-family);\n    overflow-wrap: break-word;\n    box-sizing: border-box;\n    flex-basis: 0%;\n    flex-grow: 1;\n    flex-shrink: 1;\n    display: block;\n    overflow: hidden;\n    user-select: text;\n}\n\n.theia-notification-message a {\n    border: none;\n    color: var(--theia-notificationLink-foreground);\n    outline: 0;\n    text-decoration: none;\n}\n.theia-notification-message a:focus {\n    outline-color: var(--theia-focusBorder);\n}\n\n.theia-notification-icon {\n    font-family: FontAwesome;\n    display: inline-block;\n    padding-right: 4px;\n    vertical-align: middle;\n    width: 24px;\n    height: 24px;\n}\n\n.theia-notification-icon-info:before {\n    content: \"\\F05A\";\n    color: var(--theia-notificationsInfoIcon-foreground);\n}\n\n.theia-notification-icon-warning:before {\n    content: \"\\F071\";\n    color: var(--theia-notificationsWarningIcon-foreground);\n}\n\n.theia-notification-icon-error:before {\n    content: \"\\F057\";\n    color: var(--theia-notificationsErrorIcon-foreground);\n}\n\n.theia-notification-actions {\n    display: flex;\n    flex-direction: row;\n    flex-wrap: nowrap;\n    justify-content: space-between;\n    margin: 0px;\n    padding: 0px;\n}\n\n.theia-notification-actions > li {\n    display: inline-block;\n    height: 16px;\n    width: 16px;\n    cursor: pointer;\n    margin-left: 8px;\n}\n\n.theia-notification-actions > .clear-all {\n    background: var(--theia-icon-close-all) no-repeat center center;\n}\n.theia-notification-actions > .clear-all:active {\n    transform: scale(1.1);\n}\n\n.theia-notification-actions > .clear {\n    background: var(--theia-icon-close) no-repeat center center;\n}\n.theia-notification-actions > .clear:active {\n    transform: scale(1.2);\n}\n\n.theia-notification-actions > .expand {\n    background: var(--theia-icon-collapse) no-repeat center center;\n    transform: rotate(0deg) scale(1.5);\n}\n.theia-notification-actions > .expand:active {\n    transform: rotate(0deg) scale(1.8);\n}\n\n.theia-notification-actions > .collapse {\n    background: var(--theia-icon-collapse) no-repeat center center;\n    transform: rotate(90deg) scale(1.5);\n}\n.theia-notification-actions > .collapse:active {\n    transform: rotate(90deg) scale(1.8);\n}\n\n.theia-notification-list-item-content-bottom {\n    display: flex;\n    flex-direction: row;\n    flex-wrap: wrap;\n    justify-content: space-between;\n}\n\n.theia-notification-source {\n    font-size: var(--theia-ui-font-size0);\n    font-family: var(--theia-ui-font-family);\n    overflow-wrap: break-word;\n    box-sizing: border-box;\n    flex-grow: 1;\n    padding: 4px;\n    display: block;\n    overflow: hidden;\n}\n\n.theia-notification-buttons {\n    flex-grow: 2;\n    display: flex;\n    flex-direction: row;\n    justify-content: flex-end;\n    flex-wrap: wrap;\n}\n\n.theia-notification-buttons > button {\n    margin: 4px;\n    max-width: 160px;\n    text-overflow: ellipsis;\n    white-space: nowrap;\n    overflow: hidden;\n}\n\n.theia-notification-item-progress {\n    display: block;\n}\n\n.theia-notification-item-progressbar {\n    height: 2px;\n    background-color: var(--theia-progressBar-background);\n    width: 66%;\n}\n\n/* Perfect scrollbar */\n\n.theia-notification-list-scroll-container .ps__rail-y {\n    width: var(--theia-scrollbar-rail-width);\n}\n\n.theia-notification-list-scroll-container .ps__rail-y:hover > .ps__thumb-y,\n.theia-notification-list-scroll-container .ps__rail-y:focus > .ps__thumb-y,\n.theia-notification-list-scroll-container .ps__rail-y.ps--clicking .ps__thumb-y {\n    right: calc((var(--theia-scrollbar-rail-width) - var(--theia-scrollbar-width)) / 2);\n    width: var(--theia-scrollbar-width);\n}\n\n.theia-notification-list-scroll-container .ps__rail-y > .ps__thumb-y {\n    width: var(--theia-scrollbar-width);\n    right: calc((var(--theia-scrollbar-rail-width) - var(--theia-scrollbar-width)) / 2);\n    background: var(--theia-scrollbarSlider-background);\n    border-radius: 0px;\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/messages/lib/browser/messages-frontend-module.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/messages-frontend-module.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/messages/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var notifications_manager_1 = __webpack_require__(/*! ./notifications-manager */ "../../packages/messages/lib/browser/notifications-manager.js");
var notification_preferences_1 = __webpack_require__(/*! ./notification-preferences */ "../../packages/messages/lib/browser/notification-preferences.js");
var notifications_renderer_1 = __webpack_require__(/*! ./notifications-renderer */ "../../packages/messages/lib/browser/notifications-renderer.js");
var notifications_contribution_1 = __webpack_require__(/*! ./notifications-contribution */ "../../packages/messages/lib/browser/notifications-contribution.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var color_application_contribution_1 = __webpack_require__(/*! @theia/core/lib/browser/color-application-contribution */ "../../packages/core/lib/browser/color-application-contribution.js");
exports.default = new inversify_1.ContainerModule(function (bind, unbind, isBound, rebind) {
    bind(notifications_renderer_1.NotificationsRenderer).toSelf().inSingletonScope();
    bind(notifications_contribution_1.NotificationsContribution).toSelf().inSingletonScope();
    bind(browser_1.FrontendApplicationContribution).toService(notifications_contribution_1.NotificationsContribution);
    bind(core_1.CommandContribution).toService(notifications_contribution_1.NotificationsContribution);
    bind(browser_1.KeybindingContribution).toService(notifications_contribution_1.NotificationsContribution);
    bind(color_application_contribution_1.ColorContribution).toService(notifications_contribution_1.NotificationsContribution);
    bind(notifications_contribution_1.NotificationsKeybindingContext).toSelf().inSingletonScope();
    bind(browser_1.KeybindingContext).toService(notifications_contribution_1.NotificationsKeybindingContext);
    bind(notifications_manager_1.NotificationManager).toSelf().inSingletonScope();
    rebind(common_1.MessageClient).toService(notifications_manager_1.NotificationManager);
    notification_preferences_1.bindNotificationPreferences(bind);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/messages-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notification-center-component.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notification-center-component.js ***!
  \**********************************************************************************************************/
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
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var notification_component_1 = __webpack_require__(/*! ./notification-component */ "../../packages/messages/lib/browser/notification-component.js");
var PerfectScrollbar = __webpack_require__(/*! react-perfect-scrollbar */ "../../node_modules/react-perfect-scrollbar/lib/index.js");
var NotificationCenterComponent = /** @class */ (function (_super) {
    __extends(NotificationCenterComponent, _super);
    function NotificationCenterComponent(props) {
        var _this = _super.call(this, props) || this;
        _this.toDisposeOnUnmount = new core_1.DisposableCollection();
        _this.onHide = function () {
            _this.props.manager.hideCenter();
        };
        _this.onClearAll = function () {
            _this.props.manager.clearAll();
        };
        _this.state = {
            notifications: [],
            visibilityState: 'hidden'
        };
        return _this;
    }
    NotificationCenterComponent.prototype.componentDidMount = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                this.toDisposeOnUnmount.push(this.props.manager.onUpdated(function (_a) {
                    var notifications = _a.notifications, visibilityState = _a.visibilityState;
                    _this.setState({
                        notifications: notifications,
                        visibilityState: visibilityState
                    });
                }));
                return [2 /*return*/];
            });
        });
    };
    NotificationCenterComponent.prototype.componentWillUnmount = function () {
        this.toDisposeOnUnmount.dispose();
    };
    NotificationCenterComponent.prototype.render = function () {
        var _this = this;
        var empty = this.state.notifications.length === 0;
        var title = empty ? 'NO NEW NOTIFICATIONS' : 'NOTIFICATIONS';
        return (React.createElement("div", { className: "theia-notifications-container theia-notification-center " + (this.state.visibilityState === 'center' ? 'open' : 'closed') },
            React.createElement("div", { className: 'theia-notification-center-header' },
                React.createElement("div", { className: 'theia-notification-center-header-title' }, title),
                React.createElement("div", { className: 'theia-notification-center-header-actions' },
                    React.createElement("ul", { className: 'theia-notification-actions' },
                        React.createElement("li", { className: 'collapse', title: 'Hide Notification Center', onClick: this.onHide }),
                        React.createElement("li", { className: 'clear-all', title: 'Clear All', onClick: this.onClearAll })))),
            React.createElement(PerfectScrollbar, { className: 'theia-notification-list-scroll-container' },
                React.createElement("div", { className: 'theia-notification-list' }, this.state.notifications.map(function (notification) {
                    return React.createElement(notification_component_1.NotificationComponent, { key: notification.messageId, notification: notification, manager: _this.props.manager });
                })))));
    };
    return NotificationCenterComponent;
}(React.Component));
exports.NotificationCenterComponent = NotificationCenterComponent;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notification-center-component'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notification-component.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notification-component.js ***!
  \***************************************************************************************************/
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
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var NotificationComponent = /** @class */ (function (_super) {
    __extends(NotificationComponent, _super);
    function NotificationComponent(props) {
        var _this = _super.call(this, props) || this;
        _this.onClear = function (event) {
            if (event.target instanceof HTMLElement) {
                var messageId = event.target.dataset.messageId;
                if (messageId) {
                    _this.props.manager.clear(messageId);
                }
            }
        };
        _this.onToggleExpansion = function (event) {
            if (event.target instanceof HTMLElement) {
                var messageId = event.target.dataset.messageId;
                if (messageId) {
                    _this.props.manager.toggleExpansion(messageId);
                }
            }
        };
        _this.onAction = function (event) {
            if (event.target instanceof HTMLElement) {
                var messageId = event.target.dataset.messageId;
                var action = event.target.dataset.action;
                if (messageId && action) {
                    _this.props.manager.accept(messageId, action);
                }
            }
        };
        _this.onMessageClick = function (event) {
            if (event.target instanceof HTMLAnchorElement) {
                event.stopPropagation();
                event.preventDefault();
                var link = event.target.href;
                _this.props.manager.openLink(link);
            }
        };
        _this.state = {};
        return _this;
    }
    NotificationComponent.prototype.render = function () {
        var _this = this;
        var _a = this.props.notification, messageId = _a.messageId, message = _a.message, type = _a.type, progress = _a.progress, collapsed = _a.collapsed, expandable = _a.expandable, source = _a.source, actions = _a.actions;
        return (React.createElement("div", { key: messageId, className: 'theia-notification-list-item' },
            React.createElement("div", { className: "theia-notification-list-item-content " + (collapsed ? 'collapsed' : '') },
                React.createElement("div", { className: 'theia-notification-list-item-content-main' },
                    React.createElement("div", { className: "theia-notification-icon theia-notification-icon-" + type }),
                    React.createElement("div", { className: 'theia-notification-message' },
                        React.createElement("span", { dangerouslySetInnerHTML: { __html: message }, onClick: this.onMessageClick })),
                    React.createElement("ul", { className: 'theia-notification-actions' },
                        expandable && (React.createElement("li", { className: collapsed ? 'expand' : 'collapse', title: collapsed ? 'Expand' : 'Collapse', "data-message-id": messageId, onClick: this.onToggleExpansion })),
                        React.createElement("li", { className: 'clear', title: 'Clear', "data-message-id": messageId, onClick: this.onClear }))),
                (source || !!actions.length) && (React.createElement("div", { className: 'theia-notification-list-item-content-bottom' },
                    React.createElement("div", { className: 'theia-notification-source' }, source && (React.createElement("span", null, source))),
                    React.createElement("div", { className: 'theia-notification-buttons' }, actions && actions.map(function (action, index) { return (React.createElement("button", { key: messageId + ("-action-" + index), className: 'theia-button', "data-message-id": messageId, "data-action": action, onClick: _this.onAction }, action)); }))))),
            typeof progress === 'number' && (React.createElement("div", { className: 'theia-notification-item-progress' },
                React.createElement("div", { className: 'theia-notification-item-progressbar', style: { width: progress + "%" } })))));
    };
    return NotificationComponent;
}(React.Component));
exports.NotificationComponent = NotificationComponent;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notification-component'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notification-preferences.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notification-preferences.js ***!
  \*****************************************************************************************************/
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
var preferences_1 = __webpack_require__(/*! @theia/core/lib/browser/preferences */ "../../packages/core/lib/browser/preferences/index.js");
exports.NotificationConfigSchema = {
    'type': 'object',
    'properties': {
        'notification.timeout': {
            'type': 'number',
            'description': 'Informative notifications will be hidden after this timeout.',
            'default': 30 * 1000 // `0` and negative values are treated as no timeout.
        }
    }
};
exports.NotificationPreferences = Symbol('NotificationPreferences');
function createNotificationPreferences(preferences) {
    return preferences_1.createPreferenceProxy(preferences, exports.NotificationConfigSchema);
}
exports.createNotificationPreferences = createNotificationPreferences;
function bindNotificationPreferences(bind) {
    bind(exports.NotificationPreferences).toDynamicValue(function (ctx) {
        var preferences = ctx.container.get(preferences_1.PreferenceService);
        return createNotificationPreferences(preferences);
    });
    bind(preferences_1.PreferenceContribution).toConstantValue({ schema: exports.NotificationConfigSchema });
}
exports.bindNotificationPreferences = bindNotificationPreferences;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notification-preferences'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notification-toasts-component.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notification-toasts-component.js ***!
  \**********************************************************************************************************/
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
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var notification_component_1 = __webpack_require__(/*! ./notification-component */ "../../packages/messages/lib/browser/notification-component.js");
var NotificationToastsComponent = /** @class */ (function (_super) {
    __extends(NotificationToastsComponent, _super);
    function NotificationToastsComponent(props) {
        var _this = _super.call(this, props) || this;
        _this.toDisposeOnUnmount = new core_1.DisposableCollection();
        _this.state = {
            toasts: [],
            visibilityState: 'hidden'
        };
        return _this;
    }
    NotificationToastsComponent.prototype.componentDidMount = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                this.toDisposeOnUnmount.push(this.props.manager.onUpdated(function (_a) {
                    var toasts = _a.toasts, visibilityState = _a.visibilityState;
                    _this.setState({
                        toasts: toasts.slice(-3),
                        visibilityState: visibilityState
                    });
                }));
                return [2 /*return*/];
            });
        });
    };
    NotificationToastsComponent.prototype.componentWillUnmount = function () {
        this.toDisposeOnUnmount.dispose();
    };
    NotificationToastsComponent.prototype.render = function () {
        var _this = this;
        return (React.createElement("div", { className: "theia-notifications-container theia-notification-toasts " + (this.state.visibilityState === 'toasts' ? 'open' : 'closed') },
            React.createElement("div", { className: 'theia-notification-list' }, this.state.toasts.map(function (notification) { return React.createElement(notification_component_1.NotificationComponent, { key: notification.messageId, notification: notification, manager: _this.props.manager }); }))));
    };
    return NotificationToastsComponent;
}(React.Component));
exports.NotificationToastsComponent = NotificationToastsComponent;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notification-toasts-component'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notifications-commands.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notifications-commands.js ***!
  \***************************************************************************************************/
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
var NotificationsCommands;
(function (NotificationsCommands) {
    var NOTIFICATIONS_CATEGORY = 'Notifications';
    NotificationsCommands.TOGGLE = {
        id: 'notifications.commands.toggle',
        category: NOTIFICATIONS_CATEGORY,
        iconClass: 'fa fa-th-list',
        label: 'Toggle Notifications'
    };
    NotificationsCommands.HIDE = {
        id: 'notifications.commands.hide',
    };
    NotificationsCommands.CLEAR_ALL = {
        id: 'notifications.commands.clearAll',
        category: NOTIFICATIONS_CATEGORY,
        iconClass: 'fa fa-times-circle',
        label: 'Clear All Notifications'
    };
})(NotificationsCommands = exports.NotificationsCommands || (exports.NotificationsCommands = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notifications-commands'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notifications-contribution.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notifications-contribution.js ***!
  \*******************************************************************************************************/
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
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var notifications_commands_1 = __webpack_require__(/*! ./notifications-commands */ "../../packages/messages/lib/browser/notifications-commands.js");
var notifications_manager_1 = __webpack_require__(/*! ./notifications-manager */ "../../packages/messages/lib/browser/notifications-manager.js");
var notifications_renderer_1 = __webpack_require__(/*! ./notifications-renderer */ "../../packages/messages/lib/browser/notifications-renderer.js");
var color_registry_1 = __webpack_require__(/*! @theia/core/lib/browser/color-registry */ "../../packages/core/lib/browser/color-registry.js");
var NotificationsContribution = /** @class */ (function () {
    function NotificationsContribution() {
        this.id = 'theia-notification-center';
    }
    NotificationsContribution.prototype.onStart = function (_app) {
        this.createStatusBarItem();
    };
    NotificationsContribution.prototype.createStatusBarItem = function () {
        var _this = this;
        this.updateStatusBarItem();
        this.manager.onUpdated(function (e) { return _this.updateStatusBarItem(e.notifications.length); });
    };
    NotificationsContribution.prototype.updateStatusBarItem = function (count) {
        if (count === void 0) { count = 0; }
        this.statusBar.setElement(this.id, {
            text: this.getStatusBarItemText(count),
            alignment: browser_1.StatusBarAlignment.RIGHT,
            priority: -900,
            command: notifications_commands_1.NotificationsCommands.TOGGLE.id,
            tooltip: this.getStatusBarItemTooltip(count)
        });
    };
    NotificationsContribution.prototype.getStatusBarItemText = function (count) {
        return "$(bell) " + (count ? " " + count : '');
    };
    NotificationsContribution.prototype.getStatusBarItemTooltip = function (count) {
        if (this.manager.centerVisible) {
            return 'Hide Notifications';
        }
        return count === 0
            ? 'No Notifications'
            : count === 1 ? '1 Notification' : count + " Notifications";
    };
    NotificationsContribution.prototype.registerCommands = function (commands) {
        var _this = this;
        commands.registerCommand(notifications_commands_1.NotificationsCommands.TOGGLE, {
            isEnabled: function () { return true; },
            execute: function () { return _this.manager.toggleCenter(); }
        });
        commands.registerCommand(notifications_commands_1.NotificationsCommands.HIDE, {
            execute: function () { return _this.manager.hide(); }
        });
        commands.registerCommand(notifications_commands_1.NotificationsCommands.CLEAR_ALL, {
            execute: function () { return _this.manager.clearAll(); }
        });
    };
    NotificationsContribution.prototype.registerKeybindings = function (keybindings) {
        keybindings.registerKeybinding({
            command: notifications_commands_1.NotificationsCommands.HIDE.id,
            context: NotificationsKeybindingContext.notificationsVisible,
            keybinding: 'esc'
        });
    };
    NotificationsContribution.prototype.registerColors = function (colors) {
        colors.register({
            id: 'notificationCenter.border', defaults: {
                hc: 'contrastBorder'
            }, description: 'Notifications center border color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationToast.border', defaults: {
                hc: 'contrastBorder'
            }, description: 'Notification toast border color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notifications.foreground', defaults: {
                dark: 'editorWidget.foreground',
                light: 'editorWidget.foreground',
                hc: 'editorWidget.foreground'
            }, description: 'Notifications foreground color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notifications.background', defaults: {
                dark: 'editorWidget.background',
                light: 'editorWidget.background',
                hc: 'editorWidget.background'
            }, description: 'Notifications background color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationLink.foreground', defaults: {
                dark: 'textLink.foreground',
                light: 'textLink.foreground',
                hc: 'textLink.foreground'
            }, description: 'Notification links foreground color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationCenterHeader.foreground',
            description: 'Notifications center header foreground color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationCenterHeader.background', defaults: {
                dark: color_registry_1.Color.lighten('notifications.background', 0.3),
                light: color_registry_1.Color.darken('notifications.background', 0.05),
                hc: 'notifications.background'
            }, description: 'Notifications center header background color. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notifications.border', defaults: {
                dark: 'notificationCenterHeader.background',
                light: 'notificationCenterHeader.background',
                hc: 'notificationCenterHeader.background'
                // eslint-disable-next-line max-len
            }, description: 'Notifications border color separating from other notifications in the notifications center. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationsErrorIcon.foreground', defaults: {
                dark: 'editorError.foreground',
                light: 'editorError.foreground',
                hc: 'editorError.foreground'
            }, description: 'The color used for the icon of error notifications. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationsWarningIcon.foreground', defaults: {
                dark: 'editorWarning.foreground',
                light: 'editorWarning.foreground',
                hc: 'editorWarning.foreground'
            }, description: 'The color used for the icon of warning notifications. Notifications slide in from the bottom right of the window.'
        }, {
            id: 'notificationsInfoIcon.foreground', defaults: {
                dark: 'editorInfo.foreground',
                light: 'editorInfo.foreground',
                hc: 'editorInfo.foreground'
            }, description: 'The color used for the icon of info notifications. Notifications slide in from the bottom right of the window.'
        });
    };
    __decorate([
        inversify_1.inject(notifications_manager_1.NotificationManager),
        __metadata("design:type", notifications_manager_1.NotificationManager)
    ], NotificationsContribution.prototype, "manager", void 0);
    __decorate([
        inversify_1.inject(notifications_renderer_1.NotificationsRenderer),
        __metadata("design:type", notifications_renderer_1.NotificationsRenderer)
    ], NotificationsContribution.prototype, "notificationsRenderer", void 0);
    __decorate([
        inversify_1.inject(browser_1.StatusBar),
        __metadata("design:type", Object)
    ], NotificationsContribution.prototype, "statusBar", void 0);
    NotificationsContribution = __decorate([
        inversify_1.injectable()
    ], NotificationsContribution);
    return NotificationsContribution;
}());
exports.NotificationsContribution = NotificationsContribution;
var NotificationsKeybindingContext = /** @class */ (function () {
    function NotificationsKeybindingContext() {
        this.id = NotificationsKeybindingContext_1.notificationsVisible;
    }
    NotificationsKeybindingContext_1 = NotificationsKeybindingContext;
    NotificationsKeybindingContext.prototype.isEnabled = function (_arg) {
        return this.manager.centerVisible || this.manager.toastsVisible;
    };
    var NotificationsKeybindingContext_1;
    __decorate([
        inversify_1.inject(notifications_manager_1.NotificationManager),
        __metadata("design:type", notifications_manager_1.NotificationManager)
    ], NotificationsKeybindingContext.prototype, "manager", void 0);
    NotificationsKeybindingContext = NotificationsKeybindingContext_1 = __decorate([
        inversify_1.injectable()
    ], NotificationsKeybindingContext);
    return NotificationsKeybindingContext;
}());
exports.NotificationsKeybindingContext = NotificationsKeybindingContext;
(function (NotificationsKeybindingContext) {
    NotificationsKeybindingContext.notificationsVisible = 'notificationsVisible';
})(NotificationsKeybindingContext = exports.NotificationsKeybindingContext || (exports.NotificationsKeybindingContext = {}));
exports.NotificationsKeybindingContext = NotificationsKeybindingContext;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notifications-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notifications-manager.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notifications-manager.js ***!
  \**************************************************************************************************/
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
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var objects_1 = __webpack_require__(/*! @theia/core/lib/common/objects */ "../../packages/core/lib/common/objects.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var promise_util_1 = __webpack_require__(/*! @theia/core/lib/common/promise-util */ "../../packages/core/lib/common/promise-util.js");
var ts_md5_1 = __webpack_require__(/*! ts-md5 */ "../../node_modules/ts-md5/dist/md5.js");
var markdownit = __webpack_require__(/*! markdown-it */ "../../node_modules/markdown-it/index.js");
var throttle = __webpack_require__(/*! lodash.throttle */ "../../node_modules/lodash.throttle/index.js");
var notification_preferences_1 = __webpack_require__(/*! ./notification-preferences */ "../../packages/messages/lib/browser/notification-preferences.js");
var context_key_service_1 = __webpack_require__(/*! @theia/core/lib/browser/context-key-service */ "../../packages/core/lib/browser/context-key-service.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var NotificationManager = /** @class */ (function (_super) {
    __extends(NotificationManager, _super);
    function NotificationManager() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.onUpdatedEmitter = new core_1.Emitter();
        _this.onUpdated = _this.onUpdatedEmitter.event;
        _this.fireUpdatedEvent = throttle(function () {
            var notifications = objects_1.deepClone(Array.from(_this.notifications.values()));
            var toasts = objects_1.deepClone(Array.from(_this.toasts.values()));
            var visibilityState = _this.visibilityState;
            _this.onUpdatedEmitter.fire({ notifications: notifications, toasts: toasts, visibilityState: visibilityState });
        }, 250, { leading: true, trailing: true });
        _this.deferredResults = new Map();
        _this.notifications = new Map();
        _this.toasts = new Map();
        _this.visibilityState = 'hidden';
        _this.hideTimeouts = new Map();
        _this.mdEngine = markdownit({ html: true });
        return _this;
    }
    NotificationManager.prototype.init = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                this.notificationToastsVisibleKey = this.contextKeyService.createKey('notificationToastsVisible', false);
                this.notificationCenterVisibleKey = this.contextKeyService.createKey('notificationCenterVisible', false);
                return [2 /*return*/];
            });
        });
    };
    NotificationManager.prototype.updateContextKeys = function () {
        this.notificationToastsVisibleKey.set(this.toastsVisible);
        this.notificationCenterVisibleKey.set(this.centerVisible);
    };
    Object.defineProperty(NotificationManager.prototype, "toastsVisible", {
        get: function () {
            return this.visibilityState === 'toasts';
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NotificationManager.prototype, "centerVisible", {
        get: function () {
            return this.visibilityState === 'center';
        },
        enumerable: true,
        configurable: true
    });
    NotificationManager.prototype.setVisibilityState = function (newState) {
        var changed = this.visibilityState !== newState;
        this.visibilityState = newState;
        if (changed) {
            this.fireUpdatedEvent();
            this.updateContextKeys();
        }
    };
    NotificationManager.prototype.hideCenter = function () {
        this.setVisibilityState('hidden');
    };
    NotificationManager.prototype.toggleCenter = function () {
        this.setVisibilityState(this.centerVisible ? 'hidden' : 'center');
    };
    NotificationManager.prototype.accept = function (notification, action) {
        var messageId = this.getId(notification);
        if (!messageId) {
            return;
        }
        this.notifications.delete(messageId);
        this.toasts.delete(messageId);
        var result = this.deferredResults.get(messageId);
        if (!result) {
            return;
        }
        this.deferredResults.delete(messageId);
        if (this.centerVisible && this.notifications.size === 0) {
            this.visibilityState = 'hidden';
        }
        result.resolve(action);
        this.fireUpdatedEvent();
    };
    NotificationManager.prototype.find = function (notification) {
        return typeof notification === 'string' ? this.notifications.get(notification) : notification;
    };
    NotificationManager.prototype.getId = function (notification) {
        return typeof notification === 'string' ? notification : notification.messageId;
    };
    NotificationManager.prototype.hide = function () {
        if (this.toastsVisible) {
            this.toasts.clear();
        }
        this.setVisibilityState('hidden');
    };
    NotificationManager.prototype.clearAll = function () {
        var _this = this;
        this.setVisibilityState('hidden');
        Array.from(this.notifications.values()).forEach(function (n) { return _this.clear(n); });
    };
    NotificationManager.prototype.clear = function (notification) {
        this.accept(notification, undefined);
    };
    NotificationManager.prototype.toggleExpansion = function (notificationId) {
        var notification = this.find(notificationId);
        if (!notification) {
            return;
        }
        notification.collapsed = !notification.collapsed;
        this.fireUpdatedEvent();
    };
    NotificationManager.prototype.showMessage = function (plainMessage) {
        var messageId = this.getMessageId(plainMessage);
        var notification = this.notifications.get(messageId);
        if (!notification) {
            var message = this.renderMessage(plainMessage.text);
            var type = this.toNotificationType(plainMessage.type);
            var actions = Array.from(new Set(plainMessage.actions));
            var source = plainMessage.source;
            var expandable = this.isExpandable(message, source, actions);
            var collapsed = expandable;
            notification = { messageId: messageId, message: message, type: type, actions: actions, expandable: expandable, collapsed: collapsed };
            this.notifications.set(messageId, notification);
        }
        var result = this.deferredResults.get(messageId) || new promise_util_1.Deferred();
        this.deferredResults.set(messageId, result);
        if (!this.centerVisible) {
            this.toasts.delete(messageId);
            this.toasts.set(messageId, notification);
            this.startHideTimeout(messageId, this.getTimeout(plainMessage));
            this.setVisibilityState('toasts');
        }
        this.fireUpdatedEvent();
        return result.promise;
    };
    NotificationManager.prototype.startHideTimeout = function (messageId, timeout) {
        var _this = this;
        if (timeout > 0) {
            this.hideTimeouts.set(messageId, window.setTimeout(function () {
                _this.hideToast(messageId);
            }, timeout));
        }
    };
    NotificationManager.prototype.hideToast = function (messageId) {
        this.toasts.delete(messageId);
        if (this.toastsVisible && !this.toasts.size) {
            this.setVisibilityState('hidden');
        }
        else {
            this.fireUpdatedEvent();
        }
    };
    NotificationManager.prototype.getTimeout = function (plainMessage) {
        if (plainMessage.actions && plainMessage.actions.length > 0) {
            // Ignore the timeout if at least one action is set, and we wait for user interaction.
            return 0;
        }
        return plainMessage.options && plainMessage.options.timeout || this.preferences['notification.timeout'];
    };
    NotificationManager.prototype.renderMessage = function (content) {
        var contentWithoutNewlines = content.replace(/(\r)?\n/gm, ' ');
        return this.mdEngine.renderInline(contentWithoutNewlines);
    };
    NotificationManager.prototype.isExpandable = function (message, source, actions) {
        if (!actions.length && source) {
            return true;
        }
        return message.length > 500;
    };
    NotificationManager.prototype.toNotificationType = function (type) {
        switch (type) {
            case common_1.MessageType.Error:
                return 'error';
            case common_1.MessageType.Warning:
                return 'warning';
            case common_1.MessageType.Progress:
                return 'progress';
            default:
                return 'info';
        }
    };
    NotificationManager.prototype.getMessageId = function (m) {
        return String(ts_md5_1.Md5.hashStr("[" + m.type + "] " + m.text + " : " + (m.actions || []).join(' | ') + ";"));
    };
    NotificationManager.prototype.showProgress = function (messageId, plainMessage, cancellationToken) {
        return __awaiter(this, void 0, void 0, function () {
            var notification, message, type, actions, source, expandable, collapsed, result;
            var _this = this;
            return __generator(this, function (_a) {
                notification = this.notifications.get(messageId);
                if (!notification) {
                    message = this.renderMessage(plainMessage.text);
                    type = this.toNotificationType(plainMessage.type);
                    actions = Array.from(new Set(plainMessage.actions));
                    source = plainMessage.source;
                    expandable = this.isExpandable(message, source, actions);
                    collapsed = expandable;
                    notification = { messageId: messageId, message: message, type: type, actions: actions, expandable: expandable, collapsed: collapsed };
                    this.notifications.set(messageId, notification);
                    notification.progress = 0;
                    cancellationToken.onCancellationRequested(function () {
                        _this.accept(messageId, common_1.ProgressMessage.Cancel);
                    });
                }
                result = this.deferredResults.get(messageId) || new promise_util_1.Deferred();
                this.deferredResults.set(messageId, result);
                if (!this.centerVisible) {
                    this.toasts.set(messageId, notification);
                    this.setVisibilityState('toasts');
                }
                this.fireUpdatedEvent();
                return [2 /*return*/, result.promise];
            });
        });
    };
    NotificationManager.prototype.reportProgress = function (messageId, update, originalMessage, cancellationToken) {
        return __awaiter(this, void 0, void 0, function () {
            var notification;
            return __generator(this, function (_a) {
                notification = this.find(messageId);
                if (!notification) {
                    return [2 /*return*/];
                }
                if (cancellationToken.isCancellationRequested) {
                    this.clear(messageId);
                }
                else {
                    notification.message = update.message ? originalMessage.text + ": " + update.message : originalMessage.text;
                    notification.progress = this.toPlainProgress(update);
                }
                this.fireUpdatedEvent();
                return [2 /*return*/];
            });
        });
    };
    NotificationManager.prototype.toPlainProgress = function (update) {
        return update.work && Math.min(update.work.done / update.work.total * 100, 100);
    };
    NotificationManager.prototype.openLink = function (link) {
        return __awaiter(this, void 0, void 0, function () {
            var uri, opener;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        uri = new uri_1.default(link);
                        return [4 /*yield*/, this.openerService.getOpener(uri)];
                    case 1:
                        opener = _a.sent();
                        opener.open(uri);
                        return [2 /*return*/];
                }
            });
        });
    };
    __decorate([
        inversify_1.inject(notification_preferences_1.NotificationPreferences),
        __metadata("design:type", Object)
    ], NotificationManager.prototype, "preferences", void 0);
    __decorate([
        inversify_1.inject(context_key_service_1.ContextKeyService),
        __metadata("design:type", context_key_service_1.ContextKeyService)
    ], NotificationManager.prototype, "contextKeyService", void 0);
    __decorate([
        inversify_1.inject(browser_1.OpenerService),
        __metadata("design:type", Object)
    ], NotificationManager.prototype, "openerService", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", Promise)
    ], NotificationManager.prototype, "init", null);
    NotificationManager = __decorate([
        inversify_1.injectable()
    ], NotificationManager);
    return NotificationManager;
}(common_1.MessageClient));
exports.NotificationManager = NotificationManager;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notifications-manager'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/lib/browser/notifications-renderer.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/lib/browser/notifications-renderer.js ***!
  \***************************************************************************************************/
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
var ReactDOM = __webpack_require__(/*! react-dom */ "../../node_modules/react-dom/index.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var notifications_manager_1 = __webpack_require__(/*! ./notifications-manager */ "../../packages/messages/lib/browser/notifications-manager.js");
var notification_center_component_1 = __webpack_require__(/*! ./notification-center-component */ "../../packages/messages/lib/browser/notification-center-component.js");
var notification_toasts_component_1 = __webpack_require__(/*! ./notification-toasts-component */ "../../packages/messages/lib/browser/notification-toasts-component.js");
var NotificationsRenderer = /** @class */ (function () {
    function NotificationsRenderer() {
    }
    NotificationsRenderer.prototype.init = function () {
        this.createOverlayContainer();
        this.render();
    };
    NotificationsRenderer.prototype.createOverlayContainer = function () {
        this.container = window.document.createElement('div');
        this.container.className = 'theia-notifications-overlay';
        if (window.document.body) {
            window.document.body.appendChild(this.container);
        }
    };
    NotificationsRenderer.prototype.render = function () {
        ReactDOM.render(React.createElement("div", null,
            React.createElement(notification_toasts_component_1.NotificationToastsComponent, { manager: this.manager }),
            React.createElement(notification_center_component_1.NotificationCenterComponent, { manager: this.manager })), this.container);
    };
    __decorate([
        inversify_1.inject(browser_1.ApplicationShell),
        __metadata("design:type", browser_1.ApplicationShell)
    ], NotificationsRenderer.prototype, "shell", void 0);
    __decorate([
        inversify_1.inject(notifications_manager_1.NotificationManager),
        __metadata("design:type", notifications_manager_1.NotificationManager)
    ], NotificationsRenderer.prototype, "manager", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], NotificationsRenderer.prototype, "init", null);
    NotificationsRenderer = __decorate([
        inversify_1.injectable()
    ], NotificationsRenderer);
    return NotificationsRenderer;
}());
exports.NotificationsRenderer = NotificationsRenderer;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/messages/lib/browser/notifications-renderer'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/messages/src/browser/style/index.css":
/*!*****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/messages/src/browser/style/index.css ***!
  \*****************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/messages/src/browser/style/index.css");

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
//# sourceMappingURL=30.bundle.js.map