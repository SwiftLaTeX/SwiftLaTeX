(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[28],{

/***/ "../../node_modules/css-loader/index.js!../../packages/markers/src/browser/style/index.css":
/*!****************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/markers/src/browser/style/index.css ***!
  \****************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, "/********************************************************************************\n * Copyright (C) 2017 TypeFox and others.\n *\n * This program and the accompanying materials are made available under the\n * terms of the Eclipse Public License v. 2.0 which is available at\n * http://www.eclipse.org/legal/epl-2.0.\n *\n * This Source Code may also be made available under the following Secondary\n * Licenses when the conditions for such availability set forth in the Eclipse\n * Public License v. 2.0 are satisfied: GNU General Public License, version 2\n * with the GNU Classpath Exception which is available at\n * https://www.gnu.org/software/classpath/license.html.\n *\n * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0\n ********************************************************************************/\n\n.theia-marker-container {\n    font-size: var(--theia-ui-font-size1);\n}\n\n.theia-side-panel .theia-marker-container .noMarkers {\n    padding-left: 19px;\n}\n\n.theia-marker-container .markerNode,\n.theia-marker-container .markerFileNode {\n    display: flex;\n    align-items: center;\n}\n\n.theia-marker-container .markerNode,\n.theia-marker-container .markerFileNode {\n    width: calc(100% - 32px);\n}\n\n.theia-marker-container .markerNode div,\n.theia-marker-container .markerFileNode div {\n    margin-right: 5px;\n}\n\n.theia-marker-container .markerFileNode .name,\n.theia-marker-container .markerFileNode .path {\n    overflow: hidden;\n    text-overflow: ellipsis;\n    white-space: nowrap;\n}\n\n.theia-marker-container .markerFileNode .path {\n    font-size: var(--theia-ui-font-size0);\n    color: var(--theia-descriptionForeground);\n    align-self: flex-end;\n    white-space: nowrap;\n}\n\n.theia-marker-container .error {\n    color: var(--theia-editorError-foreground);\n}\n\n.theia-marker-container .warning {\n    color: var(--theia-editorWarning-foreground);\n}\n\n.theia-marker-container .information {\n    color: var(--theia-editorInfo-foreground);\n}\n\n.theia-marker-container .hint {\n    color: var(--theia-successBackground);\n}\n\n.theia-marker-container .markerNode .position,\n.theia-marker-container .markerNode .owner,\n.theia-marker-container .markerNode .code {\n    color: var(--theia-descriptionForeground);\n    white-space: nowrap;\n}\n\n.theia-marker-container .markerNode .position,\n.theia-marker-container .markerNode .code {\n    margin-left: 5px;\n}\n\n.theia-marker-container .markerNode .message {\n    white-space: nowrap;\n    overflow: hidden;\n    text-overflow: ellipsis;\n}\n\n.problem-tab-icon::before {\n    content: \"\\F06A\"\n}\n\n.theia-marker-container .row-button-container {\n    display: none;\n}\n\n.theia-marker-container .theia-TreeNodeContent:hover .row-button-container {\n    display: flex;\n    justify-content: flex-end;\n    flex: 1;\n    align-items: center;\n    align-self: center;\n}\n\n.theia-marker-container .row-button-container .remove-node {\n    background-image: var(--theia-icon-close);\n    width: 15px;\n    height: 15px;\n}\n", ""]);

// exports


/***/ }),

/***/ "../../packages/markers/lib/browser/marker-tree-label-provider.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/marker-tree-label-provider.js ***!
  \******************************************************************************************************/
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
var label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/label-provider */ "../../packages/core/lib/browser/label-provider.js");
var marker_tree_1 = __webpack_require__(/*! ./marker-tree */ "../../packages/markers/lib/browser/marker-tree.js");
var tree_label_provider_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-label-provider */ "../../packages/core/lib/browser/tree/tree-label-provider.js");
var browser_1 = __webpack_require__(/*! @theia/workspace/lib/browser */ "../../packages/workspace/lib/browser/index.js");
var MarkerTreeLabelProvider = /** @class */ (function () {
    function MarkerTreeLabelProvider() {
    }
    MarkerTreeLabelProvider.prototype.canHandle = function (element) {
        return marker_tree_1.MarkerInfoNode.is(element) ?
            this.treeLabelProvider.canHandle(element) + 1 :
            0;
    };
    MarkerTreeLabelProvider.prototype.getIcon = function (node) {
        return this.labelProvider.getIcon(node.uri);
    };
    MarkerTreeLabelProvider.prototype.getName = function (node) {
        return this.labelProvider.getName(node.uri);
    };
    MarkerTreeLabelProvider.prototype.getLongName = function (node) {
        var description = [];
        var rootUri = this.workspaceService.getWorkspaceRootUri(node.uri);
        // In a multiple-root workspace include the root name to the label before the parent directory.
        if (this.workspaceService.isMultiRootWorkspaceOpened && rootUri) {
            description.push(this.labelProvider.getName(rootUri));
        }
        // If the given resource is not at the workspace root, include the parent directory to the label.
        if (rootUri && rootUri.toString() !== node.uri.parent.toString()) {
            description.push(this.labelProvider.getLongName(node.uri.parent));
        }
        // Get the full path of a resource which does not exist in the given workspace.
        if (!rootUri) {
            description.push(this.labelProvider.getLongName(node.uri.parent.withScheme('markers')));
        }
        return description.join(' ● ');
    };
    MarkerTreeLabelProvider.prototype.getDescription = function (node) {
        return this.labelProvider.getLongName(node.uri.parent);
    };
    MarkerTreeLabelProvider.prototype.affects = function (node, event) {
        return event.affects(node.uri) || event.affects(node.uri.parent);
    };
    __decorate([
        inversify_1.inject(label_provider_1.LabelProvider),
        __metadata("design:type", label_provider_1.LabelProvider)
    ], MarkerTreeLabelProvider.prototype, "labelProvider", void 0);
    __decorate([
        inversify_1.inject(tree_label_provider_1.TreeLabelProvider),
        __metadata("design:type", tree_label_provider_1.TreeLabelProvider)
    ], MarkerTreeLabelProvider.prototype, "treeLabelProvider", void 0);
    __decorate([
        inversify_1.inject(browser_1.WorkspaceService),
        __metadata("design:type", browser_1.WorkspaceService)
    ], MarkerTreeLabelProvider.prototype, "workspaceService", void 0);
    MarkerTreeLabelProvider = __decorate([
        inversify_1.injectable()
    ], MarkerTreeLabelProvider);
    return MarkerTreeLabelProvider;
}());
exports.MarkerTreeLabelProvider = MarkerTreeLabelProvider;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/marker-tree-label-provider'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/marker-tree-model.js":
/*!*********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/marker-tree-model.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var marker_tree_1 = __webpack_require__(/*! ./marker-tree */ "../../packages/markers/lib/browser/marker-tree.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var MarkerTreeModel = /** @class */ (function (_super) {
    __extends(MarkerTreeModel, _super);
    function MarkerTreeModel() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    MarkerTreeModel.prototype.doOpenNode = function (node) {
        if (marker_tree_1.MarkerNode.is(node)) {
            browser_1.open(this.openerService, node.uri, this.getOpenerOptionsByMarker(node));
        }
        else {
            _super.prototype.doOpenNode.call(this, node);
        }
    };
    MarkerTreeModel.prototype.getOpenerOptionsByMarker = function (node) {
        return undefined;
    };
    /**
     * Reveal the corresponding node at the marker.
     * @param node {TreeNode} the tree node.
     */
    MarkerTreeModel.prototype.revealNode = function (node) {
        if (marker_tree_1.MarkerNode.is(node)) {
            browser_1.open(this.openerService, node.uri, __assign({}, this.getOpenerOptionsByMarker(node), { mode: 'reveal' }));
        }
    };
    __decorate([
        inversify_1.inject(browser_1.OpenerService),
        __metadata("design:type", Object)
    ], MarkerTreeModel.prototype, "openerService", void 0);
    MarkerTreeModel = __decorate([
        inversify_1.injectable()
    ], MarkerTreeModel);
    return MarkerTreeModel;
}(browser_1.TreeModelImpl));
exports.MarkerTreeModel = MarkerTreeModel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/marker-tree-model'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/marker-tree.js":
/*!***************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/marker-tree.js ***!
  \***************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var marker_manager_1 = __webpack_require__(/*! ./marker-manager */ "../../packages/markers/lib/browser/marker-manager.js");
