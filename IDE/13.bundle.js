(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[13],{

/***/ "../../node_modules/@babel/runtime/helpers/arrayWithHoles.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/@babel/runtime/helpers/arrayWithHoles.js ***!
  \*************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {function _arrayWithHoles(arr) {
  if (Array.isArray(arr)) return arr;
}

module.exports = _arrayWithHoles;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['@babel/runtime/helpers/arrayWithHoles'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/@babel/runtime/helpers/iterableToArrayLimit.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/@babel/runtime/helpers/iterableToArrayLimit.js ***!
  \*******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {function _iterableToArrayLimit(arr, i) {
  if (!(Symbol.iterator in Object(arr) || Object.prototype.toString.call(arr) === "[object Arguments]")) {
    return;
  }

  var _arr = [];
  var _n = true;
  var _d = false;
  var _e = undefined;

  try {
    for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) {
      _arr.push(_s.value);

      if (i && _arr.length === i) break;
    }
  } catch (err) {
    _d = true;
    _e = err;
  } finally {
    try {
      if (!_n && _i["return"] != null) _i["return"]();
    } finally {
      if (_d) throw _e;
    }
  }

  return _arr;
}

module.exports = _iterableToArrayLimit;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['@babel/runtime/helpers/iterableToArrayLimit'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/@babel/runtime/helpers/nonIterableRest.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/@babel/runtime/helpers/nonIterableRest.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {function _nonIterableRest() {
  throw new TypeError("Invalid attempt to destructure non-iterable instance");
}

module.exports = _nonIterableRest;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['@babel/runtime/helpers/nonIterableRest'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/@babel/runtime/helpers/slicedToArray.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/@babel/runtime/helpers/slicedToArray.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {var arrayWithHoles = __webpack_require__(/*! ./arrayWithHoles */ "../../node_modules/@babel/runtime/helpers/arrayWithHoles.js");

var iterableToArrayLimit = __webpack_require__(/*! ./iterableToArrayLimit */ "../../node_modules/@babel/runtime/helpers/iterableToArrayLimit.js");

var nonIterableRest = __webpack_require__(/*! ./nonIterableRest */ "../../node_modules/@babel/runtime/helpers/nonIterableRest.js");

function _slicedToArray(arr, i) {
  return arrayWithHoles(arr) || iterableToArrayLimit(arr, i) || nonIterableRest();
}

module.exports = _slicedToArray;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['@babel/runtime/helpers/slicedToArray'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/@babel/runtime/regenerator/index.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/@babel/runtime/regenerator/index.js ***!
  \********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {module.exports = __webpack_require__(/*! regenerator-runtime */ "../../node_modules/regenerator-runtime/runtime.js");

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@babel/runtime/regenerator'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/glob-to-regexp/index.js":
/*!********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/glob-to-regexp/index.js ***!
  \********************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {module.exports = function (glob, opts) {
  if (typeof glob !== 'string') {
    throw new TypeError('Expected a string');
  }

  var str = String(glob);

  // The regexp we are building, as a string.
  var reStr = "";

  // Whether we are matching so called "extended" globs (like bash) and should
  // support single character matching, matching ranges of characters, group
  // matching, etc.
  var extended = opts ? !!opts.extended : false;

  // When globstar is _false_ (default), '/foo/*' is translated a regexp like
  // '^\/foo\/.*$' which will match any string beginning with '/foo/'
  // When globstar is _true_, '/foo/*' is translated to regexp like
  // '^\/foo\/[^/]*$' which will match any string beginning with '/foo/' BUT
  // which does not have a '/' to the right of it.
  // E.g. with '/foo/*' these will match: '/foo/bar', '/foo/bar.txt' but
  // these will not '/foo/bar/baz', '/foo/bar/baz.txt'
  // Lastely, when globstar is _true_, '/foo/**' is equivelant to '/foo/*' when
  // globstar is _false_
  var globstar = opts ? !!opts.globstar : false;

  // If we are doing extended matching, this boolean is true when we are inside
  // a group (eg {*.html,*.js}), and false otherwise.
  var inGroup = false;

  // RegExp flags (eg "i" ) to pass in to RegExp constructor.
  var flags = opts && typeof( opts.flags ) === "string" ? opts.flags : "";

  var c;
  for (var i = 0, len = str.length; i < len; i++) {
    c = str[i];

    switch (c) {
    case "\\":
    case "/":
    case "$":
    case "^":
    case "+":
    case ".":
    case "(":
    case ")":
    case "=":
    case "!":
    case "|":
      reStr += "\\" + c;
      break;

    case "?":
      if (extended) {
        reStr += ".";
	    break;
      }

    case "[":
    case "]":
      if (extended) {
        reStr += c;
	    break;
      }

    case "{":
      if (extended) {
        inGroup = true;
	    reStr += "(";
	    break;
      }

    case "}":
      if (extended) {
        inGroup = false;
	    reStr += ")";
	    break;
      }

    case ",":
      if (inGroup) {
        reStr += "|";
	    break;
      }
      reStr += "\\" + c;
      break;

    case "*":
      // Move over all consecutive "*"'s.
      // Also store the previous and next characters
      var prevChar = str[i - 1];
      var starCount = 1;
      while(str[i + 1] === "*") {
        starCount++;
        i++;
      }
      var nextChar = str[i + 1];

      if (!globstar) {
        // globstar is disabled, so treat any number of "*" as one
        reStr += ".*";
      } else {
        // globstar is enabled, so determine if this is a globstar segment
        var isGlobstar = starCount > 1                      // multiple "*"'s
          && (prevChar === "/" || prevChar === undefined)   // from the start of the segment
          && (nextChar === "/" || nextChar === undefined)   // to the end of the segment

        if (isGlobstar) {
          // it's a globstar, so match zero or more path segments
          reStr += "(?:[^/]*(?:\/|$))*";
          i++; // move over the "/"
        } else {
          // it's not a globstar, so only match one path segment
          reStr += "[^/]*";
        }
      }
      break;

    default:
      reStr += c;
    }
  }

  // When regexp 'g' flag is specified don't
  // constrain the regular expression with ^ & $
  if (!flags || !~flags.indexOf('g')) {
    reStr = "^" + reStr + "$";
  }

  return new RegExp(reStr, flags);
};

if (!global) global = {};
(global['theia'] = global['theia'] || {})['glob-to-regexp'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/connection.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/connection.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var main_1 = __webpack_require__(/*! vscode-languageserver-protocol/lib/main */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var Is = __webpack_require__(/*! vscode-languageserver-protocol/lib/utils/is */ "../../node_modules/vscode-languageserver-protocol/lib/utils/is.js");

function createConnection(connection, errorHandler, closeHandler) {
  connection.onError(function (data) {
    errorHandler(data[0], data[1], data[2]);
  });
  connection.onClose(closeHandler);
  return {
    listen: function listen() {
      return connection.listen();
    },
    sendRequest: function sendRequest(type) {
      for (var _len = arguments.length, params = new Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        params[_key - 1] = arguments[_key];
      }

      return connection.sendRequest.apply(connection, [Is.string(type) ? type : type.method].concat(params));
    },
    onRequest: function onRequest(type, handler) {
      return connection.onRequest(Is.string(type) ? type : type.method, handler);
    },
    sendNotification: function sendNotification(type, params) {
      return connection.sendNotification(Is.string(type) ? type : type.method, params);
    },
    onNotification: function onNotification(type, handler) {
      return connection.onNotification(Is.string(type) ? type : type.method, handler);
    },
    trace: function trace(value, tracer) {
      var sendNotification = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : false;
      return connection.trace(value, tracer, sendNotification);
    },
    initialize: function initialize(params) {
      return connection.sendRequest(main_1.InitializeRequest.type, params);
    },
    shutdown: function shutdown() {
      return connection.sendRequest(main_1.ShutdownRequest.type, undefined);
    },
    exit: function exit() {
      return connection.sendNotification(main_1.ExitNotification.type);
    },
    onLogMessage: function onLogMessage(handler) {
      return connection.onNotification(main_1.LogMessageNotification.type, handler);
    },
    onShowMessage: function onShowMessage(handler) {
      return connection.onNotification(main_1.ShowMessageNotification.type, handler);
    },
    onTelemetry: function onTelemetry(handler) {
      return connection.onNotification(main_1.TelemetryEventNotification.type, handler);
    },
    didChangeConfiguration: function didChangeConfiguration(params) {
      return connection.sendNotification(main_1.DidChangeConfigurationNotification.type, params);
    },
    didChangeWatchedFiles: function didChangeWatchedFiles(params) {
      return connection.sendNotification(main_1.DidChangeWatchedFilesNotification.type, params);
    },
    didOpenTextDocument: function didOpenTextDocument(params) {
      return connection.sendNotification(main_1.DidOpenTextDocumentNotification.type, params);
    },
    didChangeTextDocument: function didChangeTextDocument(params) {
      return connection.sendNotification(main_1.DidChangeTextDocumentNotification.type, params);
    },
    didCloseTextDocument: function didCloseTextDocument(params) {
      return connection.sendNotification(main_1.DidCloseTextDocumentNotification.type, params);
    },
    didSaveTextDocument: function didSaveTextDocument(params) {
      return connection.sendNotification(main_1.DidSaveTextDocumentNotification.type, params);
    },
    onDiagnostics: function onDiagnostics(handler) {
      return connection.onNotification(main_1.PublishDiagnosticsNotification.type, handler);
    },
    dispose: function dispose() {
      return connection.dispose();
    }
  };
}

exports.createConnection = createConnection;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/connection'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/console-window.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/console-window.js ***!
  \****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

var ConsoleWindow =
/*#__PURE__*/
function () {
  function ConsoleWindow() {
    _classCallCheck(this, ConsoleWindow);

    this.channels = new Map();
  }

  _createClass(ConsoleWindow, [{
    key: "showMessage",
    value: function showMessage(type, message) {
      if (type === services_1.MessageType.Error) {
        console.error(message);
      }

      if (type === services_1.MessageType.Warning) {
        console.warn(message);
      }

      if (type === services_1.MessageType.Info) {
        console.info(message);
      }

      if (type === services_1.MessageType.Log) {
        console.log(message);
      }

      return Promise.resolve(undefined);
    }
  }, {
    key: "createOutputChannel",
    value: function createOutputChannel(name) {
      var existing = this.channels.get(name);

      if (existing) {
        return existing;
      }

      var channel = {
        append: function append(value) {
          console.log(name + ': ' + value);
        },
        appendLine: function appendLine(line) {
          console.log(name + ': ' + line);
        },
        show: function show() {// no-op
        },
        dispose: function dispose() {// no-op
        }
      };
      this.channels.set(name, channel);
      return channel;
    }
  }]);

  return ConsoleWindow;
}();

exports.ConsoleWindow = ConsoleWindow;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/console-window'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/disposable.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/disposable.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

exports.Disposable = services_1.Disposable;

var DisposableCollection =
/*#__PURE__*/
function () {
  function DisposableCollection() {
    _classCallCheck(this, DisposableCollection);

    this.disposables = [];
  }

  _createClass(DisposableCollection, [{
    key: "dispose",
    value: function dispose() {
      while (this.disposables.length !== 0) {
        this.disposables.pop().dispose();
      }
    }
  }, {
    key: "push",
    value: function push(disposable) {
      var disposables = this.disposables;
      disposables.push(disposable);
      return {
        dispose: function dispose() {
          var index = disposables.indexOf(disposable);

          if (index !== -1) {
            disposables.splice(index, 1);
          }
        }
      };
    }
  }]);

  return DisposableCollection;
}();

exports.DisposableCollection = DisposableCollection;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/disposable'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/index.js":
/*!*******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/index.js ***!
  \*******************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

__export(__webpack_require__(/*! ./disposable */ "../../node_modules/monaco-languageclient/lib/disposable.js"));

__export(__webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js"));

__export(__webpack_require__(/*! ./connection */ "../../node_modules/monaco-languageclient/lib/connection.js"));

__export(__webpack_require__(/*! ./monaco-language-client */ "../../node_modules/monaco-languageclient/lib/monaco-language-client.js"));

__export(__webpack_require__(/*! ./monaco-commands */ "../../node_modules/monaco-languageclient/lib/monaco-commands.js"));

__export(__webpack_require__(/*! ./console-window */ "../../node_modules/monaco-languageclient/lib/console-window.js"));

__export(__webpack_require__(/*! ./monaco-languages */ "../../node_modules/monaco-languageclient/lib/monaco-languages.js"));

__export(__webpack_require__(/*! ./monaco-workspace */ "../../node_modules/monaco-languageclient/lib/monaco-workspace.js"));

__export(__webpack_require__(/*! ./monaco-services */ "../../node_modules/monaco-languageclient/lib/monaco-services.js"));

__export(__webpack_require__(/*! ./monaco-converter */ "../../node_modules/monaco-languageclient/lib/monaco-converter.js"));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-commands.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-commands.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var MonacoCommands =
/*#__PURE__*/
function () {
  function MonacoCommands(editor) {
    _classCallCheck(this, MonacoCommands);

    this.editor = editor;
  }

  _createClass(MonacoCommands, [{
    key: "registerCommand",
    value: function registerCommand(command, callback, thisArg) {
      return this.editor._commandService.addCommand({
        id: command,
        handler: function handler(_accessor) {
          for (var _len = arguments.length, args = new Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
            args[_key - 1] = arguments[_key];
          }

          return callback.apply(void 0, args);
        }
      });
    }
  }]);

  return MonacoCommands;
}();

exports.MonacoCommands = MonacoCommands;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-commands'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-converter.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-converter.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _toConsumableArray = __webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "../../node_modules/@babel/runtime/helpers/toConsumableArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var ls = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var Is = __webpack_require__(/*! vscode-languageserver-protocol/lib/utils/is */ "../../node_modules/vscode-languageserver-protocol/lib/utils/is.js");

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

var ProtocolDocumentLink;

(function (ProtocolDocumentLink) {
  function is(item) {
    return !!item && 'data' in item;
  }

  ProtocolDocumentLink.is = is;
})(ProtocolDocumentLink = exports.ProtocolDocumentLink || (exports.ProtocolDocumentLink = {}));

var ProtocolCodeLens;

(function (ProtocolCodeLens) {
  function is(item) {
    return !!item && 'data' in item;
  }

  ProtocolCodeLens.is = is;
})(ProtocolCodeLens = exports.ProtocolCodeLens || (exports.ProtocolCodeLens = {}));

var ProtocolCompletionItem;

(function (ProtocolCompletionItem) {
  function is(item) {
    return !!item && 'data' in item;
  }

  ProtocolCompletionItem.is = is;
})(ProtocolCompletionItem = exports.ProtocolCompletionItem || (exports.ProtocolCompletionItem = {}));

var MonacoToProtocolConverter =
/*#__PURE__*/
function () {
  function MonacoToProtocolConverter() {
    _classCallCheck(this, MonacoToProtocolConverter);
  }

  _createClass(MonacoToProtocolConverter, [{
    key: "asPosition",
    value: function asPosition(lineNumber, column) {
      var line = lineNumber === undefined || lineNumber === null ? undefined : lineNumber - 1;
      var character = column === undefined || column === null ? undefined : column - 1;
      return {
        line: line,
        character: character
      };
    }
  }, {
    key: "asRange",
    value: function asRange(range) {
      if (range === undefined) {
        return undefined;
      }

      if (range === null) {
        return null;
      }

      var start = this.asPosition(range.startLineNumber, range.startColumn);
      var end = this.asPosition(range.endLineNumber, range.endColumn);
      return {
        start: start,
        end: end
      };
    }
  }, {
    key: "asTextDocumentIdentifier",
    value: function asTextDocumentIdentifier(model) {
      return {
        uri: model.uri.toString()
      };
    }
  }, {
    key: "asTextDocumentPositionParams",
    value: function asTextDocumentPositionParams(model, position) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        position: this.asPosition(position.lineNumber, position.column)
      };
    }
  }, {
    key: "asCompletionParams",
    value: function asCompletionParams(model, position, context) {
      return Object.assign(this.asTextDocumentPositionParams(model, position), {
        context: this.asCompletionContext(context)
      });
    }
  }, {
    key: "asCompletionContext",
    value: function asCompletionContext(context) {
      return {
        triggerKind: this.asTriggerKind(context.triggerKind),
        triggerCharacter: context.triggerCharacter
      };
    }
  }, {
    key: "asTriggerKind",
    value: function asTriggerKind(triggerKind) {
      switch (triggerKind) {
        case monaco.languages.CompletionTriggerKind.TriggerCharacter:
          return services_1.CompletionTriggerKind.TriggerCharacter;

        case monaco.languages.CompletionTriggerKind.TriggerForIncompleteCompletions:
          return services_1.CompletionTriggerKind.TriggerForIncompleteCompletions;

        default:
          return services_1.CompletionTriggerKind.Invoked;
      }
    }
  }, {
    key: "asCompletionItem",
    value: function asCompletionItem(item) {
      var result = {
        label: item.label
      };
      var protocolItem = ProtocolCompletionItem.is(item) ? item : undefined;

      if (item.detail) {
        result.detail = item.detail;
      } // We only send items back we created. So this can't be something else than
      // a string right now.


      if (item.documentation) {
        if (!protocolItem || !protocolItem.documentationFormat) {
          result.documentation = item.documentation;
        } else {
          result.documentation = this.asDocumentation(protocolItem.documentationFormat, item.documentation);
        }
      }

      if (item.filterText) {
        result.filterText = item.filterText;
      }

      this.fillPrimaryInsertText(result, item);

      if (Is.number(item.kind)) {
        result.kind = this.asCompletionItemKind(item.kind, protocolItem && protocolItem.originalItemKind);
      }

      if (item.sortText) {
        result.sortText = item.sortText;
      }

      if (item.additionalTextEdits) {
        result.additionalTextEdits = this.asTextEdits(item.additionalTextEdits);
      }

      if (item.command) {
        result.command = this.asCommand(item.command);
      }

      if (item.commitCharacters) {
        result.commitCharacters = item.commitCharacters.slice();
      }

      if (item.command) {
        result.command = this.asCommand(item.command);
      } // TODO if (item.preselect === true || item.preselect === false) { result.preselect = item.preselect; }


      if (protocolItem) {
        if (protocolItem.data !== undefined) {
          result.data = protocolItem.data;
        }

        if (protocolItem.deprecated === true || protocolItem.deprecated === false) {
          result.deprecated = protocolItem.deprecated;
        }
      }

      return result;
    }
  }, {
    key: "asCompletionItemKind",
    value: function asCompletionItemKind(value, original) {
      if (original !== undefined) {
        return original;
      }

      switch (value) {
        case monaco.languages.CompletionItemKind.Method:
          return services_1.CompletionItemKind.Method;

        case monaco.languages.CompletionItemKind.Function:
          return services_1.CompletionItemKind.Function;

        case monaco.languages.CompletionItemKind.Constructor:
          return services_1.CompletionItemKind.Constructor;

        case monaco.languages.CompletionItemKind.Field:
          return services_1.CompletionItemKind.Field;

        case monaco.languages.CompletionItemKind.Variable:
          return services_1.CompletionItemKind.Variable;

        case monaco.languages.CompletionItemKind.Class:
          return services_1.CompletionItemKind.Class;

        case monaco.languages.CompletionItemKind.Struct:
          return services_1.CompletionItemKind.Struct;

        case monaco.languages.CompletionItemKind.Interface:
          return services_1.CompletionItemKind.Interface;

        case monaco.languages.CompletionItemKind.Module:
          return services_1.CompletionItemKind.Module;

        case monaco.languages.CompletionItemKind.Property:
          return services_1.CompletionItemKind.Property;

        case monaco.languages.CompletionItemKind.Event:
          return services_1.CompletionItemKind.Event;

        case monaco.languages.CompletionItemKind.Operator:
          return services_1.CompletionItemKind.Operator;

        case monaco.languages.CompletionItemKind.Unit:
          return services_1.CompletionItemKind.Unit;

        case monaco.languages.CompletionItemKind.Value:
          return services_1.CompletionItemKind.Value;

        case monaco.languages.CompletionItemKind.Constant:
          return services_1.CompletionItemKind.Constant;

        case monaco.languages.CompletionItemKind.Enum:
          return services_1.CompletionItemKind.Enum;

        case monaco.languages.CompletionItemKind.EnumMember:
          return services_1.CompletionItemKind.EnumMember;

        case monaco.languages.CompletionItemKind.Keyword:
          return services_1.CompletionItemKind.Keyword;

        case monaco.languages.CompletionItemKind.Text:
          return services_1.CompletionItemKind.Text;

        case monaco.languages.CompletionItemKind.Color:
          return services_1.CompletionItemKind.Color;

        case monaco.languages.CompletionItemKind.File:
          return services_1.CompletionItemKind.File;

        case monaco.languages.CompletionItemKind.Reference:
          return services_1.CompletionItemKind.Reference;

        case monaco.languages.CompletionItemKind.Customcolor:
          return services_1.CompletionItemKind.Color;

        case monaco.languages.CompletionItemKind.Folder:
          return services_1.CompletionItemKind.Folder;

        case monaco.languages.CompletionItemKind.TypeParameter:
          return services_1.CompletionItemKind.TypeParameter;

        case monaco.languages.CompletionItemKind.Snippet:
          return services_1.CompletionItemKind.Snippet;

        default:
          return value + 1;
      }
    }
  }, {
    key: "asDocumentation",
    value: function asDocumentation(format, documentation) {
      switch (format) {
        case services_1.MarkupKind.PlainText:
          return {
            kind: format,
            value: documentation
          };

        case services_1.MarkupKind.Markdown:
          return {
            kind: format,
            value: documentation.value
          };

        default:
          return "Unsupported Markup content received. Kind is: ".concat(format);
      }
    }
  }, {
    key: "fillPrimaryInsertText",
    value: function fillPrimaryInsertText(target, source) {
      var format = services_1.InsertTextFormat.PlainText;
      var text;
      var range;

      if (source.insertTextRules !== undefined && (source.insertTextRules & monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet) === 0) {
        format = services_1.InsertTextFormat.Snippet;
        text = source.insertText;
      }

      target.insertTextFormat = format;
      text = source.insertText;

      if (source.range) {
        range = this.asRange(source.range);
      }

      target.insertTextFormat = format;

      if (source.fromEdit && text && range) {
        target.textEdit = {
          newText: text,
          range: range
        };
      } else {
        target.insertText = text;
      }
    }
  }, {
    key: "asTextEdit",
    value: function asTextEdit(edit) {
      var range = this.asRange(edit.range);
      return {
        range: range,
        newText: edit.text || ''
      };
    }
  }, {
    key: "asTextEdits",
    value: function asTextEdits(items) {
      var _this = this;

      if (!items) {
        return undefined;
      }

      return items.map(function (item) {
        return _this.asTextEdit(item);
      });
    }
  }, {
    key: "asReferenceParams",
    value: function asReferenceParams(model, position, options) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        position: this.asPosition(position.lineNumber, position.column),
        context: {
          includeDeclaration: options.includeDeclaration
        }
      };
    }
  }, {
    key: "asDocumentSymbolParams",
    value: function asDocumentSymbolParams(model) {
      return {
        textDocument: this.asTextDocumentIdentifier(model)
      };
    }
  }, {
    key: "asCodeLensParams",
    value: function asCodeLensParams(model) {
      return {
        textDocument: this.asTextDocumentIdentifier(model)
      };
    }
  }, {
    key: "asDiagnosticSeverity",
    value: function asDiagnosticSeverity(value) {
      switch (value) {
        case monaco.MarkerSeverity.Error:
          return services_1.DiagnosticSeverity.Error;

        case monaco.MarkerSeverity.Warning:
          return services_1.DiagnosticSeverity.Warning;

        case monaco.MarkerSeverity.Info:
          return services_1.DiagnosticSeverity.Information;

        case monaco.MarkerSeverity.Hint:
          return services_1.DiagnosticSeverity.Hint;
      }

      return undefined;
    }
  }, {
    key: "asDiagnostic",
    value: function asDiagnostic(marker) {
      var range = this.asRange(new monaco.Range(marker.startLineNumber, marker.startColumn, marker.endLineNumber, marker.endColumn));
      var severity = this.asDiagnosticSeverity(marker.severity);
      return services_1.Diagnostic.create(range, marker.message, severity, marker.code, marker.source);
    }
  }, {
    key: "asDiagnostics",
    value: function asDiagnostics(markers) {
      var _this2 = this;

      if (markers === void 0 || markers === null) {
        return markers;
      }

      return markers.map(function (marker) {
        return _this2.asDiagnostic(marker);
      });
    }
  }, {
    key: "asCodeActionContext",
    value: function asCodeActionContext(context) {
      if (context === void 0 || context === null) {
        return context;
      }

      var diagnostics = this.asDiagnostics(context.markers);
      return services_1.CodeActionContext.create(diagnostics, Is.string(context.only) ? [context.only] : undefined);
    }
  }, {
    key: "asCodeActionParams",
    value: function asCodeActionParams(model, range, context) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        range: this.asRange(range),
        context: this.asCodeActionContext(context)
      };
    }
  }, {
    key: "asCommand",
    value: function asCommand(item) {
      if (item) {
        var _services_1$Command;

        var args = item.arguments || [];
        return (_services_1$Command = services_1.Command).create.apply(_services_1$Command, [item.title, item.id].concat(_toConsumableArray(args)));
      }

      return undefined;
    }
  }, {
    key: "asCodeLens",
    value: function asCodeLens(item) {
      var result = services_1.CodeLens.create(this.asRange(item.range));

      if (item.command) {
        result.command = this.asCommand(item.command);
      }

      if (ProtocolCodeLens.is(item)) {
        if (item.data) {
          result.data = item.data;
        }

        ;
      }

      return result;
    }
  }, {
    key: "asFormattingOptions",
    value: function asFormattingOptions(options) {
      return {
        tabSize: options.tabSize,
        insertSpaces: options.insertSpaces
      };
    }
  }, {
    key: "asDocumentFormattingParams",
    value: function asDocumentFormattingParams(model, options) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        options: this.asFormattingOptions(options)
      };
    }
  }, {
    key: "asDocumentRangeFormattingParams",
    value: function asDocumentRangeFormattingParams(model, range, options) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        range: this.asRange(range),
        options: this.asFormattingOptions(options)
      };
    }
  }, {
    key: "asDocumentOnTypeFormattingParams",
    value: function asDocumentOnTypeFormattingParams(model, position, ch, options) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        position: this.asPosition(position.lineNumber, position.column),
        ch: ch,
        options: this.asFormattingOptions(options)
      };
    }
  }, {
    key: "asRenameParams",
    value: function asRenameParams(model, position, newName) {
      return {
        textDocument: this.asTextDocumentIdentifier(model),
        position: this.asPosition(position.lineNumber, position.column),
        newName: newName
      };
    }
  }, {
    key: "asDocumentLinkParams",
    value: function asDocumentLinkParams(model) {
      return {
        textDocument: this.asTextDocumentIdentifier(model)
      };
    }
  }, {
    key: "asDocumentLink",
    value: function asDocumentLink(item) {
      var result = services_1.DocumentLink.create(this.asRange(item.range));

      if (item.url) {
        result.target = typeof item.url === 'string' ? item.url : item.url.toString();
      }

      if (ProtocolDocumentLink.is(item) && item.data) {
        result.data = item.data;
      }

      return result;
    }
  }]);

  return MonacoToProtocolConverter;
}();

exports.MonacoToProtocolConverter = MonacoToProtocolConverter;

var ProtocolToMonacoConverter =
/*#__PURE__*/
function () {
  function ProtocolToMonacoConverter() {
    _classCallCheck(this, ProtocolToMonacoConverter);
  }

  _createClass(ProtocolToMonacoConverter, [{
    key: "asResourceEdits",
    value: function asResourceEdits(resource, edits, modelVersionId) {
      return {
        resource: resource,
        edits: this.asTextEdits(edits),
        modelVersionId: modelVersionId
      };
    }
  }, {
    key: "asWorkspaceEdit",
    value: function asWorkspaceEdit(item) {
      var _this3 = this;

      if (!item) {
        return undefined;
      }

      var edits = [];

      if (item.documentChanges) {
        item.documentChanges.forEach(function (change) {
          if (ls.CreateFile.is(change)) {
            edits.push({
              newUri: monaco.Uri.parse(change.uri),
              options: change.options
            });
          } else if (ls.RenameFile.is(change)) {
            edits.push({
              oldUri: monaco.Uri.parse(change.oldUri),
              newUri: monaco.Uri.parse(change.newUri),
              options: change.options
            });
          } else if (ls.DeleteFile.is(change)) {
            edits.push({
              oldUri: monaco.Uri.parse(change.uri),
              options: change.options
            });
          } else if (ls.TextDocumentEdit.is(change)) {
            var resource = monaco.Uri.parse(change.textDocument.uri);
            var version = typeof change.textDocument.version === 'number' ? change.textDocument.version : undefined;
            edits.push(_this3.asResourceEdits(resource, change.edits, version));
          } else {
            console.error("Unknown workspace edit change received:\n".concat(JSON.stringify(change, undefined, 4)));
          }
        });
      } else if (item.changes) {
        for (var _i = 0, _Object$keys = Object.keys(item.changes); _i < _Object$keys.length; _i++) {
          var key = _Object$keys[_i];
          var resource = monaco.Uri.parse(key);
          edits.push(this.asResourceEdits(resource, item.changes[key]));
        }
      }

      return {
        edits: edits
      };
    }
  }, {
    key: "asTextEdit",
    value: function asTextEdit(edit) {
      if (!edit) {
        return undefined;
      }

      var range = this.asRange(edit.range);
      return {
        range: range,
        text: edit.newText
      };
    }
  }, {
    key: "asTextEdits",
    value: function asTextEdits(items) {
      var _this4 = this;

      if (!items) {
        return undefined;
      }

      return items.map(function (item) {
        return _this4.asTextEdit(item);
      });
    }
  }, {
    key: "asCodeLens",
    value: function asCodeLens(item) {
      if (!item) {
        return undefined;
      }

      var range = this.asRange(item.range);
      var result = {
        range: range
      };

      if (item.command) {
        result.command = this.asCommand(item.command);
      }

      if (item.data !== void 0 && item.data !== null) {
        result.data = item.data;
      }

      return result;
    }
  }, {
    key: "asCodeLenses",
    value: function asCodeLenses(items) {
      var _this5 = this;

      if (!items) {
        return undefined;
      }

      return items.map(function (codeLens) {
        return _this5.asCodeLens(codeLens);
      });
    }
  }, {
    key: "asCodeActions",
    value: function asCodeActions(actions) {
      var _this6 = this;

      return actions.map(function (action) {
        return _this6.asCodeAction(action);
      });
    }
  }, {
    key: "asCodeAction",
    value: function asCodeAction(item) {
      if (services_1.CodeAction.is(item)) {
        return {
          title: item.title,
          command: this.asCommand(item.command),
          edit: this.asWorkspaceEdit(item.edit),
          diagnostics: this.asDiagnostics(item.diagnostics),
          kind: item.kind
        };
      }

      return {
        command: {
          id: item.command,
          title: item.title,
          arguments: item.arguments
        },
        title: item.title
      };
    }
  }, {
    key: "asCommand",
    value: function asCommand(command) {
      if (!command) {
        return undefined;
      }

      return {
        id: command.command,
        title: command.title,
        arguments: command.arguments
      };
    }
  }, {
    key: "asDocumentSymbol",
    value: function asDocumentSymbol(value) {
      var _this7 = this;

      var children = value.children && value.children.map(function (c) {
        return _this7.asDocumentSymbol(c);
      });
      return {
        name: value.name,
        detail: value.detail || "",
        kind: this.asSymbolKind(value.kind),
        range: this.asRange(value.range),
        selectionRange: this.asRange(value.selectionRange),
        children: children
      };
    }
  }, {
    key: "asDocumentSymbols",
    value: function asDocumentSymbols(values) {
      var _this8 = this;

      if (services_1.DocumentSymbol.is(values[0])) {
        return values.map(function (s) {
          return _this8.asDocumentSymbol(s);
        });
      }

      return this.asSymbolInformations(values);
    }
  }, {
    key: "asSymbolInformations",
    value: function asSymbolInformations(values, uri) {
      var _this9 = this;

      if (!values) {
        return undefined;
      }

      return values.map(function (information) {
        return _this9.asSymbolInformation(information, uri);
      });
    }
  }, {
    key: "asSymbolInformation",
    value: function asSymbolInformation(item, uri) {
      var location = this.asLocation(uri ? Object.assign({}, item.location, {
        uri: uri.toString()
      }) : item.location);
      return {
        name: item.name,
        detail: '',
        containerName: item.containerName,
        kind: this.asSymbolKind(item.kind),
        range: location.range,
        selectionRange: location.range
      };
    }
  }, {
    key: "asSymbolKind",
    value: function asSymbolKind(item) {
      if (item <= services_1.SymbolKind.TypeParameter) {
        // Symbol kind is one based in the protocol and zero based in code.
        return item - 1;
      }

      return monaco.languages.SymbolKind.Property;
    }
  }, {
    key: "asDocumentHighlights",
    value: function asDocumentHighlights(values) {
      var _this10 = this;

      if (!values) {
        return undefined;
      }

      return values.map(function (item) {
        return _this10.asDocumentHighlight(item);
      });
    }
  }, {
    key: "asDocumentHighlight",
    value: function asDocumentHighlight(item) {
      var range = this.asRange(item.range);
      var kind = Is.number(item.kind) ? this.asDocumentHighlightKind(item.kind) : undefined;
      return {
        range: range,
        kind: kind
      };
    }
  }, {
    key: "asDocumentHighlightKind",
    value: function asDocumentHighlightKind(item) {
      switch (item) {
        case services_1.DocumentHighlightKind.Text:
          return monaco.languages.DocumentHighlightKind.Text;

        case services_1.DocumentHighlightKind.Read:
          return monaco.languages.DocumentHighlightKind.Read;

        case services_1.DocumentHighlightKind.Write:
          return monaco.languages.DocumentHighlightKind.Write;
      }

      return monaco.languages.DocumentHighlightKind.Text;
    }
  }, {
    key: "asReferences",
    value: function asReferences(values) {
      var _this11 = this;

      if (!values) {
        return undefined;
      }

      return values.map(function (location) {
        return _this11.asLocation(location);
      });
    }
  }, {
    key: "asDefinitionResult",
    value: function asDefinitionResult(item) {
      var _this12 = this;

      if (!item) {
        return undefined;
      }

      if (Is.array(item)) {
        return item.map(function (location) {
          return _this12.asLocation(location);
        });
      } else {
        return this.asLocation(item);
      }
    }
  }, {
    key: "asLocation",
    value: function asLocation(item) {
      if (!item) {
        return undefined;
      }

      var uri = monaco.Uri.parse(item.uri);
      var range = this.asRange(item.range);
      return {
        uri: uri,
        range: range
      };
    }
  }, {
    key: "asSignatureHelp",
    value: function asSignatureHelp(item) {
      if (!item) {
        return undefined;
      }

      var result = {};

      if (Is.number(item.activeSignature)) {
        result.activeSignature = item.activeSignature;
      } else {
        // activeSignature was optional in the past
        result.activeSignature = 0;
      }

      if (Is.number(item.activeParameter)) {
        result.activeParameter = item.activeParameter;
      } else {
        // activeParameter was optional in the past
        result.activeParameter = 0;
      }

      if (item.signatures) {
        result.signatures = this.asSignatureInformations(item.signatures);
      } else {
        result.signatures = [];
      }

      return result;
    }
  }, {
    key: "asSignatureInformations",
    value: function asSignatureInformations(items) {
      var _this13 = this;

      return items.map(function (item) {
        return _this13.asSignatureInformation(item);
      });
    }
  }, {
    key: "asSignatureInformation",
    value: function asSignatureInformation(item) {
      var result = {
        label: item.label
      };

      if (item.documentation) {
        result.documentation = this.asDocumentation(item.documentation);
      }

      if (item.parameters) {
        result.parameters = this.asParameterInformations(item.parameters);
      } else {
        result.parameters = [];
      }

      return result;
    }
  }, {
    key: "asParameterInformations",
    value: function asParameterInformations(item) {
      var _this14 = this;

      return item.map(function (item) {
        return _this14.asParameterInformation(item);
      });
    }
  }, {
    key: "asParameterInformation",
    value: function asParameterInformation(item) {
      var result = {
        label: item.label
      };

      if (item.documentation) {
        result.documentation = this.asDocumentation(item.documentation);
      }

      ;
      return result;
    }
  }, {
    key: "asHover",
    value: function asHover(hover) {
      if (!hover) {
        return undefined;
      }

      return {
        contents: this.asHoverContent(hover.contents),
        range: this.asRange(hover.range)
      };
    }
  }, {
    key: "asHoverContent",
    value: function asHoverContent(contents) {
      var _this15 = this;

      if (Array.isArray(contents)) {
        return contents.map(function (content) {
          return _this15.asMarkdownString(content);
        });
      }

      return [this.asMarkdownString(contents)];
    }
  }, {
    key: "asDocumentation",
    value: function asDocumentation(value) {
      if (Is.string(value)) {
        return value;
      }

      if (value.kind === services_1.MarkupKind.PlainText) {
        return value.value;
      }

      return this.asMarkdownString(value);
    }
  }, {
    key: "asMarkdownString",
    value: function asMarkdownString(content) {
      if (services_1.MarkupContent.is(content)) {
        return {
          value: content.value
        };
      }

      if (Is.string(content)) {
        return {
          value: content
        };
      }

      var language = content.language,
          value = content.value;
      return {
        value: '```' + language + '\n' + value + '\n```'
      };
    }
  }, {
    key: "asSeverity",
    value: function asSeverity(severity) {
      if (severity === 1) {
        return monaco.MarkerSeverity.Error;
      }

      if (severity === 2) {
        return monaco.MarkerSeverity.Warning;
      }

      if (severity === 3) {
        return monaco.MarkerSeverity.Info;
      }

      return monaco.MarkerSeverity.Hint;
    }
  }, {
    key: "asDiagnostics",
    value: function asDiagnostics(diagnostics) {
      var _this16 = this;

      if (!diagnostics) {
        return undefined;
      }

      return diagnostics.map(function (diagnostic) {
        return _this16.asDiagnostic(diagnostic);
      });
    }
  }, {
    key: "asDiagnostic",
    value: function asDiagnostic(diagnostic) {
      return {
        code: typeof diagnostic.code === "number" ? diagnostic.code.toString() : diagnostic.code,
        severity: this.asSeverity(diagnostic.severity),
        message: diagnostic.message,
        source: diagnostic.source,
        startLineNumber: diagnostic.range.start.line + 1,
        startColumn: diagnostic.range.start.character + 1,
        endLineNumber: diagnostic.range.end.line + 1,
        endColumn: diagnostic.range.end.character + 1,
        relatedInformation: this.asRelatedInformations(diagnostic.relatedInformation)
      };
    }
  }, {
    key: "asRelatedInformations",
    value: function asRelatedInformations(relatedInformation) {
      var _this17 = this;

      if (!relatedInformation) {
        return undefined;
      }

      return relatedInformation.map(function (item) {
        return _this17.asRelatedInformation(item);
      });
    }
  }, {
    key: "asRelatedInformation",
    value: function asRelatedInformation(relatedInformation) {
      return {
        resource: monaco.Uri.parse(relatedInformation.location.uri),
        startLineNumber: relatedInformation.location.range.start.line + 1,
        startColumn: relatedInformation.location.range.start.character + 1,
        endLineNumber: relatedInformation.location.range.end.line + 1,
        endColumn: relatedInformation.location.range.end.character + 1,
        message: relatedInformation.message
      };
    }
  }, {
    key: "asCompletionResult",
    value: function asCompletionResult(result, defaultRange) {
      var _this18 = this;

      if (!result) {
        return {
          incomplete: false,
          suggestions: []
        };
      }

      if (Array.isArray(result)) {
        var suggestions = result.map(function (item) {
          return _this18.asCompletionItem(item, defaultRange);
        });
        return {
          incomplete: false,
          suggestions: suggestions
        };
      }

      return {
        incomplete: result.isIncomplete,
        suggestions: result.items.map(function (item) {
          return _this18.asCompletionItem(item, defaultRange);
        })
      };
    }
  }, {
    key: "asCompletionItem",
    value: function asCompletionItem(item, defaultRange) {
      var result = {
        label: item.label
      };

      if (item.detail) {
        result.detail = item.detail;
      }

      if (item.documentation) {
        result.documentation = this.asDocumentation(item.documentation);
        result.documentationFormat = Is.string(item.documentation) ? undefined : item.documentation.kind;
      }

      ;

      if (item.filterText) {
        result.filterText = item.filterText;
      }

      var insertText = this.asCompletionInsertText(item, defaultRange);
      result.insertText = insertText.insertText;
      result.range = insertText.range;
      result.fromEdit = insertText.fromEdit;

      if (insertText.isSnippet) {
        result.insertTextRules = monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet;
      }

      if (Is.number(item.kind)) {
        var _this$asCompletionIte = this.asCompletionItemKind(item.kind),
            _this$asCompletionIte2 = _slicedToArray(_this$asCompletionIte, 2),
            itemKind = _this$asCompletionIte2[0],
            original = _this$asCompletionIte2[1];

        result.kind = itemKind;

        if (original) {
          result.originalItemKind = original;
        }
      }

      if (item.sortText) {
        result.sortText = item.sortText;
      }

      if (item.additionalTextEdits) {
        result.additionalTextEdits = this.asTextEdits(item.additionalTextEdits);
      }

      if (Is.stringArray(item.commitCharacters)) {
        result.commitCharacters = item.commitCharacters.slice();
      }

      if (item.command) {
        result.command = this.asCommand(item.command);
      }

      if (item.deprecated === true || item.deprecated === false) {
        result.deprecated = item.deprecated;
      }

      if (item.preselect === true || item.preselect === false) {
        result.preselect = item.preselect;
      }

      if (item.data !== undefined) {
        result.data = item.data;
      }

      if (item.deprecated === true || item.deprecated === false) {
        result.deprecated = item.deprecated;
      }

      return result;
    }
  }, {
    key: "asCompletionItemKind",
    value: function asCompletionItemKind(value) {
      if (services_1.CompletionItemKind.Text <= value && value <= services_1.CompletionItemKind.TypeParameter) {
        switch (value) {
          case services_1.CompletionItemKind.Text:
            return [monaco.languages.CompletionItemKind.Text, undefined];

          case services_1.CompletionItemKind.Method:
            return [monaco.languages.CompletionItemKind.Method, undefined];

          case services_1.CompletionItemKind.Function:
            return [monaco.languages.CompletionItemKind.Function, undefined];

          case services_1.CompletionItemKind.Constructor:
            return [monaco.languages.CompletionItemKind.Constructor, undefined];

          case services_1.CompletionItemKind.Field:
            return [monaco.languages.CompletionItemKind.Field, undefined];

          case services_1.CompletionItemKind.Variable:
            return [monaco.languages.CompletionItemKind.Variable, undefined];

          case services_1.CompletionItemKind.Class:
            return [monaco.languages.CompletionItemKind.Class, undefined];

          case services_1.CompletionItemKind.Interface:
            return [monaco.languages.CompletionItemKind.Interface, undefined];

          case services_1.CompletionItemKind.Module:
            return [monaco.languages.CompletionItemKind.Module, undefined];

          case services_1.CompletionItemKind.Property:
            return [monaco.languages.CompletionItemKind.Property, undefined];

          case services_1.CompletionItemKind.Unit:
            return [monaco.languages.CompletionItemKind.Unit, undefined];

          case services_1.CompletionItemKind.Value:
            return [monaco.languages.CompletionItemKind.Value, undefined];

          case services_1.CompletionItemKind.Enum:
            return [monaco.languages.CompletionItemKind.Enum, undefined];

          case services_1.CompletionItemKind.Keyword:
            return [monaco.languages.CompletionItemKind.Keyword, undefined];

          case services_1.CompletionItemKind.Snippet:
            return [monaco.languages.CompletionItemKind.Snippet, undefined];

          case services_1.CompletionItemKind.Color:
            return [monaco.languages.CompletionItemKind.Color, undefined];

          case services_1.CompletionItemKind.File:
            return [monaco.languages.CompletionItemKind.File, undefined];

          case services_1.CompletionItemKind.Reference:
            return [monaco.languages.CompletionItemKind.Reference, undefined];

          case services_1.CompletionItemKind.Folder:
            return [monaco.languages.CompletionItemKind.Folder, undefined];

          case services_1.CompletionItemKind.EnumMember:
            return [monaco.languages.CompletionItemKind.EnumMember, undefined];

          case services_1.CompletionItemKind.Constant:
            return [monaco.languages.CompletionItemKind.Constant, undefined];

          case services_1.CompletionItemKind.Struct:
            return [monaco.languages.CompletionItemKind.Struct, undefined];

          case services_1.CompletionItemKind.Event:
            return [monaco.languages.CompletionItemKind.Event, undefined];

          case services_1.CompletionItemKind.Operator:
            return [monaco.languages.CompletionItemKind.Operator, undefined];

          case services_1.CompletionItemKind.TypeParameter:
            return [monaco.languages.CompletionItemKind.TypeParameter, undefined];

          default:
            return [value - 1, undefined];
        }
      }

      ;
      return [services_1.CompletionItemKind.Text, value];
    }
  }, {
    key: "asCompletionInsertText",
    value: function asCompletionInsertText(item, defaultRange) {
      var isSnippet = item.insertTextFormat === services_1.InsertTextFormat.Snippet;

      if (item.textEdit) {
        var range = this.asRange(item.textEdit.range);
        var value = item.textEdit.newText;
        return {
          isSnippet: isSnippet,
          insertText: value,
          range: range,
          fromEdit: true
        };
      }

      if (item.insertText) {
        return {
          isSnippet: isSnippet,
          insertText: item.insertText,
          fromEdit: false,
          range: defaultRange
        };
      }

      return {
        insertText: item.label,
        range: defaultRange,
        fromEdit: false,
        isSnippet: false
      };
    }
  }, {
    key: "asDocumentLinks",
    value: function asDocumentLinks(documentLinks) {
      var _this19 = this;

      var links = documentLinks.map(function (link) {
        return _this19.asDocumentLink(link);
      });
      return {
        links: links
      };
    }
  }, {
    key: "asDocumentLink",
    value: function asDocumentLink(documentLink) {
      return {
        range: this.asRange(documentLink.range),
        url: documentLink.target,
        data: documentLink.data
      };
    }
  }, {
    key: "asRange",
    value: function asRange(range) {
      if (range === undefined) {
        return undefined;
      }

      if (range === null) {
        return null;
      }

      var start = this.asPosition(range.start);
      var end = this.asPosition(range.end);

      if (start instanceof monaco.Position && end instanceof monaco.Position) {
        return new monaco.Range(start.lineNumber, start.column, end.lineNumber, end.column);
      }

      var startLineNumber = !start || start.lineNumber === undefined ? undefined : start.lineNumber;
      var startColumn = !start || start.column === undefined ? undefined : start.column;
      var endLineNumber = !end || end.lineNumber === undefined ? undefined : end.lineNumber;
      var endColumn = !end || end.column === undefined ? undefined : end.column;
      return {
        startLineNumber: startLineNumber,
        startColumn: startColumn,
        endLineNumber: endLineNumber,
        endColumn: endColumn
      };
    }
  }, {
    key: "asPosition",
    value: function asPosition(position) {
      if (position === undefined) {
        return undefined;
      }

      if (position === null) {
        return null;
      }

      var line = position.line,
          character = position.character;
      var lineNumber = line === undefined ? undefined : line + 1;
      var column = character === undefined ? undefined : character + 1;

      if (lineNumber !== undefined && column !== undefined) {
        return new monaco.Position(lineNumber, column);
      }

      return {
        lineNumber: lineNumber,
        column: column
      };
    }
  }, {
    key: "asColorInformations",
    value: function asColorInformations(items) {
      var _this20 = this;

      return items.map(function (item) {
        return _this20.asColorInformation(item);
      });
    }
  }, {
    key: "asColorInformation",
    value: function asColorInformation(item) {
      return {
        range: this.asRange(item.range),
        color: item.color
      };
    }
  }, {
    key: "asColorPresentations",
    value: function asColorPresentations(items) {
      var _this21 = this;

      return items.map(function (item) {
        return _this21.asColorPresentation(item);
      });
    }
  }, {
    key: "asColorPresentation",
    value: function asColorPresentation(item) {
      return {
        label: item.label,
        textEdit: this.asTextEdit(item.textEdit),
        additionalTextEdits: this.asTextEdits(item.additionalTextEdits)
      };
    }
  }, {
    key: "asFoldingRanges",
    value: function asFoldingRanges(items) {
      var _this22 = this;

      if (!items) {
        return items;
      }

      return items.map(function (item) {
        return _this22.asFoldingRange(item);
      });
    }
  }, {
    key: "asFoldingRange",
    value: function asFoldingRange(item) {
      return {
        start: item.startLine + 1,
        end: item.endLine + 1,
        kind: this.asFoldingRangeKind(item.kind)
      };
    }
  }, {
    key: "asFoldingRangeKind",
    value: function asFoldingRangeKind(kind) {
      if (kind) {
        switch (kind) {
          case services_1.FoldingRangeKind.Comment:
            return monaco.languages.FoldingRangeKind.Comment;

          case services_1.FoldingRangeKind.Imports:
            return monaco.languages.FoldingRangeKind.Imports;

          case services_1.FoldingRangeKind.Region:
            return monaco.languages.FoldingRangeKind.Region;
        }

        ;
      }

      return undefined;
    }
  }]);

  return ProtocolToMonacoConverter;
}();

exports.ProtocolToMonacoConverter = ProtocolToMonacoConverter;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-converter'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-diagnostic-collection.js":
/*!******************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-diagnostic-collection.js ***!
  \******************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var disposable_1 = __webpack_require__(/*! ./disposable */ "../../node_modules/monaco-languageclient/lib/disposable.js");

var MonacoDiagnosticCollection =
/*#__PURE__*/
function () {
  function MonacoDiagnosticCollection(name, p2m) {
    _classCallCheck(this, MonacoDiagnosticCollection);

    this.name = name;
    this.p2m = p2m;
    this.diagnostics = new Map();
    this.toDispose = new disposable_1.DisposableCollection();
  }

  _createClass(MonacoDiagnosticCollection, [{
    key: "dispose",
    value: function dispose() {
      this.toDispose.dispose();
    }
  }, {
    key: "get",
    value: function get(uri) {
      var diagnostics = this.diagnostics.get(uri);
      return !!diagnostics ? diagnostics.diagnostics : [];
    }
  }, {
    key: "set",
    value: function set(uri, diagnostics) {
      var _this = this;

      var existing = this.diagnostics.get(uri);

      if (existing) {
        existing.diagnostics = diagnostics;
      } else {
        var modelDiagnostics = new MonacoModelDiagnostics(uri, diagnostics, this.name, this.p2m);
        this.diagnostics.set(uri, modelDiagnostics);
        this.toDispose.push(disposable_1.Disposable.create(function () {
          _this.diagnostics["delete"](uri);

          modelDiagnostics.dispose();
        }));
      }
    }
  }]);

  return MonacoDiagnosticCollection;
}();

exports.MonacoDiagnosticCollection = MonacoDiagnosticCollection;

var MonacoModelDiagnostics =
/*#__PURE__*/
function () {
  function MonacoModelDiagnostics(uri, diagnostics, owner, p2m) {
    var _this2 = this;

    _classCallCheck(this, MonacoModelDiagnostics);

    this.owner = owner;
    this.p2m = p2m;
    this._markers = [];
    this._diagnostics = [];
    this.uri = monaco.Uri.parse(uri);
    this.diagnostics = diagnostics;
    monaco.editor.onDidCreateModel(function (model) {
      return _this2.doUpdateModelMarkers(model);
    });
  }

  _createClass(MonacoModelDiagnostics, [{
    key: "dispose",
    value: function dispose() {
      this._markers = [];
      this.updateModelMarkers();
    }
  }, {
    key: "updateModelMarkers",
    value: function updateModelMarkers() {
      var model = monaco.editor.getModel(this.uri);
      this.doUpdateModelMarkers(model ? model : undefined);
    }
  }, {
    key: "doUpdateModelMarkers",
    value: function doUpdateModelMarkers(model) {
      if (model && this.uri.toString() === model.uri.toString()) {
        monaco.editor.setModelMarkers(model, this.owner, this._markers);
      }
    }
  }, {
    key: "diagnostics",
    set: function set(diagnostics) {
      this._diagnostics = diagnostics;
      this._markers = this.p2m.asDiagnostics(diagnostics);
      this.updateModelMarkers();
    },
    get: function get() {
      return this._diagnostics;
    }
  }, {
    key: "markers",
    get: function get() {
      return this._markers;
    }
  }]);

  return MonacoModelDiagnostics;
}();

exports.MonacoModelDiagnostics = MonacoModelDiagnostics;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-diagnostic-collection'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-language-client.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-language-client.js ***!
  \************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _assertThisInitialized = __webpack_require__(/*! @babel/runtime/helpers/assertThisInitialized */ "../../node_modules/@babel/runtime/helpers/assertThisInitialized.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _get = __webpack_require__(/*! @babel/runtime/helpers/get */ "../../node_modules/@babel/runtime/helpers/get.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var client_1 = __webpack_require__(/*! vscode-languageclient/lib/client */ "../../node_modules/vscode-languageclient/lib/client.js");

var typeDefinition_1 = __webpack_require__(/*! vscode-languageclient/lib/typeDefinition */ "../../node_modules/vscode-languageclient/lib/typeDefinition.js");

var configuration_1 = __webpack_require__(/*! vscode-languageclient/lib/configuration */ "../../node_modules/vscode-languageclient/lib/configuration.js");

var implementation_1 = __webpack_require__(/*! vscode-languageclient/lib/implementation */ "../../node_modules/vscode-languageclient/lib/implementation.js");

var colorProvider_1 = __webpack_require__(/*! vscode-languageclient/lib/colorProvider */ "../../node_modules/vscode-languageclient/lib/colorProvider.js");

var workspaceFolders_1 = __webpack_require__(/*! vscode-languageclient/lib/workspaceFolders */ "../../node_modules/vscode-languageclient/lib/workspaceFolders.js");

var foldingRange_1 = __webpack_require__(/*! vscode-languageclient/lib/foldingRange */ "../../node_modules/vscode-languageclient/lib/foldingRange.js");

var declaration_1 = __webpack_require__(/*! vscode-languageclient/lib/declaration */ "../../node_modules/vscode-languageclient/lib/declaration.js");

__export(__webpack_require__(/*! vscode-languageclient/lib/client */ "../../node_modules/vscode-languageclient/lib/client.js"));

var MonacoLanguageClient =
/*#__PURE__*/
function (_client_1$BaseLanguag) {
  _inherits(MonacoLanguageClient, _client_1$BaseLanguag);

  function MonacoLanguageClient(_ref) {
    var _this;

    var id = _ref.id,
        name = _ref.name,
        clientOptions = _ref.clientOptions,
        connectionProvider = _ref.connectionProvider;

    _classCallCheck(this, MonacoLanguageClient);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(MonacoLanguageClient).call(this, id || name.toLowerCase(), name, clientOptions));
    _this.connectionProvider = connectionProvider;
    _this.createConnection = _this.doCreateConnection.bind(_assertThisInitialized(_this)); // bypass LSP <=> VS Code conversion

    var self = _assertThisInitialized(_this);

    self._p2c = new Proxy(self._p2c, {
      get: function get(target, prop) {
        if (prop === 'asUri') {
          return target[prop];
        }

        return MonacoLanguageClient.bypassConversion;
      }
    });
    self._c2p = new Proxy(self._c2p, {
      get: function get(target, prop) {
        if (prop === 'asUri') {
          return target[prop];
        }

        if (prop === 'asCompletionParams') {
          return function (textDocument, position, context) {
            return {
              textDocument: target.asTextDocumentIdentifier(textDocument),
              position: position,
              context: context
            };
          };
        }

        if (prop === 'asWillSaveTextDocumentParams') {
          return function (event) {
            return {
              textDocument: target.asTextDocumentIdentifier(event.document),
              reason: event.reason
            };
          };
        }

        if (prop.endsWith('Params')) {
          return target[prop];
        }

        return MonacoLanguageClient.bypassConversion;
      }
    });
    return _this;
  }

  _createClass(MonacoLanguageClient, [{
    key: "doCreateConnection",
    value: function doCreateConnection() {
      var errorHandler = this.handleConnectionError.bind(this);
      var closeHandler = this.handleConnectionClosed.bind(this);
      return this.connectionProvider.get(errorHandler, closeHandler, this.outputChannel);
    }
  }, {
    key: "createMessageTransports",
    value: function createMessageTransports(encoding) {
      throw new Error('Unsupported');
    }
  }, {
    key: "registerBuiltinFeatures",
    value: function registerBuiltinFeatures() {
      _get(_getPrototypeOf(MonacoLanguageClient.prototype), "registerBuiltinFeatures", this).call(this);

      this.registerFeature(new configuration_1.ConfigurationFeature(this));
      this.registerFeature(new typeDefinition_1.TypeDefinitionFeature(this));
      this.registerFeature(new implementation_1.ImplementationFeature(this));
      this.registerFeature(new colorProvider_1.ColorProviderFeature(this));
      this.registerFeature(new workspaceFolders_1.WorkspaceFoldersFeature(this));
      var foldingRangeFeature = new foldingRange_1.FoldingRangeFeature(this);
      foldingRangeFeature['asFoldingRanges'] = MonacoLanguageClient.bypassConversion;
      this.registerFeature(foldingRangeFeature);
      this.registerFeature(new declaration_1.DeclarationFeature(this));
      var features = this['_features'];
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = features[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          var feature = _step.value;

          if (feature instanceof colorProvider_1.ColorProviderFeature) {
            feature['asColor'] = MonacoLanguageClient.bypassConversion;
            feature['asColorInformations'] = MonacoLanguageClient.bypassConversion;
            feature['asColorPresentations'] = MonacoLanguageClient.bypassConversion;
          }
        }
      } catch (err) {
        _didIteratorError = true;
        _iteratorError = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion && _iterator["return"] != null) {
            _iterator["return"]();
          }
        } finally {
          if (_didIteratorError) {
            throw _iteratorError;
          }
        }
      }
    }
  }]);

  return MonacoLanguageClient;
}(client_1.BaseLanguageClient);

MonacoLanguageClient.bypassConversion = function (result) {
  return result || undefined;
};

exports.MonacoLanguageClient = MonacoLanguageClient;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-language-client'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-languages.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-languages.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _toConsumableArray = __webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "../../node_modules/@babel/runtime/helpers/toConsumableArray.js");

var _regeneratorRuntime = __webpack_require__(/*! @babel/runtime/regenerator */ "../../node_modules/@babel/runtime/regenerator/index.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var __awaiter = this && this.__awaiter || function (thisArg, _arguments, P, generator) {
  return new (P || (P = Promise))(function (resolve, reject) {
    function fulfilled(value) {
      try {
        step(generator.next(value));
      } catch (e) {
        reject(e);
      }
    }

    function rejected(value) {
      try {
        step(generator["throw"](value));
      } catch (e) {
        reject(e);
      }
    }

    function step(result) {
      result.done ? resolve(result.value) : new P(function (resolve) {
        resolve(result.value);
      }).then(fulfilled, rejected);
    }

    step((generator = generator.apply(thisArg, _arguments || [])).next());
  });
};

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var globToRegExp = __webpack_require__(/*! glob-to-regexp */ "../../node_modules/glob-to-regexp/index.js");

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

var monaco_diagnostic_collection_1 = __webpack_require__(/*! ./monaco-diagnostic-collection */ "../../node_modules/monaco-languageclient/lib/monaco-diagnostic-collection.js");

var disposable_1 = __webpack_require__(/*! ./disposable */ "../../node_modules/monaco-languageclient/lib/disposable.js");

var MonacoModelIdentifier;

(function (MonacoModelIdentifier) {
  function fromDocument(document) {
    return {
      uri: monaco.Uri.parse(document.uri),
      languageId: document.languageId
    };
  }

  MonacoModelIdentifier.fromDocument = fromDocument;

  function fromModel(model) {
    return {
      uri: model.uri,
      languageId: model.getModeId()
    };
  }

  MonacoModelIdentifier.fromModel = fromModel;
})(MonacoModelIdentifier = exports.MonacoModelIdentifier || (exports.MonacoModelIdentifier = {}));

function testGlob(pattern, value) {
  var regExp = globToRegExp(pattern, {
    extended: true,
    globstar: true
  });
  return regExp.test(value);
}

exports.testGlob = testGlob;

var MonacoLanguages =
/*#__PURE__*/
function () {
  function MonacoLanguages(p2m, m2p) {
    _classCallCheck(this, MonacoLanguages);

    this.p2m = p2m;
    this.m2p = m2p;
  }

  _createClass(MonacoLanguages, [{
    key: "match",
    value: function match(selector, document) {
      return this.matchModel(selector, MonacoModelIdentifier.fromDocument(document));
    }
  }, {
    key: "createDiagnosticCollection",
    value: function createDiagnosticCollection(name) {
      return new monaco_diagnostic_collection_1.MonacoDiagnosticCollection(name || 'default', this.p2m);
    }
  }, {
    key: "registerCompletionItemProvider",
    value: function registerCompletionItemProvider(selector, provider) {
      for (var _len = arguments.length, triggerCharacters = new Array(_len > 2 ? _len - 2 : 0), _key = 2; _key < _len; _key++) {
        triggerCharacters[_key - 2] = arguments[_key];
      }

      var completionProvider = this.createCompletionProvider.apply(this, [selector, provider].concat(triggerCharacters));
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = this.matchLanguage(selector)[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          var language = _step.value;
          providers.push(monaco.languages.registerCompletionItemProvider(language, completionProvider));
        }
      } catch (err) {
        _didIteratorError = true;
        _iteratorError = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion && _iterator["return"] != null) {
            _iterator["return"]();
          }
        } finally {
          if (_didIteratorError) {
            throw _iteratorError;
          }
        }
      }

      ;
      return providers;
    }
  }, {
    key: "createCompletionProvider",
    value: function createCompletionProvider(selector, provider) {
      var _this = this;

      for (var _len2 = arguments.length, triggerCharacters = new Array(_len2 > 2 ? _len2 - 2 : 0), _key2 = 2; _key2 < _len2; _key2++) {
        triggerCharacters[_key2 - 2] = arguments[_key2];
      }

      return {
        triggerCharacters: triggerCharacters,
        provideCompletionItems: function provideCompletionItems(model, position, context, token) {
          return __awaiter(_this, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee() {
            var wordUntil, defaultRange, params, result;
            return _regeneratorRuntime.wrap(function _callee$(_context) {
              while (1) {
                switch (_context.prev = _context.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context.next = 2;
                      break;
                    }

                    return _context.abrupt("return", undefined);

                  case 2:
                    wordUntil = model.getWordUntilPosition(position);
                    defaultRange = new monaco.Range(position.lineNumber, wordUntil.startColumn, position.lineNumber, wordUntil.endColumn);
                    params = this.m2p.asCompletionParams(model, position, context);
                    _context.next = 7;
                    return provider.provideCompletionItems(params, token);

                  case 7:
                    result = _context.sent;
                    return _context.abrupt("return", result && this.p2m.asCompletionResult(result, defaultRange));

                  case 9:
                  case "end":
                    return _context.stop();
                }
              }
            }, _callee, this);
          }));
        },
        resolveCompletionItem: provider.resolveCompletionItem ? function (model, position, item, token) {
          return __awaiter(_this, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee2() {
            var protocolItem, resolvedItem, resolvedCompletionItem;
            return _regeneratorRuntime.wrap(function _callee2$(_context2) {
              while (1) {
                switch (_context2.prev = _context2.next) {
                  case 0:
                    protocolItem = this.m2p.asCompletionItem(item);
                    _context2.next = 3;
                    return provider.resolveCompletionItem(protocolItem, token);

                  case 3:
                    resolvedItem = _context2.sent;

                    if (resolvedItem) {
                      resolvedCompletionItem = this.p2m.asCompletionItem(resolvedItem, item.range);
                      Object.assign(item, resolvedCompletionItem);
                    }

                    return _context2.abrupt("return", item);

                  case 6:
                  case "end":
                    return _context2.stop();
                }
              }
            }, _callee2, this);
          }));
        } : undefined
      };
    }
  }, {
    key: "registerHoverProvider",
    value: function registerHoverProvider(selector, provider) {
      var hoverProvider = this.createHoverProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion2 = true;
      var _didIteratorError2 = false;
      var _iteratorError2 = undefined;

      try {
        for (var _iterator2 = this.matchLanguage(selector)[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
          var language = _step2.value;
          providers.push(monaco.languages.registerHoverProvider(language, hoverProvider));
        }
      } catch (err) {
        _didIteratorError2 = true;
        _iteratorError2 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion2 && _iterator2["return"] != null) {
            _iterator2["return"]();
          }
        } finally {
          if (_didIteratorError2) {
            throw _iteratorError2;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createHoverProvider",
    value: function createHoverProvider(selector, provider) {
      var _this2 = this;

      return {
        provideHover: function provideHover(model, position, token) {
          return __awaiter(_this2, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee3() {
            var params, hover;
            return _regeneratorRuntime.wrap(function _callee3$(_context3) {
              while (1) {
                switch (_context3.prev = _context3.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context3.next = 2;
                      break;
                    }

                    return _context3.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context3.next = 5;
                    return provider.provideHover(params, token);

                  case 5:
                    hover = _context3.sent;
                    return _context3.abrupt("return", hover && this.p2m.asHover(hover));

                  case 7:
                  case "end":
                    return _context3.stop();
                }
              }
            }, _callee3, this);
          }));
        }
      };
    }
  }, {
    key: "registerSignatureHelpProvider",
    value: function registerSignatureHelpProvider(selector, provider) {
      for (var _len3 = arguments.length, triggerCharacters = new Array(_len3 > 2 ? _len3 - 2 : 0), _key3 = 2; _key3 < _len3; _key3++) {
        triggerCharacters[_key3 - 2] = arguments[_key3];
      }

      var signatureHelpProvider = this.createSignatureHelpProvider.apply(this, [selector, provider].concat(triggerCharacters));
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion3 = true;
      var _didIteratorError3 = false;
      var _iteratorError3 = undefined;

      try {
        for (var _iterator3 = this.matchLanguage(selector)[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
          var language = _step3.value;
          providers.push(monaco.languages.registerSignatureHelpProvider(language, signatureHelpProvider));
        }
      } catch (err) {
        _didIteratorError3 = true;
        _iteratorError3 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion3 && _iterator3["return"] != null) {
            _iterator3["return"]();
          }
        } finally {
          if (_didIteratorError3) {
            throw _iteratorError3;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createSignatureHelpProvider",
    value: function createSignatureHelpProvider(selector, provider) {
      var _this3 = this;

      for (var _len4 = arguments.length, triggerCharacters = new Array(_len4 > 2 ? _len4 - 2 : 0), _key4 = 2; _key4 < _len4; _key4++) {
        triggerCharacters[_key4 - 2] = arguments[_key4];
      }

      var signatureHelpTriggerCharacters = _toConsumableArray(provider.triggerCharacters || triggerCharacters || []); // TODO support regrigger characters after Monaco udpate


      return {
        signatureHelpTriggerCharacters: signatureHelpTriggerCharacters,
        provideSignatureHelp: function provideSignatureHelp(model, position, token) {
          return __awaiter(_this3, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee4() {
            var params, signatureHelp;
            return _regeneratorRuntime.wrap(function _callee4$(_context4) {
              while (1) {
                switch (_context4.prev = _context4.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context4.next = 2;
                      break;
                    }

                    return _context4.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context4.next = 5;
                    return provider.provideSignatureHelp(params, token, {
                      // TODO pass context from monaco after Monaco udpate
                      triggerKind: 1,
                      isRetrigger: false
                    });

                  case 5:
                    signatureHelp = _context4.sent;
                    return _context4.abrupt("return", signatureHelp && this.p2m.asSignatureHelp(signatureHelp));

                  case 7:
                  case "end":
                    return _context4.stop();
                }
              }
            }, _callee4, this);
          }));
        }
      };
    }
  }, {
    key: "registerDefinitionProvider",
    value: function registerDefinitionProvider(selector, provider) {
      var definitionProvider = this.createDefinitionProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion4 = true;
      var _didIteratorError4 = false;
      var _iteratorError4 = undefined;

      try {
        for (var _iterator4 = this.matchLanguage(selector)[Symbol.iterator](), _step4; !(_iteratorNormalCompletion4 = (_step4 = _iterator4.next()).done); _iteratorNormalCompletion4 = true) {
          var language = _step4.value;
          providers.push(monaco.languages.registerDefinitionProvider(language, definitionProvider));
        }
      } catch (err) {
        _didIteratorError4 = true;
        _iteratorError4 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion4 && _iterator4["return"] != null) {
            _iterator4["return"]();
          }
        } finally {
          if (_didIteratorError4) {
            throw _iteratorError4;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDefinitionProvider",
    value: function createDefinitionProvider(selector, provider) {
      var _this4 = this;

      return {
        provideDefinition: function provideDefinition(model, position, token) {
          return __awaiter(_this4, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee5() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee5$(_context5) {
              while (1) {
                switch (_context5.prev = _context5.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context5.next = 2;
                      break;
                    }

                    return _context5.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context5.next = 5;
                    return provider.provideDefinition(params, token);

                  case 5:
                    result = _context5.sent;
                    return _context5.abrupt("return", result && this.p2m.asDefinitionResult(result));

                  case 7:
                  case "end":
                    return _context5.stop();
                }
              }
            }, _callee5, this);
          }));
        }
      };
    }
  }, {
    key: "registerReferenceProvider",
    value: function registerReferenceProvider(selector, provider) {
      var referenceProvider = this.createReferenceProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion5 = true;
      var _didIteratorError5 = false;
      var _iteratorError5 = undefined;

      try {
        for (var _iterator5 = this.matchLanguage(selector)[Symbol.iterator](), _step5; !(_iteratorNormalCompletion5 = (_step5 = _iterator5.next()).done); _iteratorNormalCompletion5 = true) {
          var language = _step5.value;
          providers.push(monaco.languages.registerReferenceProvider(language, referenceProvider));
        }
      } catch (err) {
        _didIteratorError5 = true;
        _iteratorError5 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion5 && _iterator5["return"] != null) {
            _iterator5["return"]();
          }
        } finally {
          if (_didIteratorError5) {
            throw _iteratorError5;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createReferenceProvider",
    value: function createReferenceProvider(selector, provider) {
      var _this5 = this;

      return {
        provideReferences: function provideReferences(model, position, context, token) {
          return __awaiter(_this5, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee6() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee6$(_context6) {
              while (1) {
                switch (_context6.prev = _context6.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context6.next = 2;
                      break;
                    }

                    return _context6.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asReferenceParams(model, position, context);
                    _context6.next = 5;
                    return provider.provideReferences(params, token);

                  case 5:
                    result = _context6.sent;
                    return _context6.abrupt("return", result && this.p2m.asReferences(result));

                  case 7:
                  case "end":
                    return _context6.stop();
                }
              }
            }, _callee6, this);
          }));
        }
      };
    }
  }, {
    key: "registerDocumentHighlightProvider",
    value: function registerDocumentHighlightProvider(selector, provider) {
      var documentHighlightProvider = this.createDocumentHighlightProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion6 = true;
      var _didIteratorError6 = false;
      var _iteratorError6 = undefined;

      try {
        for (var _iterator6 = this.matchLanguage(selector)[Symbol.iterator](), _step6; !(_iteratorNormalCompletion6 = (_step6 = _iterator6.next()).done); _iteratorNormalCompletion6 = true) {
          var language = _step6.value;
          providers.push(monaco.languages.registerDocumentHighlightProvider(language, documentHighlightProvider));
        }
      } catch (err) {
        _didIteratorError6 = true;
        _iteratorError6 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion6 && _iterator6["return"] != null) {
            _iterator6["return"]();
          }
        } finally {
          if (_didIteratorError6) {
            throw _iteratorError6;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentHighlightProvider",
    value: function createDocumentHighlightProvider(selector, provider) {
      var _this6 = this;

      return {
        provideDocumentHighlights: function provideDocumentHighlights(model, position, token) {
          return __awaiter(_this6, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee7() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee7$(_context7) {
              while (1) {
                switch (_context7.prev = _context7.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context7.next = 2;
                      break;
                    }

                    return _context7.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context7.next = 5;
                    return provider.provideDocumentHighlights(params, token);

                  case 5:
                    result = _context7.sent;
                    return _context7.abrupt("return", result && this.p2m.asDocumentHighlights(result));

                  case 7:
                  case "end":
                    return _context7.stop();
                }
              }
            }, _callee7, this);
          }));
        }
      };
    }
  }, {
    key: "registerDocumentSymbolProvider",
    value: function registerDocumentSymbolProvider(selector, provider) {
      var documentSymbolProvider = this.createDocumentSymbolProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion7 = true;
      var _didIteratorError7 = false;
      var _iteratorError7 = undefined;

      try {
        for (var _iterator7 = this.matchLanguage(selector)[Symbol.iterator](), _step7; !(_iteratorNormalCompletion7 = (_step7 = _iterator7.next()).done); _iteratorNormalCompletion7 = true) {
          var language = _step7.value;
          providers.push(monaco.languages.registerDocumentSymbolProvider(language, documentSymbolProvider));
        }
      } catch (err) {
        _didIteratorError7 = true;
        _iteratorError7 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion7 && _iterator7["return"] != null) {
            _iterator7["return"]();
          }
        } finally {
          if (_didIteratorError7) {
            throw _iteratorError7;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentSymbolProvider",
    value: function createDocumentSymbolProvider(selector, provider) {
      var _this7 = this;

      return {
        provideDocumentSymbols: function provideDocumentSymbols(model, token) {
          return __awaiter(_this7, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee8() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee8$(_context8) {
              while (1) {
                switch (_context8.prev = _context8.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context8.next = 2;
                      break;
                    }

                    return _context8.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asDocumentSymbolParams(model);
                    _context8.next = 5;
                    return provider.provideDocumentSymbols(params, token);

                  case 5:
                    result = _context8.sent;
                    return _context8.abrupt("return", result && this.p2m.asDocumentSymbols(result));

                  case 7:
                  case "end":
                    return _context8.stop();
                }
              }
            }, _callee8, this);
          }));
        }
      };
    }
  }, {
    key: "registerCodeActionsProvider",
    value: function registerCodeActionsProvider(selector, provider) {
      var codeActionProvider = this.createCodeActionProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion8 = true;
      var _didIteratorError8 = false;
      var _iteratorError8 = undefined;

      try {
        for (var _iterator8 = this.matchLanguage(selector)[Symbol.iterator](), _step8; !(_iteratorNormalCompletion8 = (_step8 = _iterator8.next()).done); _iteratorNormalCompletion8 = true) {
          var language = _step8.value;
          providers.push(monaco.languages.registerCodeActionProvider(language, codeActionProvider));
        }
      } catch (err) {
        _didIteratorError8 = true;
        _iteratorError8 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion8 && _iterator8["return"] != null) {
            _iterator8["return"]();
          }
        } finally {
          if (_didIteratorError8) {
            throw _iteratorError8;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createCodeActionProvider",
    value: function createCodeActionProvider(selector, provider) {
      var _this8 = this;

      return {
        provideCodeActions: function provideCodeActions(model, range, context, token) {
          return __awaiter(_this8, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee9() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee9$(_context9) {
              while (1) {
                switch (_context9.prev = _context9.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context9.next = 2;
                      break;
                    }

                    return _context9.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asCodeActionParams(model, range, context);
                    _context9.next = 5;
                    return provider.provideCodeActions(params, token);

                  case 5:
                    result = _context9.sent;
                    return _context9.abrupt("return", result && this.p2m.asCodeActions(result) || undefined);

                  case 7:
                  case "end":
                    return _context9.stop();
                }
              }
            }, _callee9, this);
          }));
        }
      };
    }
  }, {
    key: "registerCodeLensProvider",
    value: function registerCodeLensProvider(selector, provider) {
      var codeLensProvider = this.createCodeLensProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion9 = true;
      var _didIteratorError9 = false;
      var _iteratorError9 = undefined;

      try {
        for (var _iterator9 = this.matchLanguage(selector)[Symbol.iterator](), _step9; !(_iteratorNormalCompletion9 = (_step9 = _iterator9.next()).done); _iteratorNormalCompletion9 = true) {
          var language = _step9.value;
          providers.push(monaco.languages.registerCodeLensProvider(language, codeLensProvider));
        }
      } catch (err) {
        _didIteratorError9 = true;
        _iteratorError9 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion9 && _iterator9["return"] != null) {
            _iterator9["return"]();
          }
        } finally {
          if (_didIteratorError9) {
            throw _iteratorError9;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createCodeLensProvider",
    value: function createCodeLensProvider(selector, provider) {
      var _this9 = this;

      return {
        provideCodeLenses: function provideCodeLenses(model, token) {
          return __awaiter(_this9, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee10() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee10$(_context10) {
              while (1) {
                switch (_context10.prev = _context10.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context10.next = 2;
                      break;
                    }

                    return _context10.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asCodeLensParams(model);
                    _context10.next = 5;
                    return provider.provideCodeLenses(params, token);

                  case 5:
                    result = _context10.sent;
                    return _context10.abrupt("return", result && this.p2m.asCodeLenses(result));

                  case 7:
                  case "end":
                    return _context10.stop();
                }
              }
            }, _callee10, this);
          }));
        },
        resolveCodeLens: provider.resolveCodeLens ? function (model, codeLens, token) {
          return __awaiter(_this9, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee11() {
            var protocolCodeLens, result, resolvedCodeLens;
            return _regeneratorRuntime.wrap(function _callee11$(_context11) {
              while (1) {
                switch (_context11.prev = _context11.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context11.next = 2;
                      break;
                    }

                    return _context11.abrupt("return", codeLens);

                  case 2:
                    protocolCodeLens = this.m2p.asCodeLens(codeLens);
                    _context11.next = 5;
                    return provider.resolveCodeLens(protocolCodeLens, token);

                  case 5:
                    result = _context11.sent;

                    if (result) {
                      resolvedCodeLens = this.p2m.asCodeLens(result);
                      Object.assign(codeLens, resolvedCodeLens);
                    }

                    return _context11.abrupt("return", codeLens);

                  case 8:
                  case "end":
                    return _context11.stop();
                }
              }
            }, _callee11, this);
          }));
        } : function (_, codeLens) {
          return codeLens;
        }
      };
    }
  }, {
    key: "registerDocumentFormattingEditProvider",
    value: function registerDocumentFormattingEditProvider(selector, provider) {
      var documentFormattingEditProvider = this.createDocumentFormattingEditProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion10 = true;
      var _didIteratorError10 = false;
      var _iteratorError10 = undefined;

      try {
        for (var _iterator10 = this.matchLanguage(selector)[Symbol.iterator](), _step10; !(_iteratorNormalCompletion10 = (_step10 = _iterator10.next()).done); _iteratorNormalCompletion10 = true) {
          var language = _step10.value;
          providers.push(monaco.languages.registerDocumentFormattingEditProvider(language, documentFormattingEditProvider));
        }
      } catch (err) {
        _didIteratorError10 = true;
        _iteratorError10 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion10 && _iterator10["return"] != null) {
            _iterator10["return"]();
          }
        } finally {
          if (_didIteratorError10) {
            throw _iteratorError10;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentFormattingEditProvider",
    value: function createDocumentFormattingEditProvider(selector, provider) {
      var _this10 = this;

      return {
        provideDocumentFormattingEdits: function provideDocumentFormattingEdits(model, options, token) {
          return __awaiter(_this10, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee12() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee12$(_context12) {
              while (1) {
                switch (_context12.prev = _context12.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context12.next = 2;
                      break;
                    }

                    return _context12.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asDocumentFormattingParams(model, options);
                    _context12.next = 5;
                    return provider.provideDocumentFormattingEdits(params, token);

                  case 5:
                    result = _context12.sent;
                    return _context12.abrupt("return", result && this.p2m.asTextEdits(result));

                  case 7:
                  case "end":
                    return _context12.stop();
                }
              }
            }, _callee12, this);
          }));
        }
      };
    }
  }, {
    key: "registerDocumentRangeFormattingEditProvider",
    value: function registerDocumentRangeFormattingEditProvider(selector, provider) {
      var documentRangeFormattingEditProvider = this.createDocumentRangeFormattingEditProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion11 = true;
      var _didIteratorError11 = false;
      var _iteratorError11 = undefined;

      try {
        for (var _iterator11 = this.matchLanguage(selector)[Symbol.iterator](), _step11; !(_iteratorNormalCompletion11 = (_step11 = _iterator11.next()).done); _iteratorNormalCompletion11 = true) {
          var language = _step11.value;
          providers.push(monaco.languages.registerDocumentRangeFormattingEditProvider(language, documentRangeFormattingEditProvider));
        }
      } catch (err) {
        _didIteratorError11 = true;
        _iteratorError11 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion11 && _iterator11["return"] != null) {
            _iterator11["return"]();
          }
        } finally {
          if (_didIteratorError11) {
            throw _iteratorError11;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentRangeFormattingEditProvider",
    value: function createDocumentRangeFormattingEditProvider(selector, provider) {
      var _this11 = this;

      return {
        provideDocumentRangeFormattingEdits: function provideDocumentRangeFormattingEdits(model, range, options, token) {
          return __awaiter(_this11, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee13() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee13$(_context13) {
              while (1) {
                switch (_context13.prev = _context13.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context13.next = 2;
                      break;
                    }

                    return _context13.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asDocumentRangeFormattingParams(model, range, options);
                    _context13.next = 5;
                    return provider.provideDocumentRangeFormattingEdits(params, token);

                  case 5:
                    result = _context13.sent;
                    return _context13.abrupt("return", result && this.p2m.asTextEdits(result));

                  case 7:
                  case "end":
                    return _context13.stop();
                }
              }
            }, _callee13, this);
          }));
        }
      };
    }
  }, {
    key: "registerOnTypeFormattingEditProvider",
    value: function registerOnTypeFormattingEditProvider(selector, provider, firstTriggerCharacter) {
      for (var _len5 = arguments.length, moreTriggerCharacter = new Array(_len5 > 3 ? _len5 - 3 : 0), _key5 = 3; _key5 < _len5; _key5++) {
        moreTriggerCharacter[_key5 - 3] = arguments[_key5];
      }

      var onTypeFormattingEditProvider = this.createOnTypeFormattingEditProvider.apply(this, [selector, provider, firstTriggerCharacter].concat(moreTriggerCharacter));
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion12 = true;
      var _didIteratorError12 = false;
      var _iteratorError12 = undefined;

      try {
        for (var _iterator12 = this.matchLanguage(selector)[Symbol.iterator](), _step12; !(_iteratorNormalCompletion12 = (_step12 = _iterator12.next()).done); _iteratorNormalCompletion12 = true) {
          var language = _step12.value;
          providers.push(monaco.languages.registerOnTypeFormattingEditProvider(language, onTypeFormattingEditProvider));
        }
      } catch (err) {
        _didIteratorError12 = true;
        _iteratorError12 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion12 && _iterator12["return"] != null) {
            _iterator12["return"]();
          }
        } finally {
          if (_didIteratorError12) {
            throw _iteratorError12;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createOnTypeFormattingEditProvider",
    value: function createOnTypeFormattingEditProvider(selector, provider, firstTriggerCharacter) {
      var _this12 = this;

      for (var _len6 = arguments.length, moreTriggerCharacter = new Array(_len6 > 3 ? _len6 - 3 : 0), _key6 = 3; _key6 < _len6; _key6++) {
        moreTriggerCharacter[_key6 - 3] = arguments[_key6];
      }

      var autoFormatTriggerCharacters = [firstTriggerCharacter].concat(moreTriggerCharacter);
      return {
        autoFormatTriggerCharacters: autoFormatTriggerCharacters,
        provideOnTypeFormattingEdits: function provideOnTypeFormattingEdits(model, position, ch, options, token) {
          return __awaiter(_this12, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee14() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee14$(_context14) {
              while (1) {
                switch (_context14.prev = _context14.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context14.next = 2;
                      break;
                    }

                    return _context14.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asDocumentOnTypeFormattingParams(model, position, ch, options);
                    _context14.next = 5;
                    return provider.provideOnTypeFormattingEdits(params, token);

                  case 5:
                    result = _context14.sent;
                    return _context14.abrupt("return", result && this.p2m.asTextEdits(result));

                  case 7:
                  case "end":
                    return _context14.stop();
                }
              }
            }, _callee14, this);
          }));
        }
      };
    }
  }, {
    key: "registerRenameProvider",
    value: function registerRenameProvider(selector, provider) {
      var renameProvider = this.createRenameProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion13 = true;
      var _didIteratorError13 = false;
      var _iteratorError13 = undefined;

      try {
        for (var _iterator13 = this.matchLanguage(selector)[Symbol.iterator](), _step13; !(_iteratorNormalCompletion13 = (_step13 = _iterator13.next()).done); _iteratorNormalCompletion13 = true) {
          var language = _step13.value;
          providers.push(monaco.languages.registerRenameProvider(language, renameProvider));
        }
      } catch (err) {
        _didIteratorError13 = true;
        _iteratorError13 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion13 && _iterator13["return"] != null) {
            _iterator13["return"]();
          }
        } finally {
          if (_didIteratorError13) {
            throw _iteratorError13;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createRenameProvider",
    value: function createRenameProvider(selector, provider) {
      var _this13 = this;

      return {
        provideRenameEdits: function provideRenameEdits(model, position, newName, token) {
          return __awaiter(_this13, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee15() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee15$(_context15) {
              while (1) {
                switch (_context15.prev = _context15.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context15.next = 2;
                      break;
                    }

                    return _context15.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asRenameParams(model, position, newName);
                    _context15.next = 5;
                    return provider.provideRenameEdits(params, token);

                  case 5:
                    result = _context15.sent;
                    return _context15.abrupt("return", result && this.p2m.asWorkspaceEdit(result));

                  case 7:
                  case "end":
                    return _context15.stop();
                }
              }
            }, _callee15, this);
          }));
        }
      };
    }
  }, {
    key: "registerDocumentLinkProvider",
    value: function registerDocumentLinkProvider(selector, provider) {
      var linkProvider = this.createDocumentLinkProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion14 = true;
      var _didIteratorError14 = false;
      var _iteratorError14 = undefined;

      try {
        for (var _iterator14 = this.matchLanguage(selector)[Symbol.iterator](), _step14; !(_iteratorNormalCompletion14 = (_step14 = _iterator14.next()).done); _iteratorNormalCompletion14 = true) {
          var language = _step14.value;
          providers.push(monaco.languages.registerLinkProvider(language, linkProvider));
        }
      } catch (err) {
        _didIteratorError14 = true;
        _iteratorError14 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion14 && _iterator14["return"] != null) {
            _iterator14["return"]();
          }
        } finally {
          if (_didIteratorError14) {
            throw _iteratorError14;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentLinkProvider",
    value: function createDocumentLinkProvider(selector, provider) {
      var _this14 = this;

      return {
        provideLinks: function provideLinks(model, token) {
          return __awaiter(_this14, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee16() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee16$(_context16) {
              while (1) {
                switch (_context16.prev = _context16.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context16.next = 2;
                      break;
                    }

                    return _context16.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asDocumentLinkParams(model);
                    _context16.next = 5;
                    return provider.provideDocumentLinks(params, token);

                  case 5:
                    result = _context16.sent;
                    return _context16.abrupt("return", result && this.p2m.asDocumentLinks(result));

                  case 7:
                  case "end":
                    return _context16.stop();
                }
              }
            }, _callee16, this);
          }));
        },
        resolveLink: function resolveLink(link, token) {
          return __awaiter(_this14, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee17() {
            var documentLink, result, resolvedLink;
            return _regeneratorRuntime.wrap(function _callee17$(_context17) {
              while (1) {
                switch (_context17.prev = _context17.next) {
                  case 0:
                    if (!(provider.resolveDocumentLink && (link.url === null || link.url === undefined))) {
                      _context17.next = 6;
                      break;
                    }

                    documentLink = this.m2p.asDocumentLink(link);
                    _context17.next = 4;
                    return provider.resolveDocumentLink(documentLink, token);

                  case 4:
                    result = _context17.sent;

                    if (result) {
                      resolvedLink = this.p2m.asDocumentLink(result);
                      Object.assign(link, resolvedLink);
                    }

                  case 6:
                    return _context17.abrupt("return", link);

                  case 7:
                  case "end":
                    return _context17.stop();
                }
              }
            }, _callee17, this);
          }));
        }
      };
    }
  }, {
    key: "registerImplementationProvider",
    value: function registerImplementationProvider(selector, provider) {
      var implementationProvider = this.createImplementationProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion15 = true;
      var _didIteratorError15 = false;
      var _iteratorError15 = undefined;

      try {
        for (var _iterator15 = this.matchLanguage(selector)[Symbol.iterator](), _step15; !(_iteratorNormalCompletion15 = (_step15 = _iterator15.next()).done); _iteratorNormalCompletion15 = true) {
          var language = _step15.value;
          providers.push(monaco.languages.registerImplementationProvider(language, implementationProvider));
        }
      } catch (err) {
        _didIteratorError15 = true;
        _iteratorError15 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion15 && _iterator15["return"] != null) {
            _iterator15["return"]();
          }
        } finally {
          if (_didIteratorError15) {
            throw _iteratorError15;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createImplementationProvider",
    value: function createImplementationProvider(selector, provider) {
      var _this15 = this;

      return {
        provideImplementation: function provideImplementation(model, position, token) {
          return __awaiter(_this15, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee18() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee18$(_context18) {
              while (1) {
                switch (_context18.prev = _context18.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context18.next = 2;
                      break;
                    }

                    return _context18.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context18.next = 5;
                    return provider.provideImplementation(params, token);

                  case 5:
                    result = _context18.sent;
                    return _context18.abrupt("return", result && this.p2m.asDefinitionResult(result));

                  case 7:
                  case "end":
                    return _context18.stop();
                }
              }
            }, _callee18, this);
          }));
        }
      };
    }
  }, {
    key: "registerTypeDefinitionProvider",
    value: function registerTypeDefinitionProvider(selector, provider) {
      var typeDefinitionProvider = this.createTypeDefinitionProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion16 = true;
      var _didIteratorError16 = false;
      var _iteratorError16 = undefined;

      try {
        for (var _iterator16 = this.matchLanguage(selector)[Symbol.iterator](), _step16; !(_iteratorNormalCompletion16 = (_step16 = _iterator16.next()).done); _iteratorNormalCompletion16 = true) {
          var language = _step16.value;
          providers.push(monaco.languages.registerTypeDefinitionProvider(language, typeDefinitionProvider));
        }
      } catch (err) {
        _didIteratorError16 = true;
        _iteratorError16 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion16 && _iterator16["return"] != null) {
            _iterator16["return"]();
          }
        } finally {
          if (_didIteratorError16) {
            throw _iteratorError16;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createTypeDefinitionProvider",
    value: function createTypeDefinitionProvider(selector, provider) {
      var _this16 = this;

      return {
        provideTypeDefinition: function provideTypeDefinition(model, position, token) {
          return __awaiter(_this16, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee19() {
            var params, result;
            return _regeneratorRuntime.wrap(function _callee19$(_context19) {
              while (1) {
                switch (_context19.prev = _context19.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context19.next = 2;
                      break;
                    }

                    return _context19.abrupt("return", undefined);

                  case 2:
                    params = this.m2p.asTextDocumentPositionParams(model, position);
                    _context19.next = 5;
                    return provider.provideTypeDefinition(params, token);

                  case 5:
                    result = _context19.sent;
                    return _context19.abrupt("return", result && this.p2m.asDefinitionResult(result));

                  case 7:
                  case "end":
                    return _context19.stop();
                }
              }
            }, _callee19, this);
          }));
        }
      };
    }
  }, {
    key: "registerColorProvider",
    value: function registerColorProvider(selector, provider) {
      var documentColorProvider = this.createDocumentColorProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion17 = true;
      var _didIteratorError17 = false;
      var _iteratorError17 = undefined;

      try {
        for (var _iterator17 = this.matchLanguage(selector)[Symbol.iterator](), _step17; !(_iteratorNormalCompletion17 = (_step17 = _iterator17.next()).done); _iteratorNormalCompletion17 = true) {
          var language = _step17.value;
          providers.push(monaco.languages.registerColorProvider(language, documentColorProvider));
        }
      } catch (err) {
        _didIteratorError17 = true;
        _iteratorError17 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion17 && _iterator17["return"] != null) {
            _iterator17["return"]();
          }
        } finally {
          if (_didIteratorError17) {
            throw _iteratorError17;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createDocumentColorProvider",
    value: function createDocumentColorProvider(selector, provider) {
      var _this17 = this;

      return {
        provideDocumentColors: function provideDocumentColors(model, token) {
          return __awaiter(_this17, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee20() {
            var textDocument, result;
            return _regeneratorRuntime.wrap(function _callee20$(_context20) {
              while (1) {
                switch (_context20.prev = _context20.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context20.next = 2;
                      break;
                    }

                    return _context20.abrupt("return", undefined);

                  case 2:
                    textDocument = this.m2p.asTextDocumentIdentifier(model);
                    _context20.next = 5;
                    return provider.provideDocumentColors({
                      textDocument: textDocument
                    }, token);

                  case 5:
                    result = _context20.sent;
                    return _context20.abrupt("return", result && this.p2m.asColorInformations(result));

                  case 7:
                  case "end":
                    return _context20.stop();
                }
              }
            }, _callee20, this);
          }));
        },
        provideColorPresentations: function provideColorPresentations(model, info, token) {
          return __awaiter(_this17, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee21() {
            var textDocument, range, result;
            return _regeneratorRuntime.wrap(function _callee21$(_context21) {
              while (1) {
                switch (_context21.prev = _context21.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context21.next = 2;
                      break;
                    }

                    return _context21.abrupt("return", undefined);

                  case 2:
                    textDocument = this.m2p.asTextDocumentIdentifier(model);
                    range = this.m2p.asRange(info.range);
                    _context21.next = 6;
                    return provider.provideColorPresentations({
                      textDocument: textDocument,
                      color: info.color,
                      range: range
                    }, token);

                  case 6:
                    result = _context21.sent;
                    return _context21.abrupt("return", result && this.p2m.asColorPresentations(result));

                  case 8:
                  case "end":
                    return _context21.stop();
                }
              }
            }, _callee21, this);
          }));
        }
      };
    }
  }, {
    key: "registerFoldingRangeProvider",
    value: function registerFoldingRangeProvider(selector, provider) {
      var foldingRangeProvider = this.createFoldingRangeProvider(selector, provider);
      var providers = new disposable_1.DisposableCollection();
      var _iteratorNormalCompletion18 = true;
      var _didIteratorError18 = false;
      var _iteratorError18 = undefined;

      try {
        for (var _iterator18 = this.matchLanguage(selector)[Symbol.iterator](), _step18; !(_iteratorNormalCompletion18 = (_step18 = _iterator18.next()).done); _iteratorNormalCompletion18 = true) {
          var language = _step18.value;
          providers.push(monaco.languages.registerFoldingRangeProvider(language, foldingRangeProvider));
        }
      } catch (err) {
        _didIteratorError18 = true;
        _iteratorError18 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion18 && _iterator18["return"] != null) {
            _iterator18["return"]();
          }
        } finally {
          if (_didIteratorError18) {
            throw _iteratorError18;
          }
        }
      }

      return providers;
    }
  }, {
    key: "createFoldingRangeProvider",
    value: function createFoldingRangeProvider(selector, provider) {
      var _this18 = this;

      return {
        provideFoldingRanges: function provideFoldingRanges(model, context, token) {
          return __awaiter(_this18, void 0, void 0,
          /*#__PURE__*/
          _regeneratorRuntime.mark(function _callee22() {
            var textDocument, result;
            return _regeneratorRuntime.wrap(function _callee22$(_context22) {
              while (1) {
                switch (_context22.prev = _context22.next) {
                  case 0:
                    if (this.matchModel(selector, MonacoModelIdentifier.fromModel(model))) {
                      _context22.next = 2;
                      break;
                    }

                    return _context22.abrupt("return", undefined);

                  case 2:
                    textDocument = this.m2p.asTextDocumentIdentifier(model);
                    _context22.next = 5;
                    return provider.provideFoldingRanges({
                      textDocument: textDocument
                    }, token);

                  case 5:
                    result = _context22.sent;
                    return _context22.abrupt("return", result && this.p2m.asFoldingRanges(result));

                  case 7:
                  case "end":
                    return _context22.stop();
                }
              }
            }, _callee22, this);
          }));
        }
      };
    }
  }, {
    key: "matchModel",
    value: function matchModel(selector, model) {
      var _this19 = this;

      if (Array.isArray(selector)) {
        return selector.some(function (filter) {
          return _this19.matchModel(filter, model);
        });
      }

      if (services_1.DocumentFilter.is(selector)) {
        if (!!selector.language && selector.language !== model.languageId) {
          return false;
        }

        if (!!selector.scheme && selector.scheme !== model.uri.scheme) {
          return false;
        }

        if (!!selector.pattern && !testGlob(selector.pattern, model.uri.path)) {
          return false;
        }

        return true;
      }

      return selector === model.languageId;
    }
  }, {
    key: "matchLanguage",
    value: function matchLanguage(selector) {
      var languages = new Set();

      if (Array.isArray(selector)) {
        var _iteratorNormalCompletion19 = true;
        var _didIteratorError19 = false;
        var _iteratorError19 = undefined;

        try {
          for (var _iterator19 = selector[Symbol.iterator](), _step19; !(_iteratorNormalCompletion19 = (_step19 = _iterator19.next()).done); _iteratorNormalCompletion19 = true) {
            var filter = _step19.value;
            languages.add(this.matchLanguageByFilter(filter));
          }
        } catch (err) {
          _didIteratorError19 = true;
          _iteratorError19 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion19 && _iterator19["return"] != null) {
              _iterator19["return"]();
            }
          } finally {
            if (_didIteratorError19) {
              throw _iteratorError19;
            }
          }
        }
      } else {
        languages.add(this.matchLanguageByFilter(selector));
      }

      return languages;
    }
  }, {
    key: "matchLanguageByFilter",
    value: function matchLanguageByFilter(selector) {
      if (services_1.DocumentFilter.is(selector)) {
        if (!selector.language) {
          return '*';
        }

        return selector.language;
      }

      return selector;
    }
  }]);

  return MonacoLanguages;
}();

exports.MonacoLanguages = MonacoLanguages;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-languages'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-services.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-services.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

Object.defineProperty(exports, "__esModule", {
  value: true
});
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var monaco_converter_1 = __webpack_require__(/*! ./monaco-converter */ "../../node_modules/monaco-languageclient/lib/monaco-converter.js");

var monaco_commands_1 = __webpack_require__(/*! ./monaco-commands */ "../../node_modules/monaco-languageclient/lib/monaco-commands.js");

var monaco_languages_1 = __webpack_require__(/*! ./monaco-languages */ "../../node_modules/monaco-languageclient/lib/monaco-languages.js");

var monaco_workspace_1 = __webpack_require__(/*! ./monaco-workspace */ "../../node_modules/monaco-languageclient/lib/monaco-workspace.js");

var console_window_1 = __webpack_require__(/*! ./console-window */ "../../node_modules/monaco-languageclient/lib/console-window.js");

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

var MonacoServices;

(function (MonacoServices) {
  function create(editor) {
    var options = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : {};
    var m2p = new monaco_converter_1.MonacoToProtocolConverter();
    var p2m = new monaco_converter_1.ProtocolToMonacoConverter();
    return {
      commands: new monaco_commands_1.MonacoCommands(editor),
      languages: new monaco_languages_1.MonacoLanguages(p2m, m2p),
      workspace: new monaco_workspace_1.MonacoWorkspace(p2m, m2p, options.rootUri),
      window: new console_window_1.ConsoleWindow()
    };
  }

  MonacoServices.create = create;

  function install(editor) {
    var options = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : {};
    var services = create(editor, options);
    services_1.Services.install(services);
    return services;
  }

  MonacoServices.install = install;

  function get() {
    return services_1.Services.get();
  }

  MonacoServices.get = get;
})(MonacoServices = exports.MonacoServices || (exports.MonacoServices = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-services'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/monaco-workspace.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/monaco-workspace.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {

var _toConsumableArray = __webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "../../node_modules/@babel/runtime/helpers/toConsumableArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

var MonacoWorkspace =
/*#__PURE__*/
function () {
  function MonacoWorkspace(p2m, m2p) {
    var _this = this;

    var _rootUri = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : null;

    _classCallCheck(this, MonacoWorkspace);

    this.p2m = p2m;
    this.m2p = m2p;
    this._rootUri = _rootUri;
    this.documents = new Map();
    this.onDidOpenTextDocumentEmitter = new services_1.Emitter();
    this.onDidCloseTextDocumentEmitter = new services_1.Emitter();
    this.onDidChangeTextDocumentEmitter = new services_1.Emitter();
    var _iteratorNormalCompletion = true;
    var _didIteratorError = false;
    var _iteratorError = undefined;

    try {
      for (var _iterator = monaco.editor.getModels()[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
        var model = _step.value;
        this.addModel(model);
      }
    } catch (err) {
      _didIteratorError = true;
      _iteratorError = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion && _iterator["return"] != null) {
          _iterator["return"]();
        }
      } finally {
        if (_didIteratorError) {
          throw _iteratorError;
        }
      }
    }

    monaco.editor.onDidCreateModel(function (model) {
      return _this.addModel(model);
    });
    monaco.editor.onWillDisposeModel(function (model) {
      return _this.removeModel(model);
    });
  }

  _createClass(MonacoWorkspace, [{
    key: "removeModel",
    value: function removeModel(model) {
      var uri = model.uri.toString();
      var document = this.documents.get(uri);

      if (document) {
        this.documents["delete"](uri);
        this.onDidCloseTextDocumentEmitter.fire(document);
      }
    }
  }, {
    key: "addModel",
    value: function addModel(model) {
      var _this2 = this;

      var uri = model.uri.toString();
      var document = this.setModel(uri, model);
      this.onDidOpenTextDocumentEmitter.fire(document);
      model.onDidChangeContent(function (event) {
        return _this2.onDidChangeContent(uri, model, event);
      });
    }
  }, {
    key: "onDidChangeContent",
    value: function onDidChangeContent(uri, model, event) {
      var textDocument = this.setModel(uri, model);
      var contentChanges = [];
      var _iteratorNormalCompletion2 = true;
      var _didIteratorError2 = false;
      var _iteratorError2 = undefined;

      try {
        for (var _iterator2 = event.changes[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
          var change = _step2.value;
          var range = this.m2p.asRange(change.range);
          var rangeLength = change.rangeLength;
          var text = change.text;
          contentChanges.push({
            range: range,
            rangeLength: rangeLength,
            text: text
          });
        }
      } catch (err) {
        _didIteratorError2 = true;
        _iteratorError2 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion2 && _iterator2["return"] != null) {
            _iterator2["return"]();
          }
        } finally {
          if (_didIteratorError2) {
            throw _iteratorError2;
          }
        }
      }

      this.onDidChangeTextDocumentEmitter.fire({
        textDocument: textDocument,
        contentChanges: contentChanges
      });
    }
  }, {
    key: "setModel",
    value: function setModel(uri, model) {
      var document = services_1.TextDocument.create(uri, model.getModeId(), model.getVersionId(), model.getValue());
      this.documents.set(uri, document);
      return document;
    }
  }, {
    key: "applyEdit",
    value: function applyEdit(workspaceEdit) {
      var edit = this.p2m.asWorkspaceEdit(workspaceEdit); // Collect all referenced models

      var models = edit.edits ? edit.edits.reduce(function (acc, currentEdit) {
        var textEdit = currentEdit;
        acc[textEdit.resource.toString()] = monaco.editor.getModel(textEdit.resource);
        return acc;
      }, {}) : {}; // If any of the models do not exist, refuse to apply the edit.

      if (!Object.keys(models).map(function (uri) {
        return models[uri];
      }).every(function (model) {
        return !!model;
      })) {
        return Promise.resolve(false);
      } // Group edits by resource so we can batch them when applying


      var editsByResource = edit.edits ? edit.edits.reduce(function (acc, currentEdit) {
        var _acc$uri;

        var textEdit = currentEdit;
        var uri = textEdit.resource.toString();

        if (!(uri in acc)) {
          acc[uri] = [];
        }

        var operations = textEdit.edits.map(function (edit) {
          return {
            range: monaco.Range.lift(edit.range),
            text: edit.text
          };
        });

        (_acc$uri = acc[uri]).push.apply(_acc$uri, _toConsumableArray(operations));

        return acc;
      }, {}) : {}; // Apply edits for each resource

      Object.keys(editsByResource).forEach(function (uri) {
        models[uri].pushEditOperations([], // Do not try and preserve editor selections.
        editsByResource[uri].map(function (resourceEdit) {
          return {
            identifier: {
              major: 1,
              minor: 0
            },
            range: resourceEdit.range,
            text: resourceEdit.text,
            forceMoveMarkers: true
          };
        }), function () {
          return [];
        });
      });
      return Promise.resolve(true);
    }
  }, {
    key: "rootUri",
    get: function get() {
      return this._rootUri;
    }
  }, {
    key: "textDocuments",
    get: function get() {
      return Array.from(this.documents.values());
    }
  }, {
    key: "onDidOpenTextDocument",
    get: function get() {
      return this.onDidOpenTextDocumentEmitter.event;
    }
  }, {
    key: "onDidCloseTextDocument",
    get: function get() {
      return this.onDidCloseTextDocumentEmitter.event;
    }
  }, {
    key: "onDidChangeTextDocument",
    get: function get() {
      return this.onDidChangeTextDocumentEmitter.event;
    }
  }]);

  return MonacoWorkspace;
}();

exports.MonacoWorkspace = MonacoWorkspace;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/monaco-workspace'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/services.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/services.js ***!
  \**********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-jsonrpc/lib/main.js");

exports.Disposable = vscode_jsonrpc_1.Disposable;
exports.CancellationToken = vscode_jsonrpc_1.CancellationToken;
exports.Event = vscode_jsonrpc_1.Event;
exports.Emitter = vscode_jsonrpc_1.Emitter;

__export(__webpack_require__(/*! vscode-languageserver-protocol/lib/main */ "../../node_modules/vscode-languageserver-protocol/lib/main.js"));

var Services;

(function (Services) {
  var global = window;
  var symbol = Symbol('Services');

  Services.get = function () {
    var services = global[symbol];

    if (!services) {
      throw new Error('Language Client services has not been installed');
    }

    return services;
  };

  function install(services) {
    if (global[symbol]) {
      console.error(new Error('Language Client services has been overridden'));
    }

    global[symbol] = services;
    return vscode_jsonrpc_1.Disposable.create(function () {
      return global[symbol] = undefined;
    });
  }

  Services.install = install;
})(Services = exports.Services || (exports.Services = {}));

function isDocumentSelector(selector) {
  if (!selector || !Array.isArray(selector)) {
    return false;
  }

  return selector.every(function (value) {
    return typeof value === 'string' || vscode_languageserver_protocol_1.DocumentFilter.is(value);
  });
}

exports.isDocumentSelector = isDocumentSelector;
var SignatureHelpTriggerKind;

(function (SignatureHelpTriggerKind) {
  SignatureHelpTriggerKind[SignatureHelpTriggerKind["Invoke"] = 1] = "Invoke";
  SignatureHelpTriggerKind[SignatureHelpTriggerKind["TriggerCharacter"] = 2] = "TriggerCharacter";
  SignatureHelpTriggerKind[SignatureHelpTriggerKind["ContentChange"] = 3] = "ContentChange";
})(SignatureHelpTriggerKind = exports.SignatureHelpTriggerKind || (exports.SignatureHelpTriggerKind = {}));

var DocumentIdentifier;

(function (DocumentIdentifier) {
  function is(arg) {
    return !!arg && 'uri' in arg && 'languageId' in arg;
  }

  DocumentIdentifier.is = is;
})(DocumentIdentifier = exports.DocumentIdentifier || (exports.DocumentIdentifier = {}));

var ConfigurationTarget;

(function (ConfigurationTarget) {
  ConfigurationTarget[ConfigurationTarget["Global"] = 1] = "Global";
  ConfigurationTarget[ConfigurationTarget["Workspace"] = 2] = "Workspace";
  ConfigurationTarget[ConfigurationTarget["WorkspaceFolder"] = 3] = "WorkspaceFolder";
})(ConfigurationTarget = exports.ConfigurationTarget || (exports.ConfigurationTarget = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/services'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/vscode-api.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/vscode-api.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var _regeneratorRuntime = __webpack_require__(/*! @babel/runtime/regenerator */ "../../node_modules/@babel/runtime/regenerator/index.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var __awaiter = this && this.__awaiter || function (thisArg, _arguments, P, generator) {
  return new (P || (P = Promise))(function (resolve, reject) {
    function fulfilled(value) {
      try {
        step(generator.next(value));
      } catch (e) {
        reject(e);
      }
    }

    function rejected(value) {
      try {
        step(generator["throw"](value));
      } catch (e) {
        reject(e);
      }
    }

    function step(result) {
      result.done ? resolve(result.value) : new P(function (resolve) {
        resolve(result.value);
      }).then(fulfilled, rejected);
    }

    step((generator = generator.apply(thisArg, _arguments || [])).next());
  });
};

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_uri_1 = __webpack_require__(/*! vscode-uri */ "../../node_modules/vscode-uri/lib/esm/index.js");

var disposable_1 = __webpack_require__(/*! ./disposable */ "../../node_modules/monaco-languageclient/lib/disposable.js");

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

function createVSCodeApi(servicesProvider) {
  var _this = this;

  var unsupported = function unsupported() {
    throw new Error('unsupported');
  };

  var Uri = vscode_uri_1["default"];

  var CompletionItem = function CompletionItem(label, kind) {
    _classCallCheck(this, CompletionItem);

    this.label = label;
    this.kind = kind;
  };

  var CodeLens =
  /*#__PURE__*/
  function () {
    function CodeLens(range, command) {
      _classCallCheck(this, CodeLens);

      this.range = range;
      this.command = command;
    }

    _createClass(CodeLens, [{
      key: "isResolved",
      get: function get() {
        return !!this.command;
      }
    }]);

    return CodeLens;
  }();

  var DocumentLink = function DocumentLink(range, target) {
    _classCallCheck(this, DocumentLink);

    this.range = range;
    this.target = target;
  };

  var CodeActionKind =
  /*#__PURE__*/
  function () {
    function CodeActionKind(value) {
      _classCallCheck(this, CodeActionKind);

      this.value = value;
      this.contains = unsupported;
      this.intersects = unsupported;
    }

    _createClass(CodeActionKind, [{
      key: "append",
      value: function append(parts) {
        return new CodeActionKind(this.value ? this.value + CodeActionKind.sep + parts : parts);
      }
    }]);

    return CodeActionKind;
  }();

  CodeActionKind.sep = '.';
  CodeActionKind.Empty = new CodeActionKind('');
  CodeActionKind.QuickFix = new CodeActionKind('quickfix');
  CodeActionKind.Refactor = new CodeActionKind('refactor');
  CodeActionKind.RefactorExtract = CodeActionKind.Refactor.append('extract');
  CodeActionKind.RefactorInline = CodeActionKind.Refactor.append('inline');
  CodeActionKind.RefactorRewrite = CodeActionKind.Refactor.append('rewrite');
  CodeActionKind.Source = new CodeActionKind('source');
  CodeActionKind.SourceOrganizeImports = CodeActionKind.Source.append('organizeImports');
  CodeActionKind.SourceFixAll = CodeActionKind.Source.append('fixAll');
  var workspace = {
    createFileSystemWatcher: function createFileSystemWatcher(globPattern, ignoreCreateEvents, ignoreChangeEvents, ignoreDeleteEvents) {
      var services = servicesProvider();

      if (typeof globPattern !== 'string') {
        throw new Error('unsupported');
      }

      if (services.workspace.createFileSystemWatcher) {
        var watcher = services.workspace.createFileSystemWatcher(globPattern, ignoreCreateEvents, ignoreChangeEvents, ignoreDeleteEvents);
        return Object.assign(watcher, {
          ignoreCreateEvents: !!ignoreCreateEvents,
          ignoreChangeEvents: !!ignoreChangeEvents,
          ignoreDeleteEvents: !!ignoreDeleteEvents
        });
      }

      return {
        ignoreCreateEvents: !!ignoreCreateEvents,
        ignoreChangeEvents: !!ignoreChangeEvents,
        ignoreDeleteEvents: !!ignoreDeleteEvents,
        onDidCreate: services_1.Event.None,
        onDidChange: services_1.Event.None,
        onDidDelete: services_1.Event.None,
        dispose: function dispose() {}
      };
    },
    applyEdit: function applyEdit(edit) {
      return __awaiter(_this, void 0, void 0,
      /*#__PURE__*/
      _regeneratorRuntime.mark(function _callee() {
        var services;
        return _regeneratorRuntime.wrap(function _callee$(_context) {
          while (1) {
            switch (_context.prev = _context.next) {
              case 0:
                services = servicesProvider();

                if (!services_1.WorkspaceEdit.is(edit)) {
                  _context.next = 3;
                  break;
                }

                return _context.abrupt("return", services.workspace.applyEdit(edit));

              case 3:
                throw new Error('unsupported');

              case 4:
              case "end":
                return _context.stop();
            }
          }
        }, _callee);
      }));
    },
    getConfiguration: function getConfiguration(section, resource) {
      var _servicesProvider = servicesProvider(),
          workspace = _servicesProvider.workspace;

      var configuration = workspace.configurations ? workspace.configurations.getConfiguration(section, resource ? resource.toString() : undefined) : undefined;
      var result = {
        get: function get(section, defaultValue) {
          return configuration ? configuration.get(section, defaultValue) : defaultValue;
        },
        has: function has(section) {
          return configuration ? configuration.has(section) : false;
        },
        inspect: unsupported,
        update: unsupported
      };
      return Object.assign(result, {
        toJSON: function toJSON() {
          return configuration ? configuration.toJSON() : undefined;
        }
      });
    },

    get onDidChangeConfiguration() {
      var services = servicesProvider();

      if (services.workspace.configurations) {
        return services.workspace.configurations.onDidChangeConfiguration;
      }

      return services_1.Event.None;
    },

    get workspaceFolders() {
      var services = servicesProvider();

      if ('workspaceFolders' in services.workspace) {
        return services.workspace.workspaceFolders;
      }

      var rootUri = services.workspace.rootUri;

      if (!rootUri) {
        return undefined;
      }

      var uri = Uri.parse(rootUri);
      return [{
        uri: uri,
        index: 0,
        name: uri.toString()
      }];
    },

    get onDidChangeWorkspaceFolders() {
      var services = servicesProvider();
      return services.workspace.onDidChangeWorkspaceFolders || services_1.Event.None;
    },

    get textDocuments() {
      var services = servicesProvider();
      return services.workspace.textDocuments;
    },

    get onDidOpenTextDocument() {
      var services = servicesProvider();
      return services.workspace.onDidOpenTextDocument;
    },

    get onDidCloseTextDocument() {
      var services = servicesProvider();
      return services.workspace.onDidCloseTextDocument;
    },

    get onDidChangeTextDocument() {
      var services = servicesProvider();
      return function (listener, thisArgs, disposables) {
        return services.workspace.onDidChangeTextDocument(function (_ref) {
          var textDocument = _ref.textDocument,
              contentChanges = _ref.contentChanges;
          var l = listener.bind(thisArgs);
          l({
            document: textDocument,
            contentChanges: contentChanges
          });
        }, undefined, disposables);
      };
    },

    get onWillSaveTextDocument() {
      var services = servicesProvider();
      var onWillSaveTextDocument = services.workspace.onWillSaveTextDocument;

      if (!onWillSaveTextDocument) {
        return services_1.Event.None;
      }

      return function (listener, thisArgs, disposables) {
        return onWillSaveTextDocument(function (_ref2) {
          var textDocument = _ref2.textDocument,
              reason = _ref2.reason,
              _waitUntil = _ref2.waitUntil;
          var l = listener.bind(thisArgs);
          l({
            document: textDocument,
            reason: reason,
            waitUntil: function waitUntil(edits) {
              if (_waitUntil) {
                _waitUntil(edits);
              }
            }
          });
        }, undefined, disposables);
      };
    },

    get onDidSaveTextDocument() {
      var services = servicesProvider();
      return services.workspace.onDidSaveTextDocument || services_1.Event.None;
    },

    getWorkspaceFolder: unsupported,
    asRelativePath: unsupported,
    updateWorkspaceFolders: unsupported,
    findFiles: unsupported,
    saveAll: unsupported,
    openTextDocument: unsupported,
    registerTextDocumentContentProvider: unsupported,
    registerTaskProvider: unsupported,
    registerFileSystemProvider: unsupported,
    rootPath: undefined,
    name: undefined
  };
  var languages = {
    match: function match(selector, document) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      if (!services_1.DocumentIdentifier.is(document)) {
        throw new Error('unexpected document: ' + JSON.stringify(document));
      }

      var services = servicesProvider();
      var result = services.languages.match(selector, document);
      return result ? 1 : 0;
    },
    createDiagnosticCollection: function createDiagnosticCollection(name) {
      var services = servicesProvider();
      var collection = services.languages.createDiagnosticCollection ? services.languages.createDiagnosticCollection(name) : undefined;
      return {
        name: name || 'default',
        set: function set(arg0, arg1) {
          if (collection) {
            if (arg1) {
              collection.set(arg0.toString(), arg1);
            } else {
              collection.set(arg0.toString(), []);
            }
          }
        },
        dispose: function dispose() {
          if (collection) {
            collection.dispose();
          }
        },
        "delete": unsupported,
        clear: unsupported,
        forEach: unsupported,
        get: unsupported,
        has: unsupported
      };
    },
    registerCompletionItemProvider: function registerCompletionItemProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider2 = servicesProvider(),
          languages = _servicesProvider2.languages;

      if (!languages.registerCompletionItemProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      var resolveCompletionItem = provider.resolveCompletionItem;

      for (var _len = arguments.length, triggerCharacters = new Array(_len > 2 ? _len - 2 : 0), _key = 2; _key < _len; _key++) {
        triggerCharacters[_key - 2] = arguments[_key];
      }

      return languages.registerCompletionItemProvider.apply(languages, [selector, {
        provideCompletionItems: function provideCompletionItems(_ref3, token) {
          var textDocument = _ref3.textDocument,
              position = _ref3.position,
              context = _ref3.context;
          return provider.provideCompletionItems(textDocument, position, token, context || {
            triggerKind: services_1.CompletionTriggerKind.Invoked
          });
        },
        resolveCompletionItem: resolveCompletionItem ? function (item, token) {
          return resolveCompletionItem(item, token);
        } : undefined
      }].concat(triggerCharacters));
    },
    registerCodeActionsProvider: function registerCodeActionsProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider3 = servicesProvider(),
          languages = _servicesProvider3.languages;

      if (!languages.registerCodeActionsProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerCodeActionsProvider(selector, {
        provideCodeActions: function provideCodeActions(_ref4, token) {
          var textDocument = _ref4.textDocument,
              range = _ref4.range,
              context = _ref4.context;
          return provider.provideCodeActions(textDocument, range, context, token);
        }
      });
    },
    registerCodeLensProvider: function registerCodeLensProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider4 = servicesProvider(),
          languages = _servicesProvider4.languages;

      if (!languages.registerCodeLensProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      var resolveCodeLens = provider.resolveCodeLens;
      return languages.registerCodeLensProvider(selector, {
        provideCodeLenses: function provideCodeLenses(_ref5, token) {
          var textDocument = _ref5.textDocument;
          return provider.provideCodeLenses(textDocument, token);
        },
        resolveCodeLens: resolveCodeLens ? function (codeLens, token) {
          return resolveCodeLens(codeLens, token);
        } : undefined
      });
    },
    registerDefinitionProvider: function registerDefinitionProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider5 = servicesProvider(),
          languages = _servicesProvider5.languages;

      if (!languages.registerDefinitionProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDefinitionProvider(selector, {
        provideDefinition: function provideDefinition(_ref6, token) {
          var textDocument = _ref6.textDocument,
              position = _ref6.position;
          return provider.provideDefinition(textDocument, position, token);
        }
      });
    },
    registerImplementationProvider: function registerImplementationProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider6 = servicesProvider(),
          languages = _servicesProvider6.languages;

      if (!languages.registerImplementationProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerImplementationProvider(selector, {
        provideImplementation: function provideImplementation(_ref7, token) {
          var textDocument = _ref7.textDocument,
              position = _ref7.position;
          return provider.provideImplementation(textDocument, position, token);
        }
      });
    },
    registerTypeDefinitionProvider: function registerTypeDefinitionProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider7 = servicesProvider(),
          languages = _servicesProvider7.languages;

      if (!languages.registerTypeDefinitionProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerTypeDefinitionProvider(selector, {
        provideTypeDefinition: function provideTypeDefinition(_ref8, token) {
          var textDocument = _ref8.textDocument,
              position = _ref8.position;
          return provider.provideTypeDefinition(textDocument, position, token);
        }
      });
    },
    registerDeclarationProvider: function registerDeclarationProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider8 = servicesProvider(),
          languages = _servicesProvider8.languages;

      if (!languages.registerDeclarationProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDeclarationProvider(selector, {
        provideDeclaration: function provideDeclaration(_ref9, token) {
          var textDocument = _ref9.textDocument,
              position = _ref9.position;
          return provider.provideDeclaration(textDocument, position, token);
        }
      });
    },
    registerHoverProvider: function registerHoverProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider9 = servicesProvider(),
          languages = _servicesProvider9.languages;

      if (languages.registerHoverProvider) {
        return languages.registerHoverProvider(selector, {
          provideHover: function provideHover(_ref10, token) {
            var textDocument = _ref10.textDocument,
                position = _ref10.position;
            return provider.provideHover(textDocument, position, token);
          }
        });
      }

      return disposable_1.Disposable.create(function () {});
    },
    registerDocumentHighlightProvider: function registerDocumentHighlightProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider10 = servicesProvider(),
          languages = _servicesProvider10.languages;

      if (!languages.registerDocumentHighlightProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDocumentHighlightProvider(selector, {
        provideDocumentHighlights: function provideDocumentHighlights(_ref11, token) {
          var textDocument = _ref11.textDocument,
              position = _ref11.position;
          return provider.provideDocumentHighlights(textDocument, position, token);
        }
      });
    },
    registerDocumentSymbolProvider: function registerDocumentSymbolProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider11 = servicesProvider(),
          languages = _servicesProvider11.languages;

      if (!languages.registerDocumentSymbolProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDocumentSymbolProvider(selector, {
        provideDocumentSymbols: function provideDocumentSymbols(_ref12, token) {
          var textDocument = _ref12.textDocument;
          return provider.provideDocumentSymbols(textDocument, token);
        }
      });
    },
    registerWorkspaceSymbolProvider: function registerWorkspaceSymbolProvider(provider) {
      var _servicesProvider12 = servicesProvider(),
          languages = _servicesProvider12.languages;

      if (!languages.registerWorkspaceSymbolProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerWorkspaceSymbolProvider({
        provideWorkspaceSymbols: function provideWorkspaceSymbols(_ref13, token) {
          var query = _ref13.query;
          return provider.provideWorkspaceSymbols(query, token);
        }
      });
    },
    registerReferenceProvider: function registerReferenceProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider13 = servicesProvider(),
          languages = _servicesProvider13.languages;

      if (!languages.registerReferenceProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerReferenceProvider(selector, {
        provideReferences: function provideReferences(_ref14, token) {
          var textDocument = _ref14.textDocument,
              position = _ref14.position,
              context = _ref14.context;
          return provider.provideReferences(textDocument, position, context, token);
        }
      });
    },
    registerRenameProvider: function registerRenameProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider14 = servicesProvider(),
          languages = _servicesProvider14.languages;

      if (!languages.registerRenameProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerRenameProvider(selector, {
        provideRenameEdits: function provideRenameEdits(_ref15, token) {
          var textDocument = _ref15.textDocument,
              position = _ref15.position,
              newName = _ref15.newName;
          return provider.provideRenameEdits(textDocument, position, newName, token);
        }
      });
    },
    registerDocumentFormattingEditProvider: function registerDocumentFormattingEditProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider15 = servicesProvider(),
          languages = _servicesProvider15.languages;

      if (!languages.registerDocumentFormattingEditProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDocumentFormattingEditProvider(selector, {
        provideDocumentFormattingEdits: function provideDocumentFormattingEdits(_ref16, token) {
          var textDocument = _ref16.textDocument,
              options = _ref16.options;
          return provider.provideDocumentFormattingEdits(textDocument, options, token);
        }
      });
    },
    registerDocumentRangeFormattingEditProvider: function registerDocumentRangeFormattingEditProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider16 = servicesProvider(),
          languages = _servicesProvider16.languages;

      if (!languages.registerDocumentRangeFormattingEditProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerDocumentRangeFormattingEditProvider(selector, {
        provideDocumentRangeFormattingEdits: function provideDocumentRangeFormattingEdits(_ref17, token) {
          var textDocument = _ref17.textDocument,
              range = _ref17.range,
              options = _ref17.options;
          return provider.provideDocumentRangeFormattingEdits(textDocument, range, options, token);
        }
      });
    },
    registerOnTypeFormattingEditProvider: function registerOnTypeFormattingEditProvider(selector, provider, firstTriggerCharacter) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider17 = servicesProvider(),
          languages = _servicesProvider17.languages;

      if (!languages.registerOnTypeFormattingEditProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      for (var _len2 = arguments.length, moreTriggerCharacter = new Array(_len2 > 3 ? _len2 - 3 : 0), _key2 = 3; _key2 < _len2; _key2++) {
        moreTriggerCharacter[_key2 - 3] = arguments[_key2];
      }

      return languages.registerOnTypeFormattingEditProvider.apply(languages, [selector, {
        provideOnTypeFormattingEdits: function provideOnTypeFormattingEdits(_ref18, token) {
          var textDocument = _ref18.textDocument,
              position = _ref18.position,
              ch = _ref18.ch,
              options = _ref18.options;
          return provider.provideOnTypeFormattingEdits(textDocument, position, ch, options, token);
        }
      }, firstTriggerCharacter].concat(moreTriggerCharacter));
    },
    registerSignatureHelpProvider: function registerSignatureHelpProvider(selector, provider, firstItem) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider18 = servicesProvider(),
          languages = _servicesProvider18.languages;

      if (!languages.registerSignatureHelpProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      var triggerCharacters;
      var retriggerCharacters;

      if (typeof firstItem === 'string') {
        for (var _len3 = arguments.length, remaining = new Array(_len3 > 3 ? _len3 - 3 : 0), _key3 = 3; _key3 < _len3; _key3++) {
          remaining[_key3 - 3] = arguments[_key3];
        }

        triggerCharacters = [firstItem].concat(remaining);
      } else if (firstItem) {
        triggerCharacters = firstItem.triggerCharacters;
        retriggerCharacters = firstItem.retriggerCharacters;
      }

      return languages.registerSignatureHelpProvider(selector, {
        triggerCharacters: triggerCharacters,
        retriggerCharacters: retriggerCharacters,
        provideSignatureHelp: function provideSignatureHelp(_ref19, token, context) {
          var textDocument = _ref19.textDocument,
              position = _ref19.position;
          return provider.provideSignatureHelp(textDocument, position, token, context);
        }
      });
    },
    registerDocumentLinkProvider: function registerDocumentLinkProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider19 = servicesProvider(),
          languages = _servicesProvider19.languages;

      if (!languages.registerDocumentLinkProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      var resolveDocumentLink = provider.resolveDocumentLink;
      return languages.registerDocumentLinkProvider(selector, {
        provideDocumentLinks: function provideDocumentLinks(_ref20, token) {
          var textDocument = _ref20.textDocument;
          return provider.provideDocumentLinks(textDocument, token);
        },
        resolveDocumentLink: resolveDocumentLink ? function (link, token) {
          return resolveDocumentLink(link, token);
        } : undefined
      });
    },
    registerColorProvider: function registerColorProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider20 = servicesProvider(),
          languages = _servicesProvider20.languages;

      if (!languages.registerColorProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerColorProvider(selector, {
        provideDocumentColors: function provideDocumentColors(_ref21, token) {
          var textDocument = _ref21.textDocument;
          return provider.provideDocumentColors(textDocument, token);
        },
        provideColorPresentations: function provideColorPresentations(_ref22, token) {
          var textDocument = _ref22.textDocument,
              color = _ref22.color,
              range = _ref22.range;
          return provider.provideColorPresentations(color, {
            document: textDocument,
            range: range
          }, token);
        }
      });
    },
    registerFoldingRangeProvider: function registerFoldingRangeProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider21 = servicesProvider(),
          languages = _servicesProvider21.languages;

      if (!languages.registerFoldingRangeProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerFoldingRangeProvider(selector, {
        provideFoldingRanges: function provideFoldingRanges(_ref23, token) {
          var textDocument = _ref23.textDocument;
          return provider.provideFoldingRanges(textDocument, {}, token);
        }
      });
    },
    registerSelectionRangeProvider: function registerSelectionRangeProvider(selector, provider) {
      if (!services_1.isDocumentSelector(selector)) {
        throw new Error('unexpected selector: ' + JSON.stringify(selector));
      }

      var _servicesProvider22 = servicesProvider(),
          languages = _servicesProvider22.languages;

      if (!languages.registerSelectionRangeProvider) {
        return disposable_1.Disposable.create(function () {});
      }

      return languages.registerSelectionRangeProvider(selector, {
        provideSelectionRanges: function provideSelectionRanges(_ref24, token) {
          var textDocument = _ref24.textDocument,
              positions = _ref24.positions;
          return provider.provideSelectionRanges(textDocument, positions, token);
        }
      });
    },
    getLanguages: unsupported,
    setTextDocumentLanguage: unsupported,
    getDiagnostics: unsupported,
    setLanguageConfiguration: unsupported,
    onDidChangeDiagnostics: unsupported
  };

  function showMessage(type, arg0) {
    if (typeof arg0 !== "string") {
      throw new Error('unexpected message: ' + JSON.stringify(arg0));
    }

    var message = arg0;

    for (var _len4 = arguments.length, arg1 = new Array(_len4 > 2 ? _len4 - 2 : 0), _key4 = 2; _key4 < _len4; _key4++) {
      arg1[_key4 - 2] = arguments[_key4];
    }

    if (arg1 !== undefined && !Array.isArray(arg1)) {
      throw new Error('unexpected actions: ' + JSON.stringify(arg1));
    }

    var actions = arg1 || [];

    var _servicesProvider23 = servicesProvider(),
        window = _servicesProvider23.window;

    if (!window) {
      return Promise.resolve(undefined);
    }

    return window.showMessage.apply(window, [type, message].concat(actions));
  }

  var window = {
    showInformationMessage: showMessage.bind(undefined, services_1.MessageType.Info),
    showWarningMessage: showMessage.bind(undefined, services_1.MessageType.Warning),
    showErrorMessage: showMessage.bind(undefined, services_1.MessageType.Error),
    createOutputChannel: function createOutputChannel(name) {
      var _servicesProvider24 = servicesProvider(),
          window = _servicesProvider24.window;

      var createOutputChannel = window ? window.createOutputChannel : undefined;
      var channel = createOutputChannel ? createOutputChannel.bind(window)(name) : undefined;
      return {
        name: name,
        append: channel ? channel.append.bind(channel) : function () {},
        appendLine: channel ? channel.appendLine.bind(channel) : function () {},
        clear: unsupported,
        show: function show(arg) {
          if (arg !== undefined && typeof arg !== 'boolean') {
            throw new Error('unexpected preserveFocus argument: ' + JSON.stringify(arg, undefined, 4));
          }

          return channel ? channel.show(arg) : function () {};
        },
        hide: unsupported,
        dispose: channel ? channel.dispose.bind(channel) : function () {}
      };
    },
    withProgress: function withProgress(options, task) {
      var _servicesProvider25 = servicesProvider(),
          window = _servicesProvider25.window;

      if (window && window.withProgress) {
        return window.withProgress(options, task);
      }

      return task({
        report: function report() {}
      }, new vscode.CancellationTokenSource().token);
    },
    showTextDocument: unsupported,
    createTextEditorDecorationType: unsupported,
    showQuickPick: unsupported,
    showWorkspaceFolderPick: unsupported,
    showOpenDialog: unsupported,
    showSaveDialog: unsupported,
    showInputBox: unsupported,
    createWebviewPanel: unsupported,
    setStatusBarMessage: unsupported,
    withScmProgress: unsupported,
    createStatusBarItem: unsupported,
    createTerminal: unsupported,
    registerTreeDataProvider: unsupported,
    createTreeView: unsupported,
    registerWebviewPanelSerializer: unsupported,

    get activeTextEditor() {
      return unsupported();
    },

    get visibleTextEditors() {
      return unsupported();
    },

    onDidChangeActiveTextEditor: unsupported,
    onDidChangeVisibleTextEditors: unsupported,
    onDidChangeTextEditorSelection: unsupported,
    onDidChangeTextEditorVisibleRanges: unsupported,
    onDidChangeTextEditorOptions: unsupported,
    onDidChangeTextEditorViewColumn: unsupported,

    get terminals() {
      return unsupported();
    },

    get activeTerminal() {
      return unsupported();
    },

    onDidChangeActiveTerminal: unsupported,
    onDidOpenTerminal: unsupported,
    onDidCloseTerminal: unsupported,

    get state() {
      return unsupported();
    },

    onDidChangeWindowState: unsupported,
    createQuickPick: unsupported,
    createInputBox: unsupported,
    registerUriHandler: unsupported
  };
  var commands = {
    registerCommand: function registerCommand(command, callback, thisArg) {
      var _servicesProvider26 = servicesProvider(),
          commands = _servicesProvider26.commands;

      if (!commands) {
        return disposable_1.Disposable.create(function () {});
      }

      return commands.registerCommand(command, callback, thisArg);
    },
    registerTextEditorCommand: unsupported,
    executeCommand: unsupported,
    getCommands: unsupported
  };

  var CodeDisposable =
  /*#__PURE__*/
  function () {
    function CodeDisposable(callOnDispose) {
      _classCallCheck(this, CodeDisposable);

      this.callOnDispose = callOnDispose;
    }

    _createClass(CodeDisposable, [{
      key: "dispose",
      value: function dispose() {
        this.callOnDispose();
      }
    }]);

    return CodeDisposable;
  }();

  return {
    workspace: workspace,
    languages: languages,
    window: window,
    commands: commands,
    Uri: Uri,
    CompletionItem: CompletionItem,
    CodeLens: CodeLens,
    DocumentLink: DocumentLink,
    CodeActionKind: CodeActionKind,
    Disposable: CodeDisposable
  };
}

exports.createVSCodeApi = createVSCodeApi;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/vscode-api'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/monaco-languageclient/lib/vscode-compatibility.js ***!
  \**********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/* --------------------------------------------------------------------------------------------
 * Copyright (c) 2018 TypeFox GmbH (http://www.typefox.io). All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

var vscode_api_1 = __webpack_require__(/*! ./vscode-api */ "../../node_modules/monaco-languageclient/lib/vscode-api.js");

var services_1 = __webpack_require__(/*! ./services */ "../../node_modules/monaco-languageclient/lib/services.js");

module.exports = vscode_api_1.createVSCodeApi(services_1.Services.get);
if (!global) global = {};
(global['theia'] = global['theia'] || {})['monaco-languageclient/lib/vscode-compatibility'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/regenerator-runtime/runtime.js":
/*!***************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/regenerator-runtime/runtime.js ***!
  \***************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

var runtime = (function (exports) {
  "use strict";

  var Op = Object.prototype;
  var hasOwn = Op.hasOwnProperty;
  var undefined; // More compressible than void 0.
  var $Symbol = typeof Symbol === "function" ? Symbol : {};
  var iteratorSymbol = $Symbol.iterator || "@@iterator";
  var asyncIteratorSymbol = $Symbol.asyncIterator || "@@asyncIterator";
  var toStringTagSymbol = $Symbol.toStringTag || "@@toStringTag";

  function wrap(innerFn, outerFn, self, tryLocsList) {
    // If outerFn provided and outerFn.prototype is a Generator, then outerFn.prototype instanceof Generator.
    var protoGenerator = outerFn && outerFn.prototype instanceof Generator ? outerFn : Generator;
    var generator = Object.create(protoGenerator.prototype);
    var context = new Context(tryLocsList || []);

    // The ._invoke method unifies the implementations of the .next,
    // .throw, and .return methods.
    generator._invoke = makeInvokeMethod(innerFn, self, context);

    return generator;
  }
  exports.wrap = wrap;

  // Try/catch helper to minimize deoptimizations. Returns a completion
  // record like context.tryEntries[i].completion. This interface could
  // have been (and was previously) designed to take a closure to be
  // invoked without arguments, but in all the cases we care about we
  // already have an existing method we want to call, so there's no need
  // to create a new function object. We can even get away with assuming
  // the method takes exactly one argument, since that happens to be true
  // in every case, so we don't have to touch the arguments object. The
  // only additional allocation required is the completion record, which
  // has a stable shape and so hopefully should be cheap to allocate.
  function tryCatch(fn, obj, arg) {
    try {
      return { type: "normal", arg: fn.call(obj, arg) };
    } catch (err) {
      return { type: "throw", arg: err };
    }
  }

  var GenStateSuspendedStart = "suspendedStart";
  var GenStateSuspendedYield = "suspendedYield";
  var GenStateExecuting = "executing";
  var GenStateCompleted = "completed";

  // Returning this object from the innerFn has the same effect as
  // breaking out of the dispatch switch statement.
  var ContinueSentinel = {};

  // Dummy constructor functions that we use as the .constructor and
  // .constructor.prototype properties for functions that return Generator
  // objects. For full spec compliance, you may wish to configure your
  // minifier not to mangle the names of these two functions.
  function Generator() {}
  function GeneratorFunction() {}
  function GeneratorFunctionPrototype() {}

  // This is a polyfill for %IteratorPrototype% for environments that
  // don't natively support it.
  var IteratorPrototype = {};
  IteratorPrototype[iteratorSymbol] = function () {
    return this;
  };

  var getProto = Object.getPrototypeOf;
  var NativeIteratorPrototype = getProto && getProto(getProto(values([])));
  if (NativeIteratorPrototype &&
      NativeIteratorPrototype !== Op &&
      hasOwn.call(NativeIteratorPrototype, iteratorSymbol)) {
    // This environment has a native %IteratorPrototype%; use it instead
    // of the polyfill.
    IteratorPrototype = NativeIteratorPrototype;
  }

  var Gp = GeneratorFunctionPrototype.prototype =
    Generator.prototype = Object.create(IteratorPrototype);
  GeneratorFunction.prototype = Gp.constructor = GeneratorFunctionPrototype;
  GeneratorFunctionPrototype.constructor = GeneratorFunction;
  GeneratorFunctionPrototype[toStringTagSymbol] =
    GeneratorFunction.displayName = "GeneratorFunction";

  // Helper for defining the .next, .throw, and .return methods of the
  // Iterator interface in terms of a single ._invoke method.
  function defineIteratorMethods(prototype) {
    ["next", "throw", "return"].forEach(function(method) {
      prototype[method] = function(arg) {
        return this._invoke(method, arg);
      };
    });
  }

  exports.isGeneratorFunction = function(genFun) {
    var ctor = typeof genFun === "function" && genFun.constructor;
    return ctor
      ? ctor === GeneratorFunction ||
        // For the native GeneratorFunction constructor, the best we can
        // do is to check its .name property.
        (ctor.displayName || ctor.name) === "GeneratorFunction"
      : false;
  };

  exports.mark = function(genFun) {
    if (Object.setPrototypeOf) {
      Object.setPrototypeOf(genFun, GeneratorFunctionPrototype);
    } else {
      genFun.__proto__ = GeneratorFunctionPrototype;
      if (!(toStringTagSymbol in genFun)) {
        genFun[toStringTagSymbol] = "GeneratorFunction";
      }
    }
    genFun.prototype = Object.create(Gp);
    return genFun;
  };

  // Within the body of any async function, `await x` is transformed to
  // `yield regeneratorRuntime.awrap(x)`, so that the runtime can test
  // `hasOwn.call(value, "__await")` to determine if the yielded value is
  // meant to be awaited.
  exports.awrap = function(arg) {
    return { __await: arg };
  };

  function AsyncIterator(generator) {
    function invoke(method, arg, resolve, reject) {
      var record = tryCatch(generator[method], generator, arg);
      if (record.type === "throw") {
        reject(record.arg);
      } else {
        var result = record.arg;
        var value = result.value;
        if (value &&
            typeof value === "object" &&
            hasOwn.call(value, "__await")) {
          return Promise.resolve(value.__await).then(function(value) {
            invoke("next", value, resolve, reject);
          }, function(err) {
            invoke("throw", err, resolve, reject);
          });
        }

        return Promise.resolve(value).then(function(unwrapped) {
          // When a yielded Promise is resolved, its final value becomes
          // the .value of the Promise<{value,done}> result for the
          // current iteration.
          result.value = unwrapped;
          resolve(result);
        }, function(error) {
          // If a rejected Promise was yielded, throw the rejection back
          // into the async generator function so it can be handled there.
          return invoke("throw", error, resolve, reject);
        });
      }
    }

    var previousPromise;

    function enqueue(method, arg) {
      function callInvokeWithMethodAndArg() {
        return new Promise(function(resolve, reject) {
          invoke(method, arg, resolve, reject);
        });
      }

      return previousPromise =
        // If enqueue has been called before, then we want to wait until
        // all previous Promises have been resolved before calling invoke,
        // so that results are always delivered in the correct order. If
        // enqueue has not been called before, then it is important to
        // call invoke immediately, without waiting on a callback to fire,
        // so that the async generator function has the opportunity to do
        // any necessary setup in a predictable way. This predictability
        // is why the Promise constructor synchronously invokes its
        // executor callback, and why async functions synchronously
        // execute code before the first await. Since we implement simple
        // async functions in terms of async generators, it is especially
        // important to get this right, even though it requires care.
        previousPromise ? previousPromise.then(
          callInvokeWithMethodAndArg,
          // Avoid propagating failures to Promises returned by later
          // invocations of the iterator.
          callInvokeWithMethodAndArg
        ) : callInvokeWithMethodAndArg();
    }

    // Define the unified helper method that is used to implement .next,
    // .throw, and .return (see defineIteratorMethods).
    this._invoke = enqueue;
  }

  defineIteratorMethods(AsyncIterator.prototype);
  AsyncIterator.prototype[asyncIteratorSymbol] = function () {
    return this;
  };
  exports.AsyncIterator = AsyncIterator;

  // Note that simple async functions are implemented on top of
  // AsyncIterator objects; they just return a Promise for the value of
  // the final result produced by the iterator.
  exports.async = function(innerFn, outerFn, self, tryLocsList) {
    var iter = new AsyncIterator(
      wrap(innerFn, outerFn, self, tryLocsList)
    );

    return exports.isGeneratorFunction(outerFn)
      ? iter // If outerFn is a generator, return the full iterator.
      : iter.next().then(function(result) {
          return result.done ? result.value : iter.next();
        });
  };

  function makeInvokeMethod(innerFn, self, context) {
    var state = GenStateSuspendedStart;

    return function invoke(method, arg) {
      if (state === GenStateExecuting) {
        throw new Error("Generator is already running");
      }

      if (state === GenStateCompleted) {
        if (method === "throw") {
          throw arg;
        }

        // Be forgiving, per 25.3.3.3.3 of the spec:
        // https://people.mozilla.org/~jorendorff/es6-draft.html#sec-generatorresume
        return doneResult();
      }

      context.method = method;
      context.arg = arg;

      while (true) {
        var delegate = context.delegate;
        if (delegate) {
          var delegateResult = maybeInvokeDelegate(delegate, context);
          if (delegateResult) {
            if (delegateResult === ContinueSentinel) continue;
            return delegateResult;
          }
        }

        if (context.method === "next") {
          // Setting context._sent for legacy support of Babel's
          // function.sent implementation.
          context.sent = context._sent = context.arg;

        } else if (context.method === "throw") {
          if (state === GenStateSuspendedStart) {
            state = GenStateCompleted;
            throw context.arg;
          }

          context.dispatchException(context.arg);

        } else if (context.method === "return") {
          context.abrupt("return", context.arg);
        }

        state = GenStateExecuting;

        var record = tryCatch(innerFn, self, context);
        if (record.type === "normal") {
          // If an exception is thrown from innerFn, we leave state ===
          // GenStateExecuting and loop back for another invocation.
          state = context.done
            ? GenStateCompleted
            : GenStateSuspendedYield;

          if (record.arg === ContinueSentinel) {
            continue;
          }

          return {
            value: record.arg,
            done: context.done
          };

        } else if (record.type === "throw") {
          state = GenStateCompleted;
          // Dispatch the exception by looping back around to the
          // context.dispatchException(context.arg) call above.
          context.method = "throw";
          context.arg = record.arg;
        }
      }
    };
  }

  // Call delegate.iterator[context.method](context.arg) and handle the
  // result, either by returning a { value, done } result from the
  // delegate iterator, or by modifying context.method and context.arg,
  // setting context.delegate to null, and returning the ContinueSentinel.
  function maybeInvokeDelegate(delegate, context) {
    var method = delegate.iterator[context.method];
    if (method === undefined) {
      // A .throw or .return when the delegate iterator has no .throw
      // method always terminates the yield* loop.
      context.delegate = null;

      if (context.method === "throw") {
        // Note: ["return"] must be used for ES3 parsing compatibility.
        if (delegate.iterator["return"]) {
          // If the delegate iterator has a return method, give it a
          // chance to clean up.
          context.method = "return";
          context.arg = undefined;
          maybeInvokeDelegate(delegate, context);

          if (context.method === "throw") {
            // If maybeInvokeDelegate(context) changed context.method from
            // "return" to "throw", let that override the TypeError below.
            return ContinueSentinel;
          }
        }

        context.method = "throw";
        context.arg = new TypeError(
          "The iterator does not provide a 'throw' method");
      }

      return ContinueSentinel;
    }

    var record = tryCatch(method, delegate.iterator, context.arg);

    if (record.type === "throw") {
      context.method = "throw";
      context.arg = record.arg;
      context.delegate = null;
      return ContinueSentinel;
    }

    var info = record.arg;

    if (! info) {
      context.method = "throw";
      context.arg = new TypeError("iterator result is not an object");
      context.delegate = null;
      return ContinueSentinel;
    }

    if (info.done) {
      // Assign the result of the finished delegate to the temporary
      // variable specified by delegate.resultName (see delegateYield).
      context[delegate.resultName] = info.value;

      // Resume execution at the desired location (see delegateYield).
      context.next = delegate.nextLoc;

      // If context.method was "throw" but the delegate handled the
      // exception, let the outer generator proceed normally. If
      // context.method was "next", forget context.arg since it has been
      // "consumed" by the delegate iterator. If context.method was
      // "return", allow the original .return call to continue in the
      // outer generator.
      if (context.method !== "return") {
        context.method = "next";
        context.arg = undefined;
      }

    } else {
      // Re-yield the result returned by the delegate method.
      return info;
    }

    // The delegate iterator is finished, so forget it and continue with
    // the outer generator.
    context.delegate = null;
    return ContinueSentinel;
  }

  // Define Generator.prototype.{next,throw,return} in terms of the
  // unified ._invoke helper method.
  defineIteratorMethods(Gp);

  Gp[toStringTagSymbol] = "Generator";

  // A Generator should always return itself as the iterator object when the
  // @@iterator function is called on it. Some browsers' implementations of the
  // iterator prototype chain incorrectly implement this, causing the Generator
  // object to not be returned from this call. This ensures that doesn't happen.
  // See https://github.com/facebook/regenerator/issues/274 for more details.
  Gp[iteratorSymbol] = function() {
    return this;
  };

  Gp.toString = function() {
    return "[object Generator]";
  };

  function pushTryEntry(locs) {
    var entry = { tryLoc: locs[0] };

    if (1 in locs) {
      entry.catchLoc = locs[1];
    }

    if (2 in locs) {
      entry.finallyLoc = locs[2];
      entry.afterLoc = locs[3];
    }

    this.tryEntries.push(entry);
  }

  function resetTryEntry(entry) {
    var record = entry.completion || {};
    record.type = "normal";
    delete record.arg;
    entry.completion = record;
  }

  function Context(tryLocsList) {
    // The root entry object (effectively a try statement without a catch
    // or a finally block) gives us a place to store values thrown from
    // locations where there is no enclosing try statement.
    this.tryEntries = [{ tryLoc: "root" }];
    tryLocsList.forEach(pushTryEntry, this);
    this.reset(true);
  }

  exports.keys = function(object) {
    var keys = [];
    for (var key in object) {
      keys.push(key);
    }
    keys.reverse();

    // Rather than returning an object with a next method, we keep
    // things simple and return the next function itself.
    return function next() {
      while (keys.length) {
        var key = keys.pop();
        if (key in object) {
          next.value = key;
          next.done = false;
          return next;
        }
      }

      // To avoid creating an additional object, we just hang the .value
      // and .done properties off the next function object itself. This
      // also ensures that the minifier will not anonymize the function.
      next.done = true;
      return next;
    };
  };

  function values(iterable) {
    if (iterable) {
      var iteratorMethod = iterable[iteratorSymbol];
      if (iteratorMethod) {
        return iteratorMethod.call(iterable);
      }

      if (typeof iterable.next === "function") {
        return iterable;
      }

      if (!isNaN(iterable.length)) {
        var i = -1, next = function next() {
          while (++i < iterable.length) {
            if (hasOwn.call(iterable, i)) {
              next.value = iterable[i];
              next.done = false;
              return next;
            }
          }

          next.value = undefined;
          next.done = true;

          return next;
        };

        return next.next = next;
      }
    }

    // Return an iterator with no values.
    return { next: doneResult };
  }
  exports.values = values;

  function doneResult() {
    return { value: undefined, done: true };
  }

  Context.prototype = {
    constructor: Context,

    reset: function(skipTempReset) {
      this.prev = 0;
      this.next = 0;
      // Resetting context._sent for legacy support of Babel's
      // function.sent implementation.
      this.sent = this._sent = undefined;
      this.done = false;
      this.delegate = null;

      this.method = "next";
      this.arg = undefined;

      this.tryEntries.forEach(resetTryEntry);

      if (!skipTempReset) {
        for (var name in this) {
          // Not sure about the optimal order of these conditions:
          if (name.charAt(0) === "t" &&
              hasOwn.call(this, name) &&
              !isNaN(+name.slice(1))) {
            this[name] = undefined;
          }
        }
      }
    },

    stop: function() {
      this.done = true;

      var rootEntry = this.tryEntries[0];
      var rootRecord = rootEntry.completion;
      if (rootRecord.type === "throw") {
        throw rootRecord.arg;
      }

      return this.rval;
    },

    dispatchException: function(exception) {
      if (this.done) {
        throw exception;
      }

      var context = this;
      function handle(loc, caught) {
        record.type = "throw";
        record.arg = exception;
        context.next = loc;

        if (caught) {
          // If the dispatched exception was caught by a catch block,
          // then let that catch block handle the exception normally.
          context.method = "next";
          context.arg = undefined;
        }

        return !! caught;
      }

      for (var i = this.tryEntries.length - 1; i >= 0; --i) {
        var entry = this.tryEntries[i];
        var record = entry.completion;

        if (entry.tryLoc === "root") {
          // Exception thrown outside of any try block that could handle
          // it, so set the completion value of the entire function to
          // throw the exception.
          return handle("end");
        }

        if (entry.tryLoc <= this.prev) {
          var hasCatch = hasOwn.call(entry, "catchLoc");
          var hasFinally = hasOwn.call(entry, "finallyLoc");

          if (hasCatch && hasFinally) {
            if (this.prev < entry.catchLoc) {
              return handle(entry.catchLoc, true);
            } else if (this.prev < entry.finallyLoc) {
              return handle(entry.finallyLoc);
            }

          } else if (hasCatch) {
            if (this.prev < entry.catchLoc) {
              return handle(entry.catchLoc, true);
            }

          } else if (hasFinally) {
            if (this.prev < entry.finallyLoc) {
              return handle(entry.finallyLoc);
            }

          } else {
            throw new Error("try statement without catch or finally");
          }
        }
      }
    },

    abrupt: function(type, arg) {
      for (var i = this.tryEntries.length - 1; i >= 0; --i) {
        var entry = this.tryEntries[i];
        if (entry.tryLoc <= this.prev &&
            hasOwn.call(entry, "finallyLoc") &&
            this.prev < entry.finallyLoc) {
          var finallyEntry = entry;
          break;
        }
      }

      if (finallyEntry &&
          (type === "break" ||
           type === "continue") &&
          finallyEntry.tryLoc <= arg &&
          arg <= finallyEntry.finallyLoc) {
        // Ignore the finally entry if control is not jumping to a
        // location outside the try/catch block.
        finallyEntry = null;
      }

      var record = finallyEntry ? finallyEntry.completion : {};
      record.type = type;
      record.arg = arg;

      if (finallyEntry) {
        this.method = "next";
        this.next = finallyEntry.finallyLoc;
        return ContinueSentinel;
      }

      return this.complete(record);
    },

    complete: function(record, afterLoc) {
      if (record.type === "throw") {
        throw record.arg;
      }

      if (record.type === "break" ||
          record.type === "continue") {
        this.next = record.arg;
      } else if (record.type === "return") {
        this.rval = this.arg = record.arg;
        this.method = "return";
        this.next = "end";
      } else if (record.type === "normal" && afterLoc) {
        this.next = afterLoc;
      }

      return ContinueSentinel;
    },

    finish: function(finallyLoc) {
      for (var i = this.tryEntries.length - 1; i >= 0; --i) {
        var entry = this.tryEntries[i];
        if (entry.finallyLoc === finallyLoc) {
          this.complete(entry.completion, entry.afterLoc);
          resetTryEntry(entry);
          return ContinueSentinel;
        }
      }
    },

    "catch": function(tryLoc) {
      for (var i = this.tryEntries.length - 1; i >= 0; --i) {
        var entry = this.tryEntries[i];
        if (entry.tryLoc === tryLoc) {
          var record = entry.completion;
          if (record.type === "throw") {
            var thrown = record.arg;
            resetTryEntry(entry);
          }
          return thrown;
        }
      }

      // The context.catch method must only be called with a location
      // argument that corresponds to a known catch block.
      throw new Error("illegal catch attempt");
    },

    delegateYield: function(iterable, resultName, nextLoc) {
      this.delegate = {
        iterator: values(iterable),
        resultName: resultName,
        nextLoc: nextLoc
      };

      if (this.method === "next") {
        // Deliberately forget the last sent value so that we don't
        // accidentally pass it on to the delegate.
        this.arg = undefined;
      }

      return ContinueSentinel;
    }
  };

  // Regardless of whether this script is executing as a CommonJS module
  // or not, return the runtime object so that we can declare the variable
  // regeneratorRuntime in the outer scope, which allows this module to be
  // injected easily by `bin/regenerator --include-runtime script.js`.
  return exports;

}(
  // If this script is executing as a CommonJS module, use module.exports
  // as the regeneratorRuntime namespace. Otherwise create a new empty
  // object. Either way, the resulting object will be used to initialize
  // the regeneratorRuntime variable at the top of this file.
   true ? module.exports : undefined
));

try {
  regeneratorRuntime = runtime;
} catch (accidentalStrictMode) {
  // This module should not be running in strict mode, so the above
  // assignment should always work unless something is misconfigured. Just
  // in case runtime.js accidentally runs in strict mode, we can escape
  // strict mode using a global Function call. This could conceivably fail
  // if a Content Security Policy forbids using Function, but in that case
  // the proper solution is to fix the accidental strict mode problem. If
  // you've misconfigured your bundler to force strict mode and applied a
  // CSP to forbid Function, and you're not willing to fix either of those
  // problems, please detail your unique predicament in a GitHub issue.
  Function("r", "regeneratorRuntime = r")(runtime);
}

if (!global) global = {};
(global['theia'] = global['theia'] || {})['regenerator-runtime/runtime'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/uuid/index.js":
/*!**********************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/uuid/index.js ***!
  \**********************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {var v1 = __webpack_require__(/*! ./v1 */ "../../node_modules/uuid/v1.js");
var v4 = __webpack_require__(/*! ./v4 */ "../../node_modules/uuid/v4.js");

var uuid = v4;
uuid.v1 = v1;
uuid.v4 = v4;

module.exports = uuid;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['uuid'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/uuid/lib/bytesToUuid.js":
/*!********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/uuid/lib/bytesToUuid.js ***!
  \********************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {/**
 * Convert array of 16 byte values to UUID string format of the form:
 * XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
 */
var byteToHex = [];
for (var i = 0; i < 256; ++i) {
  byteToHex[i] = (i + 0x100).toString(16).substr(1);
}

function bytesToUuid(buf, offset) {
  var i = offset || 0;
  var bth = byteToHex;
  // join used to fix memory issue caused by concatenation: https://bugs.chromium.org/p/v8/issues/detail?id=3175#c4
  return ([
    bth[buf[i++]], bth[buf[i++]],
    bth[buf[i++]], bth[buf[i++]], '-',
    bth[buf[i++]], bth[buf[i++]], '-',
    bth[buf[i++]], bth[buf[i++]], '-',
    bth[buf[i++]], bth[buf[i++]], '-',
    bth[buf[i++]], bth[buf[i++]],
    bth[buf[i++]], bth[buf[i++]],
    bth[buf[i++]], bth[buf[i++]]
  ]).join('');
}

module.exports = bytesToUuid;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['uuid/lib/bytesToUuid'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/uuid/lib/rng-browser.js":
/*!********************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/uuid/lib/rng-browser.js ***!
  \********************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {// Unique ID creation requires a high quality random # generator.  In the
// browser this is a little complicated due to unknown quality of Math.random()
// and inconsistent support for the `crypto` API.  We do the best we can via
// feature-detection

// getRandomValues needs to be invoked in a context where "this" is a Crypto
// implementation. Also, find the complete implementation of crypto on IE11.
var getRandomValues = (typeof(crypto) != 'undefined' && crypto.getRandomValues && crypto.getRandomValues.bind(crypto)) ||
                      (typeof(msCrypto) != 'undefined' && typeof window.msCrypto.getRandomValues == 'function' && msCrypto.getRandomValues.bind(msCrypto));

if (getRandomValues) {
  // WHATWG crypto RNG - http://wiki.whatwg.org/wiki/Crypto
  var rnds8 = new Uint8Array(16); // eslint-disable-line no-undef

  module.exports = function whatwgRNG() {
    getRandomValues(rnds8);
    return rnds8;
  };
} else {
  // Math.random()-based (RNG)
  //
  // If all else fails, use Math.random().  It's fast, but is of unspecified
  // quality.
  var rnds = new Array(16);

  module.exports = function mathRNG() {
    for (var i = 0, r; i < 16; i++) {
      if ((i & 0x03) === 0) r = Math.random() * 0x100000000;
      rnds[i] = r >>> ((i & 0x03) << 3) & 0xff;
    }

    return rnds;
  };
}

if (!global) global = {};
(global['theia'] = global['theia'] || {})['uuid/lib/rng-browser'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/uuid/v1.js":
/*!*******************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/uuid/v1.js ***!
  \*******************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {var rng = __webpack_require__(/*! ./lib/rng */ "../../node_modules/uuid/lib/rng-browser.js");
var bytesToUuid = __webpack_require__(/*! ./lib/bytesToUuid */ "../../node_modules/uuid/lib/bytesToUuid.js");

// **`v1()` - Generate time-based UUID**
//
// Inspired by https://github.com/LiosK/UUID.js
// and http://docs.python.org/library/uuid.html

var _nodeId;
var _clockseq;

// Previous uuid creation time
var _lastMSecs = 0;
var _lastNSecs = 0;

// See https://github.com/uuidjs/uuid for API details
function v1(options, buf, offset) {
  var i = buf && offset || 0;
  var b = buf || [];

  options = options || {};
  var node = options.node || _nodeId;
  var clockseq = options.clockseq !== undefined ? options.clockseq : _clockseq;

  // node and clockseq need to be initialized to random values if they're not
  // specified.  We do this lazily to minimize issues related to insufficient
  // system entropy.  See #189
  if (node == null || clockseq == null) {
    var seedBytes = rng();
    if (node == null) {
      // Per 4.5, create and 48-bit node id, (47 random bits + multicast bit = 1)
      node = _nodeId = [
        seedBytes[0] | 0x01,
        seedBytes[1], seedBytes[2], seedBytes[3], seedBytes[4], seedBytes[5]
      ];
    }
    if (clockseq == null) {
      // Per 4.2.2, randomize (14 bit) clockseq
      clockseq = _clockseq = (seedBytes[6] << 8 | seedBytes[7]) & 0x3fff;
    }
  }

  // UUID timestamps are 100 nano-second units since the Gregorian epoch,
  // (1582-10-15 00:00).  JSNumbers aren't precise enough for this, so
  // time is handled internally as 'msecs' (integer milliseconds) and 'nsecs'
  // (100-nanoseconds offset from msecs) since unix epoch, 1970-01-01 00:00.
  var msecs = options.msecs !== undefined ? options.msecs : new Date().getTime();

  // Per 4.2.1.2, use count of uuid's generated during the current clock
  // cycle to simulate higher resolution clock
  var nsecs = options.nsecs !== undefined ? options.nsecs : _lastNSecs + 1;

  // Time since last uuid creation (in msecs)
  var dt = (msecs - _lastMSecs) + (nsecs - _lastNSecs)/10000;

  // Per 4.2.1.2, Bump clockseq on clock regression
  if (dt < 0 && options.clockseq === undefined) {
    clockseq = clockseq + 1 & 0x3fff;
  }

  // Reset nsecs if clock regresses (new clockseq) or we've moved onto a new
  // time interval
  if ((dt < 0 || msecs > _lastMSecs) && options.nsecs === undefined) {
    nsecs = 0;
  }

  // Per 4.2.1.2 Throw error if too many uuids are requested
  if (nsecs >= 10000) {
    throw new Error('uuid.v1(): Can\'t create more than 10M uuids/sec');
  }

  _lastMSecs = msecs;
  _lastNSecs = nsecs;
  _clockseq = clockseq;

  // Per 4.1.4 - Convert from unix epoch to Gregorian epoch
  msecs += 12219292800000;

  // `time_low`
  var tl = ((msecs & 0xfffffff) * 10000 + nsecs) % 0x100000000;
  b[i++] = tl >>> 24 & 0xff;
  b[i++] = tl >>> 16 & 0xff;
  b[i++] = tl >>> 8 & 0xff;
  b[i++] = tl & 0xff;

  // `time_mid`
  var tmh = (msecs / 0x100000000 * 10000) & 0xfffffff;
  b[i++] = tmh >>> 8 & 0xff;
  b[i++] = tmh & 0xff;

  // `time_high_and_version`
  b[i++] = tmh >>> 24 & 0xf | 0x10; // include version
  b[i++] = tmh >>> 16 & 0xff;

  // `clock_seq_hi_and_reserved` (Per 4.2.2 - include variant)
  b[i++] = clockseq >>> 8 | 0x80;

  // `clock_seq_low`
  b[i++] = clockseq & 0xff;

  // `node`
  for (var n = 0; n < 6; ++n) {
    b[i + n] = node[n];
  }

  return buf ? buf : bytesToUuid(b);
}

module.exports = v1;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['uuid/v1'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/uuid/v4.js":
/*!*******************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/uuid/v4.js ***!
  \*******************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {var rng = __webpack_require__(/*! ./lib/rng */ "../../node_modules/uuid/lib/rng-browser.js");
var bytesToUuid = __webpack_require__(/*! ./lib/bytesToUuid */ "../../node_modules/uuid/lib/bytesToUuid.js");

function v4(options, buf, offset) {
  var i = buf && offset || 0;

  if (typeof(options) == 'string') {
    buf = options === 'binary' ? new Array(16) : null;
    options = null;
  }
  options = options || {};

  var rnds = options.random || (options.rng || rng)();

  // Per 4.4, set bits for version and `clock_seq_hi_and_reserved`
  rnds[6] = (rnds[6] & 0x0f) | 0x40;
  rnds[8] = (rnds[8] & 0x3f) | 0x80;

  // Copy bytes to buffer, if provided
  if (buf) {
    for (var ii = 0; ii < 16; ++ii) {
      buf[i + ii] = rnds[ii];
    }
  }

  return buf || bytesToUuid(rnds);
}

module.exports = v4;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['uuid/v4'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/client.js":
/*!********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/client.js ***!
  \********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(process, global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _toConsumableArray = __webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "../../node_modules/@babel/runtime/helpers/toConsumableArray.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _get = __webpack_require__(/*! @babel/runtime/helpers/get */ "../../node_modules/@babel/runtime/helpers/get.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var c2p = __webpack_require__(/*! ./codeConverter */ "../../node_modules/vscode-languageclient/lib/codeConverter.js");

var p2c = __webpack_require__(/*! ./protocolConverter */ "../../node_modules/vscode-languageclient/lib/protocolConverter.js");

var Is = __webpack_require__(/*! ./utils/is */ "../../node_modules/vscode-languageclient/lib/utils/is.js");

var async_1 = __webpack_require__(/*! ./utils/async */ "../../node_modules/vscode-languageclient/lib/utils/async.js");

var UUID = __webpack_require__(/*! ./utils/uuid */ "../../node_modules/vscode-languageclient/lib/utils/uuid.js");

var progressPart_1 = __webpack_require__(/*! ./progressPart */ "../../node_modules/vscode-languageclient/lib/progressPart.js");

__export(__webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js"));

var ConsoleLogger =
/*#__PURE__*/
function () {
  function ConsoleLogger() {
    _classCallCheck(this, ConsoleLogger);
  }

  _createClass(ConsoleLogger, [{
    key: "error",
    value: function error(message) {
      console.error(message);
    }
  }, {
    key: "warn",
    value: function warn(message) {
      console.warn(message);
    }
  }, {
    key: "info",
    value: function info(message) {
      console.info(message);
    }
  }, {
    key: "log",
    value: function log(message) {
      console.log(message);
    }
  }]);

  return ConsoleLogger;
}();

function _createConnection(input, output, errorHandler, closeHandler) {
  var logger = new ConsoleLogger();
  var connection = vscode_languageserver_protocol_1.createProtocolConnection(input, output, logger);
  connection.onError(function (data) {
    errorHandler(data[0], data[1], data[2]);
  });
  connection.onClose(closeHandler);
  var result = {
    listen: function listen() {
      return connection.listen();
    },
    sendRequest: function sendRequest(type) {
      for (var _len = arguments.length, params = new Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        params[_key - 1] = arguments[_key];
      }

      return connection.sendRequest.apply(connection, [Is.string(type) ? type : type.method].concat(params));
    },
    onRequest: function onRequest(type, handler) {
      return connection.onRequest(Is.string(type) ? type : type.method, handler);
    },
    sendNotification: function sendNotification(type, params) {
      return connection.sendNotification(Is.string(type) ? type : type.method, params);
    },
    onNotification: function onNotification(type, handler) {
      return connection.onNotification(Is.string(type) ? type : type.method, handler);
    },
    onProgress: connection.onProgress,
    sendProgress: connection.sendProgress,
    trace: function trace(value, tracer, sendNotificationOrTraceOptions) {
      var defaultTraceOptions = {
        sendNotification: false,
        traceFormat: vscode_languageserver_protocol_1.TraceFormat.Text
      };

      if (sendNotificationOrTraceOptions === void 0) {
        connection.trace(value, tracer, defaultTraceOptions);
      } else if (Is["boolean"](sendNotificationOrTraceOptions)) {
        connection.trace(value, tracer, sendNotificationOrTraceOptions);
      } else {
        connection.trace(value, tracer, sendNotificationOrTraceOptions);
      }
    },
    initialize: function initialize(params) {
      return connection.sendRequest(vscode_languageserver_protocol_1.InitializeRequest.type, params);
    },
    shutdown: function shutdown() {
      return connection.sendRequest(vscode_languageserver_protocol_1.ShutdownRequest.type, undefined);
    },
    exit: function exit() {
      return connection.sendNotification(vscode_languageserver_protocol_1.ExitNotification.type);
    },
    onLogMessage: function onLogMessage(handler) {
      return connection.onNotification(vscode_languageserver_protocol_1.LogMessageNotification.type, handler);
    },
    onShowMessage: function onShowMessage(handler) {
      return connection.onNotification(vscode_languageserver_protocol_1.ShowMessageNotification.type, handler);
    },
    onTelemetry: function onTelemetry(handler) {
      return connection.onNotification(vscode_languageserver_protocol_1.TelemetryEventNotification.type, handler);
    },
    didChangeConfiguration: function didChangeConfiguration(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidChangeConfigurationNotification.type, params);
    },
    didChangeWatchedFiles: function didChangeWatchedFiles(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidChangeWatchedFilesNotification.type, params);
    },
    didOpenTextDocument: function didOpenTextDocument(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidOpenTextDocumentNotification.type, params);
    },
    didChangeTextDocument: function didChangeTextDocument(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type, params);
    },
    didCloseTextDocument: function didCloseTextDocument(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidCloseTextDocumentNotification.type, params);
    },
    didSaveTextDocument: function didSaveTextDocument(params) {
      return connection.sendNotification(vscode_languageserver_protocol_1.DidSaveTextDocumentNotification.type, params);
    },
    onDiagnostics: function onDiagnostics(handler) {
      return connection.onNotification(vscode_languageserver_protocol_1.PublishDiagnosticsNotification.type, handler);
    },
    dispose: function dispose() {
      return connection.dispose();
    }
  };
  return result;
}
/**
 * An action to be performed when the connection is producing errors.
 */


var ErrorAction;

(function (ErrorAction) {
  /**
   * Continue running the server.
   */
  ErrorAction[ErrorAction["Continue"] = 1] = "Continue";
  /**
   * Shutdown the server.
   */

  ErrorAction[ErrorAction["Shutdown"] = 2] = "Shutdown";
})(ErrorAction = exports.ErrorAction || (exports.ErrorAction = {}));
/**
 * An action to be performed when the connection to a server got closed.
 */


var CloseAction;

(function (CloseAction) {
  /**
   * Don't restart the server. The connection stays closed.
   */
  CloseAction[CloseAction["DoNotRestart"] = 1] = "DoNotRestart";
  /**
   * Restart the server.
   */

  CloseAction[CloseAction["Restart"] = 2] = "Restart";
})(CloseAction = exports.CloseAction || (exports.CloseAction = {}));

var DefaultErrorHandler =
/*#__PURE__*/
function () {
  function DefaultErrorHandler(name) {
    _classCallCheck(this, DefaultErrorHandler);

    this.name = name;
    this.restarts = [];
  }

  _createClass(DefaultErrorHandler, [{
    key: "error",
    value: function error(_error, _message, count) {
      if (count && count <= 3) {
        return ErrorAction.Continue;
      }

      return ErrorAction.Shutdown;
    }
  }, {
    key: "closed",
    value: function closed() {
      this.restarts.push(Date.now());

      if (this.restarts.length < 5) {
        return CloseAction.Restart;
      } else {
        var diff = this.restarts[this.restarts.length - 1] - this.restarts[0];

        if (diff <= 3 * 60 * 1000) {
          vscode_1.window.showErrorMessage("The ".concat(this.name, " server crashed 5 times in the last 3 minutes. The server will not be restarted."));
          return CloseAction.DoNotRestart;
        } else {
          this.restarts.shift();
          return CloseAction.Restart;
        }
      }
    }
  }]);

  return DefaultErrorHandler;
}();

var RevealOutputChannelOn;

(function (RevealOutputChannelOn) {
  RevealOutputChannelOn[RevealOutputChannelOn["Info"] = 1] = "Info";
  RevealOutputChannelOn[RevealOutputChannelOn["Warn"] = 2] = "Warn";
  RevealOutputChannelOn[RevealOutputChannelOn["Error"] = 3] = "Error";
  RevealOutputChannelOn[RevealOutputChannelOn["Never"] = 4] = "Never";
})(RevealOutputChannelOn = exports.RevealOutputChannelOn || (exports.RevealOutputChannelOn = {}));

var State;

(function (State) {
  State[State["Stopped"] = 1] = "Stopped";
  State[State["Starting"] = 3] = "Starting";
  State[State["Running"] = 2] = "Running";
})(State = exports.State || (exports.State = {}));

var ClientState;

(function (ClientState) {
  ClientState[ClientState["Initial"] = 0] = "Initial";
  ClientState[ClientState["Starting"] = 1] = "Starting";
  ClientState[ClientState["StartFailed"] = 2] = "StartFailed";
  ClientState[ClientState["Running"] = 3] = "Running";
  ClientState[ClientState["Stopping"] = 4] = "Stopping";
  ClientState[ClientState["Stopped"] = 5] = "Stopped";
})(ClientState || (ClientState = {}));

var SupportedSymbolKinds = [vscode_languageserver_protocol_1.SymbolKind.File, vscode_languageserver_protocol_1.SymbolKind.Module, vscode_languageserver_protocol_1.SymbolKind.Namespace, vscode_languageserver_protocol_1.SymbolKind.Package, vscode_languageserver_protocol_1.SymbolKind.Class, vscode_languageserver_protocol_1.SymbolKind.Method, vscode_languageserver_protocol_1.SymbolKind.Property, vscode_languageserver_protocol_1.SymbolKind.Field, vscode_languageserver_protocol_1.SymbolKind.Constructor, vscode_languageserver_protocol_1.SymbolKind.Enum, vscode_languageserver_protocol_1.SymbolKind.Interface, vscode_languageserver_protocol_1.SymbolKind.Function, vscode_languageserver_protocol_1.SymbolKind.Variable, vscode_languageserver_protocol_1.SymbolKind.Constant, vscode_languageserver_protocol_1.SymbolKind.String, vscode_languageserver_protocol_1.SymbolKind.Number, vscode_languageserver_protocol_1.SymbolKind.Boolean, vscode_languageserver_protocol_1.SymbolKind.Array, vscode_languageserver_protocol_1.SymbolKind.Object, vscode_languageserver_protocol_1.SymbolKind.Key, vscode_languageserver_protocol_1.SymbolKind.Null, vscode_languageserver_protocol_1.SymbolKind.EnumMember, vscode_languageserver_protocol_1.SymbolKind.Struct, vscode_languageserver_protocol_1.SymbolKind.Event, vscode_languageserver_protocol_1.SymbolKind.Operator, vscode_languageserver_protocol_1.SymbolKind.TypeParameter];
var SupportedCompletionItemKinds = [vscode_languageserver_protocol_1.CompletionItemKind.Text, vscode_languageserver_protocol_1.CompletionItemKind.Method, vscode_languageserver_protocol_1.CompletionItemKind.Function, vscode_languageserver_protocol_1.CompletionItemKind.Constructor, vscode_languageserver_protocol_1.CompletionItemKind.Field, vscode_languageserver_protocol_1.CompletionItemKind.Variable, vscode_languageserver_protocol_1.CompletionItemKind.Class, vscode_languageserver_protocol_1.CompletionItemKind.Interface, vscode_languageserver_protocol_1.CompletionItemKind.Module, vscode_languageserver_protocol_1.CompletionItemKind.Property, vscode_languageserver_protocol_1.CompletionItemKind.Unit, vscode_languageserver_protocol_1.CompletionItemKind.Value, vscode_languageserver_protocol_1.CompletionItemKind.Enum, vscode_languageserver_protocol_1.CompletionItemKind.Keyword, vscode_languageserver_protocol_1.CompletionItemKind.Snippet, vscode_languageserver_protocol_1.CompletionItemKind.Color, vscode_languageserver_protocol_1.CompletionItemKind.File, vscode_languageserver_protocol_1.CompletionItemKind.Reference, vscode_languageserver_protocol_1.CompletionItemKind.Folder, vscode_languageserver_protocol_1.CompletionItemKind.EnumMember, vscode_languageserver_protocol_1.CompletionItemKind.Constant, vscode_languageserver_protocol_1.CompletionItemKind.Struct, vscode_languageserver_protocol_1.CompletionItemKind.Event, vscode_languageserver_protocol_1.CompletionItemKind.Operator, vscode_languageserver_protocol_1.CompletionItemKind.TypeParameter];

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var DynamicFeature;

(function (DynamicFeature) {
  function is(value) {
    var candidate = value;
    return candidate && Is.func(candidate.register) && Is.func(candidate.unregister) && Is.func(candidate.dispose) && candidate.messages !== void 0;
  }

  DynamicFeature.is = is;
})(DynamicFeature || (DynamicFeature = {}));

var DocumentNotifiactions =
/*#__PURE__*/
function () {
  function DocumentNotifiactions(_client, _event, _type, _middleware, _createParams, _selectorFilter) {
    _classCallCheck(this, DocumentNotifiactions);

    this._client = _client;
    this._event = _event;
    this._type = _type;
    this._middleware = _middleware;
    this._createParams = _createParams;
    this._selectorFilter = _selectorFilter;
    this._selectors = new Map();
  }

  _createClass(DocumentNotifiactions, [{
    key: "register",
    value: function register(_message, data) {
      if (!data.registerOptions.documentSelector) {
        return;
      }

      if (!this._listener) {
        this._listener = this._event(this.callback, this);
      }

      this._selectors.set(data.id, data.registerOptions.documentSelector);
    }
  }, {
    key: "callback",
    value: function callback(data) {
      var _this = this;

      if (!this._selectorFilter || this._selectorFilter(this._selectors.values(), data)) {
        if (this._middleware) {
          this._middleware(data, function (data) {
            return _this._client.sendNotification(_this._type, _this._createParams(data));
          });
        } else {
          this._client.sendNotification(this._type, this._createParams(data));
        }

        this.notificationSent(data);
      }
    }
  }, {
    key: "notificationSent",
    value: function notificationSent(_data) {}
  }, {
    key: "unregister",
    value: function unregister(id) {
      this._selectors["delete"](id);

      if (this._selectors.size === 0 && this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._selectors.clear();

      if (this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }], [{
    key: "textDocumentFilter",
    value: function textDocumentFilter(selectors, textDocument) {
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = selectors[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          var selector = _step.value;

          if (vscode_1.languages.match(selector, textDocument)) {
            return true;
          }
        }
      } catch (err) {
        _didIteratorError = true;
        _iteratorError = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion && _iterator["return"] != null) {
            _iterator["return"]();
          }
        } finally {
          if (_didIteratorError) {
            throw _iteratorError;
          }
        }
      }

      return false;
    }
  }]);

  return DocumentNotifiactions;
}();

var DidOpenTextDocumentFeature =
/*#__PURE__*/
function (_DocumentNotifiaction) {
  _inherits(DidOpenTextDocumentFeature, _DocumentNotifiaction);

  function DidOpenTextDocumentFeature(client, _syncedDocuments) {
    var _this2;

    _classCallCheck(this, DidOpenTextDocumentFeature);

    _this2 = _possibleConstructorReturn(this, _getPrototypeOf(DidOpenTextDocumentFeature).call(this, client, vscode_1.workspace.onDidOpenTextDocument, vscode_languageserver_protocol_1.DidOpenTextDocumentNotification.type, client.clientOptions.middleware.didOpen, function (textDocument) {
      return client.code2ProtocolConverter.asOpenTextDocumentParams(textDocument);
    }, DocumentNotifiactions.textDocumentFilter));
    _this2._syncedDocuments = _syncedDocuments;
    return _this2;
  }

  _createClass(DidOpenTextDocumentFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'textDocument'), 'synchronization').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.openClose) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: {
            documentSelector: documentSelector
          }
        });
      }
    }
  }, {
    key: "register",
    value: function register(message, data) {
      var _this3 = this;

      _get(_getPrototypeOf(DidOpenTextDocumentFeature.prototype), "register", this).call(this, message, data);

      if (!data.registerOptions.documentSelector) {
        return;
      }

      var documentSelector = data.registerOptions.documentSelector;
      vscode_1.workspace.textDocuments.forEach(function (textDocument) {
        var uri = textDocument.uri.toString();

        if (_this3._syncedDocuments.has(uri)) {
          return;
        }

        if (vscode_1.languages.match(documentSelector, textDocument)) {
          var middleware = _this3._client.clientOptions.middleware;

          var didOpen = function didOpen(textDocument) {
            _this3._client.sendNotification(_this3._type, _this3._createParams(textDocument));
          };

          if (middleware.didOpen) {
            middleware.didOpen(textDocument, didOpen);
          } else {
            didOpen(textDocument);
          }

          _this3._syncedDocuments.set(uri, textDocument);
        }
      });
    }
  }, {
    key: "notificationSent",
    value: function notificationSent(textDocument) {
      _get(_getPrototypeOf(DidOpenTextDocumentFeature.prototype), "notificationSent", this).call(this, textDocument);

      this._syncedDocuments.set(textDocument.uri.toString(), textDocument);
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidOpenTextDocumentNotification.type;
    }
  }]);

  return DidOpenTextDocumentFeature;
}(DocumentNotifiactions);

var DidCloseTextDocumentFeature =
/*#__PURE__*/
function (_DocumentNotifiaction2) {
  _inherits(DidCloseTextDocumentFeature, _DocumentNotifiaction2);

  function DidCloseTextDocumentFeature(client, _syncedDocuments) {
    var _this4;

    _classCallCheck(this, DidCloseTextDocumentFeature);

    _this4 = _possibleConstructorReturn(this, _getPrototypeOf(DidCloseTextDocumentFeature).call(this, client, vscode_1.workspace.onDidCloseTextDocument, vscode_languageserver_protocol_1.DidCloseTextDocumentNotification.type, client.clientOptions.middleware.didClose, function (textDocument) {
      return client.code2ProtocolConverter.asCloseTextDocumentParams(textDocument);
    }, DocumentNotifiactions.textDocumentFilter));
    _this4._syncedDocuments = _syncedDocuments;
    return _this4;
  }

  _createClass(DidCloseTextDocumentFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'textDocument'), 'synchronization').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.openClose) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: {
            documentSelector: documentSelector
          }
        });
      }
    }
  }, {
    key: "notificationSent",
    value: function notificationSent(textDocument) {
      _get(_getPrototypeOf(DidCloseTextDocumentFeature.prototype), "notificationSent", this).call(this, textDocument);

      this._syncedDocuments["delete"](textDocument.uri.toString());
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var _this5 = this;

      var selector = this._selectors.get(id); // The super call removed the selector from the map
      // of selectors.


      _get(_getPrototypeOf(DidCloseTextDocumentFeature.prototype), "unregister", this).call(this, id);

      var selectors = this._selectors.values();

      this._syncedDocuments.forEach(function (textDocument) {
        if (vscode_1.languages.match(selector, textDocument) && !_this5._selectorFilter(selectors, textDocument)) {
          var middleware = _this5._client.clientOptions.middleware;

          var didClose = function didClose(textDocument) {
            _this5._client.sendNotification(_this5._type, _this5._createParams(textDocument));
          };

          _this5._syncedDocuments["delete"](textDocument.uri.toString());

          if (middleware.didClose) {
            middleware.didClose(textDocument, didClose);
          } else {
            didClose(textDocument);
          }
        }
      });
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidCloseTextDocumentNotification.type;
    }
  }]);

  return DidCloseTextDocumentFeature;
}(DocumentNotifiactions);

var DidChangeTextDocumentFeature =
/*#__PURE__*/
function () {
  function DidChangeTextDocumentFeature(_client) {
    _classCallCheck(this, DidChangeTextDocumentFeature);

    this._client = _client;
    this._changeData = new Map();
    this._forcingDelivery = false;
  }

  _createClass(DidChangeTextDocumentFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'textDocument'), 'synchronization').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.change !== void 0 && textDocumentSyncOptions.change !== vscode_languageserver_protocol_1.TextDocumentSyncKind.None) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: Object.assign({}, {
            documentSelector: documentSelector
          }, {
            syncKind: textDocumentSyncOptions.change
          })
        });
      }
    }
  }, {
    key: "register",
    value: function register(_message, data) {
      if (!data.registerOptions.documentSelector) {
        return;
      }

      if (!this._listener) {
        this._listener = vscode_1.workspace.onDidChangeTextDocument(this.callback, this);
      }

      this._changeData.set(data.id, {
        documentSelector: data.registerOptions.documentSelector,
        syncKind: data.registerOptions.syncKind
      });
    }
  }, {
    key: "callback",
    value: function callback(event) {
      var _this6 = this;

      // Text document changes are send for dirty changes as well. We don't
      // have dirty / undirty events in the LSP so we ignore content changes
      // with length zero.
      if (event.contentChanges.length === 0) {
        return;
      }

      var _iteratorNormalCompletion2 = true;
      var _didIteratorError2 = false;
      var _iteratorError2 = undefined;

      try {
        for (var _iterator2 = this._changeData.values()[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
          var changeData = _step2.value;

          if (vscode_1.languages.match(changeData.documentSelector, event.document)) {
            var middleware = this._client.clientOptions.middleware;

            if (changeData.syncKind === vscode_languageserver_protocol_1.TextDocumentSyncKind.Incremental) {
              (function () {
                var params = _this6._client.code2ProtocolConverter.asChangeTextDocumentParams(event);

                if (middleware.didChange) {
                  middleware.didChange(event, function () {
                    return _this6._client.sendNotification(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type, params);
                  });
                } else {
                  _this6._client.sendNotification(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type, params);
                }
              })();
            } else if (changeData.syncKind === vscode_languageserver_protocol_1.TextDocumentSyncKind.Full) {
              var didChange = function didChange(event) {
                if (_this6._changeDelayer) {
                  if (_this6._changeDelayer.uri !== event.document.uri.toString()) {
                    // Use this force delivery to track boolean state. Otherwise we might call two times.
                    _this6.forceDelivery();

                    _this6._changeDelayer.uri = event.document.uri.toString();
                  }

                  _this6._changeDelayer.delayer.trigger(function () {
                    _this6._client.sendNotification(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type, _this6._client.code2ProtocolConverter.asChangeTextDocumentParams(event.document));
                  });
                } else {
                  _this6._changeDelayer = {
                    uri: event.document.uri.toString(),
                    delayer: new async_1.Delayer(200)
                  };

                  _this6._changeDelayer.delayer.trigger(function () {
                    _this6._client.sendNotification(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type, _this6._client.code2ProtocolConverter.asChangeTextDocumentParams(event.document));
                  }, -1);
                }
              };

              if (middleware.didChange) {
                middleware.didChange(event, didChange);
              } else {
                didChange(event);
              }
            }
          }
        }
      } catch (err) {
        _didIteratorError2 = true;
        _iteratorError2 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion2 && _iterator2["return"] != null) {
            _iterator2["return"]();
          }
        } finally {
          if (_didIteratorError2) {
            throw _iteratorError2;
          }
        }
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      this._changeData["delete"](id);

      if (this._changeData.size === 0 && this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._changeDelayer = undefined;
      this._forcingDelivery = false;

      this._changeData.clear();

      if (this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }, {
    key: "forceDelivery",
    value: function forceDelivery() {
      if (this._forcingDelivery || !this._changeDelayer) {
        return;
      }

      try {
        this._forcingDelivery = true;

        this._changeDelayer.delayer.forceDelivery();
      } finally {
        this._forcingDelivery = false;
      }
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type;
    }
  }]);

  return DidChangeTextDocumentFeature;
}();

var WillSaveFeature =
/*#__PURE__*/
function (_DocumentNotifiaction3) {
  _inherits(WillSaveFeature, _DocumentNotifiaction3);

  function WillSaveFeature(client) {
    _classCallCheck(this, WillSaveFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(WillSaveFeature).call(this, client, vscode_1.workspace.onWillSaveTextDocument, vscode_languageserver_protocol_1.WillSaveTextDocumentNotification.type, client.clientOptions.middleware.willSave, function (willSaveEvent) {
      return client.code2ProtocolConverter.asWillSaveTextDocumentParams(willSaveEvent);
    }, function (selectors, willSaveEvent) {
      return DocumentNotifiactions.textDocumentFilter(selectors, willSaveEvent.document);
    }));
  }

  _createClass(WillSaveFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      var value = ensure(ensure(capabilities, 'textDocument'), 'synchronization');
      value.willSave = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.willSave) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: {
            documentSelector: documentSelector
          }
        });
      }
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.WillSaveTextDocumentNotification.type;
    }
  }]);

  return WillSaveFeature;
}(DocumentNotifiactions);

var WillSaveWaitUntilFeature =
/*#__PURE__*/
function () {
  function WillSaveWaitUntilFeature(_client) {
    _classCallCheck(this, WillSaveWaitUntilFeature);

    this._client = _client;
    this._selectors = new Map();
  }

  _createClass(WillSaveWaitUntilFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      var value = ensure(ensure(capabilities, 'textDocument'), 'synchronization');
      value.willSaveWaitUntil = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.willSaveWaitUntil) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: {
            documentSelector: documentSelector
          }
        });
      }
    }
  }, {
    key: "register",
    value: function register(_message, data) {
      if (!data.registerOptions.documentSelector) {
        return;
      }

      if (!this._listener) {
        this._listener = vscode_1.workspace.onWillSaveTextDocument(this.callback, this);
      }

      this._selectors.set(data.id, data.registerOptions.documentSelector);
    }
  }, {
    key: "callback",
    value: function callback(event) {
      var _this7 = this;

      if (DocumentNotifiactions.textDocumentFilter(this._selectors.values(), event.document)) {
        var middleware = this._client.clientOptions.middleware;

        var willSaveWaitUntil = function willSaveWaitUntil(event) {
          return _this7._client.sendRequest(vscode_languageserver_protocol_1.WillSaveTextDocumentWaitUntilRequest.type, _this7._client.code2ProtocolConverter.asWillSaveTextDocumentParams(event)).then(function (edits) {
            var vEdits = _this7._client.protocol2CodeConverter.asTextEdits(edits);

            return vEdits === void 0 ? [] : vEdits;
          });
        };

        event.waitUntil(middleware.willSaveWaitUntil ? middleware.willSaveWaitUntil(event, willSaveWaitUntil) : willSaveWaitUntil(event));
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      this._selectors["delete"](id);

      if (this._selectors.size === 0 && this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._selectors.clear();

      if (this._listener) {
        this._listener.dispose();

        this._listener = undefined;
      }
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.WillSaveTextDocumentWaitUntilRequest.type;
    }
  }]);

  return WillSaveWaitUntilFeature;
}();

var DidSaveTextDocumentFeature =
/*#__PURE__*/
function (_DocumentNotifiaction4) {
  _inherits(DidSaveTextDocumentFeature, _DocumentNotifiaction4);

  function DidSaveTextDocumentFeature(client) {
    var _this8;

    _classCallCheck(this, DidSaveTextDocumentFeature);

    return _this8 = _possibleConstructorReturn(this, _getPrototypeOf(DidSaveTextDocumentFeature).call(this, client, vscode_1.workspace.onDidSaveTextDocument, vscode_languageserver_protocol_1.DidSaveTextDocumentNotification.type, client.clientOptions.middleware.didSave, function (textDocument) {
      return client.code2ProtocolConverter.asSaveTextDocumentParams(textDocument, _this8._includeText);
    }, DocumentNotifiactions.textDocumentFilter));
  }

  _createClass(DidSaveTextDocumentFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'textDocument'), 'synchronization').didSave = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var textDocumentSyncOptions = capabilities.resolvedTextDocumentSync;

      if (documentSelector && textDocumentSyncOptions && textDocumentSyncOptions.save) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: Object.assign({}, {
            documentSelector: documentSelector
          }, {
            includeText: !!textDocumentSyncOptions.save.includeText
          })
        });
      }
    }
  }, {
    key: "register",
    value: function register(method, data) {
      this._includeText = !!data.registerOptions.includeText;

      _get(_getPrototypeOf(DidSaveTextDocumentFeature.prototype), "register", this).call(this, method, data);
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidSaveTextDocumentNotification.type;
    }
  }]);

  return DidSaveTextDocumentFeature;
}(DocumentNotifiactions);

var FileSystemWatcherFeature =
/*#__PURE__*/
function () {
  function FileSystemWatcherFeature(_client, _notifyFileEvent) {
    _classCallCheck(this, FileSystemWatcherFeature);

    this._client = _client;
    this._notifyFileEvent = _notifyFileEvent;
    this._watchers = new Map();
  }

  _createClass(FileSystemWatcherFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'workspace'), 'didChangeWatchedFiles').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(_capabilities, _documentSelector) {}
  }, {
    key: "register",
    value: function register(_method, data) {
      if (!Array.isArray(data.registerOptions.watchers)) {
        return;
      }

      var disposeables = [];
      var _iteratorNormalCompletion3 = true;
      var _didIteratorError3 = false;
      var _iteratorError3 = undefined;

      try {
        for (var _iterator3 = data.registerOptions.watchers[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
          var watcher = _step3.value;

          if (!Is.string(watcher.globPattern)) {
            continue;
          }

          var watchCreate = true,
              watchChange = true,
              watchDelete = true;

          if (watcher.kind !== void 0 && watcher.kind !== null) {
            watchCreate = (watcher.kind & vscode_languageserver_protocol_1.WatchKind.Create) !== 0;
            watchChange = (watcher.kind & vscode_languageserver_protocol_1.WatchKind.Change) != 0;
            watchDelete = (watcher.kind & vscode_languageserver_protocol_1.WatchKind.Delete) != 0;
          }

          var fileSystemWatcher = vscode_1.workspace.createFileSystemWatcher(watcher.globPattern, !watchCreate, !watchChange, !watchDelete);
          this.hookListeners(fileSystemWatcher, watchCreate, watchChange, watchDelete);
          disposeables.push(fileSystemWatcher);
        }
      } catch (err) {
        _didIteratorError3 = true;
        _iteratorError3 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion3 && _iterator3["return"] != null) {
            _iterator3["return"]();
          }
        } finally {
          if (_didIteratorError3) {
            throw _iteratorError3;
          }
        }
      }

      this._watchers.set(data.id, disposeables);
    }
  }, {
    key: "registerRaw",
    value: function registerRaw(id, fileSystemWatchers) {
      var disposeables = [];
      var _iteratorNormalCompletion4 = true;
      var _didIteratorError4 = false;
      var _iteratorError4 = undefined;

      try {
        for (var _iterator4 = fileSystemWatchers[Symbol.iterator](), _step4; !(_iteratorNormalCompletion4 = (_step4 = _iterator4.next()).done); _iteratorNormalCompletion4 = true) {
          var fileSystemWatcher = _step4.value;
          this.hookListeners(fileSystemWatcher, true, true, true, disposeables);
        }
      } catch (err) {
        _didIteratorError4 = true;
        _iteratorError4 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion4 && _iterator4["return"] != null) {
            _iterator4["return"]();
          }
        } finally {
          if (_didIteratorError4) {
            throw _iteratorError4;
          }
        }
      }

      this._watchers.set(id, disposeables);
    }
  }, {
    key: "hookListeners",
    value: function hookListeners(fileSystemWatcher, watchCreate, watchChange, watchDelete, listeners) {
      var _this9 = this;

      if (watchCreate) {
        fileSystemWatcher.onDidCreate(function (resource) {
          return _this9._notifyFileEvent({
            uri: _this9._client.code2ProtocolConverter.asUri(resource),
            type: vscode_languageserver_protocol_1.FileChangeType.Created
          });
        }, null, listeners);
      }

      if (watchChange) {
        fileSystemWatcher.onDidChange(function (resource) {
          return _this9._notifyFileEvent({
            uri: _this9._client.code2ProtocolConverter.asUri(resource),
            type: vscode_languageserver_protocol_1.FileChangeType.Changed
          });
        }, null, listeners);
      }

      if (watchDelete) {
        fileSystemWatcher.onDidDelete(function (resource) {
          return _this9._notifyFileEvent({
            uri: _this9._client.code2ProtocolConverter.asUri(resource),
            type: vscode_languageserver_protocol_1.FileChangeType.Deleted
          });
        }, null, listeners);
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var disposeables = this._watchers.get(id);

      if (disposeables) {
        var _iteratorNormalCompletion5 = true;
        var _didIteratorError5 = false;
        var _iteratorError5 = undefined;

        try {
          for (var _iterator5 = disposeables[Symbol.iterator](), _step5; !(_iteratorNormalCompletion5 = (_step5 = _iterator5.next()).done); _iteratorNormalCompletion5 = true) {
            var disposable = _step5.value;
            disposable.dispose();
          }
        } catch (err) {
          _didIteratorError5 = true;
          _iteratorError5 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion5 && _iterator5["return"] != null) {
              _iterator5["return"]();
            }
          } finally {
            if (_didIteratorError5) {
              throw _iteratorError5;
            }
          }
        }
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._watchers.forEach(function (disposeables) {
        var _iteratorNormalCompletion6 = true;
        var _didIteratorError6 = false;
        var _iteratorError6 = undefined;

        try {
          for (var _iterator6 = disposeables[Symbol.iterator](), _step6; !(_iteratorNormalCompletion6 = (_step6 = _iterator6.next()).done); _iteratorNormalCompletion6 = true) {
            var disposable = _step6.value;
            disposable.dispose();
          }
        } catch (err) {
          _didIteratorError6 = true;
          _iteratorError6 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion6 && _iterator6["return"] != null) {
              _iterator6["return"]();
            }
          } finally {
            if (_didIteratorError6) {
              throw _iteratorError6;
            }
          }
        }
      });

      this._watchers.clear();
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidChangeWatchedFilesNotification.type;
    }
  }]);

  return FileSystemWatcherFeature;
}();

var TextDocumentFeature =
/*#__PURE__*/
function () {
  function TextDocumentFeature(_client, _message) {
    _classCallCheck(this, TextDocumentFeature);

    this._client = _client;
    this._message = _message;
    this._providers = new Map();
  }

  _createClass(TextDocumentFeature, [{
    key: "register",
    value: function register(message, data) {
      if (message.method !== this.messages.method) {
        throw new Error("Register called on wrong feature. Requested ".concat(message.method, " but reached feature ").concat(this.messages.method));
      }

      if (!data.registerOptions.documentSelector) {
        return;
      }

      var provider = this.registerLanguageProvider(data.registerOptions);

      if (provider) {
        this._providers.set(data.id, provider);
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var provider = this._providers.get(id);

      if (provider) {
        provider.dispose();
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._providers.forEach(function (value) {
        value.dispose();
      });

      this._providers.clear();
    }
  }, {
    key: "getRegistration",
    value: function getRegistration(documentSelector, capability) {
      if (!capability) {
        return [undefined, undefined];
      } else if (vscode_languageserver_protocol_1.TextDocumentRegistrationOptions.is(capability)) {
        var id = vscode_languageserver_protocol_1.StaticRegistrationOptions.hasId(capability) ? capability.id : UUID.generateUuid();
        var selector = capability.documentSelector || documentSelector;

        if (selector) {
          return [id, Object.assign({}, capability, {
            documentSelector: selector
          })];
        }
      } else if (Is["boolean"](capability) && capability === true || vscode_languageserver_protocol_1.WorkDoneProgressOptions.is(capability)) {
        if (!documentSelector) {
          return [undefined, undefined];
        }

        var options = Is["boolean"](capability) && capability === true ? {
          documentSelector: documentSelector
        } : Object.assign({}, capability, {
          documentSelector: documentSelector
        });
        return [UUID.generateUuid(), options];
      }

      return [undefined, undefined];
    }
  }, {
    key: "getRegistrationOptions",
    value: function getRegistrationOptions(documentSelector, capability) {
      if (!documentSelector || !capability) {
        return undefined;
      }

      return Is["boolean"](capability) && capability === true ? {
        documentSelector: documentSelector
      } : Object.assign({}, capability, {
        documentSelector: documentSelector
      });
    }
  }, {
    key: "messages",
    get: function get() {
      return this._message;
    }
  }]);

  return TextDocumentFeature;
}();

exports.TextDocumentFeature = TextDocumentFeature;

var WorkspaceFeature =
/*#__PURE__*/
function () {
  function WorkspaceFeature(_client, _message) {
    _classCallCheck(this, WorkspaceFeature);

    this._client = _client;
    this._message = _message;
    this._providers = new Map();
  }

  _createClass(WorkspaceFeature, [{
    key: "register",
    value: function register(message, data) {
      if (message.method !== this.messages.method) {
        throw new Error("Register called on wron feature. Requested ".concat(message.method, " but reached feature ").concat(this.messages.method));
      }

      var provider = this.registerLanguageProvider(data.registerOptions);

      if (provider) {
        this._providers.set(data.id, provider);
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var provider = this._providers.get(id);

      if (provider) {
        provider.dispose();
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._providers.forEach(function (value) {
        value.dispose();
      });

      this._providers.clear();
    }
  }, {
    key: "messages",
    get: function get() {
      return this._message;
    }
  }]);

  return WorkspaceFeature;
}();

var CompletionItemFeature =
/*#__PURE__*/
function (_TextDocumentFeature) {
  _inherits(CompletionItemFeature, _TextDocumentFeature);

  function CompletionItemFeature(client) {
    _classCallCheck(this, CompletionItemFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(CompletionItemFeature).call(this, client, vscode_languageserver_protocol_1.CompletionRequest.type));
  }

  _createClass(CompletionItemFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var completion = ensure(ensure(capabilites, 'textDocument'), 'completion');
      completion.dynamicRegistration = true;
      completion.contextSupport = true;
      completion.completionItem = {
        snippetSupport: true,
        commitCharactersSupport: true,
        documentationFormat: [vscode_languageserver_protocol_1.MarkupKind.Markdown, vscode_languageserver_protocol_1.MarkupKind.PlainText],
        deprecatedSupport: true,
        preselectSupport: true
      };
      completion.completionItemKind = {
        valueSet: SupportedCompletionItemKinds
      };
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.completionProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var _vscode_1$languages;

      var triggerCharacters = options.triggerCharacters || [];
      var client = this._client;

      var _provideCompletionItems = function provideCompletionItems(document, position, context, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.CompletionRequest.type, client.code2ProtocolConverter.asCompletionParams(document, position, context), token).then(client.protocol2CodeConverter.asCompletionResult, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.CompletionRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var resolveCompletionItem = function resolveCompletionItem(item, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.CompletionResolveRequest.type, client.code2ProtocolConverter.asCompletionItem(item), token).then(client.protocol2CodeConverter.asCompletionItem, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.CompletionResolveRequest.type, error);
          return Promise.resolve(item);
        });
      };

      var middleware = this._client.clientOptions.middleware;
      return (_vscode_1$languages = vscode_1.languages).registerCompletionItemProvider.apply(_vscode_1$languages, [options.documentSelector, {
        provideCompletionItems: function provideCompletionItems(document, position, token, context) {
          return middleware.provideCompletionItem ? middleware.provideCompletionItem(document, position, context, token, _provideCompletionItems) : _provideCompletionItems(document, position, context, token);
        },
        resolveCompletionItem: options.resolveProvider ? function (item, token) {
          return middleware.resolveCompletionItem ? middleware.resolveCompletionItem(item, token, resolveCompletionItem) : resolveCompletionItem(item, token);
        } : undefined
      }].concat(_toConsumableArray(triggerCharacters)));
    }
  }]);

  return CompletionItemFeature;
}(TextDocumentFeature);

var HoverFeature =
/*#__PURE__*/
function (_TextDocumentFeature2) {
  _inherits(HoverFeature, _TextDocumentFeature2);

  function HoverFeature(client) {
    _classCallCheck(this, HoverFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(HoverFeature).call(this, client, vscode_languageserver_protocol_1.HoverRequest.type));
  }

  _createClass(HoverFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var hoverCapability = ensure(ensure(capabilites, 'textDocument'), 'hover');
      hoverCapability.dynamicRegistration = true;
      hoverCapability.contentFormat = [vscode_languageserver_protocol_1.MarkupKind.Markdown, vscode_languageserver_protocol_1.MarkupKind.PlainText];
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.hoverProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideHover = function provideHover(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.HoverRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asHover, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.HoverRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerHoverProvider(options.documentSelector, {
        provideHover: function provideHover(document, position, token) {
          return middleware.provideHover ? middleware.provideHover(document, position, token, _provideHover) : _provideHover(document, position, token);
        }
      });
    }
  }]);

  return HoverFeature;
}(TextDocumentFeature);

var SignatureHelpFeature =
/*#__PURE__*/
function (_TextDocumentFeature3) {
  _inherits(SignatureHelpFeature, _TextDocumentFeature3);

  function SignatureHelpFeature(client) {
    _classCallCheck(this, SignatureHelpFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(SignatureHelpFeature).call(this, client, vscode_languageserver_protocol_1.SignatureHelpRequest.type));
  }

  _createClass(SignatureHelpFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var config = ensure(ensure(capabilites, 'textDocument'), 'signatureHelp');
      config.dynamicRegistration = true;
      config.signatureInformation = {
        documentationFormat: [vscode_languageserver_protocol_1.MarkupKind.Markdown, vscode_languageserver_protocol_1.MarkupKind.PlainText]
      };
      config.signatureInformation.parameterInformation = {
        labelOffsetSupport: true
      };
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.signatureHelpProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var _vscode_1$languages2;

      var client = this._client;

      var providerSignatureHelp = function providerSignatureHelp(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.SignatureHelpRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asSignatureHelp, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.SignatureHelpRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      var triggerCharacters = options.triggerCharacters || [];
      return (_vscode_1$languages2 = vscode_1.languages).registerSignatureHelpProvider.apply(_vscode_1$languages2, [options.documentSelector, {
        provideSignatureHelp: function provideSignatureHelp(document, position, token) {
          return middleware.provideSignatureHelp ? middleware.provideSignatureHelp(document, position, token, providerSignatureHelp) : providerSignatureHelp(document, position, token);
        }
      }].concat(_toConsumableArray(triggerCharacters)));
    }
  }]);

  return SignatureHelpFeature;
}(TextDocumentFeature);

var DefinitionFeature =
/*#__PURE__*/
function (_TextDocumentFeature4) {
  _inherits(DefinitionFeature, _TextDocumentFeature4);

  function DefinitionFeature(client) {
    _classCallCheck(this, DefinitionFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DefinitionFeature).call(this, client, vscode_languageserver_protocol_1.DefinitionRequest.type));
  }

  _createClass(DefinitionFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var definitionSupport = ensure(ensure(capabilites, 'textDocument'), 'definition');
      definitionSupport.dynamicRegistration = true;
      definitionSupport.linkSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.definitionProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDefinition = function provideDefinition(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DefinitionRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asDefinitionResult, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DefinitionRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDefinitionProvider(options.documentSelector, {
        provideDefinition: function provideDefinition(document, position, token) {
          return middleware.provideDefinition ? middleware.provideDefinition(document, position, token, _provideDefinition) : _provideDefinition(document, position, token);
        }
      });
    }
  }]);

  return DefinitionFeature;
}(TextDocumentFeature);

var ReferencesFeature =
/*#__PURE__*/
function (_TextDocumentFeature5) {
  _inherits(ReferencesFeature, _TextDocumentFeature5);

  function ReferencesFeature(client) {
    _classCallCheck(this, ReferencesFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(ReferencesFeature).call(this, client, vscode_languageserver_protocol_1.ReferencesRequest.type));
  }

  _createClass(ReferencesFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'references').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.referencesProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var providerReferences = function providerReferences(document, position, options, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.ReferencesRequest.type, client.code2ProtocolConverter.asReferenceParams(document, position, options), token).then(client.protocol2CodeConverter.asReferences, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.ReferencesRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerReferenceProvider(options.documentSelector, {
        provideReferences: function provideReferences(document, position, options, token) {
          return middleware.provideReferences ? middleware.provideReferences(document, position, options, token, providerReferences) : providerReferences(document, position, options, token);
        }
      });
    }
  }]);

  return ReferencesFeature;
}(TextDocumentFeature);

var DocumentHighlightFeature =
/*#__PURE__*/
function (_TextDocumentFeature6) {
  _inherits(DocumentHighlightFeature, _TextDocumentFeature6);

  function DocumentHighlightFeature(client) {
    _classCallCheck(this, DocumentHighlightFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentHighlightFeature).call(this, client, vscode_languageserver_protocol_1.DocumentHighlightRequest.type));
  }

  _createClass(DocumentHighlightFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'documentHighlight').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentHighlightProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDocumentHighlights = function provideDocumentHighlights(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentHighlightRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asDocumentHighlights, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentHighlightRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDocumentHighlightProvider(options.documentSelector, {
        provideDocumentHighlights: function provideDocumentHighlights(document, position, token) {
          return middleware.provideDocumentHighlights ? middleware.provideDocumentHighlights(document, position, token, _provideDocumentHighlights) : _provideDocumentHighlights(document, position, token);
        }
      });
    }
  }]);

  return DocumentHighlightFeature;
}(TextDocumentFeature);

var DocumentSymbolFeature =
/*#__PURE__*/
function (_TextDocumentFeature7) {
  _inherits(DocumentSymbolFeature, _TextDocumentFeature7);

  function DocumentSymbolFeature(client) {
    _classCallCheck(this, DocumentSymbolFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentSymbolFeature).call(this, client, vscode_languageserver_protocol_1.DocumentSymbolRequest.type));
  }

  _createClass(DocumentSymbolFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var symbolCapabilities = ensure(ensure(capabilites, 'textDocument'), 'documentSymbol');
      symbolCapabilities.dynamicRegistration = true;
      symbolCapabilities.symbolKind = {
        valueSet: SupportedSymbolKinds
      };
      symbolCapabilities.hierarchicalDocumentSymbolSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentSymbolProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDocumentSymbols = function provideDocumentSymbols(document, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentSymbolRequest.type, client.code2ProtocolConverter.asDocumentSymbolParams(document), token).then(function (data) {
          if (data === null) {
            return undefined;
          }

          if (data.length === 0) {
            return [];
          } else {
            var element = data[0];

            if (vscode_languageserver_protocol_1.DocumentSymbol.is(element)) {
              return client.protocol2CodeConverter.asDocumentSymbols(data);
            } else {
              return client.protocol2CodeConverter.asSymbolInformations(data);
            }
          }
        }, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentSymbolRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDocumentSymbolProvider(options.documentSelector, {
        provideDocumentSymbols: function provideDocumentSymbols(document, token) {
          return middleware.provideDocumentSymbols ? middleware.provideDocumentSymbols(document, token, _provideDocumentSymbols) : _provideDocumentSymbols(document, token);
        }
      });
    }
  }]);

  return DocumentSymbolFeature;
}(TextDocumentFeature);

var WorkspaceSymbolFeature =
/*#__PURE__*/
function (_WorkspaceFeature) {
  _inherits(WorkspaceSymbolFeature, _WorkspaceFeature);

  function WorkspaceSymbolFeature(client) {
    _classCallCheck(this, WorkspaceSymbolFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(WorkspaceSymbolFeature).call(this, client, vscode_languageserver_protocol_1.WorkspaceSymbolRequest.type));
  }

  _createClass(WorkspaceSymbolFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var symbolCapabilities = ensure(ensure(capabilites, 'workspace'), 'symbol');
      symbolCapabilities.dynamicRegistration = true;
      symbolCapabilities.symbolKind = {
        valueSet: SupportedSymbolKinds
      };
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities) {
      if (!capabilities.workspaceSymbolProvider) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: capabilities.workspaceSymbolProvider === true ? {
          workDoneProgress: false
        } : capabilities.workspaceSymbolProvider
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(_options) {
      var client = this._client;

      var _provideWorkspaceSymbols = function provideWorkspaceSymbols(query, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.WorkspaceSymbolRequest.type, {
          query: query
        }, token).then(client.protocol2CodeConverter.asSymbolInformations, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.WorkspaceSymbolRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerWorkspaceSymbolProvider({
        provideWorkspaceSymbols: function provideWorkspaceSymbols(query, token) {
          return middleware.provideWorkspaceSymbols ? middleware.provideWorkspaceSymbols(query, token, _provideWorkspaceSymbols) : _provideWorkspaceSymbols(query, token);
        }
      });
    }
  }]);

  return WorkspaceSymbolFeature;
}(WorkspaceFeature);

var CodeActionFeature =
/*#__PURE__*/
function (_TextDocumentFeature8) {
  _inherits(CodeActionFeature, _TextDocumentFeature8);

  function CodeActionFeature(client) {
    _classCallCheck(this, CodeActionFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(CodeActionFeature).call(this, client, vscode_languageserver_protocol_1.CodeActionRequest.type));
  }

  _createClass(CodeActionFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var cap = ensure(ensure(capabilites, 'textDocument'), 'codeAction');
      cap.dynamicRegistration = true;
      cap.codeActionLiteralSupport = {
        codeActionKind: {
          valueSet: [vscode_languageserver_protocol_1.CodeActionKind.Empty, vscode_languageserver_protocol_1.CodeActionKind.QuickFix, vscode_languageserver_protocol_1.CodeActionKind.Refactor, vscode_languageserver_protocol_1.CodeActionKind.RefactorExtract, vscode_languageserver_protocol_1.CodeActionKind.RefactorInline, vscode_languageserver_protocol_1.CodeActionKind.RefactorRewrite, vscode_languageserver_protocol_1.CodeActionKind.Source, vscode_languageserver_protocol_1.CodeActionKind.SourceOrganizeImports]
        }
      };
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.codeActionProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideCodeActions = function provideCodeActions(document, range, context, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          range: client.code2ProtocolConverter.asRange(range),
          context: client.code2ProtocolConverter.asCodeActionContext(context)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.CodeActionRequest.type, params, token).then(function (values) {
          if (values === null) {
            return undefined;
          }

          var result = [];
          var _iteratorNormalCompletion7 = true;
          var _didIteratorError7 = false;
          var _iteratorError7 = undefined;

          try {
            for (var _iterator7 = values[Symbol.iterator](), _step7; !(_iteratorNormalCompletion7 = (_step7 = _iterator7.next()).done); _iteratorNormalCompletion7 = true) {
              var item = _step7.value;

              if (vscode_languageserver_protocol_1.Command.is(item)) {
                result.push(client.protocol2CodeConverter.asCommand(item));
              } else {
                result.push(client.protocol2CodeConverter.asCodeAction(item));
              }

              ;
            }
          } catch (err) {
            _didIteratorError7 = true;
            _iteratorError7 = err;
          } finally {
            try {
              if (!_iteratorNormalCompletion7 && _iterator7["return"] != null) {
                _iterator7["return"]();
              }
            } finally {
              if (_didIteratorError7) {
                throw _iteratorError7;
              }
            }
          }

          return result;
        }, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.CodeActionRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerCodeActionsProvider(options.documentSelector, {
        provideCodeActions: function provideCodeActions(document, range, context, token) {
          return middleware.provideCodeActions ? middleware.provideCodeActions(document, range, context, token, _provideCodeActions) : _provideCodeActions(document, range, context, token);
        }
      }, options.codeActionKinds ? {
        providedCodeActionKinds: client.protocol2CodeConverter.asCodeActionKinds(options.codeActionKinds)
      } : undefined);
    }
  }]);

  return CodeActionFeature;
}(TextDocumentFeature);

var CodeLensFeature =
/*#__PURE__*/
function (_TextDocumentFeature9) {
  _inherits(CodeLensFeature, _TextDocumentFeature9);

  function CodeLensFeature(client) {
    _classCallCheck(this, CodeLensFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(CodeLensFeature).call(this, client, vscode_languageserver_protocol_1.CodeLensRequest.type));
  }

  _createClass(CodeLensFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'codeLens').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.codeLensProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideCodeLenses = function provideCodeLenses(document, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.CodeLensRequest.type, client.code2ProtocolConverter.asCodeLensParams(document), token).then(client.protocol2CodeConverter.asCodeLenses, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.CodeLensRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var resolveCodeLens = function resolveCodeLens(codeLens, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.CodeLensResolveRequest.type, client.code2ProtocolConverter.asCodeLens(codeLens), token).then(client.protocol2CodeConverter.asCodeLens, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.CodeLensResolveRequest.type, error);
          return codeLens;
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerCodeLensProvider(options.documentSelector, {
        provideCodeLenses: function provideCodeLenses(document, token) {
          return middleware.provideCodeLenses ? middleware.provideCodeLenses(document, token, _provideCodeLenses) : _provideCodeLenses(document, token);
        },
        resolveCodeLens: options.resolveProvider ? function (codeLens, token) {
          return middleware.resolveCodeLens ? middleware.resolveCodeLens(codeLens, token, resolveCodeLens) : resolveCodeLens(codeLens, token);
        } : undefined
      });
    }
  }]);

  return CodeLensFeature;
}(TextDocumentFeature);

var DocumentFormattingFeature =
/*#__PURE__*/
function (_TextDocumentFeature10) {
  _inherits(DocumentFormattingFeature, _TextDocumentFeature10);

  function DocumentFormattingFeature(client) {
    _classCallCheck(this, DocumentFormattingFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentFormattingFeature).call(this, client, vscode_languageserver_protocol_1.DocumentFormattingRequest.type));
  }

  _createClass(DocumentFormattingFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'formatting').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentFormattingProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDocumentFormattingEdits = function provideDocumentFormattingEdits(document, options, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          options: client.code2ProtocolConverter.asFormattingOptions(options)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentFormattingRequest.type, params, token).then(client.protocol2CodeConverter.asTextEdits, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentFormattingRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDocumentFormattingEditProvider(options.documentSelector, {
        provideDocumentFormattingEdits: function provideDocumentFormattingEdits(document, options, token) {
          return middleware.provideDocumentFormattingEdits ? middleware.provideDocumentFormattingEdits(document, options, token, _provideDocumentFormattingEdits) : _provideDocumentFormattingEdits(document, options, token);
        }
      });
    }
  }]);

  return DocumentFormattingFeature;
}(TextDocumentFeature);

var DocumentRangeFormattingFeature =
/*#__PURE__*/
function (_TextDocumentFeature11) {
  _inherits(DocumentRangeFormattingFeature, _TextDocumentFeature11);

  function DocumentRangeFormattingFeature(client) {
    _classCallCheck(this, DocumentRangeFormattingFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentRangeFormattingFeature).call(this, client, vscode_languageserver_protocol_1.DocumentRangeFormattingRequest.type));
  }

  _createClass(DocumentRangeFormattingFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'rangeFormatting').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentRangeFormattingProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDocumentRangeFormattingEdits = function provideDocumentRangeFormattingEdits(document, range, options, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          range: client.code2ProtocolConverter.asRange(range),
          options: client.code2ProtocolConverter.asFormattingOptions(options)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentRangeFormattingRequest.type, params, token).then(client.protocol2CodeConverter.asTextEdits, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentRangeFormattingRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDocumentRangeFormattingEditProvider(options.documentSelector, {
        provideDocumentRangeFormattingEdits: function provideDocumentRangeFormattingEdits(document, range, options, token) {
          return middleware.provideDocumentRangeFormattingEdits ? middleware.provideDocumentRangeFormattingEdits(document, range, options, token, _provideDocumentRangeFormattingEdits) : _provideDocumentRangeFormattingEdits(document, range, options, token);
        }
      });
    }
  }]);

  return DocumentRangeFormattingFeature;
}(TextDocumentFeature);

var DocumentOnTypeFormattingFeature =
/*#__PURE__*/
function (_TextDocumentFeature12) {
  _inherits(DocumentOnTypeFormattingFeature, _TextDocumentFeature12);

  function DocumentOnTypeFormattingFeature(client) {
    _classCallCheck(this, DocumentOnTypeFormattingFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentOnTypeFormattingFeature).call(this, client, vscode_languageserver_protocol_1.DocumentOnTypeFormattingRequest.type));
  }

  _createClass(DocumentOnTypeFormattingFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'onTypeFormatting').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentOnTypeFormattingProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var _vscode_1$languages3;

      var client = this._client;
      var moreTriggerCharacter = options.moreTriggerCharacter || [];

      var _provideOnTypeFormattingEdits = function provideOnTypeFormattingEdits(document, position, ch, options, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          position: client.code2ProtocolConverter.asPosition(position),
          ch: ch,
          options: client.code2ProtocolConverter.asFormattingOptions(options)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentOnTypeFormattingRequest.type, params, token).then(client.protocol2CodeConverter.asTextEdits, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentOnTypeFormattingRequest.type, error);
          return Promise.resolve([]);
        });
      };

      var middleware = client.clientOptions.middleware;
      return (_vscode_1$languages3 = vscode_1.languages).registerOnTypeFormattingEditProvider.apply(_vscode_1$languages3, [options.documentSelector, {
        provideOnTypeFormattingEdits: function provideOnTypeFormattingEdits(document, position, ch, options, token) {
          return middleware.provideOnTypeFormattingEdits ? middleware.provideOnTypeFormattingEdits(document, position, ch, options, token, _provideOnTypeFormattingEdits) : _provideOnTypeFormattingEdits(document, position, ch, options, token);
        }
      }, options.firstTriggerCharacter].concat(_toConsumableArray(moreTriggerCharacter)));
    }
  }]);

  return DocumentOnTypeFormattingFeature;
}(TextDocumentFeature);

var RenameFeature =
/*#__PURE__*/
function (_TextDocumentFeature13) {
  _inherits(RenameFeature, _TextDocumentFeature13);

  function RenameFeature(client) {
    _classCallCheck(this, RenameFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(RenameFeature).call(this, client, vscode_languageserver_protocol_1.RenameRequest.type));
  }

  _createClass(RenameFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var rename = ensure(ensure(capabilites, 'textDocument'), 'rename');
      rename.dynamicRegistration = true;
      rename.prepareSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.renameProvider);

      if (!options) {
        return;
      }

      if (Is["boolean"](capabilities.renameProvider)) {
        options.prepareProvider = false;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideRenameEdits = function provideRenameEdits(document, position, newName, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          position: client.code2ProtocolConverter.asPosition(position),
          newName: newName
        };
        return client.sendRequest(vscode_languageserver_protocol_1.RenameRequest.type, params, token).then(client.protocol2CodeConverter.asWorkspaceEdit, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.RenameRequest.type, error);
          return Promise.reject(new Error(error.message));
        });
      };

      var prepareRename = function prepareRename(document, position, token) {
        var params = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document),
          position: client.code2ProtocolConverter.asPosition(position)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.PrepareRenameRequest.type, params, token).then(function (result) {
          if (vscode_languageserver_protocol_1.Range.is(result)) {
            return client.protocol2CodeConverter.asRange(result);
          } else if (result && vscode_languageserver_protocol_1.Range.is(result.range)) {
            return {
              range: client.protocol2CodeConverter.asRange(result.range),
              placeholder: result.placeholder
            };
          } // To cancel the rename vscode API expects a rejected promise.


          return Promise.reject(new Error("The element can't be renamed."));
        }, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.PrepareRenameRequest.type, error);
          return Promise.reject(new Error(error.message));
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerRenameProvider(options.documentSelector, {
        provideRenameEdits: function provideRenameEdits(document, position, newName, token) {
          return middleware.provideRenameEdits ? middleware.provideRenameEdits(document, position, newName, token, _provideRenameEdits) : _provideRenameEdits(document, position, newName, token);
        },
        prepareRename: options.prepareProvider ? function (document, position, token) {
          return middleware.prepareRename ? middleware.prepareRename(document, position, token, prepareRename) : prepareRename(document, position, token);
        } : undefined
      });
    }
  }]);

  return RenameFeature;
}(TextDocumentFeature);

var DocumentLinkFeature =
/*#__PURE__*/
function (_TextDocumentFeature14) {
  _inherits(DocumentLinkFeature, _TextDocumentFeature14);

  function DocumentLinkFeature(client) {
    _classCallCheck(this, DocumentLinkFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DocumentLinkFeature).call(this, client, vscode_languageserver_protocol_1.DocumentLinkRequest.type));
  }

  _createClass(DocumentLinkFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'documentLink').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var options = this.getRegistrationOptions(documentSelector, capabilities.documentLinkProvider);

      if (!options) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDocumentLinks = function provideDocumentLinks(document, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentLinkRequest.type, client.code2ProtocolConverter.asDocumentLinkParams(document), token).then(client.protocol2CodeConverter.asDocumentLinks, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentLinkRequest.type, error);
          Promise.resolve(new Error(error.message));
        });
      };

      var resolveDocumentLink = function resolveDocumentLink(link, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentLinkResolveRequest.type, client.code2ProtocolConverter.asDocumentLink(link), token).then(client.protocol2CodeConverter.asDocumentLink, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DocumentLinkResolveRequest.type, error);
          Promise.resolve(new Error(error.message));
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDocumentLinkProvider(options.documentSelector, {
        provideDocumentLinks: function provideDocumentLinks(document, token) {
          return middleware.provideDocumentLinks ? middleware.provideDocumentLinks(document, token, _provideDocumentLinks) : _provideDocumentLinks(document, token);
        },
        resolveDocumentLink: options.resolveProvider ? function (link, token) {
          return middleware.resolveDocumentLink ? middleware.resolveDocumentLink(link, token, resolveDocumentLink) : resolveDocumentLink(link, token);
        } : undefined
      });
    }
  }]);

  return DocumentLinkFeature;
}(TextDocumentFeature);

var ConfigurationFeature =
/*#__PURE__*/
function () {
  function ConfigurationFeature(_client) {
    _classCallCheck(this, ConfigurationFeature);

    this._client = _client;
    this._listeners = new Map();
  }

  _createClass(ConfigurationFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'workspace'), 'didChangeConfiguration').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize() {
      var section = this._client.clientOptions.synchronize.configurationSection;

      if (section !== void 0) {
        this.register(this.messages, {
          id: UUID.generateUuid(),
          registerOptions: {
            section: section
          }
        });
      }
    }
  }, {
    key: "register",
    value: function register(_message, data) {
      var _this10 = this;

      var disposable = vscode_1.workspace.onDidChangeConfiguration(function (event) {
        _this10.onDidChangeConfiguration(data.registerOptions.section, event);
      });

      this._listeners.set(data.id, disposable);

      if (data.registerOptions.section !== void 0) {
        this.onDidChangeConfiguration(data.registerOptions.section, undefined);
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var disposable = this._listeners.get(id);

      if (disposable) {
        this._listeners["delete"](id);

        disposable.dispose();
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      var _iteratorNormalCompletion8 = true;
      var _didIteratorError8 = false;
      var _iteratorError8 = undefined;

      try {
        for (var _iterator8 = this._listeners.values()[Symbol.iterator](), _step8; !(_iteratorNormalCompletion8 = (_step8 = _iterator8.next()).done); _iteratorNormalCompletion8 = true) {
          var disposable = _step8.value;
          disposable.dispose();
        }
      } catch (err) {
        _didIteratorError8 = true;
        _iteratorError8 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion8 && _iterator8["return"] != null) {
            _iterator8["return"]();
          }
        } finally {
          if (_didIteratorError8) {
            throw _iteratorError8;
          }
        }
      }

      this._listeners.clear();
    }
  }, {
    key: "onDidChangeConfiguration",
    value: function onDidChangeConfiguration(configurationSection, event) {
      var _this11 = this;

      var sections;

      if (Is.string(configurationSection)) {
        sections = [configurationSection];
      } else {
        sections = configurationSection;
      }

      if (sections !== void 0 && event !== void 0) {
        var affected = sections.some(function (section) {
          return event.affectsConfiguration(section);
        });

        if (!affected) {
          return;
        }
      }

      var didChangeConfiguration = function didChangeConfiguration(sections) {
        if (sections === void 0) {
          _this11._client.sendNotification(vscode_languageserver_protocol_1.DidChangeConfigurationNotification.type, {
            settings: null
          });

          return;
        }

        _this11._client.sendNotification(vscode_languageserver_protocol_1.DidChangeConfigurationNotification.type, {
          settings: _this11.extractSettingsInformation(sections)
        });
      };

      var middleware = this.getMiddleware();
      middleware ? middleware(sections, didChangeConfiguration) : didChangeConfiguration(sections);
    }
  }, {
    key: "extractSettingsInformation",
    value: function extractSettingsInformation(keys) {
      function ensurePath(config, path) {
        var current = config;

        for (var i = 0; i < path.length - 1; i++) {
          var obj = current[path[i]];

          if (!obj) {
            obj = Object.create(null);
            current[path[i]] = obj;
          }

          current = obj;
        }

        return current;
      }

      var resource = this._client.clientOptions.workspaceFolder ? this._client.clientOptions.workspaceFolder.uri : undefined;
      var result = Object.create(null);

      for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        var index = key.indexOf('.');
        var config = null;

        if (index >= 0) {
          config = vscode_1.workspace.getConfiguration(key.substr(0, index), resource).get(key.substr(index + 1));
        } else {
          config = vscode_1.workspace.getConfiguration(key, resource);
        }

        if (config) {
          var path = keys[i].split('.');
          ensurePath(result, path)[path[path.length - 1]] = config;
        }
      }

      return result;
    }
  }, {
    key: "getMiddleware",
    value: function getMiddleware() {
      var middleware = this._client.clientOptions.middleware;

      if (middleware.workspace && middleware.workspace.didChangeConfiguration) {
        return middleware.workspace.didChangeConfiguration;
      } else {
        return undefined;
      }
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidChangeConfigurationNotification.type;
    }
  }]);

  return ConfigurationFeature;
}();

var ExecuteCommandFeature =
/*#__PURE__*/
function () {
  function ExecuteCommandFeature(_client) {
    _classCallCheck(this, ExecuteCommandFeature);

    this._client = _client;
    this._commands = new Map();
  }

  _createClass(ExecuteCommandFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      ensure(ensure(capabilities, 'workspace'), 'executeCommand').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities) {
      if (!capabilities.executeCommandProvider) {
        return;
      }

      this.register(this.messages, {
        id: UUID.generateUuid(),
        registerOptions: Object.assign({}, capabilities.executeCommandProvider)
      });
    }
  }, {
    key: "register",
    value: function register(_message, data) {
      var client = this._client;

      if (data.registerOptions.commands) {
        var disposeables = [];
        var _iteratorNormalCompletion9 = true;
        var _didIteratorError9 = false;
        var _iteratorError9 = undefined;

        try {
          var _loop = function _loop() {
            var command = _step9.value;
            disposeables.push(vscode_1.commands.registerCommand(command, function () {
              for (var _len2 = arguments.length, args = new Array(_len2), _key2 = 0; _key2 < _len2; _key2++) {
                args[_key2] = arguments[_key2];
              }

              var params = {
                command: command,
                arguments: args
              };
              return client.sendRequest(vscode_languageserver_protocol_1.ExecuteCommandRequest.type, params).then(undefined, function (error) {
                client.logFailedRequest(vscode_languageserver_protocol_1.ExecuteCommandRequest.type, error);
              });
            }));
          };

          for (var _iterator9 = data.registerOptions.commands[Symbol.iterator](), _step9; !(_iteratorNormalCompletion9 = (_step9 = _iterator9.next()).done); _iteratorNormalCompletion9 = true) {
            _loop();
          }
        } catch (err) {
          _didIteratorError9 = true;
          _iteratorError9 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion9 && _iterator9["return"] != null) {
              _iterator9["return"]();
            }
          } finally {
            if (_didIteratorError9) {
              throw _iteratorError9;
            }
          }
        }

        this._commands.set(data.id, disposeables);
      }
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var disposeables = this._commands.get(id);

      if (disposeables) {
        disposeables.forEach(function (disposable) {
          return disposable.dispose();
        });
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._commands.forEach(function (value) {
        value.forEach(function (disposable) {
          return disposable.dispose();
        });
      });

      this._commands.clear();
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.ExecuteCommandRequest.type;
    }
  }]);

  return ExecuteCommandFeature;
}();

var MessageTransports;

(function (MessageTransports) {
  function is(value) {
    var candidate = value;
    return candidate && vscode_languageserver_protocol_1.MessageReader.is(value.reader) && vscode_languageserver_protocol_1.MessageWriter.is(value.writer);
  }

  MessageTransports.is = is;
})(MessageTransports = exports.MessageTransports || (exports.MessageTransports = {}));

var OnReady =
/*#__PURE__*/
function () {
  function OnReady(_resolve, _reject) {
    _classCallCheck(this, OnReady);

    this._resolve = _resolve;
    this._reject = _reject;
    this._used = false;
  }

  _createClass(OnReady, [{
    key: "resolve",
    value: function resolve() {
      this._used = true;

      this._resolve();
    }
  }, {
    key: "reject",
    value: function reject(error) {
      this._used = true;

      this._reject(error);
    }
  }, {
    key: "isUsed",
    get: function get() {
      return this._used;
    }
  }]);

  return OnReady;
}();

var BaseLanguageClient =
/*#__PURE__*/
function () {
  function BaseLanguageClient(id, name, clientOptions) {
    var _this12 = this;

    _classCallCheck(this, BaseLanguageClient);

    this._traceFormat = vscode_languageserver_protocol_1.TraceFormat.Text;
    this._features = [];
    this._method2Message = new Map();
    this._dynamicFeatures = new Map();
    this._id = id;
    this._name = name;
    clientOptions = clientOptions || {};
    this._clientOptions = {
      documentSelector: clientOptions.documentSelector || [],
      synchronize: clientOptions.synchronize || {},
      diagnosticCollectionName: clientOptions.diagnosticCollectionName,
      outputChannelName: clientOptions.outputChannelName || this._name,
      revealOutputChannelOn: clientOptions.revealOutputChannelOn || RevealOutputChannelOn.Error,
      stdioEncoding: clientOptions.stdioEncoding || 'utf8',
      initializationOptions: clientOptions.initializationOptions,
      initializationFailedHandler: clientOptions.initializationFailedHandler,
      progressOnInitialization: !!clientOptions.progressOnInitialization,
      errorHandler: clientOptions.errorHandler || new DefaultErrorHandler(this._name),
      middleware: clientOptions.middleware || {},
      uriConverters: clientOptions.uriConverters,
      workspaceFolder: clientOptions.workspaceFolder
    };
    this._clientOptions.synchronize = this._clientOptions.synchronize || {};
    this.state = ClientState.Initial;
    this._connectionPromise = undefined;
    this._resolvedConnection = undefined;
    this._initializeResult = undefined;

    if (clientOptions.outputChannel) {
      this._outputChannel = clientOptions.outputChannel;
      this._disposeOutputChannel = false;
    } else {
      this._outputChannel = undefined;
      this._disposeOutputChannel = true;
    }

    this._traceOutputChannel = clientOptions.traceOutputChannel;
    this._listeners = undefined;
    this._providers = undefined;
    this._diagnostics = undefined;
    this._fileEvents = [];
    this._fileEventDelayer = new async_1.Delayer(250);
    this._onReady = new Promise(function (resolve, reject) {
      _this12._onReadyCallbacks = new OnReady(resolve, reject);
    });
    this._onStop = undefined;
    this._telemetryEmitter = new vscode_languageserver_protocol_1.Emitter();
    this._stateChangeEmitter = new vscode_languageserver_protocol_1.Emitter();
    this._tracer = {
      log: function log(messageOrDataObject, data) {
        if (Is.string(messageOrDataObject)) {
          _this12.logTrace(messageOrDataObject, data);
        } else {
          _this12.logObjectTrace(messageOrDataObject);
        }
      }
    };
    this._c2p = c2p.createConverter(clientOptions.uriConverters ? clientOptions.uriConverters.code2Protocol : undefined);
    this._p2c = p2c.createConverter(clientOptions.uriConverters ? clientOptions.uriConverters.protocol2Code : undefined);
    this._syncedDocuments = new Map();
    this.registerBuiltinFeatures();
  }

  _createClass(BaseLanguageClient, [{
    key: "getPublicState",
    value: function getPublicState() {
      if (this.state === ClientState.Running) {
        return State.Running;
      } else if (this.state === ClientState.Starting) {
        return State.Starting;
      } else {
        return State.Stopped;
      }
    }
  }, {
    key: "sendRequest",
    value: function sendRequest(type) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      this.forceDocumentSync();

      try {
        var _this$_resolvedConnec;

        for (var _len3 = arguments.length, params = new Array(_len3 > 1 ? _len3 - 1 : 0), _key3 = 1; _key3 < _len3; _key3++) {
          params[_key3 - 1] = arguments[_key3];
        }

        return (_this$_resolvedConnec = this._resolvedConnection).sendRequest.apply(_this$_resolvedConnec, [type].concat(params));
      } catch (error) {
        this.error("Sending request ".concat(Is.string(type) ? type : type.method, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "onRequest",
    value: function onRequest(type, handler) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      try {
        this._resolvedConnection.onRequest(type, handler);
      } catch (error) {
        this.error("Registering request handler ".concat(Is.string(type) ? type : type.method, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "sendNotification",
    value: function sendNotification(type, params) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      this.forceDocumentSync();

      try {
        this._resolvedConnection.sendNotification(type, params);
      } catch (error) {
        this.error("Sending notification ".concat(Is.string(type) ? type : type.method, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "onNotification",
    value: function onNotification(type, handler) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      try {
        this._resolvedConnection.onNotification(type, handler);
      } catch (error) {
        this.error("Registering notification handler ".concat(Is.string(type) ? type : type.method, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "onProgress",
    value: function onProgress(type, token, handler) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      try {
        return this._resolvedConnection.onProgress(type, token, handler);
      } catch (error) {
        this.error("Registering progress handler for token ".concat(token, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "sendProgress",
    value: function sendProgress(type, token, value) {
      if (!this.isConnectionActive()) {
        throw new Error('Language client is not ready yet');
      }

      this.forceDocumentSync();

      try {
        this._resolvedConnection.sendProgress(type, token, value);
      } catch (error) {
        this.error("Sending progress for token ".concat(token, " failed."), error);
        throw error;
      }
    }
  }, {
    key: "createDefaultErrorHandler",
    value: function createDefaultErrorHandler() {
      return new DefaultErrorHandler(this._name);
    }
  }, {
    key: "data2String",
    value: function data2String(data) {
      if (data instanceof vscode_languageserver_protocol_1.ResponseError) {
        var responseError = data;
        return "  Message: ".concat(responseError.message, "\n  Code: ").concat(responseError.code, " ").concat(responseError.data ? '\n' + responseError.data.toString() : '');
      }

      if (data instanceof Error) {
        if (Is.string(data.stack)) {
          return data.stack;
        }

        return data.message;
      }

      if (Is.string(data)) {
        return data;
      }

      return data.toString();
    }
  }, {
    key: "info",
    value: function info(message, data) {
      this.outputChannel.appendLine("[Info  - ".concat(new Date().toLocaleTimeString(), "] ").concat(message));

      if (data) {
        this.outputChannel.appendLine(this.data2String(data));
      }

      if (this._clientOptions.revealOutputChannelOn <= RevealOutputChannelOn.Info) {
        this.showNotificationMessage();
      }
    }
  }, {
    key: "warn",
    value: function warn(message, data) {
      this.outputChannel.appendLine("[Warn  - ".concat(new Date().toLocaleTimeString(), "] ").concat(message));

      if (data) {
        this.outputChannel.appendLine(this.data2String(data));
      }

      if (this._clientOptions.revealOutputChannelOn <= RevealOutputChannelOn.Warn) {
        this.showNotificationMessage();
      }
    }
  }, {
    key: "error",
    value: function error(message, data) {
      this.outputChannel.appendLine("[Error - ".concat(new Date().toLocaleTimeString(), "] ").concat(message));

      if (data) {
        this.outputChannel.appendLine(this.data2String(data));
      }

      if (this._clientOptions.revealOutputChannelOn <= RevealOutputChannelOn.Error) {
        this.showNotificationMessage();
      }
    }
  }, {
    key: "showNotificationMessage",
    value: function showNotificationMessage() {
      var _this13 = this;

      vscode_1.window.showInformationMessage('A request has failed. See the output for more information.', 'Go to output').then(function () {
        _this13.outputChannel.show(true);
      });
    }
  }, {
    key: "logTrace",
    value: function logTrace(message, data) {
      this.traceOutputChannel.appendLine("[Trace - ".concat(new Date().toLocaleTimeString(), "] ").concat(message));

      if (data) {
        this.traceOutputChannel.appendLine(this.data2String(data));
      }
    }
  }, {
    key: "logObjectTrace",
    value: function logObjectTrace(data) {
      if (data.isLSPMessage && data.type) {
        this.traceOutputChannel.append("[LSP   - ".concat(new Date().toLocaleTimeString(), "] "));
      } else {
        this.traceOutputChannel.append("[Trace - ".concat(new Date().toLocaleTimeString(), "] "));
      }

      if (data) {
        this.traceOutputChannel.appendLine("".concat(JSON.stringify(data)));
      }
    }
  }, {
    key: "needsStart",
    value: function needsStart() {
      return this.state === ClientState.Initial || this.state === ClientState.Stopping || this.state === ClientState.Stopped;
    }
  }, {
    key: "needsStop",
    value: function needsStop() {
      return this.state === ClientState.Starting || this.state === ClientState.Running;
    }
  }, {
    key: "onReady",
    value: function onReady() {
      return this._onReady;
    }
  }, {
    key: "isConnectionActive",
    value: function isConnectionActive() {
      return this.state === ClientState.Running && !!this._resolvedConnection;
    }
  }, {
    key: "start",
    value: function start() {
      var _this14 = this;

      if (this._onReadyCallbacks.isUsed) {
        this._onReady = new Promise(function (resolve, reject) {
          _this14._onReadyCallbacks = new OnReady(resolve, reject);
        });
      }

      this._listeners = [];
      this._providers = []; // If we restart then the diagnostics collection is reused.

      if (!this._diagnostics) {
        this._diagnostics = this._clientOptions.diagnosticCollectionName ? vscode_1.languages.createDiagnosticCollection(this._clientOptions.diagnosticCollectionName) : vscode_1.languages.createDiagnosticCollection();
      }

      this.state = ClientState.Starting;
      this.resolveConnection().then(function (connection) {
        connection.onLogMessage(function (message) {
          switch (message.type) {
            case vscode_languageserver_protocol_1.MessageType.Error:
              _this14.error(message.message);

              break;

            case vscode_languageserver_protocol_1.MessageType.Warning:
              _this14.warn(message.message);

              break;

            case vscode_languageserver_protocol_1.MessageType.Info:
              _this14.info(message.message);

              break;

            default:
              _this14.outputChannel.appendLine(message.message);

          }
        });
        connection.onShowMessage(function (message) {
          switch (message.type) {
            case vscode_languageserver_protocol_1.MessageType.Error:
              vscode_1.window.showErrorMessage(message.message);
              break;

            case vscode_languageserver_protocol_1.MessageType.Warning:
              vscode_1.window.showWarningMessage(message.message);
              break;

            case vscode_languageserver_protocol_1.MessageType.Info:
              vscode_1.window.showInformationMessage(message.message);
              break;

            default:
              vscode_1.window.showInformationMessage(message.message);
          }
        });
        connection.onRequest(vscode_languageserver_protocol_1.ShowMessageRequest.type, function (params) {
          var messageFunc;

          switch (params.type) {
            case vscode_languageserver_protocol_1.MessageType.Error:
              messageFunc = vscode_1.window.showErrorMessage;
              break;

            case vscode_languageserver_protocol_1.MessageType.Warning:
              messageFunc = vscode_1.window.showWarningMessage;
              break;

            case vscode_languageserver_protocol_1.MessageType.Info:
              messageFunc = vscode_1.window.showInformationMessage;
              break;

            default:
              messageFunc = vscode_1.window.showInformationMessage;
          }

          var actions = params.actions || [];
          return messageFunc.apply(void 0, [params.message].concat(_toConsumableArray(actions)));
        });
        connection.onTelemetry(function (data) {
          _this14._telemetryEmitter.fire(data);
        });
        connection.listen(); // Error is handled in the initialize call.

        return _this14.initialize(connection);
      }).then(undefined, function (error) {
        _this14.state = ClientState.StartFailed;

        _this14._onReadyCallbacks.reject(error);

        _this14.error('Starting client failed', error);

        vscode_1.window.showErrorMessage("Couldn't start client ".concat(_this14._name));
      });
      return new vscode_1.Disposable(function () {
        if (_this14.needsStop()) {
          _this14.stop();
        }
      });
    }
  }, {
    key: "resolveConnection",
    value: function resolveConnection() {
      if (!this._connectionPromise) {
        this._connectionPromise = this.createConnection();
      }

      return this._connectionPromise;
    }
  }, {
    key: "initialize",
    value: function initialize(connection) {
      this.refreshTrace(connection, false);
      var initOption = this._clientOptions.initializationOptions;
      var rootPath = this._clientOptions.workspaceFolder ? this._clientOptions.workspaceFolder.uri.fsPath : this._clientGetRootPath();
      var initParams = {
        processId: process.pid,
        rootPath: rootPath ? rootPath : null,
        rootUri: rootPath ? this._c2p.asUri(vscode_1.Uri.file(rootPath)) : null,
        capabilities: this.computeClientCapabilities(),
        initializationOptions: Is.func(initOption) ? initOption() : initOption,
        trace: vscode_languageserver_protocol_1.Trace.toString(this._trace),
        workspaceFolders: null
      };
      this.fillInitializeParams(initParams);

      if (this._clientOptions.progressOnInitialization) {
        var token = UUID.generateUuid();
        var part = new progressPart_1.ProgressPart(connection, token);
        initParams.workDoneToken = token;
        return this.doInitialize(connection, initParams).then(function (result) {
          part.done();
          return result;
        }, function (error) {
          part.cancel();
          throw error;
        });
      } else {
        return this.doInitialize(connection, initParams);
      }
    }
  }, {
    key: "doInitialize",
    value: function doInitialize(connection, initParams) {
      var _this15 = this;

      return connection.initialize(initParams).then(function (result) {
        _this15._resolvedConnection = connection;
        _this15._initializeResult = result;
        _this15.state = ClientState.Running;
        var textDocumentSyncOptions = undefined;

        if (Is.number(result.capabilities.textDocumentSync)) {
          if (result.capabilities.textDocumentSync === vscode_languageserver_protocol_1.TextDocumentSyncKind.None) {
            textDocumentSyncOptions = {
              openClose: false,
              change: vscode_languageserver_protocol_1.TextDocumentSyncKind.None,
              save: undefined
            };
          } else {
            textDocumentSyncOptions = {
              openClose: true,
              change: result.capabilities.textDocumentSync,
              save: {
                includeText: false
              }
            };
          }
        } else if (result.capabilities.textDocumentSync !== void 0 && result.capabilities.textDocumentSync !== null) {
          textDocumentSyncOptions = result.capabilities.textDocumentSync;
        }

        _this15._capabilities = Object.assign({}, result.capabilities, {
          resolvedTextDocumentSync: textDocumentSyncOptions
        });
        connection.onDiagnostics(function (params) {
          return _this15.handleDiagnostics(params);
        });
        connection.onRequest(vscode_languageserver_protocol_1.RegistrationRequest.type, function (params) {
          return _this15.handleRegistrationRequest(params);
        }); // See https://github.com/Microsoft/vscode-languageserver-node/issues/199

        connection.onRequest('client/registerFeature', function (params) {
          return _this15.handleRegistrationRequest(params);
        });
        connection.onRequest(vscode_languageserver_protocol_1.UnregistrationRequest.type, function (params) {
          return _this15.handleUnregistrationRequest(params);
        }); // See https://github.com/Microsoft/vscode-languageserver-node/issues/199

        connection.onRequest('client/unregisterFeature', function (params) {
          return _this15.handleUnregistrationRequest(params);
        });
        connection.onRequest(vscode_languageserver_protocol_1.ApplyWorkspaceEditRequest.type, function (params) {
          return _this15.handleApplyWorkspaceEdit(params);
        });
        connection.sendNotification(vscode_languageserver_protocol_1.InitializedNotification.type, {});

        _this15.hookFileEvents(connection);

        _this15.hookConfigurationChanged(connection);

        _this15.initializeFeatures(connection);

        _this15._onReadyCallbacks.resolve();

        return result;
      }).then(undefined, function (error) {
        if (_this15._clientOptions.initializationFailedHandler) {
          if (_this15._clientOptions.initializationFailedHandler(error)) {
            _this15.initialize(connection);
          } else {
            _this15.stop();

            _this15._onReadyCallbacks.reject(error);
          }
        } else if (error instanceof vscode_languageserver_protocol_1.ResponseError && error.data && error.data.retry) {
          vscode_1.window.showErrorMessage(error.message, {
            title: 'Retry',
            id: "retry"
          }).then(function (item) {
            if (item && item.id === 'retry') {
              _this15.initialize(connection);
            } else {
              _this15.stop();

              _this15._onReadyCallbacks.reject(error);
            }
          });
        } else {
          if (error && error.message) {
            vscode_1.window.showErrorMessage(error.message);
          }

          _this15.error('Server initialization failed.', error);

          _this15.stop();

          _this15._onReadyCallbacks.reject(error);
        }
      });
    }
  }, {
    key: "_clientGetRootPath",
    value: function _clientGetRootPath() {
      var folders = vscode_1.workspace.workspaceFolders;

      if (!folders || folders.length === 0) {
        return undefined;
      }

      var folder = folders[0];

      if (folder.uri.scheme === 'file') {
        return folder.uri.fsPath;
      }

      return undefined;
    }
  }, {
    key: "stop",
    value: function stop() {
      var _this16 = this;

      this._initializeResult = undefined;

      if (!this._connectionPromise) {
        this.state = ClientState.Stopped;
        return Promise.resolve();
      }

      if (this.state === ClientState.Stopping && this._onStop) {
        return this._onStop;
      }

      this.state = ClientState.Stopping;
      this.cleanUp(false); // unhook listeners

      return this._onStop = this.resolveConnection().then(function (connection) {
        return connection.shutdown().then(function () {
          connection.exit();
          connection.dispose();
          _this16.state = ClientState.Stopped;

          _this16.cleanUpChannel();

          _this16._onStop = undefined;
          _this16._connectionPromise = undefined;
          _this16._resolvedConnection = undefined;
        });
      });
    }
  }, {
    key: "cleanUp",
    value: function cleanUp() {
      var channel = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : true;
      var diagnostics = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : true;

      if (this._listeners) {
        this._listeners.forEach(function (listener) {
          return listener.dispose();
        });

        this._listeners = undefined;
      }

      if (this._providers) {
        this._providers.forEach(function (provider) {
          return provider.dispose();
        });

        this._providers = undefined;
      }

      if (this._syncedDocuments) {
        this._syncedDocuments.clear();
      }

      var _iteratorNormalCompletion10 = true;
      var _didIteratorError10 = false;
      var _iteratorError10 = undefined;

      try {
        for (var _iterator10 = this._dynamicFeatures.values()[Symbol.iterator](), _step10; !(_iteratorNormalCompletion10 = (_step10 = _iterator10.next()).done); _iteratorNormalCompletion10 = true) {
          var handler = _step10.value;
          handler.dispose();
        }
      } catch (err) {
        _didIteratorError10 = true;
        _iteratorError10 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion10 && _iterator10["return"] != null) {
            _iterator10["return"]();
          }
        } finally {
          if (_didIteratorError10) {
            throw _iteratorError10;
          }
        }
      }

      if (channel) {
        this.cleanUpChannel();
      }

      if (diagnostics && this._diagnostics) {
        this._diagnostics.dispose();

        this._diagnostics = undefined;
      }
    }
  }, {
    key: "cleanUpChannel",
    value: function cleanUpChannel() {
      if (this._outputChannel && this._disposeOutputChannel) {
        this._outputChannel.dispose();

        this._outputChannel = undefined;
      }
    }
  }, {
    key: "notifyFileEvent",
    value: function notifyFileEvent(event) {
      var _this17 = this;

      this._fileEvents.push(event);

      this._fileEventDelayer.trigger(function () {
        _this17.onReady().then(function () {
          _this17.resolveConnection().then(function (connection) {
            if (_this17.isConnectionActive()) {
              _this17.forceDocumentSync();

              connection.didChangeWatchedFiles({
                changes: _this17._fileEvents
              });
            }

            _this17._fileEvents = [];
          });
        }, function (error) {
          _this17.error("Notify file events failed.", error);
        });
      });
    }
  }, {
    key: "forceDocumentSync",
    value: function forceDocumentSync() {
      this._dynamicFeatures.get(vscode_languageserver_protocol_1.DidChangeTextDocumentNotification.type.method).forceDelivery();
    }
  }, {
    key: "handleDiagnostics",
    value: function handleDiagnostics(params) {
      var _this18 = this;

      if (!this._diagnostics) {
        return;
      }

      var uri = this._p2c.asUri(params.uri);

      var diagnostics = this._p2c.asDiagnostics(params.diagnostics);

      var middleware = this.clientOptions.middleware.handleDiagnostics;

      if (middleware) {
        middleware(uri, diagnostics, function (uri, diagnostics) {
          return _this18.setDiagnostics(uri, diagnostics);
        });
      } else {
        this.setDiagnostics(uri, diagnostics);
      }
    }
  }, {
    key: "setDiagnostics",
    value: function setDiagnostics(uri, diagnostics) {
      if (!this._diagnostics) {
        return;
      }

      this._diagnostics.set(uri, diagnostics);
    }
  }, {
    key: "createConnection",
    value: function createConnection() {
      var _this19 = this;

      var errorHandler = function errorHandler(error, message, count) {
        _this19.handleConnectionError(error, message, count);
      };

      var closeHandler = function closeHandler() {
        _this19.handleConnectionClosed();
      };

      return this.createMessageTransports(this._clientOptions.stdioEncoding || 'utf8').then(function (transports) {
        return _createConnection(transports.reader, transports.writer, errorHandler, closeHandler);
      });
    }
  }, {
    key: "handleConnectionClosed",
    value: function handleConnectionClosed() {
      // Check whether this is a normal shutdown in progress or the client stopped normally.
      if (this.state === ClientState.Stopping || this.state === ClientState.Stopped) {
        return;
      }

      try {
        if (this._resolvedConnection) {
          this._resolvedConnection.dispose();
        }
      } catch (error) {// Disposing a connection could fail if error cases.
      }

      var action = CloseAction.DoNotRestart;

      try {
        action = this._clientOptions.errorHandler.closed();
      } catch (error) {// Ignore errors coming from the error handler.
      }

      this._connectionPromise = undefined;
      this._resolvedConnection = undefined;

      if (action === CloseAction.DoNotRestart) {
        this.error('Connection to server got closed. Server will not be restarted.');
        this.state = ClientState.Stopped;
        this.cleanUp(false, true);
      } else if (action === CloseAction.Restart) {
        this.info('Connection to server got closed. Server will restart.');
        this.cleanUp(false, false);
        this.state = ClientState.Initial;
        this.start();
      }
    }
  }, {
    key: "handleConnectionError",
    value: function handleConnectionError(error, message, count) {
      var action = this._clientOptions.errorHandler.error(error, message, count);

      if (action === ErrorAction.Shutdown) {
        this.error('Connection to server is erroring. Shutting down server.');
        this.stop();
      }
    }
  }, {
    key: "hookConfigurationChanged",
    value: function hookConfigurationChanged(connection) {
      var _this20 = this;

      vscode_1.workspace.onDidChangeConfiguration(function () {
        _this20.refreshTrace(connection, true);
      });
    }
  }, {
    key: "refreshTrace",
    value: function refreshTrace(connection) {
      var sendNotification = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;
      var config = vscode_1.workspace.getConfiguration(this._id);
      var trace = vscode_languageserver_protocol_1.Trace.Off;
      var traceFormat = vscode_languageserver_protocol_1.TraceFormat.Text;

      if (config) {
        var traceConfig = config.get('trace.server', 'off');

        if (typeof traceConfig === 'string') {
          trace = vscode_languageserver_protocol_1.Trace.fromString(traceConfig);
        } else {
          trace = vscode_languageserver_protocol_1.Trace.fromString(config.get('trace.server.verbosity', 'off'));
          traceFormat = vscode_languageserver_protocol_1.TraceFormat.fromString(config.get('trace.server.format', 'text'));
        }
      }

      this._trace = trace;
      this._traceFormat = traceFormat;
      connection.trace(this._trace, this._tracer, {
        sendNotification: sendNotification,
        traceFormat: this._traceFormat
      });
    }
  }, {
    key: "hookFileEvents",
    value: function hookFileEvents(_connection) {
      var fileEvents = this._clientOptions.synchronize.fileEvents;

      if (!fileEvents) {
        return;
      }

      var watchers;

      if (Is.array(fileEvents)) {
        watchers = fileEvents;
      } else {
        watchers = [fileEvents];
      }

      if (!watchers) {
        return;
      }

      this._dynamicFeatures.get(vscode_languageserver_protocol_1.DidChangeWatchedFilesNotification.type.method).registerRaw(UUID.generateUuid(), watchers);
    }
  }, {
    key: "registerFeatures",
    value: function registerFeatures(features) {
      var _iteratorNormalCompletion11 = true;
      var _didIteratorError11 = false;
      var _iteratorError11 = undefined;

      try {
        for (var _iterator11 = features[Symbol.iterator](), _step11; !(_iteratorNormalCompletion11 = (_step11 = _iterator11.next()).done); _iteratorNormalCompletion11 = true) {
          var feature = _step11.value;
          this.registerFeature(feature);
        }
      } catch (err) {
        _didIteratorError11 = true;
        _iteratorError11 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion11 && _iterator11["return"] != null) {
            _iterator11["return"]();
          }
        } finally {
          if (_didIteratorError11) {
            throw _iteratorError11;
          }
        }
      }
    }
  }, {
    key: "registerFeature",
    value: function registerFeature(feature) {
      this._features.push(feature);

      if (DynamicFeature.is(feature)) {
        var messages = feature.messages;

        if (Array.isArray(messages)) {
          var _iteratorNormalCompletion12 = true;
          var _didIteratorError12 = false;
          var _iteratorError12 = undefined;

          try {
            for (var _iterator12 = messages[Symbol.iterator](), _step12; !(_iteratorNormalCompletion12 = (_step12 = _iterator12.next()).done); _iteratorNormalCompletion12 = true) {
              var message = _step12.value;

              this._method2Message.set(message.method, message);

              this._dynamicFeatures.set(message.method, feature);
            }
          } catch (err) {
            _didIteratorError12 = true;
            _iteratorError12 = err;
          } finally {
            try {
              if (!_iteratorNormalCompletion12 && _iterator12["return"] != null) {
                _iterator12["return"]();
              }
            } finally {
              if (_didIteratorError12) {
                throw _iteratorError12;
              }
            }
          }
        } else {
          this._method2Message.set(messages.method, messages);

          this._dynamicFeatures.set(messages.method, feature);
        }
      }
    }
  }, {
    key: "registerBuiltinFeatures",
    value: function registerBuiltinFeatures() {
      var _this21 = this;

      this.registerFeature(new ConfigurationFeature(this));
      this.registerFeature(new DidOpenTextDocumentFeature(this, this._syncedDocuments));
      this.registerFeature(new DidChangeTextDocumentFeature(this));
      this.registerFeature(new WillSaveFeature(this));
      this.registerFeature(new WillSaveWaitUntilFeature(this));
      this.registerFeature(new DidSaveTextDocumentFeature(this));
      this.registerFeature(new DidCloseTextDocumentFeature(this, this._syncedDocuments));
      this.registerFeature(new FileSystemWatcherFeature(this, function (event) {
        return _this21.notifyFileEvent(event);
      }));
      this.registerFeature(new CompletionItemFeature(this));
      this.registerFeature(new HoverFeature(this));
      this.registerFeature(new SignatureHelpFeature(this));
      this.registerFeature(new DefinitionFeature(this));
      this.registerFeature(new ReferencesFeature(this));
      this.registerFeature(new DocumentHighlightFeature(this));
      this.registerFeature(new DocumentSymbolFeature(this));
      this.registerFeature(new WorkspaceSymbolFeature(this));
      this.registerFeature(new CodeActionFeature(this));
      this.registerFeature(new CodeLensFeature(this));
      this.registerFeature(new DocumentFormattingFeature(this));
      this.registerFeature(new DocumentRangeFormattingFeature(this));
      this.registerFeature(new DocumentOnTypeFormattingFeature(this));
      this.registerFeature(new RenameFeature(this));
      this.registerFeature(new DocumentLinkFeature(this));
      this.registerFeature(new ExecuteCommandFeature(this));
    }
  }, {
    key: "fillInitializeParams",
    value: function fillInitializeParams(params) {
      var _iteratorNormalCompletion13 = true;
      var _didIteratorError13 = false;
      var _iteratorError13 = undefined;

      try {
        for (var _iterator13 = this._features[Symbol.iterator](), _step13; !(_iteratorNormalCompletion13 = (_step13 = _iterator13.next()).done); _iteratorNormalCompletion13 = true) {
          var feature = _step13.value;

          if (Is.func(feature.fillInitializeParams)) {
            feature.fillInitializeParams(params);
          }
        }
      } catch (err) {
        _didIteratorError13 = true;
        _iteratorError13 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion13 && _iterator13["return"] != null) {
            _iterator13["return"]();
          }
        } finally {
          if (_didIteratorError13) {
            throw _iteratorError13;
          }
        }
      }
    }
  }, {
    key: "computeClientCapabilities",
    value: function computeClientCapabilities() {
      var result = {};
      ensure(result, 'workspace').applyEdit = true;
      var workspaceEdit = ensure(ensure(result, 'workspace'), 'workspaceEdit');
      workspaceEdit.documentChanges = true;
      workspaceEdit.resourceOperations = [vscode_languageserver_protocol_1.ResourceOperationKind.Create, vscode_languageserver_protocol_1.ResourceOperationKind.Rename, vscode_languageserver_protocol_1.ResourceOperationKind.Delete];
      workspaceEdit.failureHandling = vscode_languageserver_protocol_1.FailureHandlingKind.TextOnlyTransactional;
      var diagnostics = ensure(ensure(result, 'textDocument'), 'publishDiagnostics');
      diagnostics.relatedInformation = true;
      diagnostics.tagSupport = true;
      var _iteratorNormalCompletion14 = true;
      var _didIteratorError14 = false;
      var _iteratorError14 = undefined;

      try {
        for (var _iterator14 = this._features[Symbol.iterator](), _step14; !(_iteratorNormalCompletion14 = (_step14 = _iterator14.next()).done); _iteratorNormalCompletion14 = true) {
          var feature = _step14.value;
          feature.fillClientCapabilities(result);
        }
      } catch (err) {
        _didIteratorError14 = true;
        _iteratorError14 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion14 && _iterator14["return"] != null) {
            _iterator14["return"]();
          }
        } finally {
          if (_didIteratorError14) {
            throw _iteratorError14;
          }
        }
      }

      return result;
    }
  }, {
    key: "initializeFeatures",
    value: function initializeFeatures(_connection) {
      var documentSelector = this._clientOptions.documentSelector;
      var _iteratorNormalCompletion15 = true;
      var _didIteratorError15 = false;
      var _iteratorError15 = undefined;

      try {
        for (var _iterator15 = this._features[Symbol.iterator](), _step15; !(_iteratorNormalCompletion15 = (_step15 = _iterator15.next()).done); _iteratorNormalCompletion15 = true) {
          var feature = _step15.value;
          feature.initialize(this._capabilities, documentSelector);
        }
      } catch (err) {
        _didIteratorError15 = true;
        _iteratorError15 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion15 && _iterator15["return"] != null) {
            _iterator15["return"]();
          }
        } finally {
          if (_didIteratorError15) {
            throw _iteratorError15;
          }
        }
      }
    }
  }, {
    key: "handleRegistrationRequest",
    value: function handleRegistrationRequest(params) {
      var _this22 = this;

      return new Promise(function (resolve, reject) {
        var _iteratorNormalCompletion16 = true;
        var _didIteratorError16 = false;
        var _iteratorError16 = undefined;

        try {
          for (var _iterator16 = params.registrations[Symbol.iterator](), _step16; !(_iteratorNormalCompletion16 = (_step16 = _iterator16.next()).done); _iteratorNormalCompletion16 = true) {
            var registration = _step16.value;

            var feature = _this22._dynamicFeatures.get(registration.method);

            if (!feature) {
              reject(new Error("No feature implementation for ".concat(registration.method, " found. Registration failed.")));
              return;
            }

            var options = registration.registerOptions || {};
            options.documentSelector = options.documentSelector || _this22._clientOptions.documentSelector;
            var data = {
              id: registration.id,
              registerOptions: options
            };
            feature.register(_this22._method2Message.get(registration.method), data);
          }
        } catch (err) {
          _didIteratorError16 = true;
          _iteratorError16 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion16 && _iterator16["return"] != null) {
              _iterator16["return"]();
            }
          } finally {
            if (_didIteratorError16) {
              throw _iteratorError16;
            }
          }
        }

        resolve();
      });
    }
  }, {
    key: "handleUnregistrationRequest",
    value: function handleUnregistrationRequest(params) {
      var _this23 = this;

      return new Promise(function (resolve, reject) {
        var _iteratorNormalCompletion17 = true;
        var _didIteratorError17 = false;
        var _iteratorError17 = undefined;

        try {
          for (var _iterator17 = params.unregisterations[Symbol.iterator](), _step17; !(_iteratorNormalCompletion17 = (_step17 = _iterator17.next()).done); _iteratorNormalCompletion17 = true) {
            var unregistration = _step17.value;

            var feature = _this23._dynamicFeatures.get(unregistration.method);

            if (!feature) {
              reject(new Error("No feature implementation for ".concat(unregistration.method, " found. Unregistration failed.")));
              return;
            }

            feature.unregister(unregistration.id);
          }
        } catch (err) {
          _didIteratorError17 = true;
          _iteratorError17 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion17 && _iterator17["return"] != null) {
              _iterator17["return"]();
            }
          } finally {
            if (_didIteratorError17) {
              throw _iteratorError17;
            }
          }
        }

        ;
        resolve();
      });
    }
  }, {
    key: "handleApplyWorkspaceEdit",
    value: function handleApplyWorkspaceEdit(params) {
      // This is some sort of workaround since the version check should be done by VS Code in the Workspace.applyEdit.
      // However doing it here adds some safety since the server can lag more behind then an extension.
      var workspaceEdit = params.edit;
      var openTextDocuments = new Map();
      vscode_1.workspace.textDocuments.forEach(function (document) {
        return openTextDocuments.set(document.uri.toString(), document);
      });
      var versionMismatch = false;

      if (workspaceEdit.documentChanges) {
        var _iteratorNormalCompletion18 = true;
        var _didIteratorError18 = false;
        var _iteratorError18 = undefined;

        try {
          for (var _iterator18 = workspaceEdit.documentChanges[Symbol.iterator](), _step18; !(_iteratorNormalCompletion18 = (_step18 = _iterator18.next()).done); _iteratorNormalCompletion18 = true) {
            var change = _step18.value;

            if (vscode_languageserver_protocol_1.TextDocumentEdit.is(change) && change.textDocument.version && change.textDocument.version >= 0) {
              var textDocument = openTextDocuments.get(change.textDocument.uri);

              if (textDocument && textDocument.version !== change.textDocument.version) {
                versionMismatch = true;
                break;
              }
            }
          }
        } catch (err) {
          _didIteratorError18 = true;
          _iteratorError18 = err;
        } finally {
          try {
            if (!_iteratorNormalCompletion18 && _iterator18["return"] != null) {
              _iterator18["return"]();
            }
          } finally {
            if (_didIteratorError18) {
              throw _iteratorError18;
            }
          }
        }
      }

      if (versionMismatch) {
        return Promise.resolve({
          applied: false
        });
      }

      return vscode_1.workspace.applyEdit(this._p2c.asWorkspaceEdit(params.edit)).then(function (value) {
        return {
          applied: value
        };
      });
    }
  }, {
    key: "logFailedRequest",
    value: function logFailedRequest(type, error) {
      // If we get a request cancel don't log anything.
      if (error instanceof vscode_languageserver_protocol_1.ResponseError && error.code === vscode_languageserver_protocol_1.ErrorCodes.RequestCancelled) {
        return;
      }

      this.error("Request ".concat(type.method, " failed."), error);
    }
  }, {
    key: "state",
    get: function get() {
      return this._state;
    },
    set: function set(value) {
      var oldState = this.getPublicState();
      this._state = value;
      var newState = this.getPublicState();

      if (newState !== oldState) {
        this._stateChangeEmitter.fire({
          oldState: oldState,
          newState: newState
        });
      }
    }
  }, {
    key: "initializeResult",
    get: function get() {
      return this._initializeResult;
    }
  }, {
    key: "clientOptions",
    get: function get() {
      return this._clientOptions;
    }
  }, {
    key: "protocol2CodeConverter",
    get: function get() {
      return this._p2c;
    }
  }, {
    key: "code2ProtocolConverter",
    get: function get() {
      return this._c2p;
    }
  }, {
    key: "onTelemetry",
    get: function get() {
      return this._telemetryEmitter.event;
    }
  }, {
    key: "onDidChangeState",
    get: function get() {
      return this._stateChangeEmitter.event;
    }
  }, {
    key: "outputChannel",
    get: function get() {
      if (!this._outputChannel) {
        this._outputChannel = vscode_1.window.createOutputChannel(this._clientOptions.outputChannelName ? this._clientOptions.outputChannelName : this._name);
      }

      return this._outputChannel;
    }
  }, {
    key: "traceOutputChannel",
    get: function get() {
      if (this._traceOutputChannel) {
        return this._traceOutputChannel;
      }

      return this.outputChannel;
    }
  }, {
    key: "diagnostics",
    get: function get() {
      return this._diagnostics;
    }
  }, {
    key: "trace",
    set: function set(value) {
      var _this24 = this;

      this._trace = value;
      this.onReady().then(function () {
        _this24.resolveConnection().then(function (connection) {
          connection.trace(_this24._trace, _this24._tracer, {
            sendNotification: false,
            traceFormat: _this24._traceFormat
          });
        });
      }, function () {});
    }
  }]);

  return BaseLanguageClient;
}();

exports.BaseLanguageClient = BaseLanguageClient;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/client'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../process/browser.js */ "../../node_modules/process/browser.js"), __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/codeConverter.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/codeConverter.js ***!
  \***************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var code = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var proto = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var Is = __webpack_require__(/*! ./utils/is */ "../../node_modules/vscode-languageclient/lib/utils/is.js");

var protocolCompletionItem_1 = __webpack_require__(/*! ./protocolCompletionItem */ "../../node_modules/vscode-languageclient/lib/protocolCompletionItem.js");

var protocolCodeLens_1 = __webpack_require__(/*! ./protocolCodeLens */ "../../node_modules/vscode-languageclient/lib/protocolCodeLens.js");

var protocolDocumentLink_1 = __webpack_require__(/*! ./protocolDocumentLink */ "../../node_modules/vscode-languageclient/lib/protocolDocumentLink.js");

function createConverter(uriConverter) {
  var nullConverter = function nullConverter(value) {
    return value.toString();
  };

  var _uriConverter = uriConverter || nullConverter;

  function asUri(value) {
    return _uriConverter(value);
  }

  function asTextDocumentIdentifier(textDocument) {
    return {
      uri: _uriConverter(textDocument.uri)
    };
  }

  function asVersionedTextDocumentIdentifier(textDocument) {
    return {
      uri: _uriConverter(textDocument.uri),
      version: textDocument.version
    };
  }

  function asOpenTextDocumentParams(textDocument) {
    return {
      textDocument: {
        uri: _uriConverter(textDocument.uri),
        languageId: textDocument.languageId,
        version: textDocument.version,
        text: textDocument.getText()
      }
    };
  }

  function isTextDocumentChangeEvent(value) {
    var candidate = value;
    return !!candidate.document && !!candidate.contentChanges;
  }

  function isTextDocument(value) {
    var candidate = value;
    return !!candidate.uri && !!candidate.version;
  }

  function asChangeTextDocumentParams(arg) {
    if (isTextDocument(arg)) {
      var result = {
        textDocument: {
          uri: _uriConverter(arg.uri),
          version: arg.version
        },
        contentChanges: [{
          text: arg.getText()
        }]
      };
      return result;
    } else if (isTextDocumentChangeEvent(arg)) {
      var document = arg.document;
      var _result = {
        textDocument: {
          uri: _uriConverter(document.uri),
          version: document.version
        },
        contentChanges: arg.contentChanges.map(function (change) {
          var range = change.range;
          return {
            range: {
              start: {
                line: range.start.line,
                character: range.start.character
              },
              end: {
                line: range.end.line,
                character: range.end.character
              }
            },
            rangeLength: change.rangeLength,
            text: change.text
          };
        })
      };
      return _result;
    } else {
      throw Error('Unsupported text document change parameter');
    }
  }

  function asCloseTextDocumentParams(textDocument) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument)
    };
  }

  function asSaveTextDocumentParams(textDocument) {
    var includeContent = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;
    var result = {
      textDocument: asVersionedTextDocumentIdentifier(textDocument)
    };

    if (includeContent) {
      result.text = textDocument.getText();
    }

    return result;
  }

  function asTextDocumentSaveReason(reason) {
    switch (reason) {
      case code.TextDocumentSaveReason.Manual:
        return proto.TextDocumentSaveReason.Manual;

      case code.TextDocumentSaveReason.AfterDelay:
        return proto.TextDocumentSaveReason.AfterDelay;

      case code.TextDocumentSaveReason.FocusOut:
        return proto.TextDocumentSaveReason.FocusOut;
    }

    return proto.TextDocumentSaveReason.Manual;
  }

  function asWillSaveTextDocumentParams(event) {
    return {
      textDocument: asTextDocumentIdentifier(event.document),
      reason: asTextDocumentSaveReason(event.reason)
    };
  }

  function asTextDocumentPositionParams(textDocument, position) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument),
      position: asWorkerPosition(position)
    };
  }

  function asTriggerKind(triggerKind) {
    switch (triggerKind) {
      case code.CompletionTriggerKind.TriggerCharacter:
        return proto.CompletionTriggerKind.TriggerCharacter;

      case code.CompletionTriggerKind.TriggerForIncompleteCompletions:
        return proto.CompletionTriggerKind.TriggerForIncompleteCompletions;

      default:
        return proto.CompletionTriggerKind.Invoked;
    }
  }

  function asCompletionParams(textDocument, position, context) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument),
      position: asWorkerPosition(position),
      context: {
        triggerKind: asTriggerKind(context.triggerKind),
        triggerCharacter: context.triggerCharacter
      }
    };
  }

  function asWorkerPosition(position) {
    return {
      line: position.line,
      character: position.character
    };
  }

  function asPosition(value) {
    if (value === void 0) {
      return undefined;
    } else if (value === null) {
      return null;
    }

    return {
      line: value.line,
      character: value.character
    };
  }

  function asPositions(value) {
    var result = [];
    var _iteratorNormalCompletion = true;
    var _didIteratorError = false;
    var _iteratorError = undefined;

    try {
      for (var _iterator = value[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
        var elem = _step.value;
        result.push(asPosition(elem));
      }
    } catch (err) {
      _didIteratorError = true;
      _iteratorError = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion && _iterator["return"] != null) {
          _iterator["return"]();
        }
      } finally {
        if (_didIteratorError) {
          throw _iteratorError;
        }
      }
    }

    return result;
  }

  function asRange(value) {
    if (value === void 0 || value === null) {
      return value;
    }

    return {
      start: asPosition(value.start),
      end: asPosition(value.end)
    };
  }

  function asDiagnosticSeverity(value) {
    switch (value) {
      case code.DiagnosticSeverity.Error:
        return proto.DiagnosticSeverity.Error;

      case code.DiagnosticSeverity.Warning:
        return proto.DiagnosticSeverity.Warning;

      case code.DiagnosticSeverity.Information:
        return proto.DiagnosticSeverity.Information;

      case code.DiagnosticSeverity.Hint:
        return proto.DiagnosticSeverity.Hint;
    }
  }

  function asDiagnosticTags(tags) {
    if (!tags) {
      return undefined;
    }

    var result = [];
    var _iteratorNormalCompletion2 = true;
    var _didIteratorError2 = false;
    var _iteratorError2 = undefined;

    try {
      for (var _iterator2 = tags[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
        var tag = _step2.value;
        var converted = asDiagnosticTag(tag);

        if (converted !== undefined) {
          result.push(converted);
        }
      }
    } catch (err) {
      _didIteratorError2 = true;
      _iteratorError2 = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion2 && _iterator2["return"] != null) {
          _iterator2["return"]();
        }
      } finally {
        if (_didIteratorError2) {
          throw _iteratorError2;
        }
      }
    }

    return result.length > 0 ? result : undefined;
  }

  function asDiagnosticTag(tag) {
    switch (tag) {
      case code.DiagnosticTag.Unnecessary:
        return proto.DiagnosticTag.Unnecessary;

      case code.DiagnosticTag.Deprecated:
        return proto.DiagnosticTag.Deprecated;

      default:
        return undefined;
    }
  }

  function asDiagnostic(item) {
    var result = proto.Diagnostic.create(asRange(item.range), item.message);

    if (Is.number(item.severity)) {
      result.severity = asDiagnosticSeverity(item.severity);
    }

    if (Is.number(item.code) || Is.string(item.code)) {
      result.code = item.code;
    }

    if (Array.isArray(item.tags)) {
      result.tags = asDiagnosticTags(item.tags);
    }

    ;

    if (item.source) {
      result.source = item.source;
    }

    return result;
  }

  function asDiagnostics(items) {
    if (items === void 0 || items === null) {
      return items;
    }

    return items.map(asDiagnostic);
  }

  function asDocumentation(format, documentation) {
    switch (format) {
      case '$string':
        return documentation;

      case proto.MarkupKind.PlainText:
        return {
          kind: format,
          value: documentation
        };

      case proto.MarkupKind.Markdown:
        return {
          kind: format,
          value: documentation.value
        };

      default:
        return "Unsupported Markup content received. Kind is: ".concat(format);
    }
  }

  function asCompletionItemKind(value, original) {
    if (original !== void 0) {
      return original;
    }

    return value + 1;
  }

  function asCompletionItem(item) {
    var result = {
      label: item.label
    };
    var protocolItem = item instanceof protocolCompletionItem_1["default"] ? item : undefined;

    if (item.detail) {
      result.detail = item.detail;
    } // We only send items back we created. So this can't be something else than
    // a string right now.


    if (item.documentation) {
      if (!protocolItem || protocolItem.documentationFormat === '$string') {
        result.documentation = item.documentation;
      } else {
        result.documentation = asDocumentation(protocolItem.documentationFormat, item.documentation);
      }
    }

    if (item.filterText) {
      result.filterText = item.filterText;
    }

    fillPrimaryInsertText(result, item);

    if (Is.number(item.kind)) {
      result.kind = asCompletionItemKind(item.kind, protocolItem && protocolItem.originalItemKind);
    }

    if (item.sortText) {
      result.sortText = item.sortText;
    }

    if (item.additionalTextEdits) {
      result.additionalTextEdits = asTextEdits(item.additionalTextEdits);
    }

    if (item.commitCharacters) {
      result.commitCharacters = item.commitCharacters.slice();
    }

    if (item.command) {
      result.command = asCommand(item.command);
    }

    if (item.preselect === true || item.preselect === false) {
      result.preselect = item.preselect;
    }

    if (protocolItem) {
      if (protocolItem.data !== void 0) {
        result.data = protocolItem.data;
      }

      if (protocolItem.deprecated === true || protocolItem.deprecated === false) {
        result.deprecated = protocolItem.deprecated;
      }
    }

    return result;
  }

  function fillPrimaryInsertText(target, source) {
    var format = proto.InsertTextFormat.PlainText;
    var text;
    var range = undefined;

    if (source.textEdit) {
      text = source.textEdit.newText;
      range = asRange(source.textEdit.range);
    } else if (source.insertText instanceof code.SnippetString) {
      format = proto.InsertTextFormat.Snippet;
      text = source.insertText.value;
    } else {
      text = source.insertText;
    }

    if (source.range) {
      range = asRange(source.range);
    }

    target.insertTextFormat = format;

    if (source.fromEdit && text && range) {
      target.textEdit = {
        newText: text,
        range: range
      };
    } else {
      target.insertText = text;
    }
  }

  function asTextEdit(edit) {
    return {
      range: asRange(edit.range),
      newText: edit.newText
    };
  }

  function asTextEdits(edits) {
    if (edits === void 0 || edits === null) {
      return edits;
    }

    return edits.map(asTextEdit);
  }

  function asReferenceParams(textDocument, position, options) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument),
      position: asWorkerPosition(position),
      context: {
        includeDeclaration: options.includeDeclaration
      }
    };
  }

  function asCodeActionContext(context) {
    if (context === void 0 || context === null) {
      return context;
    }

    var only;

    if (context.only && Is.string(context.only.value)) {
      only = [context.only.value];
    }

    return proto.CodeActionContext.create(asDiagnostics(context.diagnostics), only);
  }

  function asCommand(item) {
    var result = proto.Command.create(item.title, item.command);

    if (item.arguments) {
      result.arguments = item.arguments;
    }

    return result;
  }

  function asCodeLens(item) {
    var result = proto.CodeLens.create(asRange(item.range));

    if (item.command) {
      result.command = asCommand(item.command);
    }

    if (item instanceof protocolCodeLens_1["default"]) {
      if (item.data) {
        result.data = item.data;
      }

      ;
    }

    return result;
  }

  function asFormattingOptions(item) {
    return {
      tabSize: item.tabSize,
      insertSpaces: item.insertSpaces
    };
  }

  function asDocumentSymbolParams(textDocument) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument)
    };
  }

  function asCodeLensParams(textDocument) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument)
    };
  }

  function asDocumentLink(item) {
    var result = proto.DocumentLink.create(asRange(item.range));

    if (item.target) {
      result.target = asUri(item.target);
    }

    var protocolItem = item instanceof protocolDocumentLink_1["default"] ? item : undefined;

    if (protocolItem && protocolItem.data) {
      result.data = protocolItem.data;
    }

    return result;
  }

  function asDocumentLinkParams(textDocument) {
    return {
      textDocument: asTextDocumentIdentifier(textDocument)
    };
  }

  return {
    asUri: asUri,
    asTextDocumentIdentifier: asTextDocumentIdentifier,
    asVersionedTextDocumentIdentifier: asVersionedTextDocumentIdentifier,
    asOpenTextDocumentParams: asOpenTextDocumentParams,
    asChangeTextDocumentParams: asChangeTextDocumentParams,
    asCloseTextDocumentParams: asCloseTextDocumentParams,
    asSaveTextDocumentParams: asSaveTextDocumentParams,
    asWillSaveTextDocumentParams: asWillSaveTextDocumentParams,
    asTextDocumentPositionParams: asTextDocumentPositionParams,
    asCompletionParams: asCompletionParams,
    asWorkerPosition: asWorkerPosition,
    asRange: asRange,
    asPosition: asPosition,
    asPositions: asPositions,
    asDiagnosticSeverity: asDiagnosticSeverity,
    asDiagnosticTag: asDiagnosticTag,
    asDiagnostic: asDiagnostic,
    asDiagnostics: asDiagnostics,
    asCompletionItem: asCompletionItem,
    asTextEdit: asTextEdit,
    asReferenceParams: asReferenceParams,
    asCodeActionContext: asCodeActionContext,
    asCommand: asCommand,
    asCodeLens: asCodeLens,
    asFormattingOptions: asFormattingOptions,
    asDocumentSymbolParams: asDocumentSymbolParams,
    asCodeLensParams: asCodeLensParams,
    asDocumentLink: asDocumentLink,
    asDocumentLinkParams: asDocumentLinkParams
  };
}

exports.createConverter = createConverter;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/codeConverter'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/colorProvider.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/colorProvider.js ***!
  \***************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var client_1 = __webpack_require__(/*! ./client */ "../../node_modules/vscode-languageclient/lib/client.js");

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var ColorProviderFeature =
/*#__PURE__*/
function (_client_1$TextDocumen) {
  _inherits(ColorProviderFeature, _client_1$TextDocumen);

  function ColorProviderFeature(client) {
    _classCallCheck(this, ColorProviderFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(ColorProviderFeature).call(this, client, vscode_languageserver_protocol_1.DocumentColorRequest.type));
  }

  _createClass(ColorProviderFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'colorProvider').dynamicRegistration = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var _this$getRegistration = this.getRegistration(documentSelector, capabilities.colorProvider),
          _this$getRegistration2 = _slicedToArray(_this$getRegistration, 2),
          id = _this$getRegistration2[0],
          options = _this$getRegistration2[1];

      if (!id || !options) {
        return;
      }

      this.register(this.messages, {
        id: id,
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var _this = this;

      var client = this._client;

      var _provideColorPresentations = function provideColorPresentations(color, context, token) {
        var requestParams = {
          color: color,
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(context.document),
          range: client.code2ProtocolConverter.asRange(context.range)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.ColorPresentationRequest.type, requestParams, token).then(_this.asColorPresentations.bind(_this), function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.ColorPresentationRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var _provideDocumentColors = function provideDocumentColors(document, token) {
        var requestParams = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.DocumentColorRequest.type, requestParams, token).then(_this.asColorInformations.bind(_this), function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.ColorPresentationRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerColorProvider(options.documentSelector, {
        provideColorPresentations: function provideColorPresentations(color, context, token) {
          return middleware.provideColorPresentations ? middleware.provideColorPresentations(color, context, token, _provideColorPresentations) : _provideColorPresentations(color, context, token);
        },
        provideDocumentColors: function provideDocumentColors(document, token) {
          return middleware.provideDocumentColors ? middleware.provideDocumentColors(document, token, _provideDocumentColors) : _provideDocumentColors(document, token);
        }
      });
    }
  }, {
    key: "asColor",
    value: function asColor(color) {
      return new vscode_1.Color(color.red, color.green, color.blue, color.alpha);
    }
  }, {
    key: "asColorInformations",
    value: function asColorInformations(colorInformation) {
      var _this2 = this;

      if (Array.isArray(colorInformation)) {
        return colorInformation.map(function (ci) {
          return new vscode_1.ColorInformation(_this2._client.protocol2CodeConverter.asRange(ci.range), _this2.asColor(ci.color));
        });
      }

      return [];
    }
  }, {
    key: "asColorPresentations",
    value: function asColorPresentations(colorPresentations) {
      var _this3 = this;

      if (Array.isArray(colorPresentations)) {
        return colorPresentations.map(function (cp) {
          var presentation = new vscode_1.ColorPresentation(cp.label);
          presentation.additionalTextEdits = _this3._client.protocol2CodeConverter.asTextEdits(cp.additionalTextEdits);
          presentation.textEdit = _this3._client.protocol2CodeConverter.asTextEdit(cp.textEdit);
          return presentation;
        });
      }

      return [];
    }
  }]);

  return ColorProviderFeature;
}(client_1.TextDocumentFeature);

exports.ColorProviderFeature = ColorProviderFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/colorProvider'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/configuration.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/configuration.js ***!
  \***************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var ConfigurationFeature =
/*#__PURE__*/
function () {
  function ConfigurationFeature(_client) {
    _classCallCheck(this, ConfigurationFeature);

    this._client = _client;
  }

  _createClass(ConfigurationFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      capabilities.workspace = capabilities.workspace || {};
      capabilities.workspace.configuration = true;
    }
  }, {
    key: "initialize",
    value: function initialize() {
      var _this = this;

      var client = this._client;
      client.onRequest(vscode_languageserver_protocol_1.ConfigurationRequest.type, function (params, token) {
        var configuration = function configuration(params) {
          var result = [];
          var _iteratorNormalCompletion = true;
          var _didIteratorError = false;
          var _iteratorError = undefined;

          try {
            for (var _iterator = params.items[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
              var item = _step.value;
              var resource = item.scopeUri !== void 0 && item.scopeUri !== null ? _this._client.protocol2CodeConverter.asUri(item.scopeUri) : undefined;
              result.push(_this.getConfiguration(resource, item.section !== null ? item.section : undefined));
            }
          } catch (err) {
            _didIteratorError = true;
            _iteratorError = err;
          } finally {
            try {
              if (!_iteratorNormalCompletion && _iterator["return"] != null) {
                _iterator["return"]();
              }
            } finally {
              if (_didIteratorError) {
                throw _iteratorError;
              }
            }
          }

          return result;
        };

        var middleware = client.clientOptions.middleware.workspace;
        return middleware && middleware.configuration ? middleware.configuration(params, token, configuration) : configuration(params, token);
      });
    }
  }, {
    key: "getConfiguration",
    value: function getConfiguration(resource, section) {
      var result = null;

      if (section) {
        var index = section.lastIndexOf('.');

        if (index === -1) {
          result = vscode_1.workspace.getConfiguration(undefined, resource).get(section);
        } else {
          var config = vscode_1.workspace.getConfiguration(section.substr(0, index), resource);

          if (config) {
            result = config.get(section.substr(index + 1));
          }
        }
      } else {
        var _config = vscode_1.workspace.getConfiguration(undefined, resource);

        result = {};

        for (var _i = 0, _Object$keys = Object.keys(_config); _i < _Object$keys.length; _i++) {
          var key = _Object$keys[_i];

          if (_config.has(key)) {
            result[key] = _config.get(key);
          }
        }
      }

      if (!result) {
        return null;
      }

      return result;
    }
  }]);

  return ConfigurationFeature;
}();

exports.ConfigurationFeature = ConfigurationFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/configuration'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/declaration.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/declaration.js ***!
  \*************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var client_1 = __webpack_require__(/*! ./client */ "../../node_modules/vscode-languageclient/lib/client.js");

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var DeclarationFeature =
/*#__PURE__*/
function (_client_1$TextDocumen) {
  _inherits(DeclarationFeature, _client_1$TextDocumen);

  function DeclarationFeature(client) {
    _classCallCheck(this, DeclarationFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(DeclarationFeature).call(this, client, vscode_languageserver_protocol_1.DeclarationRequest.type));
  }

  _createClass(DeclarationFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var declarationSupport = ensure(ensure(capabilites, 'textDocument'), 'declaration');
      declarationSupport.dynamicRegistration = true;
      declarationSupport.linkSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var _this$getRegistration = this.getRegistration(documentSelector, capabilities.declarationProvider),
          _this$getRegistration2 = _slicedToArray(_this$getRegistration, 2),
          id = _this$getRegistration2[0],
          options = _this$getRegistration2[1];

      if (!id || !options) {
        return;
      }

      this.register(this.messages, {
        id: id,
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideDeclaration = function provideDeclaration(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.DeclarationRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asDeclarationResult, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.DeclarationRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerDeclarationProvider(options.documentSelector, {
        provideDeclaration: function provideDeclaration(document, position, token) {
          return middleware.provideDeclaration ? middleware.provideDeclaration(document, position, token, _provideDeclaration) : _provideDeclaration(document, position, token);
        }
      });
    }
  }]);

  return DeclarationFeature;
}(client_1.TextDocumentFeature);

exports.DeclarationFeature = DeclarationFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/declaration'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/foldingRange.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/foldingRange.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var client_1 = __webpack_require__(/*! ./client */ "../../node_modules/vscode-languageclient/lib/client.js");

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var FoldingRangeFeature =
/*#__PURE__*/
function (_client_1$TextDocumen) {
  _inherits(FoldingRangeFeature, _client_1$TextDocumen);

  function FoldingRangeFeature(client) {
    _classCallCheck(this, FoldingRangeFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(FoldingRangeFeature).call(this, client, vscode_languageserver_protocol_1.FoldingRangeRequest.type));
  }

  _createClass(FoldingRangeFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var capability = ensure(ensure(capabilites, 'textDocument'), 'foldingRange');
      capability.dynamicRegistration = true;
      capability.rangeLimit = 5000;
      capability.lineFoldingOnly = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var _this$getRegistration = this.getRegistration(documentSelector, capabilities.foldingRangeProvider),
          _this$getRegistration2 = _slicedToArray(_this$getRegistration, 2),
          id = _this$getRegistration2[0],
          options = _this$getRegistration2[1];

      if (!id || !options) {
        return;
      }

      this.register(this.messages, {
        id: id,
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var _this = this;

      var client = this._client;

      var _provideFoldingRanges = function provideFoldingRanges(document, _, token) {
        var requestParams = {
          textDocument: client.code2ProtocolConverter.asTextDocumentIdentifier(document)
        };
        return client.sendRequest(vscode_languageserver_protocol_1.FoldingRangeRequest.type, requestParams, token).then(_this.asFoldingRanges.bind(_this), function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.FoldingRangeRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerFoldingRangeProvider(options.documentSelector, {
        provideFoldingRanges: function provideFoldingRanges(document, context, token) {
          return middleware.provideFoldingRanges ? middleware.provideFoldingRanges(document, context, token, _provideFoldingRanges) : _provideFoldingRanges(document, context, token);
        }
      });
    }
  }, {
    key: "asFoldingRangeKind",
    value: function asFoldingRangeKind(kind) {
      if (kind) {
        switch (kind) {
          case vscode_languageserver_protocol_1.FoldingRangeKind.Comment:
            return vscode_1.FoldingRangeKind.Comment;

          case vscode_languageserver_protocol_1.FoldingRangeKind.Imports:
            return vscode_1.FoldingRangeKind.Imports;

          case vscode_languageserver_protocol_1.FoldingRangeKind.Region:
            return vscode_1.FoldingRangeKind.Region;
        }
      }

      return void 0;
    }
  }, {
    key: "asFoldingRanges",
    value: function asFoldingRanges(foldingRanges) {
      var _this2 = this;

      if (Array.isArray(foldingRanges)) {
        return foldingRanges.map(function (r) {
          return new vscode_1.FoldingRange(r.startLine, r.endLine, _this2.asFoldingRangeKind(r.kind));
        });
      }

      return [];
    }
  }]);

  return FoldingRangeFeature;
}(client_1.TextDocumentFeature);

exports.FoldingRangeFeature = FoldingRangeFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/foldingRange'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/implementation.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/implementation.js ***!
  \****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var client_1 = __webpack_require__(/*! ./client */ "../../node_modules/vscode-languageclient/lib/client.js");

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var ImplementationFeature =
/*#__PURE__*/
function (_client_1$TextDocumen) {
  _inherits(ImplementationFeature, _client_1$TextDocumen);

  function ImplementationFeature(client) {
    _classCallCheck(this, ImplementationFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(ImplementationFeature).call(this, client, vscode_languageserver_protocol_1.ImplementationRequest.type));
  }

  _createClass(ImplementationFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      var implementationSupport = ensure(ensure(capabilites, 'textDocument'), 'implementation');
      implementationSupport.dynamicRegistration = true;
      implementationSupport.linkSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var _this$getRegistration = this.getRegistration(documentSelector, capabilities.implementationProvider),
          _this$getRegistration2 = _slicedToArray(_this$getRegistration, 2),
          id = _this$getRegistration2[0],
          options = _this$getRegistration2[1];

      if (!id || !options) {
        return;
      }

      this.register(this.messages, {
        id: id,
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideImplementation = function provideImplementation(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.ImplementationRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asDefinitionResult, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.ImplementationRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerImplementationProvider(options.documentSelector, {
        provideImplementation: function provideImplementation(document, position, token) {
          return middleware.provideImplementation ? middleware.provideImplementation(document, position, token, _provideImplementation) : _provideImplementation(document, position, token);
        }
      });
    }
  }]);

  return ImplementationFeature;
}(client_1.TextDocumentFeature);

exports.ImplementationFeature = ImplementationFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/implementation'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/progressPart.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/progressPart.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _regeneratorRuntime = __webpack_require__(/*! @babel/runtime/regenerator */ "../../node_modules/@babel/runtime/regenerator/index.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var __awaiter = this && this.__awaiter || function (thisArg, _arguments, P, generator) {
  return new (P || (P = Promise))(function (resolve, reject) {
    function fulfilled(value) {
      try {
        step(generator.next(value));
      } catch (e) {
        reject(e);
      }
    }

    function rejected(value) {
      try {
        step(generator["throw"](value));
      } catch (e) {
        reject(e);
      }
    }

    function step(result) {
      result.done ? resolve(result.value) : new P(function (resolve) {
        resolve(result.value);
      }).then(fulfilled, rejected);
    }

    step((generator = generator.apply(thisArg, _arguments || [])).next());
  });
};

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var Is = __webpack_require__(/*! ./utils/is */ "../../node_modules/vscode-languageclient/lib/utils/is.js");

var ProgressPart =
/*#__PURE__*/
function () {
  function ProgressPart(_client, _token) {
    var _this = this;

    _classCallCheck(this, ProgressPart);

    this._client = _client;
    this._token = _token;
    this._reported = 0;
    this._disposable = this._client.onProgress(vscode_languageserver_protocol_1.Proposed.WorkDoneProgress.type, this._token, function (value) {
      switch (value.kind) {
        case 'begin':
          _this.begin(value);

          break;

        case 'report':
          _this.report(value);

          break;

        case 'done':
          _this.done();

          break;
      }
    });
  }

  _createClass(ProgressPart, [{
    key: "begin",
    value: function begin(params) {
      var _this2 = this;

      var location = params.cancellable ? vscode_1.ProgressLocation.Notification : vscode_1.ProgressLocation.Window;
      vscode_1.window.withProgress({
        location: location,
        cancellable: params.cancellable,
        title: params.title
      }, function (progress, cancellationToken) {
        return __awaiter(_this2, void 0, void 0,
        /*#__PURE__*/
        _regeneratorRuntime.mark(function _callee() {
          var _this3 = this;

          return _regeneratorRuntime.wrap(function _callee$(_context) {
            while (1) {
              switch (_context.prev = _context.next) {
                case 0:
                  this._progress = progress;
                  this._infinite = params.percentage === undefined;
                  this._cancellationToken = cancellationToken;

                  this._cancellationToken.onCancellationRequested(function () {
                    _this3._client.sendNotification(vscode_languageserver_protocol_1.Proposed.WorkDoneProgressCancelNotification.type, {
                      token: _this3._token
                    });
                  });

                  this.report(params);
                  return _context.abrupt("return", new Promise(function (resolve, reject) {
                    _this3._resolve = resolve;
                    _this3._reject = reject;
                  }));

                case 6:
                case "end":
                  return _context.stop();
              }
            }
          }, _callee, this);
        }));
      });
    }
  }, {
    key: "report",
    value: function report(params) {
      if (this._infinite && Is.string(params.message)) {
        this._progress.report({
          message: params.message
        });
      } else if (Is.number(params.percentage)) {
        var percentage = Math.max(0, Math.min(params.percentage, 100));
        var delta = Math.max(0, percentage - this._reported);

        this._progress.report({
          message: params.message,
          increment: delta
        });

        this._reported += delta;
      }
    }
  }, {
    key: "cancel",
    value: function cancel() {
      if (this._disposable) {
        this._disposable.dispose();

        this._disposable = undefined;
      }

      if (this._reject) {
        this._reject();

        this._resolve = undefined;
        this._reject = undefined;
      }
    }
  }, {
    key: "done",
    value: function done() {
      if (this._disposable) {
        this._disposable.dispose();

        this._disposable = undefined;
      }

      if (this._resolve) {
        this._resolve();

        this._resolve = undefined;
        this._reject = undefined;
      }
    }
  }]);

  return ProgressPart;
}();

exports.ProgressPart = ProgressPart;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/progressPart'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/protocolCodeLens.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/protocolCodeLens.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var code = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var ProtocolCodeLens =
/*#__PURE__*/
function (_code$CodeLens) {
  _inherits(ProtocolCodeLens, _code$CodeLens);

  function ProtocolCodeLens(range) {
    _classCallCheck(this, ProtocolCodeLens);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolCodeLens).call(this, range));
  }

  return ProtocolCodeLens;
}(code.CodeLens);

exports["default"] = ProtocolCodeLens;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/protocolCodeLens'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/protocolCompletionItem.js":
/*!************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/protocolCompletionItem.js ***!
  \************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var code = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var ProtocolCompletionItem =
/*#__PURE__*/
function (_code$CompletionItem) {
  _inherits(ProtocolCompletionItem, _code$CompletionItem);

  function ProtocolCompletionItem(label) {
    _classCallCheck(this, ProtocolCompletionItem);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolCompletionItem).call(this, label));
  }

  return ProtocolCompletionItem;
}(code.CompletionItem);

exports["default"] = ProtocolCompletionItem;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/protocolCompletionItem'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/protocolConverter.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/protocolConverter.js ***!
  \*******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var code = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var ls = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var Is = __webpack_require__(/*! ./utils/is */ "../../node_modules/vscode-languageclient/lib/utils/is.js");

var protocolCompletionItem_1 = __webpack_require__(/*! ./protocolCompletionItem */ "../../node_modules/vscode-languageclient/lib/protocolCompletionItem.js");

var protocolCodeLens_1 = __webpack_require__(/*! ./protocolCodeLens */ "../../node_modules/vscode-languageclient/lib/protocolCodeLens.js");

var protocolDocumentLink_1 = __webpack_require__(/*! ./protocolDocumentLink */ "../../node_modules/vscode-languageclient/lib/protocolDocumentLink.js");

var CodeBlock;

(function (CodeBlock) {
  function is(value) {
    var candidate = value;
    return candidate && Is.string(candidate.language) && Is.string(candidate.value);
  }

  CodeBlock.is = is;
})(CodeBlock || (CodeBlock = {}));

function createConverter(uriConverter) {
  var nullConverter = function nullConverter(value) {
    return code.Uri.parse(value);
  };

  var _uriConverter = uriConverter || nullConverter;

  function asUri(value) {
    return _uriConverter(value);
  }

  function asDiagnostics(diagnostics) {
    return diagnostics.map(asDiagnostic);
  }

  function asDiagnostic(diagnostic) {
    var result = new code.Diagnostic(asRange(diagnostic.range), diagnostic.message, asDiagnosticSeverity(diagnostic.severity));

    if (Is.number(diagnostic.code) || Is.string(diagnostic.code)) {
      result.code = diagnostic.code;
    }

    if (diagnostic.source) {
      result.source = diagnostic.source;
    }

    if (diagnostic.relatedInformation) {
      result.relatedInformation = asRelatedInformation(diagnostic.relatedInformation);
    }

    if (Array.isArray(diagnostic.tags)) {
      result.tags = asDiagnosticTags(diagnostic.tags);
    }

    return result;
  }

  function asRelatedInformation(relatedInformation) {
    return relatedInformation.map(asDiagnosticRelatedInformation);
  }

  function asDiagnosticRelatedInformation(information) {
    return new code.DiagnosticRelatedInformation(asLocation(information.location), information.message);
  }

  function asDiagnosticTags(tags) {
    if (!tags) {
      return undefined;
    }

    var result = [];
    var _iteratorNormalCompletion = true;
    var _didIteratorError = false;
    var _iteratorError = undefined;

    try {
      for (var _iterator = tags[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
        var tag = _step.value;
        var converted = asDiagnosticTag(tag);

        if (converted !== undefined) {
          result.push(converted);
        }
      }
    } catch (err) {
      _didIteratorError = true;
      _iteratorError = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion && _iterator["return"] != null) {
          _iterator["return"]();
        }
      } finally {
        if (_didIteratorError) {
          throw _iteratorError;
        }
      }
    }

    return result.length > 0 ? result : undefined;
  }

  function asDiagnosticTag(tag) {
    switch (tag) {
      case ls.DiagnosticTag.Unnecessary:
        return code.DiagnosticTag.Unnecessary;

      case ls.DiagnosticTag.Deprecated:
        return code.DiagnosticTag.Deprecated;

      default:
        return undefined;
    }
  }

  function asPosition(value) {
    if (!value) {
      return undefined;
    }

    return new code.Position(value.line, value.character);
  }

  function asRange(value) {
    if (!value) {
      return undefined;
    }

    return new code.Range(asPosition(value.start), asPosition(value.end));
  }

  function asDiagnosticSeverity(value) {
    if (value === void 0 || value === null) {
      return code.DiagnosticSeverity.Error;
    }

    switch (value) {
      case ls.DiagnosticSeverity.Error:
        return code.DiagnosticSeverity.Error;

      case ls.DiagnosticSeverity.Warning:
        return code.DiagnosticSeverity.Warning;

      case ls.DiagnosticSeverity.Information:
        return code.DiagnosticSeverity.Information;

      case ls.DiagnosticSeverity.Hint:
        return code.DiagnosticSeverity.Hint;
    }

    return code.DiagnosticSeverity.Error;
  }

  function asHoverContent(value) {
    if (Is.string(value)) {
      return new code.MarkdownString(value);
    } else if (CodeBlock.is(value)) {
      var result = new code.MarkdownString();
      return result.appendCodeblock(value.value, value.language);
    } else if (Array.isArray(value)) {
      var _result = [];
      var _iteratorNormalCompletion2 = true;
      var _didIteratorError2 = false;
      var _iteratorError2 = undefined;

      try {
        for (var _iterator2 = value[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
          var element = _step2.value;
          var item = new code.MarkdownString();

          if (CodeBlock.is(element)) {
            item.appendCodeblock(element.value, element.language);
          } else {
            item.appendMarkdown(element);
          }

          _result.push(item);
        }
      } catch (err) {
        _didIteratorError2 = true;
        _iteratorError2 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion2 && _iterator2["return"] != null) {
            _iterator2["return"]();
          }
        } finally {
          if (_didIteratorError2) {
            throw _iteratorError2;
          }
        }
      }

      return _result;
    } else {
      var _result2;

      switch (value.kind) {
        case ls.MarkupKind.Markdown:
          return new code.MarkdownString(value.value);

        case ls.MarkupKind.PlainText:
          _result2 = new code.MarkdownString();

          _result2.appendText(value.value);

          return _result2;

        default:
          _result2 = new code.MarkdownString();

          _result2.appendText("Unsupported Markup content received. Kind is: ".concat(value.kind));

          return _result2;
      }
    }
  }

  function asDocumentation(value) {
    if (Is.string(value)) {
      return value;
    } else {
      switch (value.kind) {
        case ls.MarkupKind.Markdown:
          return new code.MarkdownString(value.value);

        case ls.MarkupKind.PlainText:
          return value.value;

        default:
          return "Unsupported Markup content received. Kind is: ".concat(value.kind);
      }
    }
  }

  function asHover(hover) {
    if (!hover) {
      return undefined;
    }

    return new code.Hover(asHoverContent(hover.contents), asRange(hover.range));
  }

  function asCompletionResult(result) {
    if (!result) {
      return undefined;
    }

    if (Array.isArray(result)) {
      var items = result;
      return items.map(asCompletionItem);
    }

    var list = result;
    return new code.CompletionList(list.items.map(asCompletionItem), list.isIncomplete);
  }

  function asCompletionItemKind(value) {
    // Protocol item kind is 1 based, codes item kind is zero based.
    if (ls.CompletionItemKind.Text <= value && value <= ls.CompletionItemKind.TypeParameter) {
      return [value - 1, undefined];
    }

    ;
    return [code.CompletionItemKind.Text, value];
  }

  function asCompletionItem(item) {
    var result = new protocolCompletionItem_1["default"](item.label);

    if (item.detail) {
      result.detail = item.detail;
    }

    if (item.documentation) {
      result.documentation = asDocumentation(item.documentation);
      result.documentationFormat = Is.string(item.documentation) ? '$string' : item.documentation.kind;
    }

    ;

    if (item.filterText) {
      result.filterText = item.filterText;
    }

    var insertText = asCompletionInsertText(item);

    if (insertText) {
      result.insertText = insertText.text;
      result.range = insertText.range;
      result.fromEdit = insertText.fromEdit;
    }

    if (Is.number(item.kind)) {
      var _asCompletionItemKind = asCompletionItemKind(item.kind),
          _asCompletionItemKind2 = _slicedToArray(_asCompletionItemKind, 2),
          itemKind = _asCompletionItemKind2[0],
          original = _asCompletionItemKind2[1];

      result.kind = itemKind;

      if (original) {
        result.originalItemKind = original;
      }
    }

    if (item.sortText) {
      result.sortText = item.sortText;
    }

    if (item.additionalTextEdits) {
      result.additionalTextEdits = asTextEdits(item.additionalTextEdits);
    }

    if (Is.stringArray(item.commitCharacters)) {
      result.commitCharacters = item.commitCharacters.slice();
    }

    if (item.command) {
      result.command = asCommand(item.command);
    }

    if (item.deprecated === true || item.deprecated === false) {
      result.deprecated = item.deprecated;
    }

    if (item.preselect === true || item.preselect === false) {
      result.preselect = item.preselect;
    }

    if (item.data !== void 0) {
      result.data = item.data;
    }

    return result;
  }

  function asCompletionInsertText(item) {
    if (item.textEdit) {
      if (item.insertTextFormat === ls.InsertTextFormat.Snippet) {
        return {
          text: new code.SnippetString(item.textEdit.newText),
          range: asRange(item.textEdit.range),
          fromEdit: true
        };
      } else {
        return {
          text: item.textEdit.newText,
          range: asRange(item.textEdit.range),
          fromEdit: true
        };
      }
    } else if (item.insertText) {
      if (item.insertTextFormat === ls.InsertTextFormat.Snippet) {
        return {
          text: new code.SnippetString(item.insertText),
          fromEdit: false
        };
      } else {
        return {
          text: item.insertText,
          fromEdit: false
        };
      }
    } else {
      return undefined;
    }
  }

  function asTextEdit(edit) {
    if (!edit) {
      return undefined;
    }

    return new code.TextEdit(asRange(edit.range), edit.newText);
  }

  function asTextEdits(items) {
    if (!items) {
      return undefined;
    }

    return items.map(asTextEdit);
  }

  function asSignatureHelp(item) {
    if (!item) {
      return undefined;
    }

    var result = new code.SignatureHelp();

    if (Is.number(item.activeSignature)) {
      result.activeSignature = item.activeSignature;
    } else {
      // activeSignature was optional in the past
      result.activeSignature = 0;
    }

    if (Is.number(item.activeParameter)) {
      result.activeParameter = item.activeParameter;
    } else {
      // activeParameter was optional in the past
      result.activeParameter = 0;
    }

    if (item.signatures) {
      result.signatures = asSignatureInformations(item.signatures);
    }

    return result;
  }

  function asSignatureInformations(items) {
    return items.map(asSignatureInformation);
  }

  function asSignatureInformation(item) {
    var result = new code.SignatureInformation(item.label);

    if (item.documentation) {
      result.documentation = asDocumentation(item.documentation);
    }

    if (item.parameters) {
      result.parameters = asParameterInformations(item.parameters);
    }

    return result;
  }

  function asParameterInformations(item) {
    return item.map(asParameterInformation);
  }

  function asParameterInformation(item) {
    var result = new code.ParameterInformation(item.label);

    if (item.documentation) {
      result.documentation = asDocumentation(item.documentation);
    }

    ;
    return result;
  }

  function asLocation(item) {
    if (!item) {
      return undefined;
    }

    return new code.Location(_uriConverter(item.uri), asRange(item.range));
  }

  function asDeclarationResult(item) {
    if (!item) {
      return undefined;
    }

    return asLocationResult(item);
  }

  function asDefinitionResult(item) {
    if (!item) {
      return undefined;
    }

    return asLocationResult(item);
  }

  function asLocationLink(item) {
    if (!item) {
      return undefined;
    }

    var result = {
      targetUri: _uriConverter(item.targetUri),
      targetRange: asRange(item.targetSelectionRange),
      originSelectionRange: asRange(item.originSelectionRange),
      targetSelectionRange: asRange(item.targetSelectionRange)
    };

    if (!result.targetSelectionRange) {
      throw new Error("targetSelectionRange must not be undefined or null");
    }

    return result;
  }

  function asLocationResult(item) {
    if (!item) {
      return undefined;
    }

    if (Is.array(item)) {
      if (item.length === 0) {
        return [];
      } else if (ls.LocationLink.is(item[0])) {
        var links = item;
        return links.map(function (link) {
          return asLocationLink(link);
        });
      } else {
        var locations = item;
        return locations.map(function (location) {
          return asLocation(location);
        });
      }
    } else if (ls.LocationLink.is(item)) {
      return [asLocationLink(item)];
    } else {
      return asLocation(item);
    }
  }

  function asReferences(values) {
    if (!values) {
      return undefined;
    }

    return values.map(function (location) {
      return asLocation(location);
    });
  }

  function asDocumentHighlights(values) {
    if (!values) {
      return undefined;
    }

    return values.map(asDocumentHighlight);
  }

  function asDocumentHighlight(item) {
    var result = new code.DocumentHighlight(asRange(item.range));

    if (Is.number(item.kind)) {
      result.kind = asDocumentHighlightKind(item.kind);
    }

    return result;
  }

  function asDocumentHighlightKind(item) {
    switch (item) {
      case ls.DocumentHighlightKind.Text:
        return code.DocumentHighlightKind.Text;

      case ls.DocumentHighlightKind.Read:
        return code.DocumentHighlightKind.Read;

      case ls.DocumentHighlightKind.Write:
        return code.DocumentHighlightKind.Write;
    }

    return code.DocumentHighlightKind.Text;
  }

  function asSymbolInformations(values, uri) {
    if (!values) {
      return undefined;
    }

    return values.map(function (information) {
      return asSymbolInformation(information, uri);
    });
  }

  function asSymbolKind(item) {
    if (item <= ls.SymbolKind.TypeParameter) {
      // Symbol kind is one based in the protocol and zero based in code.
      return item - 1;
    }

    return code.SymbolKind.Property;
  }

  function asSymbolInformation(item, uri) {
    // Symbol kind is one based in the protocol and zero based in code.
    var result = new code.SymbolInformation(item.name, asSymbolKind(item.kind), asRange(item.location.range), item.location.uri ? _uriConverter(item.location.uri) : uri);

    if (item.containerName) {
      result.containerName = item.containerName;
    }

    return result;
  }

  function asDocumentSymbols(values) {
    if (values === void 0 || values === null) {
      return undefined;
    }

    return values.map(asDocumentSymbol);
  }

  function asDocumentSymbol(value) {
    var result = new code.DocumentSymbol(value.name, value.detail || '', asSymbolKind(value.kind), asRange(value.range), asRange(value.selectionRange));

    if (value.children !== void 0 && value.children.length > 0) {
      var children = [];
      var _iteratorNormalCompletion3 = true;
      var _didIteratorError3 = false;
      var _iteratorError3 = undefined;

      try {
        for (var _iterator3 = value.children[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
          var child = _step3.value;
          children.push(asDocumentSymbol(child));
        }
      } catch (err) {
        _didIteratorError3 = true;
        _iteratorError3 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion3 && _iterator3["return"] != null) {
            _iterator3["return"]();
          }
        } finally {
          if (_didIteratorError3) {
            throw _iteratorError3;
          }
        }
      }

      result.children = children;
    }

    return result;
  }

  function asCommand(item) {
    var result = {
      title: item.title,
      command: item.command
    };

    if (item.arguments) {
      result.arguments = item.arguments;
    }

    return result;
  }

  function asCommands(items) {
    if (!items) {
      return undefined;
    }

    return items.map(asCommand);
  }

  var kindMapping = new Map();
  kindMapping.set(ls.CodeActionKind.Empty, code.CodeActionKind.Empty);
  kindMapping.set(ls.CodeActionKind.QuickFix, code.CodeActionKind.QuickFix);
  kindMapping.set(ls.CodeActionKind.Refactor, code.CodeActionKind.Refactor);
  kindMapping.set(ls.CodeActionKind.RefactorExtract, code.CodeActionKind.RefactorExtract);
  kindMapping.set(ls.CodeActionKind.RefactorInline, code.CodeActionKind.RefactorInline);
  kindMapping.set(ls.CodeActionKind.RefactorRewrite, code.CodeActionKind.RefactorRewrite);
  kindMapping.set(ls.CodeActionKind.Source, code.CodeActionKind.Source);
  kindMapping.set(ls.CodeActionKind.SourceOrganizeImports, code.CodeActionKind.SourceOrganizeImports);

  function asCodeActionKind(item) {
    if (item === void 0 || item === null) {
      return undefined;
    }

    var result = kindMapping.get(item);

    if (result) {
      return result;
    }

    var parts = item.split('.');
    result = code.CodeActionKind.Empty;
    var _iteratorNormalCompletion4 = true;
    var _didIteratorError4 = false;
    var _iteratorError4 = undefined;

    try {
      for (var _iterator4 = parts[Symbol.iterator](), _step4; !(_iteratorNormalCompletion4 = (_step4 = _iterator4.next()).done); _iteratorNormalCompletion4 = true) {
        var part = _step4.value;
        result = result.append(part);
      }
    } catch (err) {
      _didIteratorError4 = true;
      _iteratorError4 = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion4 && _iterator4["return"] != null) {
          _iterator4["return"]();
        }
      } finally {
        if (_didIteratorError4) {
          throw _iteratorError4;
        }
      }
    }

    return result;
  }

  function asCodeActionKinds(items) {
    if (items === void 0 || items === null) {
      return undefined;
    }

    return items.map(function (kind) {
      return asCodeActionKind(kind);
    });
  }

  function asCodeAction(item) {
    if (item === void 0 || item === null) {
      return undefined;
    }

    var result = new code.CodeAction(item.title);

    if (item.kind !== void 0) {
      result.kind = asCodeActionKind(item.kind);
    }

    if (item.diagnostics) {
      result.diagnostics = asDiagnostics(item.diagnostics);
    }

    if (item.edit) {
      result.edit = asWorkspaceEdit(item.edit);
    }

    if (item.command) {
      result.command = asCommand(item.command);
    }

    return result;
  }

  function asCodeLens(item) {
    if (!item) {
      return undefined;
    }

    var result = new protocolCodeLens_1["default"](asRange(item.range));

    if (item.command) {
      result.command = asCommand(item.command);
    }

    if (item.data !== void 0 && item.data !== null) {
      result.data = item.data;
    }

    return result;
  }

  function asCodeLenses(items) {
    if (!items) {
      return undefined;
    }

    return items.map(function (codeLens) {
      return asCodeLens(codeLens);
    });
  }

  function asWorkspaceEdit(item) {
    if (!item) {
      return undefined;
    }

    var result = new code.WorkspaceEdit();

    if (item.documentChanges) {
      item.documentChanges.forEach(function (change) {
        if (ls.CreateFile.is(change)) {
          result.createFile(_uriConverter(change.uri), change.options);
        } else if (ls.RenameFile.is(change)) {
          result.renameFile(_uriConverter(change.oldUri), _uriConverter(change.newUri), change.options);
        } else if (ls.DeleteFile.is(change)) {
          result.deleteFile(_uriConverter(change.uri), change.options);
        } else if (ls.TextDocumentEdit.is(change)) {
          result.set(_uriConverter(change.textDocument.uri), asTextEdits(change.edits));
        } else {
          console.error("Unknown workspace edit change received:\n".concat(JSON.stringify(change, undefined, 4)));
        }
      });
    } else if (item.changes) {
      Object.keys(item.changes).forEach(function (key) {
        result.set(_uriConverter(key), asTextEdits(item.changes[key]));
      });
    }

    return result;
  }

  function asDocumentLink(item) {
    var range = asRange(item.range);
    var target = item.target ? asUri(item.target) : undefined; // target must be optional in DocumentLink

    var link = new protocolDocumentLink_1["default"](range, target);

    if (item.data !== void 0 && item.data !== null) {
      link.data = item.data;
    }

    return link;
  }

  function asDocumentLinks(items) {
    if (!items) {
      return undefined;
    }

    return items.map(asDocumentLink);
  }

  function asColor(color) {
    return new code.Color(color.red, color.green, color.blue, color.alpha);
  }

  function asColorInformation(ci) {
    return new code.ColorInformation(asRange(ci.range), asColor(ci.color));
  }

  function asColorInformations(colorInformation) {
    if (Array.isArray(colorInformation)) {
      return colorInformation.map(asColorInformation);
    }

    return undefined;
  }

  function asColorPresentation(cp) {
    var presentation = new code.ColorPresentation(cp.label);
    presentation.additionalTextEdits = asTextEdits(cp.additionalTextEdits);

    if (cp.textEdit) {
      presentation.textEdit = asTextEdit(cp.textEdit);
    }

    return presentation;
  }

  function asColorPresentations(colorPresentations) {
    if (Array.isArray(colorPresentations)) {
      return colorPresentations.map(asColorPresentation);
    }

    return undefined;
  }

  function asFoldingRangeKind(kind) {
    if (kind) {
      switch (kind) {
        case ls.FoldingRangeKind.Comment:
          return code.FoldingRangeKind.Comment;

        case ls.FoldingRangeKind.Imports:
          return code.FoldingRangeKind.Imports;

        case ls.FoldingRangeKind.Region:
          return code.FoldingRangeKind.Region;
      }
    }

    return void 0;
  }

  function asFoldingRange(r) {
    return new code.FoldingRange(r.startLine, r.endLine, asFoldingRangeKind(r.kind));
  }

  function asFoldingRanges(foldingRanges) {
    if (Array.isArray(foldingRanges)) {
      return foldingRanges.map(asFoldingRange);
    }

    return void 0;
  }

  function asSelectionRange(selectionRange) {
    return new code.SelectionRange(asRange(selectionRange.range), selectionRange.parent ? asSelectionRange(selectionRange.parent) : undefined);
  }

  function asSelectionRanges(selectionRanges) {
    if (!Array.isArray(selectionRanges)) {
      return [];
    }

    var result = [];
    var _iteratorNormalCompletion5 = true;
    var _didIteratorError5 = false;
    var _iteratorError5 = undefined;

    try {
      for (var _iterator5 = selectionRanges[Symbol.iterator](), _step5; !(_iteratorNormalCompletion5 = (_step5 = _iterator5.next()).done); _iteratorNormalCompletion5 = true) {
        var range = _step5.value;
        result.push(asSelectionRange(range));
      }
    } catch (err) {
      _didIteratorError5 = true;
      _iteratorError5 = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion5 && _iterator5["return"] != null) {
          _iterator5["return"]();
        }
      } finally {
        if (_didIteratorError5) {
          throw _iteratorError5;
        }
      }
    }

    return result;
  }

  return {
    asUri: asUri,
    asDiagnostics: asDiagnostics,
    asDiagnostic: asDiagnostic,
    asRange: asRange,
    asPosition: asPosition,
    asDiagnosticSeverity: asDiagnosticSeverity,
    asDiagnosticTag: asDiagnosticTag,
    asHover: asHover,
    asCompletionResult: asCompletionResult,
    asCompletionItem: asCompletionItem,
    asTextEdit: asTextEdit,
    asTextEdits: asTextEdits,
    asSignatureHelp: asSignatureHelp,
    asSignatureInformations: asSignatureInformations,
    asSignatureInformation: asSignatureInformation,
    asParameterInformations: asParameterInformations,
    asParameterInformation: asParameterInformation,
    asDeclarationResult: asDeclarationResult,
    asDefinitionResult: asDefinitionResult,
    asLocation: asLocation,
    asReferences: asReferences,
    asDocumentHighlights: asDocumentHighlights,
    asDocumentHighlight: asDocumentHighlight,
    asDocumentHighlightKind: asDocumentHighlightKind,
    asSymbolKind: asSymbolKind,
    asSymbolInformations: asSymbolInformations,
    asSymbolInformation: asSymbolInformation,
    asDocumentSymbols: asDocumentSymbols,
    asDocumentSymbol: asDocumentSymbol,
    asCommand: asCommand,
    asCommands: asCommands,
    asCodeAction: asCodeAction,
    asCodeActionKind: asCodeActionKind,
    asCodeActionKinds: asCodeActionKinds,
    asCodeLens: asCodeLens,
    asCodeLenses: asCodeLenses,
    asWorkspaceEdit: asWorkspaceEdit,
    asDocumentLink: asDocumentLink,
    asDocumentLinks: asDocumentLinks,
    asFoldingRangeKind: asFoldingRangeKind,
    asFoldingRange: asFoldingRange,
    asFoldingRanges: asFoldingRanges,
    asColor: asColor,
    asColorInformation: asColorInformation,
    asColorInformations: asColorInformations,
    asColorPresentation: asColorPresentation,
    asColorPresentations: asColorPresentations,
    asSelectionRange: asSelectionRange,
    asSelectionRanges: asSelectionRanges
  };
}

exports.createConverter = createConverter;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/protocolConverter'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/protocolDocumentLink.js":
/*!**********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/protocolDocumentLink.js ***!
  \**********************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var code = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var ProtocolDocumentLink =
/*#__PURE__*/
function (_code$DocumentLink) {
  _inherits(ProtocolDocumentLink, _code$DocumentLink);

  function ProtocolDocumentLink(range, target) {
    _classCallCheck(this, ProtocolDocumentLink);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolDocumentLink).call(this, range, target));
  }

  return ProtocolDocumentLink;
}(code.DocumentLink);

exports["default"] = ProtocolDocumentLink;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/protocolDocumentLink'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/typeDefinition.js":
/*!****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/typeDefinition.js ***!
  \****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _slicedToArray = __webpack_require__(/*! @babel/runtime/helpers/slicedToArray */ "../../node_modules/@babel/runtime/helpers/slicedToArray.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

var client_1 = __webpack_require__(/*! ./client */ "../../node_modules/vscode-languageclient/lib/client.js");

function ensure(target, key) {
  if (target[key] === void 0) {
    target[key] = {};
  }

  return target[key];
}

var TypeDefinitionFeature =
/*#__PURE__*/
function (_client_1$TextDocumen) {
  _inherits(TypeDefinitionFeature, _client_1$TextDocumen);

  function TypeDefinitionFeature(client) {
    _classCallCheck(this, TypeDefinitionFeature);

    return _possibleConstructorReturn(this, _getPrototypeOf(TypeDefinitionFeature).call(this, client, vscode_languageserver_protocol_1.TypeDefinitionRequest.type));
  }

  _createClass(TypeDefinitionFeature, [{
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilites) {
      ensure(ensure(capabilites, 'textDocument'), 'typeDefinition').dynamicRegistration = true;
      var typeDefinitionSupport = ensure(ensure(capabilites, 'textDocument'), 'typeDefinition');
      typeDefinitionSupport.dynamicRegistration = true;
      typeDefinitionSupport.linkSupport = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities, documentSelector) {
      var _this$getRegistration = this.getRegistration(documentSelector, capabilities.typeDefinitionProvider),
          _this$getRegistration2 = _slicedToArray(_this$getRegistration, 2),
          id = _this$getRegistration2[0],
          options = _this$getRegistration2[1];

      if (!id || !options) {
        return;
      }

      this.register(this.messages, {
        id: id,
        registerOptions: options
      });
    }
  }, {
    key: "registerLanguageProvider",
    value: function registerLanguageProvider(options) {
      var client = this._client;

      var _provideTypeDefinition = function provideTypeDefinition(document, position, token) {
        return client.sendRequest(vscode_languageserver_protocol_1.TypeDefinitionRequest.type, client.code2ProtocolConverter.asTextDocumentPositionParams(document, position), token).then(client.protocol2CodeConverter.asDefinitionResult, function (error) {
          client.logFailedRequest(vscode_languageserver_protocol_1.TypeDefinitionRequest.type, error);
          return Promise.resolve(null);
        });
      };

      var middleware = client.clientOptions.middleware;
      return vscode_1.languages.registerTypeDefinitionProvider(options.documentSelector, {
        provideTypeDefinition: function provideTypeDefinition(document, position, token) {
          return middleware.provideTypeDefinition ? middleware.provideTypeDefinition(document, position, token, _provideTypeDefinition) : _provideTypeDefinition(document, position, token);
        }
      });
    }
  }]);

  return TypeDefinitionFeature;
}(client_1.TextDocumentFeature);

exports.TypeDefinitionFeature = TypeDefinitionFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/typeDefinition'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/utils/async.js":
/*!*************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/utils/async.js ***!
  \*************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var Delayer =
/*#__PURE__*/
function () {
  function Delayer(defaultDelay) {
    _classCallCheck(this, Delayer);

    this.defaultDelay = defaultDelay;
    this.timeout = undefined;
    this.completionPromise = undefined;
    this.onSuccess = undefined;
    this.task = undefined;
  }

  _createClass(Delayer, [{
    key: "trigger",
    value: function trigger(task) {
      var _this = this;

      var delay = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : this.defaultDelay;
      this.task = task;

      if (delay >= 0) {
        this.cancelTimeout();
      }

      if (!this.completionPromise) {
        this.completionPromise = new Promise(function (resolve) {
          _this.onSuccess = resolve;
        }).then(function () {
          _this.completionPromise = undefined;
          _this.onSuccess = undefined;

          var result = _this.task();

          _this.task = undefined;
          return result;
        });
      }

      if (delay >= 0 || this.timeout === void 0) {
        this.timeout = setTimeout(function () {
          _this.timeout = undefined;

          _this.onSuccess(undefined);
        }, delay >= 0 ? delay : this.defaultDelay);
      }

      return this.completionPromise;
    }
  }, {
    key: "forceDelivery",
    value: function forceDelivery() {
      if (!this.completionPromise) {
        return undefined;
      }

      this.cancelTimeout();
      var result = this.task();
      this.completionPromise = undefined;
      this.onSuccess = undefined;
      this.task = undefined;
      return result;
    }
  }, {
    key: "isTriggered",
    value: function isTriggered() {
      return this.timeout !== void 0;
    }
  }, {
    key: "cancel",
    value: function cancel() {
      this.cancelTimeout();
      this.completionPromise = undefined;
    }
  }, {
    key: "cancelTimeout",
    value: function cancelTimeout() {
      if (this.timeout !== void 0) {
        clearTimeout(this.timeout);
        this.timeout = undefined;
      }
    }
  }]);

  return Delayer;
}();

exports.Delayer = Delayer;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/utils/async'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/utils/is.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/utils/is.js ***!
  \**********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

function _boolean(value) {
  return value === true || value === false;
}

exports["boolean"] = _boolean;

function string(value) {
  return typeof value === 'string' || value instanceof String;
}

exports.string = string;

function number(value) {
  return typeof value === 'number' || value instanceof Number;
}

exports.number = number;

function error(value) {
  return value instanceof Error;
}

exports.error = error;

function func(value) {
  return typeof value === 'function';
}

exports.func = func;

function array(value) {
  return Array.isArray(value);
}

exports.array = array;

function stringArray(value) {
  return array(value) && value.every(function (elem) {
    return string(elem);
  });
}

exports.stringArray = stringArray;

function typedArray(value, check) {
  return Array.isArray(value) && value.every(check);
}

exports.typedArray = typedArray;

function thenable(value) {
  return value && func(value.then);
}

exports.thenable = thenable;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/utils/is'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/utils/uuid.js":
/*!************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/utils/uuid.js ***!
  \************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/


var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var ValueUUID =
/*#__PURE__*/
function () {
  function ValueUUID(_value) {
    _classCallCheck(this, ValueUUID);

    this._value = _value; // empty
  }

  _createClass(ValueUUID, [{
    key: "asHex",
    value: function asHex() {
      return this._value;
    }
  }, {
    key: "equals",
    value: function equals(other) {
      return this.asHex() === other.asHex();
    }
  }]);

  return ValueUUID;
}();

var V4UUID =
/*#__PURE__*/
function (_ValueUUID) {
  _inherits(V4UUID, _ValueUUID);

  function V4UUID() {
    _classCallCheck(this, V4UUID);

    return _possibleConstructorReturn(this, _getPrototypeOf(V4UUID).call(this, [V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), '-', V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), '-', '4', V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), '-', V4UUID._oneOf(V4UUID._timeHighBits), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), '-', V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex(), V4UUID._randomHex()].join('')));
  }

  _createClass(V4UUID, null, [{
    key: "_oneOf",
    value: function _oneOf(array) {
      return array[Math.floor(array.length * Math.random())];
    }
  }, {
    key: "_randomHex",
    value: function _randomHex() {
      return V4UUID._oneOf(V4UUID._chars);
    }
  }]);

  return V4UUID;
}(ValueUUID);

V4UUID._chars = ['0', '1', '2', '3', '4', '5', '6', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'];
V4UUID._timeHighBits = ['8', '9', 'a', 'b'];
/**
 * An empty UUID that contains only zeros.
 */

exports.empty = new ValueUUID('00000000-0000-0000-0000-000000000000');

function v4() {
  return new V4UUID();
}

exports.v4 = v4;
var _UUIDPattern = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;

function isUUID(value) {
  return _UUIDPattern.test(value);
}

exports.isUUID = isUUID;
/**
 * Parses a UUID that is of the format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.
 * @param value A uuid string.
 */

function parse(value) {
  if (!isUUID(value)) {
    throw new Error('invalid uuid');
  }

  return new ValueUUID(value);
}

exports.parse = parse;

function generateUuid() {
  return v4().asHex();
}

exports.generateUuid = generateUuid;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/utils/uuid'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageclient/lib/workspaceFolders.js":
/*!******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageclient/lib/workspaceFolders.js ***!
  \******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var UUID = __webpack_require__(/*! ./utils/uuid */ "../../node_modules/vscode-languageclient/lib/utils/uuid.js");

var vscode_1 = __webpack_require__(/*! vscode */ "../../node_modules/monaco-languageclient/lib/vscode-compatibility.js");

var vscode_languageserver_protocol_1 = __webpack_require__(/*! vscode-languageserver-protocol */ "../../node_modules/vscode-languageserver-protocol/lib/main.js");

function access(target, key) {
  if (target === void 0) {
    return undefined;
  }

  return target[key];
}

var WorkspaceFoldersFeature =
/*#__PURE__*/
function () {
  function WorkspaceFoldersFeature(_client) {
    _classCallCheck(this, WorkspaceFoldersFeature);

    this._client = _client;
    this._listeners = new Map();
  }

  _createClass(WorkspaceFoldersFeature, [{
    key: "fillInitializeParams",
    value: function fillInitializeParams(params) {
      var _this = this;

      var folders = vscode_1.workspace.workspaceFolders;

      if (folders === void 0) {
        params.workspaceFolders = null;
      } else {
        params.workspaceFolders = folders.map(function (folder) {
          return _this.asProtocol(folder);
        });
      }
    }
  }, {
    key: "fillClientCapabilities",
    value: function fillClientCapabilities(capabilities) {
      capabilities.workspace = capabilities.workspace || {};
      capabilities.workspace.workspaceFolders = true;
    }
  }, {
    key: "initialize",
    value: function initialize(capabilities) {
      var _this2 = this;

      var client = this._client;
      client.onRequest(vscode_languageserver_protocol_1.WorkspaceFoldersRequest.type, function (token) {
        var workspaceFolders = function workspaceFolders() {
          var folders = vscode_1.workspace.workspaceFolders;

          if (folders === void 0) {
            return null;
          }

          var result = folders.map(function (folder) {
            return _this2.asProtocol(folder);
          });
          return result;
        };

        var middleware = client.clientOptions.middleware.workspace;
        return middleware && middleware.workspaceFolders ? middleware.workspaceFolders(token, workspaceFolders) : workspaceFolders(token);
      });
      var value = access(access(access(capabilities, 'workspace'), 'workspaceFolders'), 'changeNotifications');
      var id;

      if (typeof value === 'string') {
        id = value;
      } else if (value === true) {
        id = UUID.generateUuid();
      }

      if (id) {
        this.register(this.messages, {
          id: id,
          registerOptions: undefined
        });
      }
    }
  }, {
    key: "register",
    value: function register(_message, data) {
      var _this3 = this;

      var id = data.id;
      var client = this._client;
      var disposable = vscode_1.workspace.onDidChangeWorkspaceFolders(function (event) {
        var didChangeWorkspaceFolders = function didChangeWorkspaceFolders(event) {
          var params = {
            event: {
              added: event.added.map(function (folder) {
                return _this3.asProtocol(folder);
              }),
              removed: event.removed.map(function (folder) {
                return _this3.asProtocol(folder);
              })
            }
          };

          _this3._client.sendNotification(vscode_languageserver_protocol_1.DidChangeWorkspaceFoldersNotification.type, params);
        };

        var middleware = client.clientOptions.middleware.workspace;
        middleware && middleware.didChangeWorkspaceFolders ? middleware.didChangeWorkspaceFolders(event, didChangeWorkspaceFolders) : didChangeWorkspaceFolders(event);
      });

      this._listeners.set(id, disposable);
    }
  }, {
    key: "unregister",
    value: function unregister(id) {
      var disposable = this._listeners.get(id);

      if (disposable === void 0) {
        return;
      }

      this._listeners["delete"](id);

      disposable.dispose();
    }
  }, {
    key: "dispose",
    value: function dispose() {
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = this._listeners.values()[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          var disposable = _step.value;
          disposable.dispose();
        }
      } catch (err) {
        _didIteratorError = true;
        _iteratorError = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion && _iterator["return"] != null) {
            _iterator["return"]();
          }
        } finally {
          if (_didIteratorError) {
            throw _iteratorError;
          }
        }
      }

      this._listeners.clear();
    }
  }, {
    key: "asProtocol",
    value: function asProtocol(workspaceFolder) {
      if (workspaceFolder === void 0) {
        return null;
      }

      return {
        uri: this._client.code2ProtocolConverter.asUri(workspaceFolder.uri),
        name: workspaceFolder.name
      };
    }
  }, {
    key: "messages",
    get: function get() {
      return vscode_languageserver_protocol_1.DidChangeWorkspaceFoldersNotification.type;
    }
  }]);

  return WorkspaceFoldersFeature;
}();

exports.WorkspaceFoldersFeature = WorkspaceFoldersFeature;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageclient/lib/workspaceFolders'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/main.js":
/*!***************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/main.js ***!
  \***************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

exports.ErrorCodes = vscode_jsonrpc_1.ErrorCodes;
exports.ResponseError = vscode_jsonrpc_1.ResponseError;
exports.CancellationToken = vscode_jsonrpc_1.CancellationToken;
exports.CancellationTokenSource = vscode_jsonrpc_1.CancellationTokenSource;
exports.Disposable = vscode_jsonrpc_1.Disposable;
exports.Event = vscode_jsonrpc_1.Event;
exports.Emitter = vscode_jsonrpc_1.Emitter;
exports.Trace = vscode_jsonrpc_1.Trace;
exports.TraceFormat = vscode_jsonrpc_1.TraceFormat;
exports.SetTraceNotification = vscode_jsonrpc_1.SetTraceNotification;
exports.LogTraceNotification = vscode_jsonrpc_1.LogTraceNotification;
exports.RequestType = vscode_jsonrpc_1.RequestType;
exports.RequestType0 = vscode_jsonrpc_1.RequestType0;
exports.NotificationType = vscode_jsonrpc_1.NotificationType;
exports.NotificationType0 = vscode_jsonrpc_1.NotificationType0;
exports.MessageReader = vscode_jsonrpc_1.MessageReader;
exports.MessageWriter = vscode_jsonrpc_1.MessageWriter;
exports.ConnectionStrategy = vscode_jsonrpc_1.ConnectionStrategy;
exports.StreamMessageReader = vscode_jsonrpc_1.StreamMessageReader;
exports.StreamMessageWriter = vscode_jsonrpc_1.StreamMessageWriter;
exports.IPCMessageReader = vscode_jsonrpc_1.IPCMessageReader;
exports.IPCMessageWriter = vscode_jsonrpc_1.IPCMessageWriter;
exports.createClientPipeTransport = vscode_jsonrpc_1.createClientPipeTransport;
exports.createServerPipeTransport = vscode_jsonrpc_1.createServerPipeTransport;
exports.generateRandomPipeName = vscode_jsonrpc_1.generateRandomPipeName;
exports.createClientSocketTransport = vscode_jsonrpc_1.createClientSocketTransport;
exports.createServerSocketTransport = vscode_jsonrpc_1.createServerSocketTransport;
exports.ProgressType = vscode_jsonrpc_1.ProgressType;

__export(__webpack_require__(/*! vscode-languageserver-types */ "../../node_modules/vscode-languageserver-types/lib/esm/main.js"));

__export(__webpack_require__(/*! ./protocol */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.js"));

var callHierarchy = __webpack_require__(/*! ./protocol.callHierarchy.proposed */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.callHierarchy.proposed.js");

var st = __webpack_require__(/*! ./protocol.sematicTokens.proposed */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.sematicTokens.proposed.js");

var Proposed;

(function (Proposed) {
  var CallHierarchyPrepareRequest;

  (function (CallHierarchyPrepareRequest) {
    CallHierarchyPrepareRequest.method = callHierarchy.CallHierarchyPrepareRequest.method;
    CallHierarchyPrepareRequest.type = callHierarchy.CallHierarchyPrepareRequest.type;
  })(CallHierarchyPrepareRequest = Proposed.CallHierarchyPrepareRequest || (Proposed.CallHierarchyPrepareRequest = {}));

  var CallHierarchyIncomingCallsRequest;

  (function (CallHierarchyIncomingCallsRequest) {
    CallHierarchyIncomingCallsRequest.method = callHierarchy.CallHierarchyIncomingCallsRequest.method;
    CallHierarchyIncomingCallsRequest.type = callHierarchy.CallHierarchyIncomingCallsRequest.type;
  })(CallHierarchyIncomingCallsRequest = Proposed.CallHierarchyIncomingCallsRequest || (Proposed.CallHierarchyIncomingCallsRequest = {}));

  var CallHierarchyOutgoingCallsRequest;

  (function (CallHierarchyOutgoingCallsRequest) {
    CallHierarchyOutgoingCallsRequest.method = callHierarchy.CallHierarchyOutgoingCallsRequest.method;
    CallHierarchyOutgoingCallsRequest.type = callHierarchy.CallHierarchyOutgoingCallsRequest.type;
  })(CallHierarchyOutgoingCallsRequest = Proposed.CallHierarchyOutgoingCallsRequest || (Proposed.CallHierarchyOutgoingCallsRequest = {}));

  Proposed.SemanticTokenTypes = st.SemanticTokenTypes;
  Proposed.SemanticTokenModifiers = st.SemanticTokenModifiers;
  Proposed.SemanticTokens = st.SemanticTokens;
  var SemanticTokensRequest;

  (function (SemanticTokensRequest) {
    SemanticTokensRequest.method = st.SemanticTokensRequest.method;
    SemanticTokensRequest.type = st.SemanticTokensRequest.type;
  })(SemanticTokensRequest = Proposed.SemanticTokensRequest || (Proposed.SemanticTokensRequest = {}));

  var SemanticTokensEditsRequest;

  (function (SemanticTokensEditsRequest) {
    SemanticTokensEditsRequest.method = st.SemanticTokensEditsRequest.method;
    SemanticTokensEditsRequest.type = st.SemanticTokensEditsRequest.type;
  })(SemanticTokensEditsRequest = Proposed.SemanticTokensEditsRequest || (Proposed.SemanticTokensEditsRequest = {}));

  var SemanticTokensRangeRequest;

  (function (SemanticTokensRangeRequest) {
    SemanticTokensRangeRequest.method = st.SemanticTokensRangeRequest.method;
    SemanticTokensRangeRequest.type = st.SemanticTokensRangeRequest.type;
  })(SemanticTokensRangeRequest = Proposed.SemanticTokensRangeRequest || (Proposed.SemanticTokensRangeRequest = {}));
})(Proposed = exports.Proposed || (exports.Proposed = {}));

function createProtocolConnection(reader, writer, logger, strategy) {
  return vscode_jsonrpc_1.createMessageConnection(reader, writer, logger, strategy);
}

exports.createProtocolConnection = createProtocolConnection;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/main'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/messages.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/messages.js ***!
  \*******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var ProtocolRequestType0 =
/*#__PURE__*/
function (_vscode_jsonrpc_1$Req) {
  _inherits(ProtocolRequestType0, _vscode_jsonrpc_1$Req);

  function ProtocolRequestType0(method) {
    _classCallCheck(this, ProtocolRequestType0);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolRequestType0).call(this, method));
  }

  return ProtocolRequestType0;
}(vscode_jsonrpc_1.RequestType0);

exports.ProtocolRequestType0 = ProtocolRequestType0;

var ProtocolRequestType =
/*#__PURE__*/
function (_vscode_jsonrpc_1$Req2) {
  _inherits(ProtocolRequestType, _vscode_jsonrpc_1$Req2);

  function ProtocolRequestType(method) {
    _classCallCheck(this, ProtocolRequestType);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolRequestType).call(this, method));
  }

  return ProtocolRequestType;
}(vscode_jsonrpc_1.RequestType);

exports.ProtocolRequestType = ProtocolRequestType;

var ProtocolNotificationType =
/*#__PURE__*/
function (_vscode_jsonrpc_1$Not) {
  _inherits(ProtocolNotificationType, _vscode_jsonrpc_1$Not);

  function ProtocolNotificationType(method) {
    _classCallCheck(this, ProtocolNotificationType);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolNotificationType).call(this, method));
  }

  return ProtocolNotificationType;
}(vscode_jsonrpc_1.NotificationType);

exports.ProtocolNotificationType = ProtocolNotificationType;

var ProtocolNotificationType0 =
/*#__PURE__*/
function (_vscode_jsonrpc_1$Not2) {
  _inherits(ProtocolNotificationType0, _vscode_jsonrpc_1$Not2);

  function ProtocolNotificationType0(method) {
    _classCallCheck(this, ProtocolNotificationType0);

    return _possibleConstructorReturn(this, _getPrototypeOf(ProtocolNotificationType0).call(this, method));
  }

  return ProtocolNotificationType0;
}(vscode_jsonrpc_1.NotificationType0);

exports.ProtocolNotificationType0 = ProtocolNotificationType0;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/messages'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.callHierarchy.proposed.js":
/*!******************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.callHierarchy.proposed.js ***!
  \******************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) TypeFox and others. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * A request to result a `CallHierarchyItem` in a document at a given position.
 * Can be used as an input to a incoming or outgoing call hierarchy.
 *
 * @since 3.16.0 - Proposed state
 */


var CallHierarchyPrepareRequest;

(function (CallHierarchyPrepareRequest) {
  CallHierarchyPrepareRequest.method = 'textDocument/prepareCallHierarchy';
  CallHierarchyPrepareRequest.type = new messages_1.ProtocolRequestType(CallHierarchyPrepareRequest.method);
})(CallHierarchyPrepareRequest = exports.CallHierarchyPrepareRequest || (exports.CallHierarchyPrepareRequest = {}));
/**
 * A request to resolve the incoming calls for a given `CallHierarchyItem`.
 *
 * @since 3.16.0 - Proposed state
 */


var CallHierarchyIncomingCallsRequest;

(function (CallHierarchyIncomingCallsRequest) {
  CallHierarchyIncomingCallsRequest.method = 'callHierarchy/incomingCalls';
  CallHierarchyIncomingCallsRequest.type = new messages_1.ProtocolRequestType(CallHierarchyIncomingCallsRequest.method);
})(CallHierarchyIncomingCallsRequest = exports.CallHierarchyIncomingCallsRequest || (exports.CallHierarchyIncomingCallsRequest = {}));
/**
 * A request to resolve the outgoing calls for a given `CallHierarchyItem`.
 *
 * @since 3.16.0 - Proposed state
 */


var CallHierarchyOutgoingCallsRequest;

(function (CallHierarchyOutgoingCallsRequest) {
  CallHierarchyOutgoingCallsRequest.method = 'callHierarchy/outgoingCalls';
  CallHierarchyOutgoingCallsRequest.type = new messages_1.ProtocolRequestType(CallHierarchyOutgoingCallsRequest.method);
})(CallHierarchyOutgoingCallsRequest = exports.CallHierarchyOutgoingCallsRequest || (exports.CallHierarchyOutgoingCallsRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.callHierarchy.proposed'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.colorProvider.js":
/*!*********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.colorProvider.js ***!
  \*********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * A request to list all color symbols found in a given text document. The request's
 * parameter is of type [DocumentColorParams](#DocumentColorParams) the
 * response is of type [ColorInformation[]](#ColorInformation) or a Thenable
 * that resolves to such.
 */


var DocumentColorRequest;

(function (DocumentColorRequest) {
  DocumentColorRequest.method = 'textDocument/documentColor';
  DocumentColorRequest.type = new messages_1.ProtocolRequestType(DocumentColorRequest.method);
  /** @deprecated Use DocumentColorRequest.type */

  DocumentColorRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DocumentColorRequest = exports.DocumentColorRequest || (exports.DocumentColorRequest = {}));
/**
 * A request to list all presentation for a color. The request's
 * parameter is of type [ColorPresentationParams](#ColorPresentationParams) the
 * response is of type [ColorInformation[]](#ColorInformation) or a Thenable
 * that resolves to such.
 */


var ColorPresentationRequest;

(function (ColorPresentationRequest) {
  ColorPresentationRequest.type = new messages_1.ProtocolRequestType('textDocument/colorPresentation');
})(ColorPresentationRequest = exports.ColorPresentationRequest || (exports.ColorPresentationRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.colorProvider'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.configuration.js":
/*!*********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.configuration.js ***!
  \*********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * The 'workspace/configuration' request is sent from the server to the client to fetch a certain
 * configuration setting.
 *
 * This pull model replaces the old push model were the client signaled configuration change via an
 * event. If the server still needs to react to configuration changes (since the server caches the
 * result of `workspace/configuration` requests) the server should register for an empty configuration
 * change event and empty the cache if such an event is received.
 */


var ConfigurationRequest;

(function (ConfigurationRequest) {
  ConfigurationRequest.type = new messages_1.ProtocolRequestType('workspace/configuration');
})(ConfigurationRequest = exports.ConfigurationRequest || (exports.ConfigurationRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.configuration'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.declaration.js":
/*!*******************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.declaration.js ***!
  \*******************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js"); // @ts-ignore: to avoid inlining LocatioLink as dynamic import


var __noDynamicImport;
/**
 * A request to resolve the type definition locations of a symbol at a given text
 * document position. The request's parameter is of type [TextDocumentPositioParams]
 * (#TextDocumentPositionParams) the response is of type [Declaration](#Declaration)
 * or a typed array of [DeclarationLink](#DeclarationLink) or a Thenable that resolves
 * to such.
 */


var DeclarationRequest;

(function (DeclarationRequest) {
  DeclarationRequest.method = 'textDocument/declaration';
  DeclarationRequest.type = new messages_1.ProtocolRequestType(DeclarationRequest.method);
  /** @deprecated Use DeclarationRequest.type */

  DeclarationRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DeclarationRequest = exports.DeclarationRequest || (exports.DeclarationRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.declaration'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.foldingRange.js":
/*!********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.foldingRange.js ***!
  \********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * Enum of known range kinds
 */


var FoldingRangeKind;

(function (FoldingRangeKind) {
  /**
   * Folding range for a comment
   */
  FoldingRangeKind["Comment"] = "comment";
  /**
   * Folding range for a imports or includes
   */

  FoldingRangeKind["Imports"] = "imports";
  /**
   * Folding range for a region (e.g. `#region`)
   */

  FoldingRangeKind["Region"] = "region";
})(FoldingRangeKind = exports.FoldingRangeKind || (exports.FoldingRangeKind = {}));
/**
 * A request to provide folding ranges in a document. The request's
 * parameter is of type [FoldingRangeParams](#FoldingRangeParams), the
 * response is of type [FoldingRangeList](#FoldingRangeList) or a Thenable
 * that resolves to such.
 */


var FoldingRangeRequest;

(function (FoldingRangeRequest) {
  FoldingRangeRequest.method = 'textDocument/foldingRange';
  FoldingRangeRequest.type = new messages_1.ProtocolRequestType(FoldingRangeRequest.method);
  /** @deprecated Use FoldingRangeRequest.type */

  FoldingRangeRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(FoldingRangeRequest = exports.FoldingRangeRequest || (exports.FoldingRangeRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.foldingRange'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.implementation.js":
/*!**********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.implementation.js ***!
  \**********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js"); // @ts-ignore: to avoid inlining LocatioLink as dynamic import


var __noDynamicImport;
/**
 * A request to resolve the implementation locations of a symbol at a given text
 * document position. The request's parameter is of type [TextDocumentPositioParams]
 * (#TextDocumentPositionParams) the response is of type [Definition](#Definition) or a
 * Thenable that resolves to such.
 */


var ImplementationRequest;

(function (ImplementationRequest) {
  ImplementationRequest.method = 'textDocument/implementation';
  ImplementationRequest.type = new messages_1.ProtocolRequestType(ImplementationRequest.method);
  /** @deprecated Use ImplementationRequest.type */

  ImplementationRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(ImplementationRequest = exports.ImplementationRequest || (exports.ImplementationRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.implementation'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.js ***!
  \*******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var Is = __webpack_require__(/*! ./utils/is */ "../../node_modules/vscode-languageserver-protocol/lib/utils/is.js");

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");

var protocol_implementation_1 = __webpack_require__(/*! ./protocol.implementation */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.implementation.js");

exports.ImplementationRequest = protocol_implementation_1.ImplementationRequest;

var protocol_typeDefinition_1 = __webpack_require__(/*! ./protocol.typeDefinition */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.typeDefinition.js");

exports.TypeDefinitionRequest = protocol_typeDefinition_1.TypeDefinitionRequest;

var protocol_workspaceFolders_1 = __webpack_require__(/*! ./protocol.workspaceFolders */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.workspaceFolders.js");

exports.WorkspaceFoldersRequest = protocol_workspaceFolders_1.WorkspaceFoldersRequest;
exports.DidChangeWorkspaceFoldersNotification = protocol_workspaceFolders_1.DidChangeWorkspaceFoldersNotification;

var protocol_configuration_1 = __webpack_require__(/*! ./protocol.configuration */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.configuration.js");

exports.ConfigurationRequest = protocol_configuration_1.ConfigurationRequest;

var protocol_colorProvider_1 = __webpack_require__(/*! ./protocol.colorProvider */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.colorProvider.js");

exports.DocumentColorRequest = protocol_colorProvider_1.DocumentColorRequest;
exports.ColorPresentationRequest = protocol_colorProvider_1.ColorPresentationRequest;

var protocol_foldingRange_1 = __webpack_require__(/*! ./protocol.foldingRange */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.foldingRange.js");

exports.FoldingRangeRequest = protocol_foldingRange_1.FoldingRangeRequest;

var protocol_declaration_1 = __webpack_require__(/*! ./protocol.declaration */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.declaration.js");

exports.DeclarationRequest = protocol_declaration_1.DeclarationRequest;

var protocol_selectionRange_1 = __webpack_require__(/*! ./protocol.selectionRange */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.selectionRange.js");

exports.SelectionRangeRequest = protocol_selectionRange_1.SelectionRangeRequest;

var protocol_progress_1 = __webpack_require__(/*! ./protocol.progress */ "../../node_modules/vscode-languageserver-protocol/lib/protocol.progress.js");

exports.WorkDoneProgress = protocol_progress_1.WorkDoneProgress;
exports.WorkDoneProgressCreateRequest = protocol_progress_1.WorkDoneProgressCreateRequest;
exports.WorkDoneProgressCancelNotification = protocol_progress_1.WorkDoneProgressCancelNotification; // @ts-ignore: to avoid inlining LocatioLink as dynamic import

var __noDynamicImport;
/**
 * The DocumentFilter namespace provides helper functions to work with
 * [DocumentFilter](#DocumentFilter) literals.
 */


var DocumentFilter;

(function (DocumentFilter) {
  function is(value) {
    var candidate = value;
    return Is.string(candidate.language) || Is.string(candidate.scheme) || Is.string(candidate.pattern);
  }

  DocumentFilter.is = is;
})(DocumentFilter = exports.DocumentFilter || (exports.DocumentFilter = {}));
/**
 * The DocumentSelector namespace provides helper functions to work with
 * [DocumentSelector](#DocumentSelector)s.
 */


var DocumentSelector;

(function (DocumentSelector) {
  function is(value) {
    if (!Array.isArray(value)) {
      return false;
    }

    var _iteratorNormalCompletion = true;
    var _didIteratorError = false;
    var _iteratorError = undefined;

    try {
      for (var _iterator = value[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
        var elem = _step.value;

        if (!Is.string(elem) && !DocumentFilter.is(elem)) {
          return false;
        }
      }
    } catch (err) {
      _didIteratorError = true;
      _iteratorError = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion && _iterator["return"] != null) {
          _iterator["return"]();
        }
      } finally {
        if (_didIteratorError) {
          throw _iteratorError;
        }
      }
    }

    return true;
  }

  DocumentSelector.is = is;
})(DocumentSelector = exports.DocumentSelector || (exports.DocumentSelector = {}));
/**
 * The `client/registerCapability` request is sent from the server to the client to register a new capability
 * handler on the client side.
 */


var RegistrationRequest;

(function (RegistrationRequest) {
  RegistrationRequest.type = new messages_1.ProtocolRequestType('client/registerCapability');
})(RegistrationRequest = exports.RegistrationRequest || (exports.RegistrationRequest = {}));
/**
 * The `client/unregisterCapability` request is sent from the server to the client to unregister a previously registered capability
 * handler on the client side.
 */


var UnregistrationRequest;

(function (UnregistrationRequest) {
  UnregistrationRequest.type = new messages_1.ProtocolRequestType('client/unregisterCapability');
})(UnregistrationRequest = exports.UnregistrationRequest || (exports.UnregistrationRequest = {}));

var ResourceOperationKind;

(function (ResourceOperationKind) {
  /**
   * Supports creating new files and folders.
   */
  ResourceOperationKind.Create = 'create';
  /**
   * Supports renaming existing files and folders.
   */

  ResourceOperationKind.Rename = 'rename';
  /**
   * Supports deleting existing files and folders.
   */

  ResourceOperationKind.Delete = 'delete';
})(ResourceOperationKind = exports.ResourceOperationKind || (exports.ResourceOperationKind = {}));

var FailureHandlingKind;

(function (FailureHandlingKind) {
  /**
   * Applying the workspace change is simply aborted if one of the changes provided
   * fails. All operations executed before the failing operation stay executed.
   */
  FailureHandlingKind.Abort = 'abort';
  /**
   * All operations are executed transactional. That means they either all
   * succeed or no changes at all are applied to the workspace.
   */

  FailureHandlingKind.Transactional = 'transactional';
  /**
   * If the workspace edit contains only textual file changes they are executed transactional.
   * If resource changes (create, rename or delete file) are part of the change the failure
   * handling startegy is abort.
   */

  FailureHandlingKind.TextOnlyTransactional = 'textOnlyTransactional';
  /**
   * The client tries to undo the operations already executed. But there is no
   * guarantee that this is succeeding.
   */

  FailureHandlingKind.Undo = 'undo';
})(FailureHandlingKind = exports.FailureHandlingKind || (exports.FailureHandlingKind = {}));
/**
 * The StaticRegistrationOptions namespace provides helper functions to work with
 * [StaticRegistrationOptions](#StaticRegistrationOptions) literals.
 */


var StaticRegistrationOptions;

(function (StaticRegistrationOptions) {
  function hasId(value) {
    var candidate = value;
    return candidate && Is.string(candidate.id) && candidate.id.length > 0;
  }

  StaticRegistrationOptions.hasId = hasId;
})(StaticRegistrationOptions = exports.StaticRegistrationOptions || (exports.StaticRegistrationOptions = {}));
/**
 * The TextDocumentRegistrationOptions namespace provides helper functions to work with
 * [TextDocumentRegistrationOptions](#TextDocumentRegistrationOptions) literals.
 */


var TextDocumentRegistrationOptions;

(function (TextDocumentRegistrationOptions) {
  function is(value) {
    var candidate = value;
    return candidate && (candidate.documentSelector === null || DocumentSelector.is(candidate.documentSelector));
  }

  TextDocumentRegistrationOptions.is = is;
})(TextDocumentRegistrationOptions = exports.TextDocumentRegistrationOptions || (exports.TextDocumentRegistrationOptions = {}));
/**
 * The WorkDoneProgressOptions namespace provides helper functions to work with
 * [WorkDoneProgressOptions](#WorkDoneProgressOptions) literals.
 */


var WorkDoneProgressOptions;

(function (WorkDoneProgressOptions) {
  function is(value) {
    var candidate = value;
    return Is.objectLiteral(candidate) && (candidate.workDoneProgress === undefined || Is["boolean"](candidate.workDoneProgress));
  }

  WorkDoneProgressOptions.is = is;

  function hasWorkDoneProgress(value) {
    var candidate = value;
    return candidate && Is["boolean"](candidate.workDoneProgress);
  }

  WorkDoneProgressOptions.hasWorkDoneProgress = hasWorkDoneProgress;
})(WorkDoneProgressOptions = exports.WorkDoneProgressOptions || (exports.WorkDoneProgressOptions = {}));
/**
 * The initialize request is sent from the client to the server.
 * It is sent once as the request after starting up the server.
 * The requests parameter is of type [InitializeParams](#InitializeParams)
 * the response if of type [InitializeResult](#InitializeResult) of a Thenable that
 * resolves to such.
 */


var InitializeRequest;

(function (InitializeRequest) {
  InitializeRequest.type = new messages_1.ProtocolRequestType('initialize');
})(InitializeRequest = exports.InitializeRequest || (exports.InitializeRequest = {}));
/**
 * Known error codes for an `InitializeError`;
 */


var InitializeError;

(function (InitializeError) {
  /**
   * If the protocol version provided by the client can't be handled by the server.
   * @deprecated This initialize error got replaced by client capabilities. There is
   * no version handshake in version 3.0x
   */
  InitializeError.unknownProtocolVersion = 1;
})(InitializeError = exports.InitializeError || (exports.InitializeError = {}));
/**
 * The intialized notification is sent from the client to the
 * server after the client is fully initialized and the server
 * is allowed to send requests from the server to the client.
 */


var InitializedNotification;

(function (InitializedNotification) {
  InitializedNotification.type = new messages_1.ProtocolNotificationType('initialized');
})(InitializedNotification = exports.InitializedNotification || (exports.InitializedNotification = {})); //---- Shutdown Method ----

/**
 * A shutdown request is sent from the client to the server.
 * It is sent once when the client decides to shutdown the
 * server. The only notification that is sent after a shutdown request
 * is the exit event.
 */


var ShutdownRequest;

(function (ShutdownRequest) {
  ShutdownRequest.type = new messages_1.ProtocolRequestType0('shutdown');
})(ShutdownRequest = exports.ShutdownRequest || (exports.ShutdownRequest = {})); //---- Exit Notification ----

/**
 * The exit event is sent from the client to the server to
 * ask the server to exit its process.
 */


var ExitNotification;

(function (ExitNotification) {
  ExitNotification.type = new messages_1.ProtocolNotificationType0('exit');
})(ExitNotification = exports.ExitNotification || (exports.ExitNotification = {}));
/**
 * The configuration change notification is sent from the client to the server
 * when the client's configuration has changed. The notification contains
 * the changed configuration as defined by the language client.
 */


var DidChangeConfigurationNotification;

(function (DidChangeConfigurationNotification) {
  DidChangeConfigurationNotification.type = new messages_1.ProtocolNotificationType('workspace/didChangeConfiguration');
})(DidChangeConfigurationNotification = exports.DidChangeConfigurationNotification || (exports.DidChangeConfigurationNotification = {})); //---- Message show and log notifications ----

/**
 * The message type
 */


var MessageType;

(function (MessageType) {
  /**
   * An error message.
   */
  MessageType.Error = 1;
  /**
   * A warning message.
   */

  MessageType.Warning = 2;
  /**
   * An information message.
   */

  MessageType.Info = 3;
  /**
   * A log message.
   */

  MessageType.Log = 4;
})(MessageType = exports.MessageType || (exports.MessageType = {}));
/**
 * The show message notification is sent from a server to a client to ask
 * the client to display a particular message in the user interface.
 */


var ShowMessageNotification;

(function (ShowMessageNotification) {
  ShowMessageNotification.type = new messages_1.ProtocolNotificationType('window/showMessage');
})(ShowMessageNotification = exports.ShowMessageNotification || (exports.ShowMessageNotification = {}));
/**
 * The show message request is sent from the server to the client to show a message
 * and a set of options actions to the user.
 */


var ShowMessageRequest;

(function (ShowMessageRequest) {
  ShowMessageRequest.type = new messages_1.ProtocolRequestType('window/showMessageRequest');
})(ShowMessageRequest = exports.ShowMessageRequest || (exports.ShowMessageRequest = {}));
/**
 * The log message notification is sent from the server to the client to ask
 * the client to log a particular message.
 */


var LogMessageNotification;

(function (LogMessageNotification) {
  LogMessageNotification.type = new messages_1.ProtocolNotificationType('window/logMessage');
})(LogMessageNotification = exports.LogMessageNotification || (exports.LogMessageNotification = {})); //---- Telemetry notification

/**
 * The telemetry event notification is sent from the server to the client to ask
 * the client to log telemetry data.
 */


var TelemetryEventNotification;

(function (TelemetryEventNotification) {
  TelemetryEventNotification.type = new messages_1.ProtocolNotificationType('telemetry/event');
})(TelemetryEventNotification = exports.TelemetryEventNotification || (exports.TelemetryEventNotification = {}));
/**
 * Defines how the host (editor) should sync
 * document changes to the language server.
 */


var TextDocumentSyncKind;

(function (TextDocumentSyncKind) {
  /**
   * Documents should not be synced at all.
   */
  TextDocumentSyncKind.None = 0;
  /**
   * Documents are synced by always sending the full content
   * of the document.
   */

  TextDocumentSyncKind.Full = 1;
  /**
   * Documents are synced by sending the full content on open.
   * After that only incremental updates to the document are
   * send.
   */

  TextDocumentSyncKind.Incremental = 2;
})(TextDocumentSyncKind = exports.TextDocumentSyncKind || (exports.TextDocumentSyncKind = {}));
/**
 * The document open notification is sent from the client to the server to signal
 * newly opened text documents. The document's truth is now managed by the client
 * and the server must not try to read the document's truth using the document's
 * uri. Open in this sense means it is managed by the client. It doesn't necessarily
 * mean that its content is presented in an editor. An open notification must not
 * be sent more than once without a corresponding close notification send before.
 * This means open and close notification must be balanced and the max open count
 * is one.
 */


var DidOpenTextDocumentNotification;

(function (DidOpenTextDocumentNotification) {
  DidOpenTextDocumentNotification.method = 'textDocument/didOpen';
  DidOpenTextDocumentNotification.type = new messages_1.ProtocolNotificationType(DidOpenTextDocumentNotification.method);
})(DidOpenTextDocumentNotification = exports.DidOpenTextDocumentNotification || (exports.DidOpenTextDocumentNotification = {}));
/**
 * The document change notification is sent from the client to the server to signal
 * changes to a text document.
 */


var DidChangeTextDocumentNotification;

(function (DidChangeTextDocumentNotification) {
  DidChangeTextDocumentNotification.method = 'textDocument/didChange';
  DidChangeTextDocumentNotification.type = new messages_1.ProtocolNotificationType(DidChangeTextDocumentNotification.method);
})(DidChangeTextDocumentNotification = exports.DidChangeTextDocumentNotification || (exports.DidChangeTextDocumentNotification = {}));
/**
 * The document close notification is sent from the client to the server when
 * the document got closed in the client. The document's truth now exists where
 * the document's uri points to (e.g. if the document's uri is a file uri the
 * truth now exists on disk). As with the open notification the close notification
 * is about managing the document's content. Receiving a close notification
 * doesn't mean that the document was open in an editor before. A close
 * notification requires a previous open notification to be sent.
 */


var DidCloseTextDocumentNotification;

(function (DidCloseTextDocumentNotification) {
  DidCloseTextDocumentNotification.method = 'textDocument/didClose';
  DidCloseTextDocumentNotification.type = new messages_1.ProtocolNotificationType(DidCloseTextDocumentNotification.method);
})(DidCloseTextDocumentNotification = exports.DidCloseTextDocumentNotification || (exports.DidCloseTextDocumentNotification = {}));
/**
 * The document save notification is sent from the client to the server when
 * the document got saved in the client.
 */


var DidSaveTextDocumentNotification;

(function (DidSaveTextDocumentNotification) {
  DidSaveTextDocumentNotification.method = 'textDocument/didSave';
  DidSaveTextDocumentNotification.type = new messages_1.ProtocolNotificationType(DidSaveTextDocumentNotification.method);
})(DidSaveTextDocumentNotification = exports.DidSaveTextDocumentNotification || (exports.DidSaveTextDocumentNotification = {}));
/**
 * Represents reasons why a text document is saved.
 */


var TextDocumentSaveReason;

(function (TextDocumentSaveReason) {
  /**
   * Manually triggered, e.g. by the user pressing save, by starting debugging,
   * or by an API call.
   */
  TextDocumentSaveReason.Manual = 1;
  /**
   * Automatic after a delay.
   */

  TextDocumentSaveReason.AfterDelay = 2;
  /**
   * When the editor lost focus.
   */

  TextDocumentSaveReason.FocusOut = 3;
})(TextDocumentSaveReason = exports.TextDocumentSaveReason || (exports.TextDocumentSaveReason = {}));
/**
 * A document will save notification is sent from the client to the server before
 * the document is actually saved.
 */


var WillSaveTextDocumentNotification;

(function (WillSaveTextDocumentNotification) {
  WillSaveTextDocumentNotification.method = 'textDocument/willSave';
  WillSaveTextDocumentNotification.type = new messages_1.ProtocolNotificationType(WillSaveTextDocumentNotification.method);
})(WillSaveTextDocumentNotification = exports.WillSaveTextDocumentNotification || (exports.WillSaveTextDocumentNotification = {}));
/**
 * A document will save request is sent from the client to the server before
 * the document is actually saved. The request can return an array of TextEdits
 * which will be applied to the text document before it is saved. Please note that
 * clients might drop results if computing the text edits took too long or if a
 * server constantly fails on this request. This is done to keep the save fast and
 * reliable.
 */


var WillSaveTextDocumentWaitUntilRequest;

(function (WillSaveTextDocumentWaitUntilRequest) {
  WillSaveTextDocumentWaitUntilRequest.method = 'textDocument/willSaveWaitUntil';
  WillSaveTextDocumentWaitUntilRequest.type = new messages_1.ProtocolRequestType(WillSaveTextDocumentWaitUntilRequest.method);
})(WillSaveTextDocumentWaitUntilRequest = exports.WillSaveTextDocumentWaitUntilRequest || (exports.WillSaveTextDocumentWaitUntilRequest = {}));
/**
 * The watched files notification is sent from the client to the server when
 * the client detects changes to file watched by the language client.
 */


var DidChangeWatchedFilesNotification;

(function (DidChangeWatchedFilesNotification) {
  DidChangeWatchedFilesNotification.type = new messages_1.ProtocolNotificationType('workspace/didChangeWatchedFiles');
})(DidChangeWatchedFilesNotification = exports.DidChangeWatchedFilesNotification || (exports.DidChangeWatchedFilesNotification = {}));
/**
 * The file event type
 */


var FileChangeType;

(function (FileChangeType) {
  /**
   * The file got created.
   */
  FileChangeType.Created = 1;
  /**
   * The file got changed.
   */

  FileChangeType.Changed = 2;
  /**
   * The file got deleted.
   */

  FileChangeType.Deleted = 3;
})(FileChangeType = exports.FileChangeType || (exports.FileChangeType = {}));

var WatchKind;

(function (WatchKind) {
  /**
   * Interested in create events.
   */
  WatchKind.Create = 1;
  /**
   * Interested in change events
   */

  WatchKind.Change = 2;
  /**
   * Interested in delete events
   */

  WatchKind.Delete = 4;
})(WatchKind = exports.WatchKind || (exports.WatchKind = {}));
/**
 * Diagnostics notification are sent from the server to the client to signal
 * results of validation runs.
 */


var PublishDiagnosticsNotification;

(function (PublishDiagnosticsNotification) {
  PublishDiagnosticsNotification.type = new messages_1.ProtocolNotificationType('textDocument/publishDiagnostics');
})(PublishDiagnosticsNotification = exports.PublishDiagnosticsNotification || (exports.PublishDiagnosticsNotification = {}));
/**
 * How a completion was triggered
 */


var CompletionTriggerKind;

(function (CompletionTriggerKind) {
  /**
   * Completion was triggered by typing an identifier (24x7 code
   * complete), manual invocation (e.g Ctrl+Space) or via API.
   */
  CompletionTriggerKind.Invoked = 1;
  /**
   * Completion was triggered by a trigger character specified by
   * the `triggerCharacters` properties of the `CompletionRegistrationOptions`.
   */

  CompletionTriggerKind.TriggerCharacter = 2;
  /**
   * Completion was re-triggered as current completion list is incomplete
   */

  CompletionTriggerKind.TriggerForIncompleteCompletions = 3;
})(CompletionTriggerKind = exports.CompletionTriggerKind || (exports.CompletionTriggerKind = {}));
/**
 * Request to request completion at a given text document position. The request's
 * parameter is of type [TextDocumentPosition](#TextDocumentPosition) the response
 * is of type [CompletionItem[]](#CompletionItem) or [CompletionList](#CompletionList)
 * or a Thenable that resolves to such.
 *
 * The request can delay the computation of the [`detail`](#CompletionItem.detail)
 * and [`documentation`](#CompletionItem.documentation) properties to the `completionItem/resolve`
 * request. However, properties that are needed for the initial sorting and filtering, like `sortText`,
 * `filterText`, `insertText`, and `textEdit`, must not be changed during resolve.
 */


var CompletionRequest;

(function (CompletionRequest) {
  CompletionRequest.method = 'textDocument/completion';
  CompletionRequest.type = new messages_1.ProtocolRequestType(CompletionRequest.method);
  /** @deprecated Use CompletionRequest.type */

  CompletionRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(CompletionRequest = exports.CompletionRequest || (exports.CompletionRequest = {}));
/**
 * Request to resolve additional information for a given completion item.The request's
 * parameter is of type [CompletionItem](#CompletionItem) the response
 * is of type [CompletionItem](#CompletionItem) or a Thenable that resolves to such.
 */


var CompletionResolveRequest;

(function (CompletionResolveRequest) {
  CompletionResolveRequest.method = 'completionItem/resolve';
  CompletionResolveRequest.type = new messages_1.ProtocolRequestType(CompletionResolveRequest.method);
})(CompletionResolveRequest = exports.CompletionResolveRequest || (exports.CompletionResolveRequest = {}));
/**
 * Request to request hover information at a given text document position. The request's
 * parameter is of type [TextDocumentPosition](#TextDocumentPosition) the response is of
 * type [Hover](#Hover) or a Thenable that resolves to such.
 */


var HoverRequest;

(function (HoverRequest) {
  HoverRequest.method = 'textDocument/hover';
  HoverRequest.type = new messages_1.ProtocolRequestType(HoverRequest.method);
})(HoverRequest = exports.HoverRequest || (exports.HoverRequest = {}));
/**
 * How a signature help was triggered.
 *
 * @since 3.15.0
 */


var SignatureHelpTriggerKind;

(function (SignatureHelpTriggerKind) {
  /**
   * Signature help was invoked manually by the user or by a command.
   */
  SignatureHelpTriggerKind.Invoked = 1;
  /**
   * Signature help was triggered by a trigger character.
   */

  SignatureHelpTriggerKind.TriggerCharacter = 2;
  /**
   * Signature help was triggered by the cursor moving or by the document content changing.
   */

  SignatureHelpTriggerKind.ContentChange = 3;
})(SignatureHelpTriggerKind = exports.SignatureHelpTriggerKind || (exports.SignatureHelpTriggerKind = {}));

var SignatureHelpRequest;

(function (SignatureHelpRequest) {
  SignatureHelpRequest.method = 'textDocument/signatureHelp';
  SignatureHelpRequest.type = new messages_1.ProtocolRequestType(SignatureHelpRequest.method);
})(SignatureHelpRequest = exports.SignatureHelpRequest || (exports.SignatureHelpRequest = {}));
/**
 * A request to resolve the definition location of a symbol at a given text
 * document position. The request's parameter is of type [TextDocumentPosition]
 * (#TextDocumentPosition) the response is of either type [Definition](#Definition)
 * or a typed array of [DefinitionLink](#DefinitionLink) or a Thenable that resolves
 * to such.
 */


var DefinitionRequest;

(function (DefinitionRequest) {
  DefinitionRequest.method = 'textDocument/definition';
  DefinitionRequest.type = new messages_1.ProtocolRequestType(DefinitionRequest.method);
  /** @deprecated Use DefinitionRequest.type */

  DefinitionRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DefinitionRequest = exports.DefinitionRequest || (exports.DefinitionRequest = {}));
/**
 * A request to resolve project-wide references for the symbol denoted
 * by the given text document position. The request's parameter is of
 * type [ReferenceParams](#ReferenceParams) the response is of type
 * [Location[]](#Location) or a Thenable that resolves to such.
 */


var ReferencesRequest;

(function (ReferencesRequest) {
  ReferencesRequest.method = 'textDocument/references';
  ReferencesRequest.type = new messages_1.ProtocolRequestType(ReferencesRequest.method);
  /** @deprecated Use ReferencesRequest.type */

  ReferencesRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(ReferencesRequest = exports.ReferencesRequest || (exports.ReferencesRequest = {}));
/**
 * Request to resolve a [DocumentHighlight](#DocumentHighlight) for a given
 * text document position. The request's parameter is of type [TextDocumentPosition]
 * (#TextDocumentPosition) the request response is of type [DocumentHighlight[]]
 * (#DocumentHighlight) or a Thenable that resolves to such.
 */


var DocumentHighlightRequest;

(function (DocumentHighlightRequest) {
  DocumentHighlightRequest.method = 'textDocument/documentHighlight';
  DocumentHighlightRequest.type = new messages_1.ProtocolRequestType(DocumentHighlightRequest.method);
  /** @deprecated Use DocumentHighlightRequest.type */

  DocumentHighlightRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DocumentHighlightRequest = exports.DocumentHighlightRequest || (exports.DocumentHighlightRequest = {}));
/**
 * A request to list all symbols found in a given text document. The request's
 * parameter is of type [TextDocumentIdentifier](#TextDocumentIdentifier) the
 * response is of type [SymbolInformation[]](#SymbolInformation) or a Thenable
 * that resolves to such.
 */


var DocumentSymbolRequest;

(function (DocumentSymbolRequest) {
  DocumentSymbolRequest.method = 'textDocument/documentSymbol';
  DocumentSymbolRequest.type = new messages_1.ProtocolRequestType(DocumentSymbolRequest.method);
  /** @deprecated Use DocumentSymbolRequest.type */

  DocumentSymbolRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DocumentSymbolRequest = exports.DocumentSymbolRequest || (exports.DocumentSymbolRequest = {}));
/**
 * A request to provide commands for the given text document and range.
 */


var CodeActionRequest;

(function (CodeActionRequest) {
  CodeActionRequest.method = 'textDocument/codeAction';
  CodeActionRequest.type = new messages_1.ProtocolRequestType(CodeActionRequest.method);
  /** @deprecated Use CodeActionRequest.type */

  CodeActionRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(CodeActionRequest = exports.CodeActionRequest || (exports.CodeActionRequest = {}));
/**
 * A request to list project-wide symbols matching the query string given
 * by the [WorkspaceSymbolParams](#WorkspaceSymbolParams). The response is
 * of type [SymbolInformation[]](#SymbolInformation) or a Thenable that
 * resolves to such.
 */


var WorkspaceSymbolRequest;

(function (WorkspaceSymbolRequest) {
  WorkspaceSymbolRequest.method = 'workspace/symbol';
  WorkspaceSymbolRequest.type = new messages_1.ProtocolRequestType(WorkspaceSymbolRequest.method);
  /** @deprecated Use WorkspaceSymbolRequest.type */

  WorkspaceSymbolRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(WorkspaceSymbolRequest = exports.WorkspaceSymbolRequest || (exports.WorkspaceSymbolRequest = {}));
/**
 * A request to provide code lens for the given text document.
 */


var CodeLensRequest;

(function (CodeLensRequest) {
  CodeLensRequest.type = new messages_1.ProtocolRequestType('textDocument/codeLens');
  /** @deprecated Use CodeLensRequest.type */

  CodeLensRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(CodeLensRequest = exports.CodeLensRequest || (exports.CodeLensRequest = {}));
/**
 * A request to resolve a command for a given code lens.
 */


var CodeLensResolveRequest;

(function (CodeLensResolveRequest) {
  CodeLensResolveRequest.type = new messages_1.ProtocolRequestType('codeLens/resolve');
})(CodeLensResolveRequest = exports.CodeLensResolveRequest || (exports.CodeLensResolveRequest = {}));
/**
 * A request to provide document links
 */


var DocumentLinkRequest;

(function (DocumentLinkRequest) {
  DocumentLinkRequest.method = 'textDocument/documentLink';
  DocumentLinkRequest.type = new messages_1.ProtocolRequestType(DocumentLinkRequest.method);
  /** @deprecated Use DocumentLinkRequest.type */

  DocumentLinkRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(DocumentLinkRequest = exports.DocumentLinkRequest || (exports.DocumentLinkRequest = {}));
/**
 * Request to resolve additional information for a given document link. The request's
 * parameter is of type [DocumentLink](#DocumentLink) the response
 * is of type [DocumentLink](#DocumentLink) or a Thenable that resolves to such.
 */


var DocumentLinkResolveRequest;

(function (DocumentLinkResolveRequest) {
  DocumentLinkResolveRequest.type = new messages_1.ProtocolRequestType('documentLink/resolve');
})(DocumentLinkResolveRequest = exports.DocumentLinkResolveRequest || (exports.DocumentLinkResolveRequest = {}));
/**
 * A request to to format a whole document.
 */


var DocumentFormattingRequest;

(function (DocumentFormattingRequest) {
  DocumentFormattingRequest.method = 'textDocument/formatting';
  DocumentFormattingRequest.type = new messages_1.ProtocolRequestType(DocumentFormattingRequest.method);
})(DocumentFormattingRequest = exports.DocumentFormattingRequest || (exports.DocumentFormattingRequest = {}));
/**
 * A request to to format a range in a document.
 */


var DocumentRangeFormattingRequest;

(function (DocumentRangeFormattingRequest) {
  DocumentRangeFormattingRequest.method = 'textDocument/rangeFormatting';
  DocumentRangeFormattingRequest.type = new messages_1.ProtocolRequestType(DocumentRangeFormattingRequest.method);
})(DocumentRangeFormattingRequest = exports.DocumentRangeFormattingRequest || (exports.DocumentRangeFormattingRequest = {}));
/**
 * A request to format a document on type.
 */


var DocumentOnTypeFormattingRequest;

(function (DocumentOnTypeFormattingRequest) {
  DocumentOnTypeFormattingRequest.method = 'textDocument/onTypeFormatting';
  DocumentOnTypeFormattingRequest.type = new messages_1.ProtocolRequestType(DocumentOnTypeFormattingRequest.method);
})(DocumentOnTypeFormattingRequest = exports.DocumentOnTypeFormattingRequest || (exports.DocumentOnTypeFormattingRequest = {}));
/**
 * A request to rename a symbol.
 */


var RenameRequest;

(function (RenameRequest) {
  RenameRequest.method = 'textDocument/rename';
  RenameRequest.type = new messages_1.ProtocolRequestType(RenameRequest.method);
})(RenameRequest = exports.RenameRequest || (exports.RenameRequest = {}));
/**
 * A request to test and perform the setup necessary for a rename.
 */


var PrepareRenameRequest;

(function (PrepareRenameRequest) {
  PrepareRenameRequest.method = 'textDocument/prepareRename';
  PrepareRenameRequest.type = new messages_1.ProtocolRequestType(PrepareRenameRequest.method);
})(PrepareRenameRequest = exports.PrepareRenameRequest || (exports.PrepareRenameRequest = {}));
/**
 * A request send from the client to the server to execute a command. The request might return
 * a workspace edit which the client will apply to the workspace.
 */


var ExecuteCommandRequest;

(function (ExecuteCommandRequest) {
  ExecuteCommandRequest.type = new messages_1.ProtocolRequestType('workspace/executeCommand');
})(ExecuteCommandRequest = exports.ExecuteCommandRequest || (exports.ExecuteCommandRequest = {}));
/**
 * A request sent from the server to the client to modified certain resources.
 */


var ApplyWorkspaceEditRequest;

(function (ApplyWorkspaceEditRequest) {
  ApplyWorkspaceEditRequest.type = new messages_1.ProtocolRequestType('workspace/applyEdit');
})(ApplyWorkspaceEditRequest = exports.ApplyWorkspaceEditRequest || (exports.ApplyWorkspaceEditRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.progress.js":
/*!****************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.progress.js ***!
  \****************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");

var WorkDoneProgress;

(function (WorkDoneProgress) {
  WorkDoneProgress.type = new vscode_jsonrpc_1.ProgressType();
})(WorkDoneProgress = exports.WorkDoneProgress || (exports.WorkDoneProgress = {}));
/**
 * The `window/workDoneProgress/create` request is sent from the server to the client to initiate progress
 * reporting from the server.
 */


var WorkDoneProgressCreateRequest;

(function (WorkDoneProgressCreateRequest) {
  WorkDoneProgressCreateRequest.type = new messages_1.ProtocolRequestType('window/workDoneProgress/create');
})(WorkDoneProgressCreateRequest = exports.WorkDoneProgressCreateRequest || (exports.WorkDoneProgressCreateRequest = {}));
/**
 * The `window/workDoneProgress/cancel` notification is sent from  the client to the server to cancel a progress
 * initiated on the server side.
 */


var WorkDoneProgressCancelNotification;

(function (WorkDoneProgressCancelNotification) {
  WorkDoneProgressCancelNotification.type = new messages_1.ProtocolNotificationType('window/workDoneProgress/cancel');
})(WorkDoneProgressCancelNotification = exports.WorkDoneProgressCancelNotification || (exports.WorkDoneProgressCancelNotification = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.progress'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.selectionRange.js":
/*!**********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.selectionRange.js ***!
  \**********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * A request to provide selection ranges in a document. The request's
 * parameter is of type [SelectionRangeParams](#SelectionRangeParams), the
 * response is of type [SelectionRange[]](#SelectionRange[]) or a Thenable
 * that resolves to such.
 */


var SelectionRangeRequest;

(function (SelectionRangeRequest) {
  SelectionRangeRequest.method = 'textDocument/selectionRange';
  SelectionRangeRequest.type = new messages_1.ProtocolRequestType(SelectionRangeRequest.method);
  /** @deprecated  Use SelectionRangeRequest.type */

  SelectionRangeRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(SelectionRangeRequest = exports.SelectionRangeRequest || (exports.SelectionRangeRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.selectionRange'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.sematicTokens.proposed.js":
/*!******************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.sematicTokens.proposed.js ***!
  \******************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * A set of predefined token types. This set is not fixed
 * an clients can specify additional token types via the
 * corresponding client capabilities.
 *
 * @since 3.16.0 - Proposed state
 */


var SemanticTokenTypes;

(function (SemanticTokenTypes) {
  SemanticTokenTypes["comment"] = "comment";
  SemanticTokenTypes["keyword"] = "keyword";
  SemanticTokenTypes["string"] = "string";
  SemanticTokenTypes["number"] = "number";
  SemanticTokenTypes["regexp"] = "regexp";
  SemanticTokenTypes["operator"] = "operator";
  SemanticTokenTypes["namespace"] = "namespace";
  SemanticTokenTypes["type"] = "type";
  SemanticTokenTypes["struct"] = "struct";
  SemanticTokenTypes["class"] = "class";
  SemanticTokenTypes["interface"] = "interface";
  SemanticTokenTypes["enum"] = "enum";
  SemanticTokenTypes["typeParameter"] = "typeParameter";
  SemanticTokenTypes["function"] = "function";
  SemanticTokenTypes["member"] = "member";
  SemanticTokenTypes["property"] = "property";
  SemanticTokenTypes["marco"] = "marco";
  SemanticTokenTypes["variable"] = "variable";
  SemanticTokenTypes["parameter"] = "parameter";
  SemanticTokenTypes["label"] = "label";
})(SemanticTokenTypes = exports.SemanticTokenTypes || (exports.SemanticTokenTypes = {}));
/**
 * A set of predefined token modifiers. This set is not fixed
 * an clients can specify additional token types via the
 * corresponding client capabilities.
 *
 * @since 3.16.0 - Proposed state
 */


var SemanticTokenModifiers;

(function (SemanticTokenModifiers) {
  SemanticTokenModifiers["documentation"] = "documentation";
  SemanticTokenModifiers["declaration"] = "declaration";
  SemanticTokenModifiers["definition"] = "definition";
  SemanticTokenModifiers["reference"] = "reference";
  SemanticTokenModifiers["static"] = "static";
  SemanticTokenModifiers["abstract"] = "abstract";
  SemanticTokenModifiers["deprecated"] = "deprected";
  SemanticTokenModifiers["async"] = "async";
  SemanticTokenModifiers["volatile"] = "volatile";
  SemanticTokenModifiers["final"] = "final";
})(SemanticTokenModifiers = exports.SemanticTokenModifiers || (exports.SemanticTokenModifiers = {}));
/**
 * @since 3.16.0 - Proposed state
 */


var SemanticTokens;

(function (SemanticTokens) {
  function is(value) {
    var candidate = value;
    return candidate !== undefined && (candidate.resultId === undefined || typeof candidate.resultId === 'string') && Array.isArray(candidate.data) && (candidate.data.length === 0 || typeof candidate.data[0] === 'number');
  }

  SemanticTokens.is = is;
})(SemanticTokens = exports.SemanticTokens || (exports.SemanticTokens = {}));
/**
 * @since 3.16.0 - Proposed state
 */


var SemanticTokensRequest;

(function (SemanticTokensRequest) {
  SemanticTokensRequest.method = 'textDocument/semanticTokens';
  SemanticTokensRequest.type = new messages_1.ProtocolRequestType(SemanticTokensRequest.method);
})(SemanticTokensRequest = exports.SemanticTokensRequest || (exports.SemanticTokensRequest = {}));
/**
 * @since 3.16.0 - Proposed state
 */


var SemanticTokensEditsRequest;

(function (SemanticTokensEditsRequest) {
  SemanticTokensEditsRequest.method = 'textDocument/semanticTokens/edits';
  SemanticTokensEditsRequest.type = new messages_1.ProtocolRequestType(SemanticTokensEditsRequest.method);
})(SemanticTokensEditsRequest = exports.SemanticTokensEditsRequest || (exports.SemanticTokensEditsRequest = {}));
/**
 * @since 3.16.0 - Proposed state
 */


var SemanticTokensRangeRequest;

(function (SemanticTokensRangeRequest) {
  SemanticTokensRangeRequest.method = 'textDocument/semanticTokens/range';
  SemanticTokensRangeRequest.type = new messages_1.ProtocolRequestType(SemanticTokensRangeRequest.method);
})(SemanticTokensRangeRequest = exports.SemanticTokensRangeRequest || (exports.SemanticTokensRangeRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.sematicTokens.proposed'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.typeDefinition.js":
/*!**********************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.typeDefinition.js ***!
  \**********************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var vscode_jsonrpc_1 = __webpack_require__(/*! vscode-jsonrpc */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js"); // @ts-ignore: to avoid inlining LocatioLink as dynamic import


var __noDynamicImport;
/**
 * A request to resolve the type definition locations of a symbol at a given text
 * document position. The request's parameter is of type [TextDocumentPositioParams]
 * (#TextDocumentPositionParams) the response is of type [Definition](#Definition) or a
 * Thenable that resolves to such.
 */


var TypeDefinitionRequest;

(function (TypeDefinitionRequest) {
  TypeDefinitionRequest.method = 'textDocument/typeDefinition';
  TypeDefinitionRequest.type = new messages_1.ProtocolRequestType(TypeDefinitionRequest.method);
  /** @deprecated Use TypeDefinitionRequest.type */

  TypeDefinitionRequest.resultType = new vscode_jsonrpc_1.ProgressType();
})(TypeDefinitionRequest = exports.TypeDefinitionRequest || (exports.TypeDefinitionRequest = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.typeDefinition'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/protocol.workspaceFolders.js":
/*!************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/protocol.workspaceFolders.js ***!
  \************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/lib/messages.js");
/**
 * The `workspace/workspaceFolders` is sent from the server to the client to fetch the open workspace folders.
 */


var WorkspaceFoldersRequest;

(function (WorkspaceFoldersRequest) {
  WorkspaceFoldersRequest.type = new messages_1.ProtocolRequestType0('workspace/workspaceFolders');
})(WorkspaceFoldersRequest = exports.WorkspaceFoldersRequest || (exports.WorkspaceFoldersRequest = {}));
/**
 * The `workspace/didChangeWorkspaceFolders` notification is sent from the client to the server when the workspace
 * folder configuration changes.
 */


var DidChangeWorkspaceFoldersNotification;

(function (DidChangeWorkspaceFoldersNotification) {
  DidChangeWorkspaceFoldersNotification.type = new messages_1.ProtocolNotificationType('workspace/didChangeWorkspaceFolders');
})(DidChangeWorkspaceFoldersNotification = exports.DidChangeWorkspaceFoldersNotification || (exports.DidChangeWorkspaceFoldersNotification = {}));

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/protocol.workspaceFolders'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/lib/utils/is.js":
/*!*******************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/lib/utils/is.js ***!
  \*******************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _typeof = __webpack_require__(/*! @babel/runtime/helpers/typeof */ "../../node_modules/@babel/runtime/helpers/typeof.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

function _boolean(value) {
  return value === true || value === false;
}

exports["boolean"] = _boolean;

function string(value) {
  return typeof value === 'string' || value instanceof String;
}

exports.string = string;

function number(value) {
  return typeof value === 'number' || value instanceof Number;
}

exports.number = number;

function error(value) {
  return value instanceof Error;
}

exports.error = error;

function func(value) {
  return typeof value === 'function';
}

exports.func = func;

function array(value) {
  return Array.isArray(value);
}

exports.array = array;

function stringArray(value) {
  return array(value) && value.every(function (elem) {
    return string(elem);
  });
}

exports.stringArray = stringArray;

function typedArray(value, check) {
  return Array.isArray(value) && value.every(check);
}

exports.typedArray = typedArray;

function objectLiteral(value) {
  // Strictly speaking class instances pass this check as well. Since the LSP
  // doesn't use classes we ignore this for now. If we do we need to add something
  // like this: `Object.getPrototypeOf(Object.getPrototypeOf(x)) === null`
  return value !== null && _typeof(value) === 'object';
}

exports.objectLiteral = objectLiteral;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/lib/utils/is'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/cancellation.js":
/*!***************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/cancellation.js ***!
  \***************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var events_1 = __webpack_require__(/*! ./events */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js");

var Is = __webpack_require__(/*! ./is */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js");

var CancellationToken;

(function (CancellationToken) {
  CancellationToken.None = Object.freeze({
    isCancellationRequested: false,
    onCancellationRequested: events_1.Event.None
  });
  CancellationToken.Cancelled = Object.freeze({
    isCancellationRequested: true,
    onCancellationRequested: events_1.Event.None
  });

  function is(value) {
    var candidate = value;
    return candidate && (candidate === CancellationToken.None || candidate === CancellationToken.Cancelled || Is["boolean"](candidate.isCancellationRequested) && !!candidate.onCancellationRequested);
  }

  CancellationToken.is = is;
})(CancellationToken = exports.CancellationToken || (exports.CancellationToken = {}));

var shortcutEvent = Object.freeze(function (callback, context) {
  var handle = setTimeout(callback.bind(context), 0);
  return {
    dispose: function dispose() {
      clearTimeout(handle);
    }
  };
});

var MutableToken =
/*#__PURE__*/
function () {
  function MutableToken() {
    _classCallCheck(this, MutableToken);

    this._isCancelled = false;
  }

  _createClass(MutableToken, [{
    key: "cancel",
    value: function cancel() {
      if (!this._isCancelled) {
        this._isCancelled = true;

        if (this._emitter) {
          this._emitter.fire(undefined);

          this.dispose();
        }
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      if (this._emitter) {
        this._emitter.dispose();

        this._emitter = undefined;
      }
    }
  }, {
    key: "isCancellationRequested",
    get: function get() {
      return this._isCancelled;
    }
  }, {
    key: "onCancellationRequested",
    get: function get() {
      if (this._isCancelled) {
        return shortcutEvent;
      }

      if (!this._emitter) {
        this._emitter = new events_1.Emitter();
      }

      return this._emitter.event;
    }
  }]);

  return MutableToken;
}();

var CancellationTokenSource =
/*#__PURE__*/
function () {
  function CancellationTokenSource() {
    _classCallCheck(this, CancellationTokenSource);
  }

  _createClass(CancellationTokenSource, [{
    key: "cancel",
    value: function cancel() {
      if (!this._token) {
        // save an object by returning the default
        // cancelled token when cancellation happens
        // before someone asks for the token
        this._token = CancellationToken.Cancelled;
      } else {
        this._token.cancel();
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      if (!this._token) {
        // ensure to initialize with an empty token if we had none
        this._token = CancellationToken.None;
      } else if (this._token instanceof MutableToken) {
        // actually dispose
        this._token.dispose();
      }
    }
  }, {
    key: "token",
    get: function get() {
      if (!this._token) {
        // be lazy and create the token only when
        // actually needed
        this._token = new MutableToken();
      }

      return this._token;
    }
  }]);

  return CancellationTokenSource;
}();

exports.CancellationTokenSource = CancellationTokenSource;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/cancellation'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js":
/*!*********************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js ***!
  \*********************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
var Disposable;

(function (Disposable) {
  function create(func) {
    return {
      dispose: func
    };
  }

  Disposable.create = create;
})(Disposable = exports.Disposable || (exports.Disposable = {}));

var Event;

(function (Event) {
  var _disposable = {
    dispose: function dispose() {}
  };

  Event.None = function () {
    return _disposable;
  };
})(Event = exports.Event || (exports.Event = {}));

var CallbackList =
/*#__PURE__*/
function () {
  function CallbackList() {
    _classCallCheck(this, CallbackList);
  }

  _createClass(CallbackList, [{
    key: "add",
    value: function add(callback) {
      var _this = this;

      var context = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : null;
      var bucket = arguments.length > 2 ? arguments[2] : undefined;

      if (!this._callbacks) {
        this._callbacks = [];
        this._contexts = [];
      }

      this._callbacks.push(callback);

      this._contexts.push(context);

      if (Array.isArray(bucket)) {
        bucket.push({
          dispose: function dispose() {
            return _this.remove(callback, context);
          }
        });
      }
    }
  }, {
    key: "remove",
    value: function remove(callback) {
      var context = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : null;

      if (!this._callbacks) {
        return;
      }

      var foundCallbackWithDifferentContext = false;

      for (var i = 0, len = this._callbacks.length; i < len; i++) {
        if (this._callbacks[i] === callback) {
          if (this._contexts[i] === context) {
            // callback & context match => remove it
            this._callbacks.splice(i, 1);

            this._contexts.splice(i, 1);

            return;
          } else {
            foundCallbackWithDifferentContext = true;
          }
        }
      }

      if (foundCallbackWithDifferentContext) {
        throw new Error('When adding a listener with a context, you should remove it with the same context');
      }
    }
  }, {
    key: "invoke",
    value: function invoke() {
      if (!this._callbacks) {
        return [];
      }

      var ret = [],
          callbacks = this._callbacks.slice(0),
          contexts = this._contexts.slice(0);

      for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
        args[_key] = arguments[_key];
      }

      for (var i = 0, len = callbacks.length; i < len; i++) {
        try {
          ret.push(callbacks[i].apply(contexts[i], args));
        } catch (e) {
          // eslint-disable-next-line no-console
          console.error(e);
        }
      }

      return ret;
    }
  }, {
    key: "isEmpty",
    value: function isEmpty() {
      return !this._callbacks || this._callbacks.length === 0;
    }
  }, {
    key: "dispose",
    value: function dispose() {
      this._callbacks = undefined;
      this._contexts = undefined;
    }
  }]);

  return CallbackList;
}();

var Emitter =
/*#__PURE__*/
function () {
  function Emitter(_options) {
    _classCallCheck(this, Emitter);

    this._options = _options;
  }
  /**
   * For the public to allow to subscribe
   * to events from this Emitter
   */


  _createClass(Emitter, [{
    key: "fire",

    /**
     * To be kept private to fire an event to
     * subscribers
     */
    value: function fire(event) {
      if (this._callbacks) {
        this._callbacks.invoke.call(this._callbacks, event);
      }
    }
  }, {
    key: "dispose",
    value: function dispose() {
      if (this._callbacks) {
        this._callbacks.dispose();

        this._callbacks = undefined;
      }
    }
  }, {
    key: "event",
    get: function get() {
      var _this2 = this;

      if (!this._event) {
        this._event = function (listener, thisArgs, disposables) {
          if (!_this2._callbacks) {
            _this2._callbacks = new CallbackList();
          }

          if (_this2._options && _this2._options.onFirstListenerAdd && _this2._callbacks.isEmpty()) {
            _this2._options.onFirstListenerAdd(_this2);
          }

          _this2._callbacks.add(listener, thisArgs);

          var result;
          result = {
            dispose: function dispose() {
              _this2._callbacks.remove(listener, thisArgs);

              result.dispose = Emitter._noop;

              if (_this2._options && _this2._options.onLastListenerRemove && _this2._callbacks.isEmpty()) {
                _this2._options.onLastListenerRemove(_this2);
              }
            }
          };

          if (Array.isArray(disposables)) {
            disposables.push(result);
          }

          return result;
        };
      }

      return this._event;
    }
  }]);

  return Emitter;
}();

exports.Emitter = Emitter;

Emitter._noop = function () {};

if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js":
/*!*****************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js ***!
  \*****************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

function _boolean(value) {
  return value === true || value === false;
}

exports["boolean"] = _boolean;

function string(value) {
  return typeof value === 'string' || value instanceof String;
}

exports.string = string;

function number(value) {
  return typeof value === 'number' || value instanceof Number;
}

exports.number = number;

function error(value) {
  return value instanceof Error;
}

exports.error = error;

function func(value) {
  return typeof value === 'function';
}

exports.func = func;

function array(value) {
  return Array.isArray(value);
}

exports.array = array;

function stringArray(value) {
  return array(value) && value.every(function (elem) {
    return string(elem);
  });
}

exports.stringArray = stringArray;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/linkedMap.js":
/*!************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/linkedMap.js ***!
  \************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
var Touch;

(function (Touch) {
  Touch.None = 0;
  Touch.First = 1;
  Touch.Last = 2;
})(Touch = exports.Touch || (exports.Touch = {}));

var LinkedMap =
/*#__PURE__*/
function () {
  function LinkedMap() {
    _classCallCheck(this, LinkedMap);

    this._map = new Map();
    this._head = undefined;
    this._tail = undefined;
    this._size = 0;
  }

  _createClass(LinkedMap, [{
    key: "clear",
    value: function clear() {
      this._map.clear();

      this._head = undefined;
      this._tail = undefined;
      this._size = 0;
    }
  }, {
    key: "isEmpty",
    value: function isEmpty() {
      return !this._head && !this._tail;
    }
  }, {
    key: "has",
    value: function has(key) {
      return this._map.has(key);
    }
  }, {
    key: "get",
    value: function get(key) {
      var item = this._map.get(key);

      if (!item) {
        return undefined;
      }

      return item.value;
    }
  }, {
    key: "set",
    value: function set(key, value) {
      var touch = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : Touch.None;

      var item = this._map.get(key);

      if (item) {
        item.value = value;

        if (touch !== Touch.None) {
          this.touch(item, touch);
        }
      } else {
        item = {
          key: key,
          value: value,
          next: undefined,
          previous: undefined
        };

        switch (touch) {
          case Touch.None:
            this.addItemLast(item);
            break;

          case Touch.First:
            this.addItemFirst(item);
            break;

          case Touch.Last:
            this.addItemLast(item);
            break;

          default:
            this.addItemLast(item);
            break;
        }

        this._map.set(key, item);

        this._size++;
      }
    }
  }, {
    key: "delete",
    value: function _delete(key) {
      var item = this._map.get(key);

      if (!item) {
        return false;
      }

      this._map["delete"](key);

      this.removeItem(item);
      this._size--;
      return true;
    }
  }, {
    key: "shift",
    value: function shift() {
      if (!this._head && !this._tail) {
        return undefined;
      }

      if (!this._head || !this._tail) {
        throw new Error('Invalid list');
      }

      var item = this._head;

      this._map["delete"](item.key);

      this.removeItem(item);
      this._size--;
      return item.value;
    }
  }, {
    key: "forEach",
    value: function forEach(callbackfn, thisArg) {
      var current = this._head;

      while (current) {
        if (thisArg) {
          callbackfn.bind(thisArg)(current.value, current.key, this);
        } else {
          callbackfn(current.value, current.key, this);
        }

        current = current.next;
      }
    }
  }, {
    key: "forEachReverse",
    value: function forEachReverse(callbackfn, thisArg) {
      var current = this._tail;

      while (current) {
        if (thisArg) {
          callbackfn.bind(thisArg)(current.value, current.key, this);
        } else {
          callbackfn(current.value, current.key, this);
        }

        current = current.previous;
      }
    }
  }, {
    key: "values",
    value: function values() {
      var result = [];
      var current = this._head;

      while (current) {
        result.push(current.value);
        current = current.next;
      }

      return result;
    }
  }, {
    key: "keys",
    value: function keys() {
      var result = [];
      var current = this._head;

      while (current) {
        result.push(current.key);
        current = current.next;
      }

      return result;
    }
    /* JSON RPC run on es5 which has no Symbol.iterator
    public keys(): IterableIterator<K> {
        let current = this._head;
        let iterator: IterableIterator<K> = {
            [Symbol.iterator]() {
                return iterator;
            },
            next():IteratorResult<K> {
                if (current) {
                    let result = { value: current.key, done: false };
                    current = current.next;
                    return result;
                } else {
                    return { value: undefined, done: true };
                }
            }
        };
        return iterator;
    }
      public values(): IterableIterator<V> {
        let current = this._head;
        let iterator: IterableIterator<V> = {
            [Symbol.iterator]() {
                return iterator;
            },
            next():IteratorResult<V> {
                if (current) {
                    let result = { value: current.value, done: false };
                    current = current.next;
                    return result;
                } else {
                    return { value: undefined, done: true };
                }
            }
        };
        return iterator;
    }
    */

  }, {
    key: "addItemFirst",
    value: function addItemFirst(item) {
      // First time Insert
      if (!this._head && !this._tail) {
        this._tail = item;
      } else if (!this._head) {
        throw new Error('Invalid list');
      } else {
        item.next = this._head;
        this._head.previous = item;
      }

      this._head = item;
    }
  }, {
    key: "addItemLast",
    value: function addItemLast(item) {
      // First time Insert
      if (!this._head && !this._tail) {
        this._head = item;
      } else if (!this._tail) {
        throw new Error('Invalid list');
      } else {
        item.previous = this._tail;
        this._tail.next = item;
      }

      this._tail = item;
    }
  }, {
    key: "removeItem",
    value: function removeItem(item) {
      if (item === this._head && item === this._tail) {
        this._head = undefined;
        this._tail = undefined;
      } else if (item === this._head) {
        this._head = item.next;
      } else if (item === this._tail) {
        this._tail = item.previous;
      } else {
        var next = item.next;
        var previous = item.previous;

        if (!next || !previous) {
          throw new Error('Invalid list');
        }

        next.previous = previous;
        previous.next = next;
      }
    }
  }, {
    key: "touch",
    value: function touch(item, _touch) {
      if (!this._head || !this._tail) {
        throw new Error('Invalid list');
      }

      if (_touch !== Touch.First && _touch !== Touch.Last) {
        return;
      }

      if (_touch === Touch.First) {
        if (item === this._head) {
          return;
        }

        var next = item.next;
        var previous = item.previous; // Unlink the item

        if (item === this._tail) {
          // previous must be defined since item was not head but is tail
          // So there are more than on item in the map
          previous.next = undefined;
          this._tail = previous;
        } else {
          // Both next and previous are not undefined since item was neither head nor tail.
          next.previous = previous;
          previous.next = next;
        } // Insert the node at head


        item.previous = undefined;
        item.next = this._head;
        this._head.previous = item;
        this._head = item;
      } else if (_touch === Touch.Last) {
        if (item === this._tail) {
          return;
        }

        var _next = item.next;
        var _previous = item.previous; // Unlink the item.

        if (item === this._head) {
          // next must be defined since item was not tail but is head
          // So there are more than on item in the map
          _next.previous = undefined;
          this._head = _next;
        } else {
          // Both next and previous are not undefined since item was neither head nor tail.
          _next.previous = _previous;
          _previous.next = _next;
        }

        item.next = undefined;
        item.previous = this._tail;
        this._tail.next = item;
        this._tail = item;
      }
    }
  }, {
    key: "size",
    get: function get() {
      return this._size;
    }
  }]);

  return LinkedMap;
}();

exports.LinkedMap = LinkedMap;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/linkedMap'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js":
/*!*******************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main.js ***!
  \*******************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(setImmediate, global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */
/// <reference path="../typings/thenable.d.ts" />


var _toConsumableArray = __webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "../../node_modules/@babel/runtime/helpers/toConsumableArray.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _assertThisInitialized = __webpack_require__(/*! @babel/runtime/helpers/assertThisInitialized */ "../../node_modules/@babel/runtime/helpers/assertThisInitialized.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _wrapNativeSuper = __webpack_require__(/*! @babel/runtime/helpers/wrapNativeSuper */ "../../node_modules/@babel/runtime/helpers/wrapNativeSuper.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

function __export(m) {
  for (var p in m) {
    if (!exports.hasOwnProperty(p)) exports[p] = m[p];
  }
}

Object.defineProperty(exports, "__esModule", {
  value: true
});

var Is = __webpack_require__(/*! ./is */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js");

var messages_1 = __webpack_require__(/*! ./messages */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messages.js");

exports.RequestType = messages_1.RequestType;
exports.RequestType0 = messages_1.RequestType0;
exports.RequestType1 = messages_1.RequestType1;
exports.RequestType2 = messages_1.RequestType2;
exports.RequestType3 = messages_1.RequestType3;
exports.RequestType4 = messages_1.RequestType4;
exports.RequestType5 = messages_1.RequestType5;
exports.RequestType6 = messages_1.RequestType6;
exports.RequestType7 = messages_1.RequestType7;
exports.RequestType8 = messages_1.RequestType8;
exports.RequestType9 = messages_1.RequestType9;
exports.ResponseError = messages_1.ResponseError;
exports.ErrorCodes = messages_1.ErrorCodes;
exports.NotificationType = messages_1.NotificationType;
exports.NotificationType0 = messages_1.NotificationType0;
exports.NotificationType1 = messages_1.NotificationType1;
exports.NotificationType2 = messages_1.NotificationType2;
exports.NotificationType3 = messages_1.NotificationType3;
exports.NotificationType4 = messages_1.NotificationType4;
exports.NotificationType5 = messages_1.NotificationType5;
exports.NotificationType6 = messages_1.NotificationType6;
exports.NotificationType7 = messages_1.NotificationType7;
exports.NotificationType8 = messages_1.NotificationType8;
exports.NotificationType9 = messages_1.NotificationType9;

var messageReader_1 = __webpack_require__(/*! ./messageReader */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader.js");

exports.MessageReader = messageReader_1.MessageReader;
exports.StreamMessageReader = messageReader_1.StreamMessageReader;
exports.IPCMessageReader = messageReader_1.IPCMessageReader;
exports.SocketMessageReader = messageReader_1.SocketMessageReader;

var messageWriter_1 = __webpack_require__(/*! ./messageWriter */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter.js");

exports.MessageWriter = messageWriter_1.MessageWriter;
exports.StreamMessageWriter = messageWriter_1.StreamMessageWriter;
exports.IPCMessageWriter = messageWriter_1.IPCMessageWriter;
exports.SocketMessageWriter = messageWriter_1.SocketMessageWriter;

var events_1 = __webpack_require__(/*! ./events */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js");

exports.Disposable = events_1.Disposable;
exports.Event = events_1.Event;
exports.Emitter = events_1.Emitter;

var cancellation_1 = __webpack_require__(/*! ./cancellation */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/cancellation.js");

exports.CancellationTokenSource = cancellation_1.CancellationTokenSource;
exports.CancellationToken = cancellation_1.CancellationToken;

var linkedMap_1 = __webpack_require__(/*! ./linkedMap */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/linkedMap.js");

__export(__webpack_require__(/*! ./pipeSupport */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/pipeSupport.js"));

__export(__webpack_require__(/*! ./socketSupport */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/socketSupport.js"));

var CancelNotification;

(function (CancelNotification) {
  CancelNotification.type = new messages_1.NotificationType('$/cancelRequest');
})(CancelNotification || (CancelNotification = {}));

var ProgressNotification;

(function (ProgressNotification) {
  ProgressNotification.type = new messages_1.NotificationType('$/progress');
})(ProgressNotification || (ProgressNotification = {}));

var ProgressType = function ProgressType() {
  _classCallCheck(this, ProgressType);
};

exports.ProgressType = ProgressType;
exports.NullLogger = Object.freeze({
  error: function error() {},
  warn: function warn() {},
  info: function info() {},
  log: function log() {}
});
var Trace;

(function (Trace) {
  Trace[Trace["Off"] = 0] = "Off";
  Trace[Trace["Messages"] = 1] = "Messages";
  Trace[Trace["Verbose"] = 2] = "Verbose";
})(Trace = exports.Trace || (exports.Trace = {}));

(function (Trace) {
  function fromString(value) {
    if (!Is.string(value)) {
      return Trace.Off;
    }

    value = value.toLowerCase();

    switch (value) {
      case 'off':
        return Trace.Off;

      case 'messages':
        return Trace.Messages;

      case 'verbose':
        return Trace.Verbose;

      default:
        return Trace.Off;
    }
  }

  Trace.fromString = fromString;

  function toString(value) {
    switch (value) {
      case Trace.Off:
        return 'off';

      case Trace.Messages:
        return 'messages';

      case Trace.Verbose:
        return 'verbose';

      default:
        return 'off';
    }
  }

  Trace.toString = toString;
})(Trace = exports.Trace || (exports.Trace = {}));

var TraceFormat;

(function (TraceFormat) {
  TraceFormat["Text"] = "text";
  TraceFormat["JSON"] = "json";
})(TraceFormat = exports.TraceFormat || (exports.TraceFormat = {}));

(function (TraceFormat) {
  function fromString(value) {
    value = value.toLowerCase();

    if (value === 'json') {
      return TraceFormat.JSON;
    } else {
      return TraceFormat.Text;
    }
  }

  TraceFormat.fromString = fromString;
})(TraceFormat = exports.TraceFormat || (exports.TraceFormat = {}));

var SetTraceNotification;

(function (SetTraceNotification) {
  SetTraceNotification.type = new messages_1.NotificationType('$/setTraceNotification');
})(SetTraceNotification = exports.SetTraceNotification || (exports.SetTraceNotification = {}));

var LogTraceNotification;

(function (LogTraceNotification) {
  LogTraceNotification.type = new messages_1.NotificationType('$/logTraceNotification');
})(LogTraceNotification = exports.LogTraceNotification || (exports.LogTraceNotification = {}));

var ConnectionErrors;

(function (ConnectionErrors) {
  /**
   * The connection is closed.
   */
  ConnectionErrors[ConnectionErrors["Closed"] = 1] = "Closed";
  /**
   * The connection got disposed.
   */

  ConnectionErrors[ConnectionErrors["Disposed"] = 2] = "Disposed";
  /**
   * The connection is already in listening mode.
   */

  ConnectionErrors[ConnectionErrors["AlreadyListening"] = 3] = "AlreadyListening";
})(ConnectionErrors = exports.ConnectionErrors || (exports.ConnectionErrors = {}));

var ConnectionError =
/*#__PURE__*/
function (_Error) {
  _inherits(ConnectionError, _Error);

  function ConnectionError(code, message) {
    var _this;

    _classCallCheck(this, ConnectionError);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(ConnectionError).call(this, message));
    _this.code = code;
    Object.setPrototypeOf(_assertThisInitialized(_this), ConnectionError.prototype);
    return _this;
  }

  return ConnectionError;
}(_wrapNativeSuper(Error));

exports.ConnectionError = ConnectionError;
var ConnectionStrategy;

(function (ConnectionStrategy) {
  function is(value) {
    var candidate = value;
    return candidate && Is.func(candidate.cancelUndispatched);
  }

  ConnectionStrategy.is = is;
})(ConnectionStrategy = exports.ConnectionStrategy || (exports.ConnectionStrategy = {}));

var ConnectionState;

(function (ConnectionState) {
  ConnectionState[ConnectionState["New"] = 1] = "New";
  ConnectionState[ConnectionState["Listening"] = 2] = "Listening";
  ConnectionState[ConnectionState["Closed"] = 3] = "Closed";
  ConnectionState[ConnectionState["Disposed"] = 4] = "Disposed";
})(ConnectionState || (ConnectionState = {}));

function _createMessageConnection(messageReader, messageWriter, logger, strategy) {
  var sequenceNumber = 0;
  var notificationSquenceNumber = 0;
  var unknownResponseSquenceNumber = 0;
  var version = '2.0';
  var starRequestHandler = undefined;
  var requestHandlers = Object.create(null);
  var starNotificationHandler = undefined;
  var notificationHandlers = Object.create(null);
  var progressHandlers = new Map();
  var timer;
  var messageQueue = new linkedMap_1.LinkedMap();
  var responsePromises = Object.create(null);
  var requestTokens = Object.create(null);
  var _trace = Trace.Off;
  var traceFormat = TraceFormat.Text;
  var tracer;
  var state = ConnectionState.New;
  var errorEmitter = new events_1.Emitter();
  var closeEmitter = new events_1.Emitter();
  var unhandledNotificationEmitter = new events_1.Emitter();
  var unhandledProgressEmitter = new events_1.Emitter();
  var disposeEmitter = new events_1.Emitter();

  function createRequestQueueKey(id) {
    return 'req-' + id.toString();
  }

  function createResponseQueueKey(id) {
    if (id === null) {
      return 'res-unknown-' + (++unknownResponseSquenceNumber).toString();
    } else {
      return 'res-' + id.toString();
    }
  }

  function createNotificationQueueKey() {
    return 'not-' + (++notificationSquenceNumber).toString();
  }

  function addMessageToQueue(queue, message) {
    if (messages_1.isRequestMessage(message)) {
      queue.set(createRequestQueueKey(message.id), message);
    } else if (messages_1.isResponseMessage(message)) {
      queue.set(createResponseQueueKey(message.id), message);
    } else {
      queue.set(createNotificationQueueKey(), message);
    }
  }

  function cancelUndispatched(_message) {
    return undefined;
  }

  function isListening() {
    return state === ConnectionState.Listening;
  }

  function isClosed() {
    return state === ConnectionState.Closed;
  }

  function isDisposed() {
    return state === ConnectionState.Disposed;
  }

  function closeHandler() {
    if (state === ConnectionState.New || state === ConnectionState.Listening) {
      state = ConnectionState.Closed;
      closeEmitter.fire(undefined);
    } // If the connection is disposed don't sent close events.

  }

  function readErrorHandler(error) {
    errorEmitter.fire([error, undefined, undefined]);
  }

  function writeErrorHandler(data) {
    errorEmitter.fire(data);
  }

  messageReader.onClose(closeHandler);
  messageReader.onError(readErrorHandler);
  messageWriter.onClose(closeHandler);
  messageWriter.onError(writeErrorHandler);

  function triggerMessageQueue() {
    if (timer || messageQueue.size === 0) {
      return;
    }

    timer = setImmediate(function () {
      timer = undefined;
      processMessageQueue();
    });
  }

  function processMessageQueue() {
    if (messageQueue.size === 0) {
      return;
    }

    var message = messageQueue.shift();

    try {
      if (messages_1.isRequestMessage(message)) {
        handleRequest(message);
      } else if (messages_1.isNotificationMessage(message)) {
        handleNotification(message);
      } else if (messages_1.isResponseMessage(message)) {
        handleResponse(message);
      } else {
        handleInvalidMessage(message);
      }
    } finally {
      triggerMessageQueue();
    }
  }

  var callback = function callback(message) {
    try {
      // We have received a cancellation message. Check if the message is still in the queue
      // and cancel it if allowed to do so.
      if (messages_1.isNotificationMessage(message) && message.method === CancelNotification.type.method) {
        var key = createRequestQueueKey(message.params.id);
        var toCancel = messageQueue.get(key);

        if (messages_1.isRequestMessage(toCancel)) {
          var response = strategy && strategy.cancelUndispatched ? strategy.cancelUndispatched(toCancel, cancelUndispatched) : cancelUndispatched(toCancel);

          if (response && (response.error !== void 0 || response.result !== void 0)) {
            messageQueue["delete"](key);
            response.id = toCancel.id;
            traceSendingResponse(response, message.method, Date.now());
            messageWriter.write(response);
            return;
          }
        }
      }

      addMessageToQueue(messageQueue, message);
    } finally {
      triggerMessageQueue();
    }
  };

  function handleRequest(requestMessage) {
    if (isDisposed()) {
      // we return here silently since we fired an event when the
      // connection got disposed.
      return;
    }

    function reply(resultOrError, method, startTime) {
      var message = {
        jsonrpc: version,
        id: requestMessage.id
      };

      if (resultOrError instanceof messages_1.ResponseError) {
        message.error = resultOrError.toJson();
      } else {
        message.result = resultOrError === void 0 ? null : resultOrError;
      }

      traceSendingResponse(message, method, startTime);
      messageWriter.write(message);
    }

    function replyError(error, method, startTime) {
      var message = {
        jsonrpc: version,
        id: requestMessage.id,
        error: error.toJson()
      };
      traceSendingResponse(message, method, startTime);
      messageWriter.write(message);
    }

    function replySuccess(result, method, startTime) {
      // The JSON RPC defines that a response must either have a result or an error
      // So we can't treat undefined as a valid response result.
      if (result === void 0) {
        result = null;
      }

      var message = {
        jsonrpc: version,
        id: requestMessage.id,
        result: result
      };
      traceSendingResponse(message, method, startTime);
      messageWriter.write(message);
    }

    traceReceivedRequest(requestMessage);
    var element = requestHandlers[requestMessage.method];
    var type;
    var requestHandler;

    if (element) {
      type = element.type;
      requestHandler = element.handler;
    }

    var startTime = Date.now();

    if (requestHandler || starRequestHandler) {
      var cancellationSource = new cancellation_1.CancellationTokenSource();
      var tokenKey = String(requestMessage.id);
      requestTokens[tokenKey] = cancellationSource;

      try {
        var handlerResult;

        if (requestMessage.params === void 0 || type !== void 0 && type.numberOfParams === 0) {
          handlerResult = requestHandler ? requestHandler(cancellationSource.token) : starRequestHandler(requestMessage.method, cancellationSource.token);
        } else if (Is.array(requestMessage.params) && (type === void 0 || type.numberOfParams > 1)) {
          handlerResult = requestHandler ? requestHandler.apply(void 0, _toConsumableArray(requestMessage.params).concat([cancellationSource.token])) : starRequestHandler.apply(void 0, [requestMessage.method].concat(_toConsumableArray(requestMessage.params), [cancellationSource.token]));
        } else {
          handlerResult = requestHandler ? requestHandler(requestMessage.params, cancellationSource.token) : starRequestHandler(requestMessage.method, requestMessage.params, cancellationSource.token);
        }

        var promise = handlerResult;

        if (!handlerResult) {
          delete requestTokens[tokenKey];
          replySuccess(handlerResult, requestMessage.method, startTime);
        } else if (promise.then) {
          promise.then(function (resultOrError) {
            delete requestTokens[tokenKey];
            reply(resultOrError, requestMessage.method, startTime);
          }, function (error) {
            delete requestTokens[tokenKey];

            if (error instanceof messages_1.ResponseError) {
              replyError(error, requestMessage.method, startTime);
            } else if (error && Is.string(error.message)) {
              replyError(new messages_1.ResponseError(messages_1.ErrorCodes.InternalError, "Request ".concat(requestMessage.method, " failed with message: ").concat(error.message)), requestMessage.method, startTime);
            } else {
              replyError(new messages_1.ResponseError(messages_1.ErrorCodes.InternalError, "Request ".concat(requestMessage.method, " failed unexpectedly without providing any details.")), requestMessage.method, startTime);
            }
          });
        } else {
          delete requestTokens[tokenKey];
          reply(handlerResult, requestMessage.method, startTime);
        }
      } catch (error) {
        delete requestTokens[tokenKey];

        if (error instanceof messages_1.ResponseError) {
          reply(error, requestMessage.method, startTime);
        } else if (error && Is.string(error.message)) {
          replyError(new messages_1.ResponseError(messages_1.ErrorCodes.InternalError, "Request ".concat(requestMessage.method, " failed with message: ").concat(error.message)), requestMessage.method, startTime);
        } else {
          replyError(new messages_1.ResponseError(messages_1.ErrorCodes.InternalError, "Request ".concat(requestMessage.method, " failed unexpectedly without providing any details.")), requestMessage.method, startTime);
        }
      }
    } else {
      replyError(new messages_1.ResponseError(messages_1.ErrorCodes.MethodNotFound, "Unhandled method ".concat(requestMessage.method)), requestMessage.method, startTime);
    }
  }

  function handleResponse(responseMessage) {
    if (isDisposed()) {
      // See handle request.
      return;
    }

    if (responseMessage.id === null) {
      if (responseMessage.error) {
        logger.error("Received response message without id: Error is: \n".concat(JSON.stringify(responseMessage.error, undefined, 4)));
      } else {
        logger.error("Received response message without id. No further error information provided.");
      }
    } else {
      var key = String(responseMessage.id);
      var responsePromise = responsePromises[key];
      traceReceivedResponse(responseMessage, responsePromise);

      if (responsePromise) {
        delete responsePromises[key];

        try {
          if (responseMessage.error) {
            var error = responseMessage.error;
            responsePromise.reject(new messages_1.ResponseError(error.code, error.message, error.data));
          } else if (responseMessage.result !== void 0) {
            responsePromise.resolve(responseMessage.result);
          } else {
            throw new Error('Should never happen.');
          }
        } catch (error) {
          if (error.message) {
            logger.error("Response handler '".concat(responsePromise.method, "' failed with message: ").concat(error.message));
          } else {
            logger.error("Response handler '".concat(responsePromise.method, "' failed unexpectedly."));
          }
        }
      }
    }
  }

  function handleNotification(message) {
    if (isDisposed()) {
      // See handle request.
      return;
    }

    var type = undefined;
    var notificationHandler;

    if (message.method === CancelNotification.type.method) {
      notificationHandler = function notificationHandler(params) {
        var id = params.id;
        var source = requestTokens[String(id)];

        if (source) {
          source.cancel();
        }
      };
    } else {
      var element = notificationHandlers[message.method];

      if (element) {
        notificationHandler = element.handler;
        type = element.type;
      }
    }

    if (notificationHandler || starNotificationHandler) {
      try {
        traceReceivedNotification(message);

        if (message.params === void 0 || type !== void 0 && type.numberOfParams === 0) {
          notificationHandler ? notificationHandler() : starNotificationHandler(message.method);
        } else if (Is.array(message.params) && (type === void 0 || type.numberOfParams > 1)) {
          notificationHandler ? notificationHandler.apply(void 0, _toConsumableArray(message.params)) : starNotificationHandler.apply(void 0, [message.method].concat(_toConsumableArray(message.params)));
        } else {
          notificationHandler ? notificationHandler(message.params) : starNotificationHandler(message.method, message.params);
        }
      } catch (error) {
        if (error.message) {
          logger.error("Notification handler '".concat(message.method, "' failed with message: ").concat(error.message));
        } else {
          logger.error("Notification handler '".concat(message.method, "' failed unexpectedly."));
        }
      }
    } else {
      unhandledNotificationEmitter.fire(message);
    }
  }

  function handleInvalidMessage(message) {
    if (!message) {
      logger.error('Received empty message.');
      return;
    }

    logger.error("Received message which is neither a response nor a notification message:\n".concat(JSON.stringify(message, null, 4))); // Test whether we find an id to reject the promise

    var responseMessage = message;

    if (Is.string(responseMessage.id) || Is.number(responseMessage.id)) {
      var key = String(responseMessage.id);
      var responseHandler = responsePromises[key];

      if (responseHandler) {
        responseHandler.reject(new Error('The received response has neither a result nor an error property.'));
      }
    }
  }

  function traceSendingRequest(message) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose && message.params) {
        data = "Params: ".concat(JSON.stringify(message.params, null, 4), "\n\n");
      }

      tracer.log("Sending request '".concat(message.method, " - (").concat(message.id, ")'."), data);
    } else {
      logLSPMessage('send-request', message);
    }
  }

  function traceSendingNotification(message) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose) {
        if (message.params) {
          data = "Params: ".concat(JSON.stringify(message.params, null, 4), "\n\n");
        } else {
          data = 'No parameters provided.\n\n';
        }
      }

      tracer.log("Sending notification '".concat(message.method, "'."), data);
    } else {
      logLSPMessage('send-notification', message);
    }
  }

  function traceSendingResponse(message, method, startTime) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose) {
        if (message.error && message.error.data) {
          data = "Error data: ".concat(JSON.stringify(message.error.data, null, 4), "\n\n");
        } else {
          if (message.result) {
            data = "Result: ".concat(JSON.stringify(message.result, null, 4), "\n\n");
          } else if (message.error === void 0) {
            data = 'No result returned.\n\n';
          }
        }
      }

      tracer.log("Sending response '".concat(method, " - (").concat(message.id, ")'. Processing request took ").concat(Date.now() - startTime, "ms"), data);
    } else {
      logLSPMessage('send-response', message);
    }
  }

  function traceReceivedRequest(message) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose && message.params) {
        data = "Params: ".concat(JSON.stringify(message.params, null, 4), "\n\n");
      }

      tracer.log("Received request '".concat(message.method, " - (").concat(message.id, ")'."), data);
    } else {
      logLSPMessage('receive-request', message);
    }
  }

  function traceReceivedNotification(message) {
    if (_trace === Trace.Off || !tracer || message.method === LogTraceNotification.type.method) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose) {
        if (message.params) {
          data = "Params: ".concat(JSON.stringify(message.params, null, 4), "\n\n");
        } else {
          data = 'No parameters provided.\n\n';
        }
      }

      tracer.log("Received notification '".concat(message.method, "'."), data);
    } else {
      logLSPMessage('receive-notification', message);
    }
  }

  function traceReceivedResponse(message, responsePromise) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    if (traceFormat === TraceFormat.Text) {
      var data = undefined;

      if (_trace === Trace.Verbose) {
        if (message.error && message.error.data) {
          data = "Error data: ".concat(JSON.stringify(message.error.data, null, 4), "\n\n");
        } else {
          if (message.result) {
            data = "Result: ".concat(JSON.stringify(message.result, null, 4), "\n\n");
          } else if (message.error === void 0) {
            data = 'No result returned.\n\n';
          }
        }
      }

      if (responsePromise) {
        var error = message.error ? " Request failed: ".concat(message.error.message, " (").concat(message.error.code, ").") : '';
        tracer.log("Received response '".concat(responsePromise.method, " - (").concat(message.id, ")' in ").concat(Date.now() - responsePromise.timerStart, "ms.").concat(error), data);
      } else {
        tracer.log("Received response ".concat(message.id, " without active response promise."), data);
      }
    } else {
      logLSPMessage('receive-response', message);
    }
  }

  function logLSPMessage(type, message) {
    if (!tracer || _trace === Trace.Off) {
      return;
    }

    var lspMessage = {
      isLSPMessage: true,
      type: type,
      message: message,
      timestamp: Date.now()
    };
    tracer.log(lspMessage);
  }

  function throwIfClosedOrDisposed() {
    if (isClosed()) {
      throw new ConnectionError(ConnectionErrors.Closed, 'Connection is closed.');
    }

    if (isDisposed()) {
      throw new ConnectionError(ConnectionErrors.Disposed, 'Connection is disposed.');
    }
  }

  function throwIfListening() {
    if (isListening()) {
      throw new ConnectionError(ConnectionErrors.AlreadyListening, 'Connection is already listening');
    }
  }

  function throwIfNotListening() {
    if (!isListening()) {
      throw new Error('Call listen() first.');
    }
  }

  function undefinedToNull(param) {
    if (param === void 0) {
      return null;
    } else {
      return param;
    }
  }

  function computeMessageParams(type, params) {
    var result;
    var numberOfParams = type.numberOfParams;

    switch (numberOfParams) {
      case 0:
        result = null;
        break;

      case 1:
        result = undefinedToNull(params[0]);
        break;

      default:
        result = [];

        for (var i = 0; i < params.length && i < numberOfParams; i++) {
          result.push(undefinedToNull(params[i]));
        }

        if (params.length < numberOfParams) {
          for (var _i = params.length; _i < numberOfParams; _i++) {
            result.push(null);
          }
        }

        break;
    }

    return result;
  }

  var connection = {
    sendNotification: function sendNotification(type) {
      throwIfClosedOrDisposed();
      var method;
      var messageParams;

      for (var _len = arguments.length, params = new Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        params[_key - 1] = arguments[_key];
      }

      if (Is.string(type)) {
        method = type;

        switch (params.length) {
          case 0:
            messageParams = null;
            break;

          case 1:
            messageParams = params[0];
            break;

          default:
            messageParams = params;
            break;
        }
      } else {
        method = type.method;
        messageParams = computeMessageParams(type, params);
      }

      var notificationMessage = {
        jsonrpc: version,
        method: method,
        params: messageParams
      };
      traceSendingNotification(notificationMessage);
      messageWriter.write(notificationMessage);
    },
    onNotification: function onNotification(type, handler) {
      throwIfClosedOrDisposed();

      if (Is.func(type)) {
        starNotificationHandler = type;
      } else if (handler) {
        if (Is.string(type)) {
          notificationHandlers[type] = {
            type: undefined,
            handler: handler
          };
        } else {
          notificationHandlers[type.method] = {
            type: type,
            handler: handler
          };
        }
      }
    },
    onProgress: function onProgress(_type, token, handler) {
      if (progressHandlers.has(token)) {
        throw new Error("Progress handler for token ".concat(token, " already registered"));
      }

      progressHandlers.set(token, handler);
      return {
        dispose: function dispose() {
          progressHandlers["delete"](token);
        }
      };
    },
    sendProgress: function sendProgress(_type, token, value) {
      connection.sendNotification(ProgressNotification.type, {
        token: token,
        value: value
      });
    },
    onUnhandledProgress: unhandledProgressEmitter.event,
    sendRequest: function sendRequest(type) {
      throwIfClosedOrDisposed();
      throwIfNotListening();
      var method;
      var messageParams;
      var token = undefined;

      for (var _len2 = arguments.length, params = new Array(_len2 > 1 ? _len2 - 1 : 0), _key2 = 1; _key2 < _len2; _key2++) {
        params[_key2 - 1] = arguments[_key2];
      }

      if (Is.string(type)) {
        method = type;

        switch (params.length) {
          case 0:
            messageParams = null;
            break;

          case 1:
            // The cancellation token is optional so it can also be undefined.
            if (cancellation_1.CancellationToken.is(params[0])) {
              messageParams = null;
              token = params[0];
            } else {
              messageParams = undefinedToNull(params[0]);
            }

            break;

          default:
            var last = params.length - 1;

            if (cancellation_1.CancellationToken.is(params[last])) {
              token = params[last];

              if (params.length === 2) {
                messageParams = undefinedToNull(params[0]);
              } else {
                messageParams = params.slice(0, last).map(function (value) {
                  return undefinedToNull(value);
                });
              }
            } else {
              messageParams = params.map(function (value) {
                return undefinedToNull(value);
              });
            }

            break;
        }
      } else {
        method = type.method;
        messageParams = computeMessageParams(type, params);
        var numberOfParams = type.numberOfParams;
        token = cancellation_1.CancellationToken.is(params[numberOfParams]) ? params[numberOfParams] : undefined;
      }

      var id = sequenceNumber++;
      var result = new Promise(function (resolve, reject) {
        var requestMessage = {
          jsonrpc: version,
          id: id,
          method: method,
          params: messageParams
        };
        var responsePromise = {
          method: method,
          timerStart: Date.now(),
          resolve: resolve,
          reject: reject
        };
        traceSendingRequest(requestMessage);

        try {
          messageWriter.write(requestMessage);
        } catch (e) {
          // Writing the message failed. So we need to reject the promise.
          responsePromise.reject(new messages_1.ResponseError(messages_1.ErrorCodes.MessageWriteError, e.message ? e.message : 'Unknown reason'));
          responsePromise = null;
        }

        if (responsePromise) {
          responsePromises[String(id)] = responsePromise;
        }
      });

      if (token) {
        token.onCancellationRequested(function () {
          connection.sendNotification(CancelNotification.type, {
            id: id
          });
        });
      }

      return result;
    },
    onRequest: function onRequest(type, handler) {
      throwIfClosedOrDisposed();

      if (Is.func(type)) {
        starRequestHandler = type;
      } else if (handler) {
        if (Is.string(type)) {
          requestHandlers[type] = {
            type: undefined,
            handler: handler
          };
        } else {
          requestHandlers[type.method] = {
            type: type,
            handler: handler
          };
        }
      }
    },
    trace: function trace(_value, _tracer, sendNotificationOrTraceOptions) {
      var _sendNotification = false;
      var _traceFormat = TraceFormat.Text;

      if (sendNotificationOrTraceOptions !== void 0) {
        if (Is["boolean"](sendNotificationOrTraceOptions)) {
          _sendNotification = sendNotificationOrTraceOptions;
        } else {
          _sendNotification = sendNotificationOrTraceOptions.sendNotification || false;
          _traceFormat = sendNotificationOrTraceOptions.traceFormat || TraceFormat.Text;
        }
      }

      _trace = _value;
      traceFormat = _traceFormat;

      if (_trace === Trace.Off) {
        tracer = undefined;
      } else {
        tracer = _tracer;
      }

      if (_sendNotification && !isClosed() && !isDisposed()) {
        connection.sendNotification(SetTraceNotification.type, {
          value: Trace.toString(_value)
        });
      }
    },
    onError: errorEmitter.event,
    onClose: closeEmitter.event,
    onUnhandledNotification: unhandledNotificationEmitter.event,
    onDispose: disposeEmitter.event,
    dispose: function dispose() {
      if (isDisposed()) {
        return;
      }

      state = ConnectionState.Disposed;
      disposeEmitter.fire(undefined);
      var error = new Error('Connection got disposed.');
      Object.keys(responsePromises).forEach(function (key) {
        responsePromises[key].reject(error);
      });
      responsePromises = Object.create(null);
      requestTokens = Object.create(null);
      messageQueue = new linkedMap_1.LinkedMap(); // Test for backwards compatibility

      if (Is.func(messageWriter.dispose)) {
        messageWriter.dispose();
      }

      if (Is.func(messageReader.dispose)) {
        messageReader.dispose();
      }
    },
    listen: function listen() {
      throwIfClosedOrDisposed();
      throwIfListening();
      state = ConnectionState.Listening;
      messageReader.listen(callback);
    },
    inspect: function inspect() {
      // eslint-disable-next-line no-console
      console.log('inspect');
    }
  };
  connection.onNotification(LogTraceNotification.type, function (params) {
    if (_trace === Trace.Off || !tracer) {
      return;
    }

    tracer.log(params.message, _trace === Trace.Verbose ? params.verbose : undefined);
  });
  connection.onNotification(ProgressNotification.type, function (params) {
    var handler = progressHandlers.get(params.token);

    if (handler) {
      handler(params.value);
    } else {
      unhandledProgressEmitter.fire(params);
    }
  });
  return connection;
}

function isMessageReader(value) {
  return value.listen !== void 0 && value.read === void 0;
}

function isMessageWriter(value) {
  return value.write !== void 0 && value.end === void 0;
}

function createMessageConnection(input, output, logger, strategy) {
  if (!logger) {
    logger = exports.NullLogger;
  }

  var reader = isMessageReader(input) ? input : new messageReader_1.StreamMessageReader(input);
  var writer = isMessageWriter(output) ? output : new messageWriter_1.StreamMessageWriter(output);
  return _createMessageConnection(reader, writer, logger, strategy);
}

exports.createMessageConnection = createMessageConnection;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/main'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../timers-browserify/main.js */ "../../node_modules/timers-browserify/main.js").setImmediate, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader.js":
/*!****************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader.js ***!
  \****************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(Buffer, global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var events_1 = __webpack_require__(/*! ./events */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js");

var Is = __webpack_require__(/*! ./is */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js");

var DefaultSize = 8192;
var CR = Buffer.from('\r', 'ascii')[0];
var LF = Buffer.from('\n', 'ascii')[0];
var CRLF = '\r\n';

var MessageBuffer =
/*#__PURE__*/
function () {
  function MessageBuffer() {
    var encoding = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'utf8';

    _classCallCheck(this, MessageBuffer);

    this.encoding = encoding;
    this.index = 0;
    this.buffer = Buffer.allocUnsafe(DefaultSize);
  }

  _createClass(MessageBuffer, [{
    key: "append",
    value: function append(chunk) {
      var toAppend = chunk;

      if (typeof chunk === 'string') {
        var str = chunk;
        var bufferLen = Buffer.byteLength(str, this.encoding);
        toAppend = Buffer.allocUnsafe(bufferLen);
        toAppend.write(str, 0, bufferLen, this.encoding);
      }

      if (this.buffer.length - this.index >= toAppend.length) {
        toAppend.copy(this.buffer, this.index, 0, toAppend.length);
      } else {
        var newSize = (Math.ceil((this.index + toAppend.length) / DefaultSize) + 1) * DefaultSize;

        if (this.index === 0) {
          this.buffer = Buffer.allocUnsafe(newSize);
          toAppend.copy(this.buffer, 0, 0, toAppend.length);
        } else {
          this.buffer = Buffer.concat([this.buffer.slice(0, this.index), toAppend], newSize);
        }
      }

      this.index += toAppend.length;
    }
  }, {
    key: "tryReadHeaders",
    value: function tryReadHeaders() {
      var result = undefined;
      var current = 0;

      while (current + 3 < this.index && (this.buffer[current] !== CR || this.buffer[current + 1] !== LF || this.buffer[current + 2] !== CR || this.buffer[current + 3] !== LF)) {
        current++;
      } // No header / body separator found (e.g CRLFCRLF)


      if (current + 3 >= this.index) {
        return result;
      }

      result = Object.create(null);
      var headers = this.buffer.toString('ascii', 0, current).split(CRLF);
      headers.forEach(function (header) {
        var index = header.indexOf(':');

        if (index === -1) {
          throw new Error('Message header must separate key and value using :');
        }

        var key = header.substr(0, index);
        var value = header.substr(index + 1).trim();
        result[key] = value;
      });
      var nextStart = current + 4;
      this.buffer = this.buffer.slice(nextStart);
      this.index = this.index - nextStart;
      return result;
    }
  }, {
    key: "tryReadContent",
    value: function tryReadContent(length) {
      if (this.index < length) {
        return null;
      }

      var result = this.buffer.toString(this.encoding, 0, length);
      var nextStart = length;
      this.buffer.copy(this.buffer, 0, nextStart);
      this.index = this.index - nextStart;
      return result;
    }
  }, {
    key: "numberOfBytes",
    get: function get() {
      return this.index;
    }
  }]);

  return MessageBuffer;
}();

var MessageReader;

(function (MessageReader) {
  function is(value) {
    var candidate = value;
    return candidate && Is.func(candidate.listen) && Is.func(candidate.dispose) && Is.func(candidate.onError) && Is.func(candidate.onClose) && Is.func(candidate.onPartialMessage);
  }

  MessageReader.is = is;
})(MessageReader = exports.MessageReader || (exports.MessageReader = {}));

var AbstractMessageReader =
/*#__PURE__*/
function () {
  function AbstractMessageReader() {
    _classCallCheck(this, AbstractMessageReader);

    this.errorEmitter = new events_1.Emitter();
    this.closeEmitter = new events_1.Emitter();
    this.partialMessageEmitter = new events_1.Emitter();
  }

  _createClass(AbstractMessageReader, [{
    key: "dispose",
    value: function dispose() {
      this.errorEmitter.dispose();
      this.closeEmitter.dispose();
    }
  }, {
    key: "fireError",
    value: function fireError(error) {
      this.errorEmitter.fire(this.asError(error));
    }
  }, {
    key: "fireClose",
    value: function fireClose() {
      this.closeEmitter.fire(undefined);
    }
  }, {
    key: "firePartialMessage",
    value: function firePartialMessage(info) {
      this.partialMessageEmitter.fire(info);
    }
  }, {
    key: "asError",
    value: function asError(error) {
      if (error instanceof Error) {
        return error;
      } else {
        return new Error("Reader received error. Reason: ".concat(Is.string(error.message) ? error.message : 'unknown'));
      }
    }
  }, {
    key: "onError",
    get: function get() {
      return this.errorEmitter.event;
    }
  }, {
    key: "onClose",
    get: function get() {
      return this.closeEmitter.event;
    }
  }, {
    key: "onPartialMessage",
    get: function get() {
      return this.partialMessageEmitter.event;
    }
  }]);

  return AbstractMessageReader;
}();

exports.AbstractMessageReader = AbstractMessageReader;

var StreamMessageReader =
/*#__PURE__*/
function (_AbstractMessageReade) {
  _inherits(StreamMessageReader, _AbstractMessageReade);

  function StreamMessageReader(readable) {
    var _this;

    var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf8';

    _classCallCheck(this, StreamMessageReader);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(StreamMessageReader).call(this));
    _this.readable = readable;
    _this.buffer = new MessageBuffer(encoding);
    _this._partialMessageTimeout = 10000;
    return _this;
  }

  _createClass(StreamMessageReader, [{
    key: "listen",
    value: function listen(callback) {
      var _this2 = this;

      this.nextMessageLength = -1;
      this.messageToken = 0;
      this.partialMessageTimer = undefined;
      this.callback = callback;
      this.readable.on('data', function (data) {
        _this2.onData(data);
      });
      this.readable.on('error', function (error) {
        return _this2.fireError(error);
      });
      this.readable.on('close', function () {
        return _this2.fireClose();
      });
    }
  }, {
    key: "onData",
    value: function onData(data) {
      this.buffer.append(data);

      while (true) {
        if (this.nextMessageLength === -1) {
          var headers = this.buffer.tryReadHeaders();

          if (!headers) {
            return;
          }

          var contentLength = headers['Content-Length'];

          if (!contentLength) {
            throw new Error('Header must provide a Content-Length property.');
          }

          var length = parseInt(contentLength);

          if (isNaN(length)) {
            throw new Error('Content-Length value must be a number.');
          }

          this.nextMessageLength = length; // Take the encoding form the header. For compatibility
          // treat both utf-8 and utf8 as node utf8
        }

        var msg = this.buffer.tryReadContent(this.nextMessageLength);

        if (msg === null) {
          /** We haven't received the full message yet. */
          this.setPartialMessageTimer();
          return;
        }

        this.clearPartialMessageTimer();
        this.nextMessageLength = -1;
        this.messageToken++;
        var json = JSON.parse(msg);
        this.callback(json);
      }
    }
  }, {
    key: "clearPartialMessageTimer",
    value: function clearPartialMessageTimer() {
      if (this.partialMessageTimer) {
        clearTimeout(this.partialMessageTimer);
        this.partialMessageTimer = undefined;
      }
    }
  }, {
    key: "setPartialMessageTimer",
    value: function setPartialMessageTimer() {
      var _this3 = this;

      this.clearPartialMessageTimer();

      if (this._partialMessageTimeout <= 0) {
        return;
      }

      this.partialMessageTimer = setTimeout(function (token, timeout) {
        _this3.partialMessageTimer = undefined;

        if (token === _this3.messageToken) {
          _this3.firePartialMessage({
            messageToken: token,
            waitingTime: timeout
          });

          _this3.setPartialMessageTimer();
        }
      }, this._partialMessageTimeout, this.messageToken, this._partialMessageTimeout);
    }
  }, {
    key: "partialMessageTimeout",
    set: function set(timeout) {
      this._partialMessageTimeout = timeout;
    },
    get: function get() {
      return this._partialMessageTimeout;
    }
  }]);

  return StreamMessageReader;
}(AbstractMessageReader);

exports.StreamMessageReader = StreamMessageReader;

var IPCMessageReader =
/*#__PURE__*/
function (_AbstractMessageReade2) {
  _inherits(IPCMessageReader, _AbstractMessageReade2);

  function IPCMessageReader(process) {
    var _this4;

    _classCallCheck(this, IPCMessageReader);

    _this4 = _possibleConstructorReturn(this, _getPrototypeOf(IPCMessageReader).call(this));
    _this4.process = process;
    var eventEmitter = _this4.process;
    eventEmitter.on('error', function (error) {
      return _this4.fireError(error);
    });
    eventEmitter.on('close', function () {
      return _this4.fireClose();
    });
    return _this4;
  }

  _createClass(IPCMessageReader, [{
    key: "listen",
    value: function listen(callback) {
      this.process.on('message', callback);
    }
  }]);

  return IPCMessageReader;
}(AbstractMessageReader);

exports.IPCMessageReader = IPCMessageReader;

var SocketMessageReader =
/*#__PURE__*/
function (_StreamMessageReader) {
  _inherits(SocketMessageReader, _StreamMessageReader);

  function SocketMessageReader(socket) {
    var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf-8';

    _classCallCheck(this, SocketMessageReader);

    return _possibleConstructorReturn(this, _getPrototypeOf(SocketMessageReader).call(this, socket, encoding));
  }

  return SocketMessageReader;
}(StreamMessageReader);

exports.SocketMessageReader = SocketMessageReader;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node-libs-browser/node_modules/buffer/index.js */ "../../node_modules/node-libs-browser/node_modules/buffer/index.js").Buffer, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter.js":
/*!****************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter.js ***!
  \****************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(Buffer, global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _get = __webpack_require__(/*! @babel/runtime/helpers/get */ "../../node_modules/@babel/runtime/helpers/get.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var events_1 = __webpack_require__(/*! ./events */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/events.js");

var Is = __webpack_require__(/*! ./is */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js");

var ContentLength = 'Content-Length: ';
var CRLF = '\r\n';
var MessageWriter;

(function (MessageWriter) {
  function is(value) {
    var candidate = value;
    return candidate && Is.func(candidate.dispose) && Is.func(candidate.onClose) && Is.func(candidate.onError) && Is.func(candidate.write);
  }

  MessageWriter.is = is;
})(MessageWriter = exports.MessageWriter || (exports.MessageWriter = {}));

var AbstractMessageWriter =
/*#__PURE__*/
function () {
  function AbstractMessageWriter() {
    _classCallCheck(this, AbstractMessageWriter);

    this.errorEmitter = new events_1.Emitter();
    this.closeEmitter = new events_1.Emitter();
  }

  _createClass(AbstractMessageWriter, [{
    key: "dispose",
    value: function dispose() {
      this.errorEmitter.dispose();
      this.closeEmitter.dispose();
    }
  }, {
    key: "fireError",
    value: function fireError(error, message, count) {
      this.errorEmitter.fire([this.asError(error), message, count]);
    }
  }, {
    key: "fireClose",
    value: function fireClose() {
      this.closeEmitter.fire(undefined);
    }
  }, {
    key: "asError",
    value: function asError(error) {
      if (error instanceof Error) {
        return error;
      } else {
        return new Error("Writer received error. Reason: ".concat(Is.string(error.message) ? error.message : 'unknown'));
      }
    }
  }, {
    key: "onError",
    get: function get() {
      return this.errorEmitter.event;
    }
  }, {
    key: "onClose",
    get: function get() {
      return this.closeEmitter.event;
    }
  }]);

  return AbstractMessageWriter;
}();

exports.AbstractMessageWriter = AbstractMessageWriter;

var StreamMessageWriter =
/*#__PURE__*/
function (_AbstractMessageWrite) {
  _inherits(StreamMessageWriter, _AbstractMessageWrite);

  function StreamMessageWriter(writable) {
    var _this;

    var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf8';

    _classCallCheck(this, StreamMessageWriter);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(StreamMessageWriter).call(this));
    _this.writable = writable;
    _this.encoding = encoding;
    _this.errorCount = 0;

    _this.writable.on('error', function (error) {
      return _this.fireError(error);
    });

    _this.writable.on('close', function () {
      return _this.fireClose();
    });

    return _this;
  }

  _createClass(StreamMessageWriter, [{
    key: "write",
    value: function write(msg) {
      var json = JSON.stringify(msg);
      var contentLength = Buffer.byteLength(json, this.encoding);
      var headers = [ContentLength, contentLength.toString(), CRLF, CRLF];

      try {
        // Header must be written in ASCII encoding
        this.writable.write(headers.join(''), 'ascii'); // Now write the content. This can be written in any encoding

        this.writable.write(json, this.encoding);
        this.errorCount = 0;
      } catch (error) {
        this.errorCount++;
        this.fireError(error, msg, this.errorCount);
      }
    }
  }]);

  return StreamMessageWriter;
}(AbstractMessageWriter);

exports.StreamMessageWriter = StreamMessageWriter;

var IPCMessageWriter =
/*#__PURE__*/
function (_AbstractMessageWrite2) {
  _inherits(IPCMessageWriter, _AbstractMessageWrite2);

  function IPCMessageWriter(process) {
    var _this2;

    _classCallCheck(this, IPCMessageWriter);

    _this2 = _possibleConstructorReturn(this, _getPrototypeOf(IPCMessageWriter).call(this));
    _this2.process = process;
    _this2.errorCount = 0;
    _this2.queue = [];
    _this2.sending = false;
    var eventEmitter = _this2.process;
    eventEmitter.on('error', function (error) {
      return _this2.fireError(error);
    });
    eventEmitter.on('close', function () {
      return _this2.fireClose;
    });
    return _this2;
  }

  _createClass(IPCMessageWriter, [{
    key: "write",
    value: function write(msg) {
      if (!this.sending && this.queue.length === 0) {
        // See https://github.com/nodejs/node/issues/7657
        this.doWriteMessage(msg);
      } else {
        this.queue.push(msg);
      }
    }
  }, {
    key: "doWriteMessage",
    value: function doWriteMessage(msg) {
      var _this3 = this;

      try {
        if (this.process.send) {
          this.sending = true;
          this.process.send(msg, undefined, undefined, function (error) {
            _this3.sending = false;

            if (error) {
              _this3.errorCount++;

              _this3.fireError(error, msg, _this3.errorCount);
            } else {
              _this3.errorCount = 0;
            }

            if (_this3.queue.length > 0) {
              _this3.doWriteMessage(_this3.queue.shift());
            }
          });
        }
      } catch (error) {
        this.errorCount++;
        this.fireError(error, msg, this.errorCount);
      }
    }
  }]);

  return IPCMessageWriter;
}(AbstractMessageWriter);

exports.IPCMessageWriter = IPCMessageWriter;

var SocketMessageWriter =
/*#__PURE__*/
function (_AbstractMessageWrite3) {
  _inherits(SocketMessageWriter, _AbstractMessageWrite3);

  function SocketMessageWriter(socket) {
    var _this4;

    var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf8';

    _classCallCheck(this, SocketMessageWriter);

    _this4 = _possibleConstructorReturn(this, _getPrototypeOf(SocketMessageWriter).call(this));
    _this4.socket = socket;
    _this4.queue = [];
    _this4.sending = false;
    _this4.encoding = encoding;
    _this4.errorCount = 0;

    _this4.socket.on('error', function (error) {
      return _this4.fireError(error);
    });

    _this4.socket.on('close', function () {
      return _this4.fireClose();
    });

    return _this4;
  }

  _createClass(SocketMessageWriter, [{
    key: "dispose",
    value: function dispose() {
      _get(_getPrototypeOf(SocketMessageWriter.prototype), "dispose", this).call(this);

      this.socket.destroy();
    }
  }, {
    key: "write",
    value: function write(msg) {
      if (!this.sending && this.queue.length === 0) {
        // See https://github.com/nodejs/node/issues/7657
        this.doWriteMessage(msg);
      } else {
        this.queue.push(msg);
      }
    }
  }, {
    key: "doWriteMessage",
    value: function doWriteMessage(msg) {
      var _this5 = this;

      var json = JSON.stringify(msg);
      var contentLength = Buffer.byteLength(json, this.encoding);
      var headers = [ContentLength, contentLength.toString(), CRLF, CRLF];

      try {
        // Header must be written in ASCII encoding
        this.sending = true;
        this.socket.write(headers.join(''), 'ascii', function (error) {
          if (error) {
            _this5.handleError(error, msg);
          }

          try {
            // Now write the content. This can be written in any encoding
            _this5.socket.write(json, _this5.encoding, function (error) {
              _this5.sending = false;

              if (error) {
                _this5.handleError(error, msg);
              } else {
                _this5.errorCount = 0;
              }

              if (_this5.queue.length > 0) {
                _this5.doWriteMessage(_this5.queue.shift());
              }
            });
          } catch (error) {
            _this5.handleError(error, msg);
          }
        });
      } catch (error) {
        this.handleError(error, msg);
      }
    }
  }, {
    key: "handleError",
    value: function handleError(error, msg) {
      this.errorCount++;
      this.fireError(error, msg, this.errorCount);
    }
  }]);

  return SocketMessageWriter;
}(AbstractMessageWriter);

exports.SocketMessageWriter = SocketMessageWriter;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node-libs-browser/node_modules/buffer/index.js */ "../../node_modules/node-libs-browser/node_modules/buffer/index.js").Buffer, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messages.js":
/*!***********************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messages.js ***!
  \***********************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


var _classCallCheck = __webpack_require__(/*! @babel/runtime/helpers/classCallCheck */ "../../node_modules/@babel/runtime/helpers/classCallCheck.js");

var _createClass = __webpack_require__(/*! @babel/runtime/helpers/createClass */ "../../node_modules/@babel/runtime/helpers/createClass.js");

var _possibleConstructorReturn = __webpack_require__(/*! @babel/runtime/helpers/possibleConstructorReturn */ "../../node_modules/@babel/runtime/helpers/possibleConstructorReturn.js");

var _getPrototypeOf = __webpack_require__(/*! @babel/runtime/helpers/getPrototypeOf */ "../../node_modules/@babel/runtime/helpers/getPrototypeOf.js");

var _assertThisInitialized = __webpack_require__(/*! @babel/runtime/helpers/assertThisInitialized */ "../../node_modules/@babel/runtime/helpers/assertThisInitialized.js");

var _inherits = __webpack_require__(/*! @babel/runtime/helpers/inherits */ "../../node_modules/@babel/runtime/helpers/inherits.js");

var _wrapNativeSuper = __webpack_require__(/*! @babel/runtime/helpers/wrapNativeSuper */ "../../node_modules/@babel/runtime/helpers/wrapNativeSuper.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});

var is = __webpack_require__(/*! ./is */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/is.js");
/**
 * Predefined error codes.
 */


var ErrorCodes;

(function (ErrorCodes) {
  // Defined by JSON RPC
  ErrorCodes.ParseError = -32700;
  ErrorCodes.InvalidRequest = -32600;
  ErrorCodes.MethodNotFound = -32601;
  ErrorCodes.InvalidParams = -32602;
  ErrorCodes.InternalError = -32603;
  ErrorCodes.serverErrorStart = -32099;
  ErrorCodes.serverErrorEnd = -32000;
  ErrorCodes.ServerNotInitialized = -32002;
  ErrorCodes.UnknownErrorCode = -32001; // Defined by the protocol.

  ErrorCodes.RequestCancelled = -32800;
  ErrorCodes.ContentModified = -32801; // Defined by VSCode library.

  ErrorCodes.MessageWriteError = 1;
  ErrorCodes.MessageReadError = 2;
})(ErrorCodes = exports.ErrorCodes || (exports.ErrorCodes = {}));
/**
 * An error object return in a response in case a request
 * has failed.
 */


var ResponseError =
/*#__PURE__*/
function (_Error) {
  _inherits(ResponseError, _Error);

  function ResponseError(code, message, data) {
    var _this;

    _classCallCheck(this, ResponseError);

    _this = _possibleConstructorReturn(this, _getPrototypeOf(ResponseError).call(this, message));
    _this.code = is.number(code) ? code : ErrorCodes.UnknownErrorCode;
    _this.data = data;
    Object.setPrototypeOf(_assertThisInitialized(_this), ResponseError.prototype);
    return _this;
  }

  _createClass(ResponseError, [{
    key: "toJson",
    value: function toJson() {
      return {
        code: this.code,
        message: this.message,
        data: this.data
      };
    }
  }]);

  return ResponseError;
}(_wrapNativeSuper(Error));

exports.ResponseError = ResponseError;
/**
 * An abstract implementation of a MessageType.
 */

var AbstractMessageType =
/*#__PURE__*/
function () {
  function AbstractMessageType(_method, _numberOfParams) {
    _classCallCheck(this, AbstractMessageType);

    this._method = _method;
    this._numberOfParams = _numberOfParams;
  }

  _createClass(AbstractMessageType, [{
    key: "method",
    get: function get() {
      return this._method;
    }
  }, {
    key: "numberOfParams",
    get: function get() {
      return this._numberOfParams;
    }
  }]);

  return AbstractMessageType;
}();

exports.AbstractMessageType = AbstractMessageType;
/**
 * Classes to type request response pairs
 *
 * The type parameter RO will be removed in the next major version
 * of the JSON RPC library since it is a LSP concept and doesn't
 * belong here. For now it is tagged as default never.
 */

var RequestType0 =
/*#__PURE__*/
function (_AbstractMessageType) {
  _inherits(RequestType0, _AbstractMessageType);

  function RequestType0(method) {
    _classCallCheck(this, RequestType0);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType0).call(this, method, 0));
  }

  return RequestType0;
}(AbstractMessageType);

exports.RequestType0 = RequestType0;

var RequestType =
/*#__PURE__*/
function (_AbstractMessageType2) {
  _inherits(RequestType, _AbstractMessageType2);

  function RequestType(method) {
    _classCallCheck(this, RequestType);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType).call(this, method, 1));
  }

  return RequestType;
}(AbstractMessageType);

exports.RequestType = RequestType;

var RequestType1 =
/*#__PURE__*/
function (_AbstractMessageType3) {
  _inherits(RequestType1, _AbstractMessageType3);

  function RequestType1(method) {
    _classCallCheck(this, RequestType1);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType1).call(this, method, 1));
  }

  return RequestType1;
}(AbstractMessageType);

exports.RequestType1 = RequestType1;

var RequestType2 =
/*#__PURE__*/
function (_AbstractMessageType4) {
  _inherits(RequestType2, _AbstractMessageType4);

  function RequestType2(method) {
    _classCallCheck(this, RequestType2);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType2).call(this, method, 2));
  }

  return RequestType2;
}(AbstractMessageType);

exports.RequestType2 = RequestType2;

var RequestType3 =
/*#__PURE__*/
function (_AbstractMessageType5) {
  _inherits(RequestType3, _AbstractMessageType5);

  function RequestType3(method) {
    _classCallCheck(this, RequestType3);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType3).call(this, method, 3));
  }

  return RequestType3;
}(AbstractMessageType);

exports.RequestType3 = RequestType3;

var RequestType4 =
/*#__PURE__*/
function (_AbstractMessageType6) {
  _inherits(RequestType4, _AbstractMessageType6);

  function RequestType4(method) {
    _classCallCheck(this, RequestType4);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType4).call(this, method, 4));
  }

  return RequestType4;
}(AbstractMessageType);

exports.RequestType4 = RequestType4;

var RequestType5 =
/*#__PURE__*/
function (_AbstractMessageType7) {
  _inherits(RequestType5, _AbstractMessageType7);

  function RequestType5(method) {
    _classCallCheck(this, RequestType5);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType5).call(this, method, 5));
  }

  return RequestType5;
}(AbstractMessageType);

exports.RequestType5 = RequestType5;

var RequestType6 =
/*#__PURE__*/
function (_AbstractMessageType8) {
  _inherits(RequestType6, _AbstractMessageType8);

  function RequestType6(method) {
    _classCallCheck(this, RequestType6);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType6).call(this, method, 6));
  }

  return RequestType6;
}(AbstractMessageType);

exports.RequestType6 = RequestType6;

var RequestType7 =
/*#__PURE__*/
function (_AbstractMessageType9) {
  _inherits(RequestType7, _AbstractMessageType9);

  function RequestType7(method) {
    _classCallCheck(this, RequestType7);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType7).call(this, method, 7));
  }

  return RequestType7;
}(AbstractMessageType);

exports.RequestType7 = RequestType7;

var RequestType8 =
/*#__PURE__*/
function (_AbstractMessageType10) {
  _inherits(RequestType8, _AbstractMessageType10);

  function RequestType8(method) {
    _classCallCheck(this, RequestType8);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType8).call(this, method, 8));
  }

  return RequestType8;
}(AbstractMessageType);

exports.RequestType8 = RequestType8;

var RequestType9 =
/*#__PURE__*/
function (_AbstractMessageType11) {
  _inherits(RequestType9, _AbstractMessageType11);

  function RequestType9(method) {
    _classCallCheck(this, RequestType9);

    return _possibleConstructorReturn(this, _getPrototypeOf(RequestType9).call(this, method, 9));
  }

  return RequestType9;
}(AbstractMessageType);

exports.RequestType9 = RequestType9;
/**
 * The type parameter RO will be removed in the next major version
 * of the JSON RPC library since it is a LSP concept and doesn't
 * belong here. For now it is tagged as default never.
 */

var NotificationType =
/*#__PURE__*/
function (_AbstractMessageType12) {
  _inherits(NotificationType, _AbstractMessageType12);

  function NotificationType(method) {
    var _this2;

    _classCallCheck(this, NotificationType);

    _this2 = _possibleConstructorReturn(this, _getPrototypeOf(NotificationType).call(this, method, 1));
    _this2._ = undefined;
    return _this2;
  }

  return NotificationType;
}(AbstractMessageType);

exports.NotificationType = NotificationType;

var NotificationType0 =
/*#__PURE__*/
function (_AbstractMessageType13) {
  _inherits(NotificationType0, _AbstractMessageType13);

  function NotificationType0(method) {
    _classCallCheck(this, NotificationType0);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType0).call(this, method, 0));
  }

  return NotificationType0;
}(AbstractMessageType);

exports.NotificationType0 = NotificationType0;

var NotificationType1 =
/*#__PURE__*/
function (_AbstractMessageType14) {
  _inherits(NotificationType1, _AbstractMessageType14);

  function NotificationType1(method) {
    _classCallCheck(this, NotificationType1);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType1).call(this, method, 1));
  }

  return NotificationType1;
}(AbstractMessageType);

exports.NotificationType1 = NotificationType1;

var NotificationType2 =
/*#__PURE__*/
function (_AbstractMessageType15) {
  _inherits(NotificationType2, _AbstractMessageType15);

  function NotificationType2(method) {
    _classCallCheck(this, NotificationType2);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType2).call(this, method, 2));
  }

  return NotificationType2;
}(AbstractMessageType);

exports.NotificationType2 = NotificationType2;

var NotificationType3 =
/*#__PURE__*/
function (_AbstractMessageType16) {
  _inherits(NotificationType3, _AbstractMessageType16);

  function NotificationType3(method) {
    _classCallCheck(this, NotificationType3);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType3).call(this, method, 3));
  }

  return NotificationType3;
}(AbstractMessageType);

exports.NotificationType3 = NotificationType3;

var NotificationType4 =
/*#__PURE__*/
function (_AbstractMessageType17) {
  _inherits(NotificationType4, _AbstractMessageType17);

  function NotificationType4(method) {
    _classCallCheck(this, NotificationType4);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType4).call(this, method, 4));
  }

  return NotificationType4;
}(AbstractMessageType);

exports.NotificationType4 = NotificationType4;

var NotificationType5 =
/*#__PURE__*/
function (_AbstractMessageType18) {
  _inherits(NotificationType5, _AbstractMessageType18);

  function NotificationType5(method) {
    _classCallCheck(this, NotificationType5);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType5).call(this, method, 5));
  }

  return NotificationType5;
}(AbstractMessageType);

exports.NotificationType5 = NotificationType5;

var NotificationType6 =
/*#__PURE__*/
function (_AbstractMessageType19) {
  _inherits(NotificationType6, _AbstractMessageType19);

  function NotificationType6(method) {
    _classCallCheck(this, NotificationType6);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType6).call(this, method, 6));
  }

  return NotificationType6;
}(AbstractMessageType);

exports.NotificationType6 = NotificationType6;

var NotificationType7 =
/*#__PURE__*/
function (_AbstractMessageType20) {
  _inherits(NotificationType7, _AbstractMessageType20);

  function NotificationType7(method) {
    _classCallCheck(this, NotificationType7);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType7).call(this, method, 7));
  }

  return NotificationType7;
}(AbstractMessageType);

exports.NotificationType7 = NotificationType7;

var NotificationType8 =
/*#__PURE__*/
function (_AbstractMessageType21) {
  _inherits(NotificationType8, _AbstractMessageType21);

  function NotificationType8(method) {
    _classCallCheck(this, NotificationType8);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType8).call(this, method, 8));
  }

  return NotificationType8;
}(AbstractMessageType);

exports.NotificationType8 = NotificationType8;

var NotificationType9 =
/*#__PURE__*/
function (_AbstractMessageType22) {
  _inherits(NotificationType9, _AbstractMessageType22);

  function NotificationType9(method) {
    _classCallCheck(this, NotificationType9);

    return _possibleConstructorReturn(this, _getPrototypeOf(NotificationType9).call(this, method, 9));
  }

  return NotificationType9;
}(AbstractMessageType);

exports.NotificationType9 = NotificationType9;
/**
 * Tests if the given message is a request message
 */

function isRequestMessage(message) {
  var candidate = message;
  return candidate && is.string(candidate.method) && (is.string(candidate.id) || is.number(candidate.id));
}

exports.isRequestMessage = isRequestMessage;
/**
 * Tests if the given message is a notification message
 */

function isNotificationMessage(message) {
  var candidate = message;
  return candidate && is.string(candidate.method) && message.id === void 0;
}

exports.isNotificationMessage = isNotificationMessage;
/**
 * Tests if the given message is a response message
 */

function isResponseMessage(message) {
  var candidate = message;
  return candidate && (candidate.result !== void 0 || !!candidate.error) && (is.string(candidate.id) || is.number(candidate.id) || candidate.id === null);
}

exports.isResponseMessage = isResponseMessage;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messages'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/pipeSupport.js":
/*!**************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/pipeSupport.js ***!
  \**************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(process, global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var path_1 = __webpack_require__(/*! path */ "../../node_modules/path-browserify/index.js");

var os_1 = __webpack_require__(/*! os */ "../../node_modules/os-browserify/browser.js");

var crypto_1 = __webpack_require__(/*! crypto */ "../../node_modules/node-libs-browser/mock/empty.js");

var net_1 = __webpack_require__(/*! net */ "../../node_modules/node-libs-browser/mock/empty.js");

var messageReader_1 = __webpack_require__(/*! ./messageReader */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader.js");

var messageWriter_1 = __webpack_require__(/*! ./messageWriter */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter.js");

function generateRandomPipeName() {
  var randomSuffix = crypto_1.randomBytes(21).toString('hex');

  if (process.platform === 'win32') {
    return "\\\\.\\pipe\\vscode-jsonrpc-".concat(randomSuffix, "-sock");
  } else {
    // Mac/Unix: use socket file
    return path_1.join(os_1.tmpdir(), "vscode-".concat(randomSuffix, ".sock"));
  }
}

exports.generateRandomPipeName = generateRandomPipeName;

function createClientPipeTransport(pipeName) {
  var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf-8';
  var connectResolve;
  var connected = new Promise(function (resolve, _reject) {
    connectResolve = resolve;
  });
  return new Promise(function (resolve, reject) {
    var server = net_1.createServer(function (socket) {
      server.close();
      connectResolve([new messageReader_1.SocketMessageReader(socket, encoding), new messageWriter_1.SocketMessageWriter(socket, encoding)]);
    });
    server.on('error', reject);
    server.listen(pipeName, function () {
      server.removeListener('error', reject);
      resolve({
        onConnected: function onConnected() {
          return connected;
        }
      });
    });
  });
}

exports.createClientPipeTransport = createClientPipeTransport;

function createServerPipeTransport(pipeName) {
  var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf-8';
  var socket = net_1.createConnection(pipeName);
  return [new messageReader_1.SocketMessageReader(socket, encoding), new messageWriter_1.SocketMessageWriter(socket, encoding)];
}

exports.createServerPipeTransport = createServerPipeTransport;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/pipeSupport'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../process/browser.js */ "../../node_modules/process/browser.js"), __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/socketSupport.js":
/*!****************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/socketSupport.js ***!
  \****************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */


Object.defineProperty(exports, "__esModule", {
  value: true
});

var net_1 = __webpack_require__(/*! net */ "../../node_modules/node-libs-browser/mock/empty.js");

var messageReader_1 = __webpack_require__(/*! ./messageReader */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageReader.js");

var messageWriter_1 = __webpack_require__(/*! ./messageWriter */ "../../node_modules/vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/messageWriter.js");

function createClientSocketTransport(port) {
  var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf-8';
  var connectResolve;
  var connected = new Promise(function (resolve, _reject) {
    connectResolve = resolve;
  });
  return new Promise(function (resolve, reject) {
    var server = net_1.createServer(function (socket) {
      server.close();
      connectResolve([new messageReader_1.SocketMessageReader(socket, encoding), new messageWriter_1.SocketMessageWriter(socket, encoding)]);
    });
    server.on('error', reject);
    server.listen(port, '127.0.0.1', function () {
      server.removeListener('error', reject);
      resolve({
        onConnected: function onConnected() {
          return connected;
        }
      });
    });
  });
}

exports.createClientSocketTransport = createClientSocketTransport;

function createServerSocketTransport(port) {
  var encoding = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'utf-8';
  var socket = net_1.createConnection(port, '127.0.0.1');
  return [new messageReader_1.SocketMessageReader(socket, encoding), new messageWriter_1.SocketMessageWriter(socket, encoding)];
}

exports.createServerSocketTransport = createServerSocketTransport;
if (!global) global = {};
(global['theia'] = global['theia'] || {})['vscode-languageserver-protocol/node_modules/vscode-jsonrpc/lib/socketSupport'] = this;
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ })

}]);
//# sourceMappingURL=13.bundle.js.map