var selection_1 = __webpack_require__(/*! @theia/core/lib/common/selection */ "../../packages/core/lib/common/selection.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var problem_selection_1 = __webpack_require__(/*! ./problem/problem-selection */ "../../packages/markers/lib/browser/problem/problem-selection.js");
exports.MarkerOptions = Symbol('MarkerOptions');
var MarkerTree = /** @class */ (function (_super) {
    __extends(MarkerTree, _super);
    function MarkerTree(markerManager, markerOptions) {
        var _this = _super.call(this) || this;
        _this.markerManager = markerManager;
        _this.markerOptions = markerOptions;
        _this.toDispose.push(markerManager.onDidChangeMarkers(function (uri) { return _this.refreshMarkerInfo(uri); }));
        _this.root = {
            visible: false,
            id: 'theia-' + markerOptions.kind + '-marker-widget',
            name: 'MarkerTree',
            kind: markerOptions.kind,
            children: [],
            parent: undefined
        };
        return _this;
    }
    MarkerTree.prototype.refreshMarkerInfo = function (uri) {
        return __awaiter(this, void 0, void 0, function () {
            var id, existing, markers, node, children;
            return __generator(this, function (_a) {
                id = uri.toString();
                existing = this.getNode(id);
                markers = this.markerManager.findMarkers({ uri: uri });
                if (markers.length <= 0) {
                    if (MarkerInfoNode.is(existing)) {
                        browser_1.CompositeTreeNode.removeChild(existing.parent, existing);
                        this.removeNode(existing);
                        this.fireChanged();
                    }
                    return [2 /*return*/];
                }
                node = MarkerInfoNode.is(existing) ? existing : this.createMarkerInfo(id, uri);
                browser_1.CompositeTreeNode.addChild(node.parent, node);
                children = this.getMarkerNodes(node, markers);
                node.numberOfMarkers = markers.length;
                this.setChildren(node, children);
                return [2 /*return*/];
            });
        });
    };
    MarkerTree.prototype.resolveChildren = function (parent) {
        return __awaiter(this, void 0, void 0, function () {
            var nodes, _a, _b, id, uri, existing, markers, node;
            var e_1, _c;
            return __generator(this, function (_d) {
                if (MarkerRootNode.is(parent)) {
                    nodes = [];
                    try {
                        for (_a = __values(this.markerManager.getUris()), _b = _a.next(); !_b.done; _b = _a.next()) {
                            id = _b.value;
                            uri = new uri_1.default(id);
                            existing = this.getNode(id);
                            markers = this.markerManager.findMarkers({ uri: uri });
                            node = MarkerInfoNode.is(existing) ? existing : this.createMarkerInfo(id, uri);
                            node.children = this.getMarkerNodes(node, markers);
                            node.numberOfMarkers = node.children.length;
                            nodes.push(node);
                        }
                    }
                    catch (e_1_1) { e_1 = { error: e_1_1 }; }
                    finally {
                        try {
                            if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                        }
                        finally { if (e_1) throw e_1.error; }
                    }
                    return [2 /*return*/, nodes];
                }
                return [2 /*return*/, _super.prototype.resolveChildren.call(this, parent)];
            });
        });
    };
    MarkerTree.prototype.createMarkerInfo = function (id, uri) {
        return {
            children: [],
            expanded: true,
            uri: uri,
            id: id,
            parent: this.root,
            selected: false,
            numberOfMarkers: 0
        };
    };
    MarkerTree.prototype.getMarkerNodes = function (parent, markers) {
        var _this = this;
        return markers.map(function (marker, index) {
            return _this.createMarkerNode(marker, index, parent);
        });
    };
    MarkerTree.prototype.createMarkerNode = function (marker, index, parent) {
        var id = parent.id + '_' + index;
        var existing = this.getNode(id);
        if (MarkerNode.is(existing)) {
            existing.marker = marker;
            return existing;
        }
        return {
            id: id,
            name: 'marker',
            parent: parent,
            selected: false,
            uri: parent.uri,
            marker: marker
        };
    };
    MarkerTree = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [marker_manager_1.MarkerManager, Object])
    ], MarkerTree);
    return MarkerTree;
}(browser_1.TreeImpl));
exports.MarkerTree = MarkerTree;
var MarkerNode;
(function (MarkerNode) {
    function is(node) {
        return selection_1.UriSelection.is(node) && browser_1.SelectableTreeNode.is(node) && problem_selection_1.ProblemSelection.is(node);
    }
    MarkerNode.is = is;
})(MarkerNode = exports.MarkerNode || (exports.MarkerNode = {}));
var MarkerInfoNode;
(function (MarkerInfoNode) {
    function is(node) {
        return browser_1.ExpandableTreeNode.is(node) && selection_1.UriSelection.is(node) && 'numberOfMarkers' in node;
    }
    MarkerInfoNode.is = is;
})(MarkerInfoNode = exports.MarkerInfoNode || (exports.MarkerInfoNode = {}));
var MarkerRootNode;
(function (MarkerRootNode) {
    function is(node) {
        return browser_1.CompositeTreeNode.is(node) && 'kind' in node;
    }
    MarkerRootNode.is = is;
})(MarkerRootNode = exports.MarkerRootNode || (exports.MarkerRootNode = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/marker-tree'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-container.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-container.js ***!
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
var marker_tree_1 = __webpack_require__(/*! ../marker-tree */ "../../packages/markers/lib/browser/marker-tree.js");
var problem_widget_1 = __webpack_require__(/*! ./problem-widget */ "../../packages/markers/lib/browser/problem/problem-widget.js");
var problem_tree_model_1 = __webpack_require__(/*! ./problem-tree-model */ "../../packages/markers/lib/browser/problem/problem-tree-model.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
exports.PROBLEM_TREE_PROPS = __assign({}, browser_1.defaultTreeProps, { contextMenuPath: [problem_marker_1.PROBLEM_KIND], globalSelection: true });
exports.PROBLEM_OPTIONS = {
    kind: 'problem'
};
function createProblemTreeContainer(parent) {
    var child = browser_1.createTreeContainer(parent);
    child.unbind(browser_1.TreeImpl);
    child.bind(problem_tree_model_1.ProblemTree).toSelf();
    child.rebind(browser_1.Tree).toService(problem_tree_model_1.ProblemTree);
    child.unbind(browser_1.TreeWidget);
    child.bind(problem_widget_1.ProblemWidget).toSelf();
    child.unbind(browser_1.TreeModelImpl);
    child.bind(problem_tree_model_1.ProblemTreeModel).toSelf();
    child.rebind(browser_1.TreeModel).toService(problem_tree_model_1.ProblemTreeModel);
    child.rebind(browser_1.TreeProps).toConstantValue(exports.PROBLEM_TREE_PROPS);
    child.bind(marker_tree_1.MarkerOptions).toConstantValue(exports.PROBLEM_OPTIONS);
    return child;
}
exports.createProblemTreeContainer = createProblemTreeContainer;
function createProblemWidget(parent) {
    return createProblemTreeContainer(parent).get(problem_widget_1.ProblemWidget);
}
exports.createProblemWidget = createProblemWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-container'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-contribution.js":
/*!********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-contribution.js ***!
  \********************************************************************************************************/
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
var debounce = __webpack_require__(/*! lodash.debounce */ "../../node_modules/lodash.debounce/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var status_bar_1 = __webpack_require__(/*! @theia/core/lib/browser/status-bar/status-bar */ "../../packages/core/lib/browser/status-bar/status-bar.js");
var view_contribution_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/view-contribution */ "../../packages/core/lib/browser/shell/view-contribution.js");
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var problem_widget_1 = __webpack_require__(/*! ./problem-widget */ "../../packages/markers/lib/browser/problem/problem-widget.js");
var selection_service_1 = __webpack_require__(/*! @theia/core/lib/common/selection-service */ "../../packages/core/lib/common/selection-service.js");
var problem_selection_1 = __webpack_require__(/*! ./problem-selection */ "../../packages/markers/lib/browser/problem/problem-selection.js");
exports.PROBLEMS_CONTEXT_MENU = [problem_marker_1.PROBLEM_KIND];
var ProblemsMenu;
(function (ProblemsMenu) {
    ProblemsMenu.CLIPBOARD = __spread(exports.PROBLEMS_CONTEXT_MENU, ['1_clipboard']);
    ProblemsMenu.PROBLEMS = __spread(exports.PROBLEMS_CONTEXT_MENU, ['2_problems']);
})(ProblemsMenu = exports.ProblemsMenu || (exports.ProblemsMenu = {}));
var ProblemsCommands;
(function (ProblemsCommands) {
    ProblemsCommands.COLLAPSE_ALL = {
        id: 'problems.collapse.all'
    };
    ProblemsCommands.COLLAPSE_ALL_TOOLBAR = {
        id: 'problems.collapse.all.toolbar',
        iconClass: 'theia-collapse-all-icon'
    };
    ProblemsCommands.COPY = {
        id: 'problems.copy'
    };
    ProblemsCommands.COPY_MESSAGE = {
        id: 'problems.copy.message',
    };
})(ProblemsCommands = exports.ProblemsCommands || (exports.ProblemsCommands = {}));
var ProblemContribution = /** @class */ (function (_super) {
    __extends(ProblemContribution, _super);
    function ProblemContribution() {
        var _this = _super.call(this, {
            widgetId: problem_widget_1.PROBLEMS_WIDGET_ID,
            widgetName: 'Problems',
            defaultWidgetOptions: {
                area: 'bottom'
            },
            toggleCommandId: 'problemsView:toggle',
            toggleKeybinding: 'ctrlcmd+shift+m'
        }) || this;
        _this.updateStatusBarElement = debounce(function () { return _this.setStatusBarElement(_this.problemManager.getProblemStat()); }, 10);
        return _this;
    }
    ProblemContribution.prototype.onStart = function (app) {
        this.updateStatusBarElement();
        this.problemManager.onDidChangeMarkers(this.updateStatusBarElement);
    };
    ProblemContribution.prototype.initializeLayout = function (app) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.openView()];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    ProblemContribution.prototype.setStatusBarElement = function (problemStat) {
        this.statusBar.setElement('problem-marker-status', {
            text: problemStat.infos <= 0
                ? "$(times-circle) " + problemStat.errors + " $(exclamation-triangle) " + problemStat.warnings
                : "$(times-circle) " + problemStat.errors + " $(exclamation-triangle) " + problemStat.warnings + " $(info-circle) " + problemStat.infos,
            alignment: status_bar_1.StatusBarAlignment.LEFT,
            priority: 10,
            command: this.toggleCommand ? this.toggleCommand.id : undefined,
            tooltip: this.getStatusBarTooltip(problemStat)
        });
    };
    /**
     * Get the tooltip to be displayed when hovering over the problem statusbar item.
     * - Displays `No Problems` when no problems are present.
     * - Displays a human-readable label which describes for each type of problem stat properties,
     * their overall count and type when any one of these properties has a positive count.
     * @param stat the problem stat describing the number of `errors`, `warnings` and `infos`.
     *
     * @return the tooltip to be displayed in the statusbar.
     */
    ProblemContribution.prototype.getStatusBarTooltip = function (stat) {
        if (stat.errors <= 0 && stat.warnings <= 0 && stat.infos <= 0) {
            return 'No Problems';
        }
        var tooltip = [];
        if (stat.errors > 0) {
            tooltip.push(stat.errors + " Errors");
        }
        if (stat.warnings > 0) {
            tooltip.push(stat.warnings + " Warnings");
        }
        if (stat.infos > 0) {
            tooltip.push(stat.infos + " Infos");
        }
        return tooltip.join(', ');
    };
    ProblemContribution.prototype.registerCommands = function (commands) {
        var _this = this;
        _super.prototype.registerCommands.call(this, commands);
        commands.registerCommand(ProblemsCommands.COLLAPSE_ALL, {
            execute: function () { return _this.collapseAllProblems(); }
        });
        commands.registerCommand(ProblemsCommands.COLLAPSE_ALL_TOOLBAR, {
            isEnabled: function (widget) { return _this.withWidget(widget, function () { return true; }); },
            isVisible: function (widget) { return _this.withWidget(widget, function () { return true; }); },
            execute: function (widget) { return _this.withWidget(widget, function () { return _this.collapseAllProblems(); }); }
        });
        commands.registerCommand(ProblemsCommands.COPY, new problem_selection_1.ProblemSelection.CommandHandler(this.selectionService, {
            multi: false,
            isEnabled: function () { return true; },
            isVisible: function () { return true; },
            execute: function (selection) { return _this.copy(selection); }
        }));
        commands.registerCommand(ProblemsCommands.COPY_MESSAGE, new problem_selection_1.ProblemSelection.CommandHandler(this.selectionService, {
            multi: false,
            isEnabled: function () { return true; },
            isVisible: function () { return true; },
            execute: function (selection) { return _this.copyMessage(selection); }
        }));
    };
    ProblemContribution.prototype.registerMenus = function (menus) {
        _super.prototype.registerMenus.call(this, menus);
        menus.registerMenuAction(ProblemsMenu.CLIPBOARD, {
            commandId: ProblemsCommands.COPY.id,
            label: 'Copy',
            order: '0'
        });
        menus.registerMenuAction(ProblemsMenu.CLIPBOARD, {
            commandId: ProblemsCommands.COPY_MESSAGE.id,
            label: 'Copy Message',
            order: '1'
        });
        menus.registerMenuAction(ProblemsMenu.PROBLEMS, {
            commandId: ProblemsCommands.COLLAPSE_ALL.id,
            label: 'Collapse All',
            order: '2'
        });
    };
    ProblemContribution.prototype.registerToolbarItems = function (toolbarRegistry) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                toolbarRegistry.registerItem({
                    id: ProblemsCommands.COLLAPSE_ALL_TOOLBAR.id,
                    command: ProblemsCommands.COLLAPSE_ALL_TOOLBAR.id,
                    tooltip: 'Collapse All',
                    priority: 0,
                });
                return [2 /*return*/];
            });
        });
    };
    ProblemContribution.prototype.collapseAllProblems = function () {
        return __awaiter(this, void 0, void 0, function () {
            var model, root, firstChild;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.widget];
                    case 1:
                        model = (_a.sent()).model;
                        root = model.root;
                        firstChild = root.children[0];
                        root.children.forEach(function (child) { return browser_1.CompositeTreeNode.is(child) && model.collapseAll(child); });
                        if (browser_1.SelectableTreeNode.is(firstChild)) {
                            model.selectNode(firstChild);
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    ProblemContribution.prototype.addToClipboard = function (content) {
        var handleCopy = function (e) {
            document.removeEventListener('copy', handleCopy);
            if (e.clipboardData) {
                e.clipboardData.setData('text/plain', content);
                e.preventDefault();
            }
        };
        document.addEventListener('copy', handleCopy);
        document.execCommand('copy');
    };
    ProblemContribution.prototype.copy = function (selection) {
        var marker = selection.marker;
        var serializedProblem = JSON.stringify({
            resource: marker.uri,
            owner: marker.uri,
            code: marker.data.code,
            severity: marker.data.severity,
            message: marker.data.message,
            source: marker.data.source,
            startLineNumber: marker.data.range.start.line,
            startColumn: marker.data.range.start.character,
            endLineNumber: marker.data.range.end.line,
            endColumn: marker.data.range.end.character
        }, undefined, '\t');
        this.addToClipboard(serializedProblem);
    };
    ProblemContribution.prototype.copyMessage = function (selection) {
        var marker = selection.marker;
        this.addToClipboard(marker.data.message);
    };
    ProblemContribution.prototype.withWidget = function (widget, cb) {
        if (widget === void 0) { widget = this.tryGetWidget(); }
        if (widget instanceof problem_widget_1.ProblemWidget && widget.id === problem_widget_1.PROBLEMS_WIDGET_ID) {
            return cb(widget);
        }
        return false;
    };
    __decorate([
        inversify_1.inject(problem_manager_1.ProblemManager),
        __metadata("design:type", problem_manager_1.ProblemManager)
    ], ProblemContribution.prototype, "problemManager", void 0);
    __decorate([
        inversify_1.inject(status_bar_1.StatusBar),
        __metadata("design:type", Object)
    ], ProblemContribution.prototype, "statusBar", void 0);
    __decorate([
        inversify_1.inject(selection_service_1.SelectionService),
        __metadata("design:type", selection_service_1.SelectionService)
    ], ProblemContribution.prototype, "selectionService", void 0);
    ProblemContribution = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], ProblemContribution);
    return ProblemContribution;
}(view_contribution_1.AbstractViewContribution));
exports.ProblemContribution = ProblemContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-decorator.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-decorator.js ***!
  \*****************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var vscode_languageserver_types_1 = __webpack_require__(/*! vscode-languageserver-types */ "../../node_modules/vscode-languageserver-types/lib/esm/main.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var objects_1 = __webpack_require__(/*! @theia/core/lib/common/objects */ "../../packages/core/lib/common/objects.js");
var event_1 = __webpack_require__(/*! @theia/core/lib/common/event */ "../../packages/core/lib/common/event.js");
var tree_iterator_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-iterator */ "../../packages/core/lib/browser/tree/tree-iterator.js");
var tree_decorator_1 = __webpack_require__(/*! @theia/core/lib/browser/tree/tree-decorator */ "../../packages/core/lib/browser/tree/tree-decorator.js");
var browser_1 = __webpack_require__(/*! @theia/filesystem/lib/browser */ "../../packages/filesystem/lib/browser/index.js");
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var problem_preferences_1 = __webpack_require__(/*! ./problem-preferences */ "../../packages/markers/lib/browser/problem/problem-preferences.js");
var ProblemDecorator = /** @class */ (function () {
    function ProblemDecorator(problemManager) {
        var _this = this;
        this.problemManager = problemManager;
        this.id = 'theia-problem-decorator';
        this.emitter = new event_1.Emitter();
        this.problemManager.onDidChangeMarkers(function () { return _this.fireDidChangeDecorations(function (tree) { return _this.collectDecorators(tree); }); });
    }
    ProblemDecorator_1 = ProblemDecorator;
    ProblemDecorator.prototype.init = function () {
        var _this = this;
        this.problemPreferences.onPreferenceChanged(function (event) {
            var preferenceName = event.preferenceName;
            if (preferenceName === 'problems.decorations.enabled') {
                _this.fireDidChangeDecorations(function (tree) { return _this.collectDecorators(tree); });
            }
        });
    };
    ProblemDecorator.prototype.decorations = function (tree) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, this.collectDecorators(tree)];
            });
        });
    };
    Object.defineProperty(ProblemDecorator.prototype, "onDidChangeDecorations", {
        get: function () {
            return this.emitter.event;
        },
        enumerable: true,
        configurable: true
    });
    ProblemDecorator.prototype.fireDidChangeDecorations = function (event) {
        this.emitter.fire(event);
    };
    ProblemDecorator.prototype.collectDecorators = function (tree) {
        var e_1, _a;
        var _this = this;
        var result = new Map();
        // If the tree root is undefined or the preference for the decorations is disabled, return an empty result map.
        if (tree.root === undefined || !this.problemPreferences['problems.decorations.enabled']) {
            return result;
        }
        var markers = this.appendContainerMarkers(tree, this.collectMarkers(tree));
        try {
            for (var _b = __values(new tree_iterator_1.DepthFirstTreeIterator(tree.root)), _c = _b.next(); !_c.done; _c = _b.next()) {
                var node = _c.value;
                var nodeUri = browser_1.FileStatNode.getUri(node);
                if (nodeUri) {
                    var marker = markers.get(nodeUri);
                    if (marker) {
                        result.set(node.id, marker);
                    }
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
        return new Map(Array.from(result.entries()).map(function (m) { return [m[0], _this.toDecorator(m[1])]; }));
    };
    ProblemDecorator.prototype.appendContainerMarkers = function (tree, markers) {
        var e_2, _a;
        var result = new Map();
        try {
            // We traverse up and assign the diagnostic to the container directory.
            // Note, instead of stopping at the WS root, we traverse up the driver root.
            // We will filter them later based on the expansion state of the tree.
            for (var _b = __values(new Map(markers.map(function (m) { return [new uri_1.default(m.uri), m]; })).entries()), _c = _b.next(); !_c.done; _c = _b.next()) {
                var _d = __read(_c.value, 2), uri = _d[0], marker = _d[1];
                var uriString = uri.toString();
                result.set(uriString, marker);
                var parentUri = uri.parent;
                while (parentUri && !parentUri.path.isRoot) {
                    var parentUriString = parentUri.toString();
                    var existing = result.get(parentUriString);
                    // Make sure the highest diagnostic severity (smaller number) will be propagated to the container directory.
                    if (existing === undefined || this.compare(marker, existing) < 0) {
                        result.set(parentUriString, {
                            data: marker.data,
                            uri: parentUriString,
                            owner: marker.owner,
                            kind: marker.kind
                        });
                        parentUri = parentUri.parent;
                    }
                    else {
                        parentUri = undefined;
                    }
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
        return result;
    };
    ProblemDecorator.prototype.collectMarkers = function (tree) {
        var _this = this;
        return Array.from(this.problemManager.getUris())
            .map(function (uri) { return new uri_1.default(uri); })
            .map(function (uri) { return _this.problemManager.findMarkers({ uri: uri }); })
            .map(function (markers) { return markers.sort(_this.compare.bind(_this)); })
            .map(function (markers) { return markers.shift(); })
            .filter(objects_1.notEmpty)
            .filter(this.filterMarker.bind(this));
    };
    ProblemDecorator.prototype.toDecorator = function (marker) {
        var position = tree_decorator_1.TreeDecoration.IconOverlayPosition.BOTTOM_RIGHT;
        var icon = this.getOverlayIcon(marker);
        var color = this.getOverlayIconColor(marker);
        return {
            iconOverlay: {
                position: position,
                icon: icon,
                color: color,
                background: {
                    shape: 'circle',
                    color: 'transparent'
                }
            }
        };
    };
    ProblemDecorator.prototype.getOverlayIcon = function (marker) {
        var severity = marker.data.severity;
        switch (severity) {
            case 1: return 'times-circle';
            case 2: return 'exclamation-circle';
            case 3: return 'info-circle';
            default: return 'hand-o-up';
        }
    };
    ProblemDecorator.prototype.getOverlayIconColor = function (marker) {
        var severity = marker.data.severity;
        switch (severity) {
            case 1: return 'var(--theia-editorError-foreground)';
            case 2: return 'var(--theia-editorWarning-foreground)';
            case 3: return 'var(--theia-editorInfo-foreground)';
            default: return 'var(--theia-successBackground)';
        }
    };
    /**
     * Returns `true` if the diagnostic (`data`) of the marker argument has `Error`, `Warning`, or `Information` severity.
     * Otherwise, returns `false`.
     */
    ProblemDecorator.prototype.filterMarker = function (marker) {
        var severity = marker.data.severity;
        return severity === vscode_languageserver_types_1.DiagnosticSeverity.Error
            || severity === vscode_languageserver_types_1.DiagnosticSeverity.Warning
            || severity === vscode_languageserver_types_1.DiagnosticSeverity.Information;
    };
    ProblemDecorator.prototype.compare = function (left, right) {
        return ProblemDecorator_1.severityCompare(left, right);
    };
    var ProblemDecorator_1;
    __decorate([
        inversify_1.inject(problem_preferences_1.ProblemPreferences),
        __metadata("design:type", Object)
    ], ProblemDecorator.prototype, "problemPreferences", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], ProblemDecorator.prototype, "init", null);
    ProblemDecorator = ProblemDecorator_1 = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(problem_manager_1.ProblemManager)),
        __metadata("design:paramtypes", [problem_manager_1.ProblemManager])
    ], ProblemDecorator);
    return ProblemDecorator;
}());
exports.ProblemDecorator = ProblemDecorator;
(function (ProblemDecorator) {
    // Highest severities (errors) come first, then the others. Undefined severities treated as the last ones.
    ProblemDecorator.severityCompare = function (left, right) {
        return (left.data.severity || Number.MAX_SAFE_INTEGER) - (right.data.severity || Number.MAX_SAFE_INTEGER);
    };
})(ProblemDecorator = exports.ProblemDecorator || (exports.ProblemDecorator = {}));
exports.ProblemDecorator = ProblemDecorator;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-decorator'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-frontend-module.js":
/*!***********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-frontend-module.js ***!
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
Object.defineProperty(exports, "__esModule", { value: true });
__webpack_require__(/*! ../../../src/browser/style/index.css */ "../../packages/markers/src/browser/style/index.css");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var problem_widget_1 = __webpack_require__(/*! ./problem-widget */ "../../packages/markers/lib/browser/problem/problem-widget.js");
var problem_contribution_1 = __webpack_require__(/*! ./problem-contribution */ "../../packages/markers/lib/browser/problem/problem-contribution.js");
var problem_container_1 = __webpack_require__(/*! ./problem-container */ "../../packages/markers/lib/browser/problem/problem-container.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var widget_manager_1 = __webpack_require__(/*! @theia/core/lib/browser/widget-manager */ "../../packages/core/lib/browser/widget-manager.js");
var navigator_decorator_service_1 = __webpack_require__(/*! @theia/navigator/lib/browser/navigator-decorator-service */ "../../packages/navigator/lib/browser/navigator-decorator-service.js");
var problem_decorator_1 = __webpack_require__(/*! ./problem-decorator */ "../../packages/markers/lib/browser/problem/problem-decorator.js");
var problem_tabbar_decorator_1 = __webpack_require__(/*! ./problem-tabbar-decorator */ "../../packages/markers/lib/browser/problem/problem-tabbar-decorator.js");
var tab_bar_toolbar_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-toolbar */ "../../packages/core/lib/browser/shell/tab-bar-toolbar.js");
var problem_layout_migrations_1 = __webpack_require__(/*! ./problem-layout-migrations */ "../../packages/markers/lib/browser/problem/problem-layout-migrations.js");
var tab_bar_decorator_1 = __webpack_require__(/*! @theia/core/lib/browser/shell/tab-bar-decorator */ "../../packages/core/lib/browser/shell/tab-bar-decorator.js");
var problem_preferences_1 = __webpack_require__(/*! ./problem-preferences */ "../../packages/markers/lib/browser/problem/problem-preferences.js");
var marker_tree_label_provider_1 = __webpack_require__(/*! ../marker-tree-label-provider */ "../../packages/markers/lib/browser/marker-tree-label-provider.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    problem_preferences_1.bindProblemPreferences(bind);
    bind(problem_manager_1.ProblemManager).toSelf().inSingletonScope();
    bind(problem_widget_1.ProblemWidget).toDynamicValue(function (ctx) {
        return problem_container_1.createProblemWidget(ctx.container);
    });
    bind(widget_manager_1.WidgetFactory).toDynamicValue(function (context) { return ({
        id: problem_widget_1.PROBLEMS_WIDGET_ID,
        createWidget: function () { return context.container.get(problem_widget_1.ProblemWidget); }
    }); });
    bind(browser_1.ApplicationShellLayoutMigration).to(problem_layout_migrations_1.ProblemLayoutVersion3Migration).inSingletonScope();
    browser_1.bindViewContribution(bind, problem_contribution_1.ProblemContribution);
    bind(browser_1.FrontendApplicationContribution).toService(problem_contribution_1.ProblemContribution);
    bind(tab_bar_toolbar_1.TabBarToolbarContribution).toService(problem_contribution_1.ProblemContribution);
    bind(problem_decorator_1.ProblemDecorator).toSelf().inSingletonScope();
    bind(navigator_decorator_service_1.NavigatorTreeDecorator).toService(problem_decorator_1.ProblemDecorator);
    bind(problem_tabbar_decorator_1.ProblemTabBarDecorator).toSelf().inSingletonScope();
    bind(tab_bar_decorator_1.TabBarDecorator).toService(problem_tabbar_decorator_1.ProblemTabBarDecorator);
    bind(marker_tree_label_provider_1.MarkerTreeLabelProvider).toSelf().inSingletonScope();
    bind(browser_1.LabelProviderContribution).toService(marker_tree_label_provider_1.MarkerTreeLabelProvider);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-layout-migrations.js":
/*!*************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-layout-migrations.js ***!
  \*************************************************************************************************************/
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
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
var problem_widget_1 = __webpack_require__(/*! ./problem-widget */ "../../packages/markers/lib/browser/problem/problem-widget.js");
var ProblemLayoutVersion3Migration = /** @class */ (function () {
    function ProblemLayoutVersion3Migration() {
        this.layoutVersion = 3.0;
    }
    ProblemLayoutVersion3Migration.prototype.onWillInflateWidget = function (desc) {
        if (desc.constructionOptions.factoryId === problem_marker_1.PROBLEM_KIND) {
            desc.constructionOptions.factoryId = problem_widget_1.PROBLEMS_WIDGET_ID;
            return desc;
        }
        return undefined;
    };
    ProblemLayoutVersion3Migration = __decorate([
        inversify_1.injectable()
    ], ProblemLayoutVersion3Migration);
    return ProblemLayoutVersion3Migration;
}());
exports.ProblemLayoutVersion3Migration = ProblemLayoutVersion3Migration;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-layout-migrations'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-preferences.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-preferences.js ***!
  \*******************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
exports.ProblemConfigSchema = {
    'type': 'object',
    'properties': {
        'problems.decorations.enabled': {
            'type': 'boolean',
            'description': 'Show problem decorators (diagnostic markers) in tree widgets.',
            'default': true,
        },
        'problems.decorations.tabbar.enabled': {
            'type': 'boolean',
            'description': 'Show problem decorators (diagnostic markers) in the tab bars.',
            'default': true
        }
    }
};
exports.ProblemPreferences = Symbol('ProblemPreferences');
exports.createProblemPreferences = function (preferences) {
    return browser_1.createPreferenceProxy(preferences, exports.ProblemConfigSchema);
};
exports.bindProblemPreferences = function (bind) {
    bind(exports.ProblemPreferences).toDynamicValue(function (ctx) {
        var preferences = ctx.container.get(browser_1.PreferenceService);
        return exports.createProblemPreferences(preferences);
    });
    bind(browser_1.PreferenceContribution).toConstantValue({ schema: exports.ProblemConfigSchema });
};

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-preferences'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-selection.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-selection.js ***!
  \*****************************************************************************************************/
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
Object.defineProperty(exports, "__esModule", { value: true });
var selection_command_handler_1 = __webpack_require__(/*! @theia/core/lib/common/selection-command-handler */ "../../packages/core/lib/common/selection-command-handler.js");
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
var ProblemSelection;
(function (ProblemSelection) {
    function is(arg) {
        return typeof arg === 'object' && ('marker' in arg) && problem_marker_1.ProblemMarker.is(arg['marker']);
    }
    ProblemSelection.is = is;
    var CommandHandler = /** @class */ (function (_super) {
        __extends(CommandHandler, _super);
        function CommandHandler(selectionService, options) {
            var _this = _super.call(this, selectionService, function (arg) { return ProblemSelection.is(arg) ? arg : undefined; }, options) || this;
            _this.selectionService = selectionService;
            _this.options = options;
            return _this;
        }
        return CommandHandler;
    }(selection_command_handler_1.SelectionCommandHandler));
    ProblemSelection.CommandHandler = CommandHandler;
})(ProblemSelection = exports.ProblemSelection || (exports.ProblemSelection = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-selection'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-tabbar-decorator.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-tabbar-decorator.js ***!
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
var vscode_languageserver_types_1 = __webpack_require__(/*! vscode-languageserver-types */ "../../node_modules/vscode-languageserver-types/lib/esm/main.js");
var event_1 = __webpack_require__(/*! @theia/core/lib/common/event */ "../../packages/core/lib/common/event.js");
var widget_decoration_1 = __webpack_require__(/*! @theia/core/lib/browser/widget-decoration */ "../../packages/core/lib/browser/widget-decoration.js");
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var problem_preferences_1 = __webpack_require__(/*! ./problem-preferences */ "../../packages/markers/lib/browser/problem/problem-preferences.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var ProblemTabBarDecorator = /** @class */ (function () {
    function ProblemTabBarDecorator() {
        this.id = 'theia-problem-tabbar-decorator';
        this.emitter = new event_1.Emitter();
    }
    ProblemTabBarDecorator.prototype.init = function () {
        var _this = this;
        this.problemManager.onDidChangeMarkers(function () { return _this.fireDidChangeDecorations(); });
        this.preferences.onPreferenceChanged(function (event) { return _this.handlePreferenceChange(event); });
    };
    ProblemTabBarDecorator.prototype.decorate = function (title) {
        var e_1, _a;
        var widget = title.owner;
        if (browser_1.Navigatable.is(widget)) {
            var resourceUri = widget.getResourceUri();
            if (resourceUri) {
                // Get the list of problem markers for the given resource URI.
                var markers = this.problemManager.findMarkers({ uri: resourceUri });
                // If no markers are available, return early.
                if (markers.length === 0) {
                    return [];
                }
                // Store the marker with the highest severity.
                var maxSeverity = void 0;
                try {
                    // Iterate over available markers to determine that which has the highest severity.
                    // Only display a decoration if an error or warning marker is available.
                    for (var markers_1 = __values(markers), markers_1_1 = markers_1.next(); !markers_1_1.done; markers_1_1 = markers_1.next()) {
                        var marker = markers_1_1.value;
                        // Break early if an error marker is present, since it represents the highest severity.
                        if (marker.data.severity === vscode_languageserver_types_1.DiagnosticSeverity.Error) {
                            maxSeverity = marker;
                            break;
                        }
                        else if (marker.data.severity === vscode_languageserver_types_1.DiagnosticSeverity.Warning) {
                            maxSeverity = marker;
                        }
                    }
                }
                catch (e_1_1) { e_1 = { error: e_1_1 }; }
                finally {
                    try {
                        if (markers_1_1 && !markers_1_1.done && (_a = markers_1.return)) _a.call(markers_1);
                    }
                    finally { if (e_1) throw e_1.error; }
                }
                // Decorate the tabbar with the highest marker severity if available.
                return maxSeverity ? [this.toDecorator(maxSeverity)] : [];
            }
        }
        return [];
    };
    Object.defineProperty(ProblemTabBarDecorator.prototype, "onDidChangeDecorations", {
        get: function () {
            return this.emitter.event;
        },
        enumerable: true,
        configurable: true
    });
    ProblemTabBarDecorator.prototype.fireDidChangeDecorations = function () {
        this.emitter.fire(undefined);
    };
    /**
     * Handle changes in preference.
     * @param {PreferenceChangeEvent<ProblemConfiguration>} event The event of the changes in preference.
     */
    ProblemTabBarDecorator.prototype.handlePreferenceChange = function (event) {
        return __awaiter(this, void 0, void 0, function () {
            var preferenceName;
            return __generator(this, function (_a) {
                preferenceName = event.preferenceName;
                if (preferenceName === 'problems.decorations.tabbar.enabled') {
                    this.fireDidChangeDecorations();
                }
                return [2 /*return*/];
            });
        });
    };
    /**
     * Convert a diagnostic marker to a decorator.
     * @param {Marker<Diagnostic>} marker A diagnostic marker.
     * @returns {WidgetDecoration.Data} The decoration data.
     */
    ProblemTabBarDecorator.prototype.toDecorator = function (marker) {
        var position = widget_decoration_1.WidgetDecoration.IconOverlayPosition.BOTTOM_RIGHT;
        var icon = this.getOverlayIcon(marker);
        var color = this.getOverlayIconColor(marker);
        return {
            iconOverlay: {
                position: position,
                icon: icon,
                color: color,
                background: {
                    shape: 'circle',
                    color: 'transparent'
                }
            }
        };
    };
    /**
     * Get the appropriate overlay icon for decoration.
     * @param {Marker<Diagnostic>} marker A diagnostic marker.
     * @returns {string} A string representing the overlay icon class.
     */
    ProblemTabBarDecorator.prototype.getOverlayIcon = function (marker) {
        var severity = marker.data.severity;
        switch (severity) {
            case 1: return 'times-circle';
            case 2: return 'exclamation-circle';
            case 3: return 'info-circle';
            default: return 'hand-o-up';
        }
    };
    /**
     * Get the appropriate overlay icon color for decoration.
     * @param {Marker<Diagnostic>} marker A diagnostic marker.
     * @returns {WidgetDecoration.Color} The decoration color.
     */
    ProblemTabBarDecorator.prototype.getOverlayIconColor = function (marker) {
        var severity = marker.data.severity;
        switch (severity) {
            case 1: return 'var(--theia-editorError-foreground)';
            case 2: return 'var(--theia-editorWarning-foreground)';
            case 3: return 'var(--theia-editorInfo-foreground)';
            default: return 'var(--theia-successBackground)';
        }
    };
    __decorate([
        inversify_1.inject(problem_preferences_1.ProblemPreferences),
        __metadata("design:type", Object)
    ], ProblemTabBarDecorator.prototype, "preferences", void 0);
    __decorate([
        inversify_1.inject(problem_manager_1.ProblemManager),
        __metadata("design:type", problem_manager_1.ProblemManager)
    ], ProblemTabBarDecorator.prototype, "problemManager", void 0);
    __decorate([
        inversify_1.postConstruct(),
        __metadata("design:type", Function),
        __metadata("design:paramtypes", []),
        __metadata("design:returntype", void 0)
    ], ProblemTabBarDecorator.prototype, "init", null);
    ProblemTabBarDecorator = __decorate([
        inversify_1.injectable()
    ], ProblemTabBarDecorator);
    return ProblemTabBarDecorator;
}());
exports.ProblemTabBarDecorator = ProblemTabBarDecorator;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-tabbar-decorator'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-tree-model.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-tree-model.js ***!
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
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var marker_tree_1 = __webpack_require__(/*! ../marker-tree */ "../../packages/markers/lib/browser/marker-tree.js");
var marker_tree_model_1 = __webpack_require__(/*! ../marker-tree-model */ "../../packages/markers/lib/browser/marker-tree-model.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var ProblemTree = /** @class */ (function (_super) {
    __extends(ProblemTree, _super);
    function ProblemTree(problemManager, markerOptions) {
        var _this = _super.call(this, problemManager, markerOptions) || this;
        _this.problemManager = problemManager;
        _this.markerOptions = markerOptions;
        return _this;
    }
    ProblemTree = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(problem_manager_1.ProblemManager)),
        __param(1, inversify_1.inject(marker_tree_1.MarkerOptions)),
        __metadata("design:paramtypes", [problem_manager_1.ProblemManager, Object])
    ], ProblemTree);
    return ProblemTree;
}(marker_tree_1.MarkerTree));
exports.ProblemTree = ProblemTree;
var ProblemTreeModel = /** @class */ (function (_super) {
    __extends(ProblemTreeModel, _super);
    function ProblemTreeModel() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    ProblemTreeModel.prototype.getOpenerOptionsByMarker = function (node) {
        if (problem_marker_1.ProblemMarker.is(node.marker)) {
            return {
                selection: node.marker.data.range
            };
        }
        return undefined;
    };
    ProblemTreeModel.prototype.removeNode = function (node) {
        if (marker_tree_1.MarkerInfoNode.is(node)) {
            this.problemManager.cleanAllMarkers(node.uri);
        }
        if (marker_tree_1.MarkerNode.is(node)) {
            var uri = node.uri;
            var owner = node.marker.owner;
            var diagnostics = this.problemManager.findMarkers({ uri: uri, owner: owner, dataFilter: function (data) { return node.marker.data !== data; } }).map(function (_a) {
                var data = _a.data;
                return data;
            });
            this.problemManager.setMarkers(uri, owner, diagnostics);
        }
    };
    __decorate([
        inversify_1.inject(problem_manager_1.ProblemManager),
        __metadata("design:type", problem_manager_1.ProblemManager)
    ], ProblemTreeModel.prototype, "problemManager", void 0);
    ProblemTreeModel = __decorate([
        inversify_1.injectable()
    ], ProblemTreeModel);
    return ProblemTreeModel;
}(marker_tree_model_1.MarkerTreeModel));
exports.ProblemTreeModel = ProblemTreeModel;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-tree-model'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/lib/browser/problem/problem-widget.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/lib/browser/problem/problem-widget.js ***!
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
var problem_manager_1 = __webpack_require__(/*! ./problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var problem_marker_1 = __webpack_require__(/*! ../../common/problem-marker */ "../../packages/markers/lib/common/problem-marker.js");
var problem_tree_model_1 = __webpack_require__(/*! ./problem-tree-model */ "../../packages/markers/lib/browser/problem/problem-tree-model.js");
var marker_tree_1 = __webpack_require__(/*! ../marker-tree */ "../../packages/markers/lib/browser/marker-tree.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var React = __webpack_require__(/*! react */ "../../node_modules/react/index.js");
exports.PROBLEMS_WIDGET_ID = 'problems';
var ProblemWidget = /** @class */ (function (_super) {
    __extends(ProblemWidget, _super);
    function ProblemWidget(problemManager, treeProps, model, contextMenuRenderer) {
        var _this = _super.call(this, treeProps, model, contextMenuRenderer) || this;
        _this.problemManager = problemManager;
        _this.treeProps = treeProps;
        _this.model = model;
        _this.contextMenuRenderer = contextMenuRenderer;
        _this.id = exports.PROBLEMS_WIDGET_ID;
        _this.title.label = 'Problems';
        _this.title.caption = 'Problems';
        _this.title.iconClass = 'fa problem-tab-icon';
        _this.title.closable = true;
        _this.addClass('theia-marker-container');
        _this.addClipboardListener(_this.node, 'copy', function (e) { return _this.handleCopy(e); });
        return _this;
    }
    ProblemWidget.prototype.storeState = function () {
        // no-op
        return {};
    };
    ProblemWidget.prototype.superStoreState = function () {
        return _super.prototype.storeState.call(this);
    };
    ProblemWidget.prototype.restoreState = function (state) {
        // no-op
    };
    ProblemWidget.prototype.superRestoreState = function (state) {
        _super.prototype.restoreState.call(this, state);
        return;
    };
    ProblemWidget.prototype.handleClickEvent = function (node, event) {
        _super.prototype.handleClickEvent.call(this, node, event);
        if (marker_tree_1.MarkerNode.is(node)) {
            this.model.revealNode(node);
        }
    };
    ProblemWidget.prototype.handleCopy = function (event) {
        var uris = this.model.selectedNodes.filter(marker_tree_1.MarkerNode.is).map(function (node) { return node.uri.toString(); });
        if (uris.length > 0 && event.clipboardData) {
            event.clipboardData.setData('text/plain', uris.join('\n'));
            event.preventDefault();
        }
    };
    ProblemWidget.prototype.handleDown = function (event) {
        var node = this.model.getNextSelectableNode();
        _super.prototype.handleDown.call(this, event);
        if (marker_tree_1.MarkerNode.is(node)) {
            this.model.revealNode(node);
        }
    };
    ProblemWidget.prototype.handleUp = function (event) {
        var node = this.model.getPrevSelectableNode();
        _super.prototype.handleUp.call(this, event);
        if (marker_tree_1.MarkerNode.is(node)) {
            this.model.revealNode(node);
        }
    };
    ProblemWidget.prototype.renderTree = function (model) {
        if (marker_tree_1.MarkerRootNode.is(model.root) && model.root.children.length > 0) {
            return _super.prototype.renderTree.call(this, model);
        }
        return React.createElement("div", { className: 'theia-widget-noInfo noMarkers' }, "No problems have been detected in the workspace so far.");
    };
    ProblemWidget.prototype.renderCaption = function (node, props) {
        if (marker_tree_1.MarkerInfoNode.is(node)) {
            return this.decorateMarkerFileNode(node);
        }
        else if (marker_tree_1.MarkerNode.is(node)) {
            return this.decorateMarkerNode(node);
        }
        return 'caption';
    };
    ProblemWidget.prototype.renderTailDecorations = function (node, props) {
        return React.createElement("div", { className: 'row-button-container' }, this.renderRemoveButton(node));
    };
    ProblemWidget.prototype.renderRemoveButton = function (node) {
        return React.createElement(ProblemMarkerRemoveButton, { model: this.model, node: node });
    };
    ProblemWidget.prototype.decorateMarkerNode = function (node) {
        if (problem_marker_1.ProblemMarker.is(node.marker)) {
            var severityClass = '';
            var problemMarker = node.marker;
            if (problemMarker.data.severity) {
                severityClass = this.getSeverityClass(problemMarker.data.severity);
            }
            return React.createElement("div", { className: 'markerNode', title: problemMarker.data.message + " (" + (problemMarker.data.range.start.line + 1) + ", " + (problemMarker.data.range.start.character + 1) + ")" },
                React.createElement("div", null,
                    React.createElement("i", { className: severityClass })),
                React.createElement("div", { className: 'owner' }, '[' + (problemMarker.data.source || problemMarker.owner) + ']'),
                React.createElement("div", { className: 'message' },
                    problemMarker.data.message,
                    (problemMarker.data.code) ? React.createElement("span", { className: 'code' }, '[' + problemMarker.data.code + ']') : '',
                    React.createElement("span", { className: 'position' }, '(' + (problemMarker.data.range.start.line + 1) + ', ' + (problemMarker.data.range.start.character + 1) + ')')));
        }
        return '';
    };
    ProblemWidget.prototype.getSeverityClass = function (severity) {
        switch (severity) {
            case 1: return 'fa fa-times-circle error';
            case 2: return 'fa fa-exclamation-circle warning';
            case 3: return 'fa fa-info-circle information';
            default: return 'fa fa-hand-o-up hint';
        }
    };
    ProblemWidget.prototype.decorateMarkerFileNode = function (node) {
        var icon = this.toNodeIcon(node);
        var name = this.toNodeName(node);
        var description = this.toNodeDescription(node);
        // Use a custom scheme so that we fallback to the `DefaultUriLabelProviderContribution`.
        var path = this.labelProvider.getLongName(node.uri.withScheme('marker'));
        return React.createElement("div", { title: path, className: 'markerFileNode' },
            icon && React.createElement("div", { className: icon + ' file-icon' }),
            React.createElement("div", { className: 'name' }, name),
            React.createElement("div", { className: 'path' }, description),
            React.createElement("div", { className: 'notification-count-container' },
                React.createElement("span", { className: 'notification-count' }, node.numberOfMarkers.toString())));
    };
    ProblemWidget = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(problem_manager_1.ProblemManager)),
        __param(1, inversify_1.inject(browser_1.TreeProps)),
        __param(2, inversify_1.inject(problem_tree_model_1.ProblemTreeModel)),
        __param(3, inversify_1.inject(browser_1.ContextMenuRenderer)),
        __metadata("design:paramtypes", [problem_manager_1.ProblemManager, Object, problem_tree_model_1.ProblemTreeModel, Object])
    ], ProblemWidget);
    return ProblemWidget;
}(browser_1.TreeWidget));
exports.ProblemWidget = ProblemWidget;
var ProblemMarkerRemoveButton = /** @class */ (function (_super) {
    __extends(ProblemMarkerRemoveButton, _super);
    function ProblemMarkerRemoveButton() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.remove = function (e) { return _this.doRemove(e); };
        return _this;
    }
    ProblemMarkerRemoveButton.prototype.render = function () {
        return React.createElement("span", { className: 'remove-node', onClick: this.remove });
    };
    ProblemMarkerRemoveButton.prototype.doRemove = function (e) {
        this.props.model.removeNode(this.props.node);
        e.stopPropagation();
    };
    return ProblemMarkerRemoveButton;
}(React.Component));
exports.ProblemMarkerRemoveButton = ProblemMarkerRemoveButton;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/markers/lib/browser/problem/problem-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/markers/src/browser/style/index.css":
/*!****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/markers/src/browser/style/index.css ***!
  \****************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/markers/src/browser/style/index.css");

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
//# sourceMappingURL=28.bundle.js.map