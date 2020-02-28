(window["webpackJsonp"] = window["webpackJsonp"] || []).push([[26],{

/***/ "../../node_modules/css-loader/index.js!../../packages/latex/src/browser/style/index.css":
/*!**************************************************************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/node_modules/css-loader!/home/elliott/playground/test/theia/packages/latex/src/browser/style/index.css ***!
  \**************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

exports = module.exports = __webpack_require__(/*! ../../../../../node_modules/css-loader/lib/css-base.js */ "../../node_modules/css-loader/lib/css-base.js")(false);
// imports


// module
exports.push([module.i, ".swiftlatex-preview-widget {\n\n}\n\n.pf-line {\n  line-height: 1.00;\n  white-space: pre;\n  position: absolute;\n}\n\n.pf-space {\n  display: inline-block;\n}\n\n#swiftlatex_preview_viewer {\n  background-color: #9c9c9c; margin: 0; padding:0; border: 0;\n  width: 100%;\n  height: 96%; \n  overflow-x: scroll;\n  overflow-y: scroll;\n}\n\n.page_decoration {\n  margin: 0 auto; background-color: white;\n  transform-origin: 50% 0;\n}\n\n\n.swiftlatex-preview-toolbar {\n    margin-top: 8px;\n    display: flex;\n    align-items: center;\n    justify-content: space-evenly;\n    padding: 0 10px;\n}\n\n.swiftlatex-preview-toolbar-button {\n    min-width: 1rem;\n    text-align: center;\n    flex-grow: 0;\n    font-family: FontAwesome;\n    font-size: 16px;\n    margin: 0px 4px 0px 4px;\n}\n\n.swiftlatex-preview-toolbar-previous::before {\n    content: \"\\F053\";\n}\n\n.swiftlatex-preview-toolbar-next::before {\n    content: \"\\F054\";\n}\n\n.swiftlatex-preview-toolbar-zoomout::before {\n    content: \"\\F010\";\n}\n\n.swiftlatex-preview-toolbar-zoomin::before {\n    content: \"\\F00E\";\n}\n\n.swiftlatex-preview-toolbar-input {\n    width: 40%;\n    line-height: 1.00;\n    margin-left: 4px;\n    margin-right: 4px;\n    text-align: center; \n}\n\n.viewercursor {\n  border-right: 1px solid red;\n}", ""]);

// exports


/***/ }),

/***/ "../../packages/latex/data/Bibtex.tmLanguage.json":
/*!**************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/data/Bibtex.tmLanguage.json ***!
  \**************************************************************************************/
/*! exports provided: comment, fileTypes, name, patterns, repository, scopeName, uuid, default */
/***/ (function(module) {

module.exports = JSON.parse("{\"comment\":\"Grammar based on description from http://artis.imag.fr/~Xavier.Decoret/resources/xdkbibtex/bibtex_summary.html#comment\\n\\t\\n\\tTODO: Does not support @preamble\\n\\t\",\"fileTypes\":[\"bib\"],\"name\":\"BibTeX\",\"patterns\":[{\"begin\":\"@Comment\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.comment.bibtex\"}},\"end\":\"$\\\\n?\",\"name\":\"comment.line.at-sign.bibtex\"},{\"begin\":\"((@)String)\\\\s*(\\\\{)\\\\s*([a-zA-Z]*)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.other.string-constant.bibtex\"},\"2\":{\"name\":\"punctuation.definition.keyword.bibtex\"},\"3\":{\"name\":\"punctuation.section.string-constant.begin.bibtex\"},\"4\":{\"name\":\"variable.other.bibtex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.section.string-constant.end.bibtex\"}},\"name\":\"meta.string-constant.braces.bibtex\",\"patterns\":[{\"include\":\"#string_content\"}]},{\"begin\":\"((@)String)\\\\s*(\\\\()\\\\s*([a-zA-Z]*)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.other.string-constant.bibtex\"},\"2\":{\"name\":\"punctuation.definition.keyword.bibtex\"},\"3\":{\"name\":\"punctuation.section.string-constant.begin.bibtex\"},\"4\":{\"name\":\"variable.other.bibtex\"}},\"end\":\"\\\\)\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.section.string-constant.end.bibtex\"}},\"name\":\"meta.string-constant.parenthesis.bibtex\",\"patterns\":[{\"include\":\"#string_content\"}]},{\"begin\":\"((@)[a-zA-Z]+)\\\\s*(\\\\{)\\\\s*([^\\\\s,]*)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.other.entry-type.bibtex\"},\"2\":{\"name\":\"punctuation.definition.keyword.bibtex\"},\"3\":{\"name\":\"punctuation.section.entry.begin.bibtex\"},\"4\":{\"name\":\"entity.name.type.entry-key.bibtex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.section.entry.end.bibtex\"}},\"name\":\"meta.entry.braces.bibtex\",\"patterns\":[{\"begin\":\"([a-zA-Z0-9\\\\!\\\\$\\\\&\\\\*\\\\+\\\\-\\\\.\\\\/\\\\:\\\\;\\\\<\\\\>\\\\?\\\\[\\\\]\\\\^\\\\_\\\\`\\\\|]+)\\\\s*(\\\\=)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.key.bibtex\"},\"2\":{\"name\":\"punctuation.separator.key-value.bibtex\"}},\"end\":\"(?=[,}])\",\"name\":\"meta.key-assignment.bibtex\",\"patterns\":[{\"include\":\"#string_content\"},{\"include\":\"#integer\"}]}]},{\"begin\":\"((@)[a-zA-Z]+)\\\\s*(\\\\()\\\\s*([^\\\\s,]*)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.other.entry-type.bibtex\"},\"2\":{\"name\":\"punctuation.definition.keyword.bibtex\"},\"3\":{\"name\":\"punctuation.section.entry.begin.bibtex\"},\"4\":{\"name\":\"entity.name.type.entry-key.bibtex\"}},\"end\":\"\\\\)\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.section.entry.end.bibtex\"}},\"name\":\"meta.entry.parenthesis.bibtex\",\"patterns\":[{\"begin\":\"([a-zA-Z0-9\\\\!\\\\$\\\\&\\\\*\\\\+\\\\-\\\\.\\\\/\\\\:\\\\;\\\\<\\\\>\\\\?\\\\[\\\\]\\\\^\\\\_\\\\`\\\\|]+)\\\\s*(\\\\=)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.key.bibtex\"},\"2\":{\"name\":\"punctuation.separator.key-value.bibtex\"}},\"end\":\"(?=[,)])\",\"name\":\"meta.key-assignment.bibtex\",\"patterns\":[{\"include\":\"#string_content\"},{\"include\":\"#integer\"}]}]},{\"begin\":\"[^@\\\\n]\",\"end\":\"(?=@)\",\"name\":\"comment.block.bibtex\"}],\"repository\":{\"integer\":{\"match\":\"\\\\d+\",\"name\":\"constant.numeric.bibtex\"},\"nested_braces\":{\"begin\":\"(?<!\\\\\\\\)\\\\{\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.group.begin.bibtex\"}},\"end\":\"(?<!\\\\\\\\)\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.group.end.bibtex\"}},\"patterns\":[{\"include\":\"#nested_braces\"}]},\"string_content\":{\"patterns\":[{\"begin\":\"\\\\{\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.begin.bibtex\"}},\"end\":\"\\\\}(?=,?\\\\s*\\\\n)\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.end.bibtex\"}},\"patterns\":[{\"match\":\"@\",\"name\":\"invalid.illegal.at-sign.bibtex\"},{\"include\":\"#nested_braces\"}]},{\"begin\":\"\\\"\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.begin.bibtex\"}},\"end\":\"\\\"(?=,?\\\\s*\\\\n)\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.end.bibtex\"}},\"patterns\":[{\"match\":\"@\",\"name\":\"invalid.illegal.at-sign.bibtex\"}]}]}},\"scopeName\":\"text.bibtex\",\"uuid\":\"47F30BA1-6B1D-11D9-9A60-000D93589AF6\"}");

/***/ }),

/***/ "../../packages/latex/data/LaTeX.tmLanguage.json":
/*!*************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/data/LaTeX.tmLanguage.json ***!
  \*************************************************************************************/
/*! exports provided: fileTypes, firstLineMatch, keyEquivalent, name, patterns, repository, scopeName, uuid, default */
/***/ (function(module) {

module.exports = JSON.parse("{\"fileTypes\":[\"tex\"],\"firstLineMatch\":\"^\\\\\\\\documentclass(\\\\[.*\\\\])?\\\\{.*\\\\}\",\"keyEquivalent\":\"^~L\",\"name\":\"LaTeX\",\"patterns\":[{\"comment\":\"This scope identifies partially typed commands such as `\\\\tab`. We use this to trigger “Command Completion” only when it makes sense.\",\"match\":\"(?<=\\\\\\\\[\\\\w@]|\\\\\\\\[\\\\w@]{2}|\\\\\\\\[\\\\w@]{3}|\\\\\\\\[\\\\w@]{4}|\\\\\\\\[\\\\w@]{5}|\\\\\\\\[\\\\w@]{6})\\\\s\",\"name\":\"meta.space-after-command.latex\"},{\"begin\":\"((\\\\\\\\)(?:usepackage|documentclass))((?:\\\\[[^\\\\[]*?\\\\])*)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.control.preamble.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"4\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"contentName\":\"support.class.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.preamble.latex\",\"patterns\":[{\"include\":\"$self\"}]},{\"begin\":\"((\\\\\\\\)(?:include|input))(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.control.include.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"contentName\":\"support.class.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.include.latex\",\"patterns\":[{\"include\":\"$self\"}]},{\"begin\":\"((\\\\\\\\)((?:sub){0,2}section|(?:sub)?paragraph|chapter|part|addpart|addchap|addsec|minisec|frametitle)(?:\\\\*)?)((?:\\\\[[^\\\\[]*?\\\\]){0,2})(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.section.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"4\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"5\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"comment\":\"this works OK with all kinds of crazy stuff as long as section is one line\",\"contentName\":\"entity.name.section.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.function.section.$3.latex\",\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"(^\\\\s*)?(?=\\\\\\\\begin\\\\{(lstlisting|minted|pyglist)\\\\})\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.whitespace.embedded.leading.latex\"}},\"end\":\"(?!\\\\G)(\\\\s*$\\\\n?)?\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.whitespace.embedded.trailing.latex\"}},\"patterns\":[{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{(?:cpp|c)\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.cpp.embedded.latex\",\"patterns\":[{\"include\":\"source.cpp.embedded.latex\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{css\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.css\",\"patterns\":[{\"include\":\"source.css\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{html\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"text.html\",\"patterns\":[{\"include\":\"text.html\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{java\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.java\",\"patterns\":[{\"include\":\"source.java\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{(?:js|javascript)\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.js\",\"patterns\":[{\"include\":\"source.js\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{lua\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.lua\",\"patterns\":[{\"include\":\"source.lua\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{(?:python|py)\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.python\",\"patterns\":[{\"include\":\"source.python\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{xml\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"yaml.xml\",\"patterns\":[{\"include\":\"yaml.xml\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{minted\\\\}(?:\\\\[.*\\\\])?\\\\{yaml\\\\})\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"source.yaml\",\"patterns\":[{\"include\":\"source.yaml\"}],\"end\":\"(\\\\\\\\end\\\\{minted\\\\})\"},{\"begin\":\"(\\\\\\\\begin\\\\{(?:lstlisting|minted|pyglist)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#minted-env\"}]}},\"contentName\":\"meta.function.embedded.latex\",\"end\":\"(\\\\\\\\end\\\\{(?:lstlisting|minted|pyglist)\\\\})\",\"name\":\"meta.embedded.block.generic\"}]},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{(cppcode(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.cpp.embedded.latex\",\"patterns\":[{\"include\":\"source.cpp.embedded.latex\"},{\"include\":\"source.cpp\"}],\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{(hscode(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.haskell\",\"patterns\":[{\"include\":\"source.haskell\"}],\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{(luacode(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.lua\",\"patterns\":[{\"include\":\"source.lua\"}],\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{((?:pycode|sageblock|sagesilent|sageverbatim|sageexample|sagecommandline)(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.python\",\"patterns\":[{\"include\":\"source.python\"}],\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{(scalacode(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.scala\",\"patterns\":[{\"include\":\"source.scala\"}],\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{([a-z]*code(?:\\\\*)?)\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"end\":\"(\\\\\\\\end\\\\{\\\\2\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{asy\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.asymptote\",\"patterns\":[{\"include\":\"source.asymptote\"}],\"end\":\"(\\\\\\\\end\\\\{asy\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{dot2tex\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.dot\",\"patterns\":[{\"include\":\"source.dot\"}],\"end\":\"(\\\\\\\\end\\\\{dot2tex\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"((?:\\\\s*)\\\\\\\\begin\\\\{gnuplot\\\\}(?:\\\\[.*\\\\])?)\",\"captures\":{\"1\":{\"patterns\":[{\"include\":\"#code-env\"}]}},\"contentName\":\"source.gnuplot\",\"patterns\":[{\"include\":\"source.gnuplot\"}],\"end\":\"(\\\\\\\\end\\\\{gnuplot\\\\}(?:\\\\s*\\\\n)?)\"},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)((?:fboxv|boxedv|V|v)erbatim\\\\*?)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"markup.raw.verbatim.latex\",\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})\",\"name\":\"meta.function.verbatim.latex\"},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(VerbatimOut)(\\\\})(\\\\{)([^\\\\}]*)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"},\"6\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"7\":{\"name\":\"support.class.latex\"},\"8\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"markup.raw.verbatim.latex\",\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\VerbatimOut)(\\\\})\",\"name\":\"meta.function.verbatim.latex\"},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(alltt)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"markup.raw.verbatim.latex\",\"end\":\"((\\\\\\\\)end)(\\\\{)(alltt)(\\\\})\",\"name\":\"meta.function.alltt.latex\",\"patterns\":[{\"captures\":{\"1\":{\"name\":\"punctuation.definition.function.tex\"}},\"match\":\"(\\\\\\\\)[A-Za-z]+\",\"name\":\"support.function.general.tex\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)((?:C|c)omment)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"punctuation.definition.comment.tex\",\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})\",\"name\":\"meta.function.verbatim.latex\"},{\"captures\":{\"1\":{\"name\":\"support.function.url.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"markup.underline.link.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"match\":\"(?:\\\\s*)((\\\\\\\\)(?:url|href))(\\\\{)([^}]*)(\\\\})\",\"name\":\"meta.function.link.url.latex\"},{\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"comment\":\"These two patterns match the \\\\begin{document} and \\\\end{document} commands, so that the environment matching pattern following them will ignore those commands.\",\"match\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(document)(\\\\})\",\"name\":\"meta.function.begin-document.latex\"},{\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"match\":\"(?:\\\\s*)((\\\\\\\\)end)(\\\\{)(document)(\\\\})\",\"name\":\"meta.function.end-document.latex\"},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)((?:align|equation|(?:IEEE)?eqnarray|multline|aligned|alignat|split|gather|gathered|cases|displaymath|[a-zA-Z]*matrix)(?:\\\\*)?)(\\\\})(\\\\s*\\\\n)?\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"support.class.math.block.environment.latex\",\"end\":\"(?:\\\\s*)((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.math.latex\",\"patterns\":[{\"match\":\"(?<!\\\\\\\\)&\",\"name\":\"keyword.control.equation.align.latex\"},{\"match\":\"\\\\\\\\\\\\\\\\\",\"name\":\"keyword.control.equation.newline.latex\"},{\"include\":\"#definition-label\"},{\"include\":\"text.tex#math\"},{\"include\":\"$base\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(array|tabular[xy*]?|xltabular)(\\\\})(\\\\s*\\\\n)?\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"contentName\":\"meta.data.environment.tabular.latex\",\"end\":\"(?:\\\\s*)((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.tabular.latex\",\"patterns\":[{\"match\":\"(?<!\\\\\\\\)&\",\"name\":\"keyword.control.table.cell.latex\"},{\"match\":\"\\\\\\\\\\\\\\\\\",\"name\":\"keyword.control.table.newline.latex\"},{\"include\":\"$base\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(itemize|enumerate|description|list)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.list.latex\",\"patterns\":[{\"include\":\"$base\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(tikzpicture)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"end\":\"((\\\\\\\\)end)(\\\\{)(tikzpicture)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.latex.tikz\",\"patterns\":[{\"include\":\"$base\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(frame)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"end\":\"((\\\\\\\\)end)(\\\\{)(frame)(\\\\})\",\"name\":\"meta.function.environment.frame.latex\",\"patterns\":[{\"include\":\"$base\"}]},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(mpost[*]?)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.latex.mpost\"},{\"begin\":\"(?:\\\\s*)((\\\\\\\\)begin)(\\\\{)(\\\\w+[*]?)(\\\\})\",\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"end\":\"((\\\\\\\\)end)(\\\\{)(\\\\4)(\\\\})(?:\\\\s*\\\\n)?\",\"name\":\"meta.function.environment.general.latex\",\"patterns\":[{\"include\":\"$base\"}]},{\"captures\":{\"1\":{\"name\":\"storage.type.function.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.begin.latex\"},\"4\":{\"name\":\"support.function.general.tex\"},\"5\":{\"name\":\"punctuation.definition.function.latex\"},\"6\":{\"name\":\"punctuation.definition.end.latex\"}},\"match\":\"((\\\\\\\\)(?:newcommand|renewcommand|(?:re)?newrobustcmd|DeclareRobustCommand))\\\\*?({)((\\\\\\\\)[^}]*)(})\"},{\"begin\":\"((\\\\\\\\)marginpar)((?:\\\\[[^\\\\[]*?\\\\])*)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.marginpar.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"4\":{\"name\":\"punctuation.definition.marginpar.begin.latex\"}},\"contentName\":\"meta.paragraph.margin.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.marginpar.end.latex\"}},\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"((\\\\\\\\)footnote)((?:\\\\[[^\\\\[]*?\\\\])*)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.footnote.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"4\":{\"name\":\"punctuation.definition.footnote.begin.latex\"}},\"contentName\":\"entity.name.footnote.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.footnote.end.latex\"}},\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"((\\\\\\\\)emph)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.emph.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.emph.begin.latex\"}},\"contentName\":\"markup.italic.emph.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.emph.end.latex\"}},\"name\":\"meta.function.emph.latex\",\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"((\\\\\\\\)textit)(\\\\{)\",\"captures\":{\"1\":{\"name\":\"support.function.textit.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.textit.begin.latex\"}},\"comment\":\"We put the keyword in a capture and name this capture, so that disabling spell checking for “keyword” won't be inherited by the argument to \\\\textit{...}.\\n\\nPut specific matches for particular LaTeX keyword.functions before the last two more general functions\",\"contentName\":\"markup.italic.textit.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.textit.end.latex\"}},\"name\":\"meta.function.textit.latex\",\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"((\\\\\\\\)textbf)(\\\\{)\",\"captures\":{\"1\":{\"name\":\"support.function.textbf.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.textbf.begin.latex\"}},\"contentName\":\"markup.bold.textbf.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.textbf.end.latex\"}},\"name\":\"meta.function.textbf.latex\",\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"begin\":\"((\\\\\\\\)texttt)(\\\\{)\",\"captures\":{\"1\":{\"name\":\"support.function.texttt.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.texttt.begin.latex\"}},\"contentName\":\"markup.raw.texttt.latex\",\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.texttt.end.latex\"}},\"name\":\"meta.function.texttt.latex\",\"patterns\":[{\"include\":\"text.tex#braces\"},{\"include\":\"$base\"}]},{\"captures\":{\"0\":{\"name\":\"keyword.other.item.latex\"},\"1\":{\"name\":\"punctuation.definition.keyword.latex\"}},\"match\":\"(\\\\\\\\)item\\\\b\",\"name\":\"meta.scope.item.latex\"},{\"begin\":\"((\\\\\\\\)(?:auto|foot|full|no|short|text|paren)?[cC]ite(?:al)?(?:p|s|t|author|year(?:par)?|title)?[ANP]*\\\\*?)((?:(?:\\\\[[^\\\\]]*\\\\]){0,2}\\\\{[\\\\w:.]+\\\\})*)(?:(\\\\[)[^\\\\]]*(\\\\]))?(?:(\\\\[)[^\\\\]]*(\\\\]))?(\\\\{)\",\"captures\":{\"1\":{\"name\":\"keyword.control.cite.latex\"},\"2\":{\"name\":\"punctuation.definition.keyword.latex\"},\"3\":{\"patterns\":[{\"include\":\"#autocites-arg\"}]},\"4\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"},\"6\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"7\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"},\"8\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.citation.latex\",\"patterns\":[{\"match\":\"[\\\\w:.]+\",\"name\":\"constant.other.reference.citation.latex\"}]},{\"begin\":\"((\\\\\\\\)bibentry)(\\\\{)\",\"captures\":{\"1\":{\"name\":\"keyword.control.cite.latex\"},\"2\":{\"name\":\"punctuation.definition.keyword.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.citation.latex\",\"patterns\":[{\"match\":\"[\\\\w:.]+\",\"name\":\"constant.other.reference.citation.latex\"}]},{\"begin\":\"((\\\\\\\\)(?:\\\\w*[r|R]ef\\\\*?))(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.control.ref.latex\"},\"2\":{\"name\":\"punctuation.definition.keyword.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.reference.label.latex\",\"patterns\":[{\"match\":\"[a-zA-Z0-9\\\\.,:/*!^_-]\",\"name\":\"constant.other.reference.label.latex\"}]},{\"include\":\"#definition-label\"},{\"begin\":\"((\\\\\\\\)verb[\\\\*]?)\\\\s*((\\\\\\\\)scantokens)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"support.function.verb.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"support.function.verb.latex\"},\"4\":{\"name\":\"punctuation.definition.verb.latex\"},\"5\":{\"name\":\"punctuation.definition.begin.latex\"}},\"contentName\":\"markup.raw.verb.latex\",\"end\":\"(\\\\})\",\"endCaptures\":{\"1\":{\"name\":\"punctuation.definition.end.latex\"}},\"name\":\"meta.function.verb.latex\",\"patterns\":[{\"include\":\"$self\"}]},{\"captures\":{\"1\":{\"name\":\"support.function.verb.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.verb.latex\"},\"4\":{\"name\":\"markup.raw.verb.latex\"},\"5\":{\"name\":\"punctuation.definition.verb.latex\"}},\"match\":\"((\\\\\\\\)verb[\\\\*]?)\\\\s*((?<=\\\\s)\\\\S|[^a-zA-Z])(.*?)(\\\\3|$)\",\"name\":\"meta.function.verb.latex\"},{\"captures\":{\"1\":{\"name\":\"support.function.verb.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"4\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"},\"6\":{\"name\":\"punctuation.definition.verb.latex\"},\"7\":{\"name\":\"markup.raw.verb.latex\"},\"8\":{\"name\":\"punctuation.definition.verb.latex\"},\"9\":{\"name\":\"punctuation.definition.verb.latex\"},\"10\":{\"name\":\"markup.raw.verb.latex\"},\"11\":{\"name\":\"punctuation.definition.verb.latex\"}},\"match\":\"((\\\\\\\\)(?:mint|mintinline))((?:\\\\[[^\\\\[]*?\\\\])?)(\\\\{)[a-zA-Z]*(\\\\})(?:(?:([^a-zA-Z\\\\{])(.*?)(\\\\6))|(?:(\\\\{)(.*?)(\\\\})))\",\"name\":\"meta.function.verb.latex\"},{\"captures\":{\"1\":{\"name\":\"support.function.verb.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"patterns\":[{\"include\":\"#optional-arg\"}]},\"4\":{\"name\":\"punctuation.definition.verb.latex\"},\"5\":{\"name\":\"markup.raw.verb.latex\"},\"6\":{\"name\":\"punctuation.definition.verb.latex\"},\"7\":{\"name\":\"punctuation.definition.verb.latex\"},\"8\":{\"name\":\"markup.raw.verb.latex\"},\"9\":{\"name\":\"punctuation.definition.verb.latex\"}},\"match\":\"((\\\\\\\\)[a-z]+inline)((?:\\\\[[^\\\\[]*?\\\\])?)(?:(?:([^a-zA-Z\\\\{])(.*?)(\\\\4))|(?:(\\\\{)(.*?)(\\\\})))\",\"name\":\"meta.function.verb.latex\"},{\"match\":\"\\\\\\\\(?:newline|pagebreak|clearpage|linebreak|pause)(?:\\\\b)\",\"name\":\"keyword.control.layout.tex\"},{\"begin\":\"\\\\\\\\\\\\(\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.begin.latex\"}},\"end\":\"\\\\\\\\\\\\)\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.end.latex\"}},\"name\":\"support.class.math.latex\",\"patterns\":[{\"include\":\"text.tex#math\"},{\"include\":\"$base\"}]},{\"begin\":\"\\\\$\\\\$\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.begin.latex\"}},\"end\":\"\\\\$\\\\$\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.end.latex\"}},\"name\":\"support.class.math.latex\",\"patterns\":[{\"match\":\"\\\\\\\\\\\\$\",\"name\":\"constant.character.escape.tex\"},{\"include\":\"text.tex#math\"},{\"include\":\"$base\"}]},{\"begin\":\"\\\\\\\\\\\\[\",\"beginCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.begin.latex\"}},\"end\":\"\\\\\\\\\\\\]\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.string.end.latex\"}},\"name\":\"support.class.math.latex\",\"patterns\":[{\"include\":\"text.tex#math\"},{\"include\":\"$base\"}]},{\"captures\":{\"1\":{\"name\":\"punctuation.definition.constant.latex\"}},\"match\":\"(\\\\\\\\)(text(s(terling|ixoldstyle|urd|e(ction|venoldstyle|rvicemark))|yen|n(ineoldstyle|umero|aira)|c(ircledP|o(py(left|right)|lonmonetary)|urrency|e(nt(oldstyle)?|lsius))|t(hree(superior|oldstyle|quarters(emdash)?)|i(ldelow|mes)|w(o(superior|oldstyle)|elveudash)|rademark)|interrobang(down)?|zerooldstyle|o(hm|ne(superior|half|oldstyle|quarter)|penbullet|rd(feminine|masculine))|d(i(scount|ed|v(orced)?)|o(ng|wnarrow|llar(oldstyle)?)|egree|agger(dbl)?|blhyphen(char)?)|uparrow|p(ilcrow|e(so|r(t(housand|enthousand)|iodcentered))|aragraph|m)|e(stimated|ightoldstyle|uro)|quotes(traight(dblbase|base)|ingle)|f(iveoldstyle|ouroldstyle|lorin|ractionsolidus)|won|l(not|ira|e(ftarrow|af)|quill|angle|brackdbl)|a(s(cii(caron|dieresis|acute|grave|macron|breve)|teriskcentered)|cutedbl)|r(ightarrow|e(cipe|ferencemark|gistered)|quill|angle|brackdbl)|g(uarani|ravedbl)|m(ho|inus|u(sicalnote)?|arried)|b(igcircle|orn|ullet|lank|a(ht|rdbl)|rokenbar)))\\\\b\",\"name\":\"constant.character.latex\"},{\"captures\":{\"1\":{\"name\":\"punctuation.definition.column-specials.begin.latex\"},\"2\":{\"name\":\"punctuation.definition.column-specials.end.latex\"}},\"match\":\"(?:<|>)(\\\\{)\\\\$(\\\\})\",\"name\":\"meta.column-specials.latex\"},{\"include\":\"text.tex\"}],\"repository\":{\"optional-arg\":{\"patterns\":[{\"captures\":{\"1\":{\"name\":\"punctuation.definition.optional.arguments.begin.latex\"},\"2\":{\"name\":\"variable.parameter.latex\"},\"3\":{\"name\":\"punctuation.definition.optional.arguments.end.latex\"}},\"match\":\"(\\\\[)([^\\\\[]*?)(\\\\])\"}]},\"autocites-arg\":{\"patterns\":[{\"captures\":{\"1\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"2\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"4\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"6\":{\"name\":\"constant.other.reference.citation.latex\"},\"7\":{\"name\":\"punctuation.definition.arguments.end.latex\"},\"8\":{\"patterns\":[{\"include\":\"#autocites-arg\"}]}},\"match\":\"(?:(\\\\[)[^\\\\]]*(\\\\]))?(?:(\\\\[)[^\\\\]]*(\\\\]))?(\\\\{)([\\\\w:.]+)(\\\\})(.*)\"}]},\"minted-env\":{\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"},\"6\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"7\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"},\"8\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"9\":{\"name\":\"variable.parameter.function.latex\"},\"10\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"match\":\"((\\\\\\\\)(?:begin|end))(\\\\{)(lstlisting|minted|pyglist)(\\\\})(?:(\\\\[).*(\\\\]))?(?:(\\\\{)([a-z]*)(\\\\}))?\"},\"code-env\":{\"captures\":{\"1\":{\"name\":\"support.function.be.latex\"},\"2\":{\"name\":\"punctuation.definition.function.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"},\"4\":{\"name\":\"variable.parameter.function.latex\"},\"5\":{\"name\":\"punctuation.definition.arguments.end.latex\"},\"6\":{\"name\":\"punctuation.definition.arguments.optional.begin.latex\"},\"7\":{\"name\":\"punctuation.definition.arguments.optional.end.latex\"}},\"match\":\"(?:\\\\s*)((\\\\\\\\)(?:begin|end))(\\\\{)([a-z]*code(?:\\\\*)?|gnuplot|dot2tex|asy|sage[a-z]*)(\\\\})(?:(\\\\[).*(\\\\]))?\"},\"definition-label\":{\"begin\":\"((\\\\\\\\)label)(\\\\{)\",\"beginCaptures\":{\"1\":{\"name\":\"keyword.control.label.latex\"},\"2\":{\"name\":\"punctuation.definition.keyword.latex\"},\"3\":{\"name\":\"punctuation.definition.arguments.begin.latex\"}},\"end\":\"\\\\}\",\"endCaptures\":{\"0\":{\"name\":\"punctuation.definition.arguments.end.latex\"}},\"name\":\"meta.definition.label.latex\",\"patterns\":[{\"match\":\"[a-zA-Z0-9\\\\.,:/*!^_-]\",\"name\":\"variable.parameter.definition.label.latex\"}]}},\"scopeName\":\"text.tex.latex\",\"uuid\":\"3BEEA00C-6B1D-11D9-B8AD-000D93589AF6\"}");

/***/ }),

/***/ "../../packages/latex/lib/browser/bibtex-grammar.js":
/*!****************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/bibtex-grammar.js ***!
  \****************************************************************************************/
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
var BibTeXContribution = /** @class */ (function () {
    function BibTeXContribution() {
        this.BibTEX_LANGUAGE_ID = 'bibtex';
        this.config = {
            'comments': {
                'lineComment': '%'
            },
            'brackets': [
                ['{', '}'],
                ['[', ']'],
                ['(', ')']
            ],
            'autoClosingPairs': [
                { 'open': '{', 'close': '}', 'notIn': ['string'] },
                { 'open': '[', 'close': ']', 'notIn': ['string'] },
                { 'open': '(', 'close': ')', 'notIn': ['string'] },
                { 'open': '`', 'close': '`', 'notIn': ['string', 'comment'] },
                { 'open': '"', 'close': '"', 'notIn': ['string', 'comment'] },
                { 'open': '$', 'close': '$', 'notIn': ['string', 'comment'] }
            ],
            'surroundingPairs': [
                { 'open': '{', 'close': '}' },
                { 'open': '[', 'close': ']' },
                { 'open': '(', 'close': ')' },
                { 'open': '`', 'close': '`' },
                { 'open': '"', 'close': '"' },
                { 'open': '$', 'close': '$' }
            ]
        };
    }
    BibTeXContribution.prototype.registerTextmateLanguage = function (registry) {
        monaco.languages.register({
            id: this.BibTEX_LANGUAGE_ID,
            'aliases': [
                'BibTeX',
                'bibtex'
            ],
            'extensions': [
                '.bib'
            ],
            'mimetypes': [
                'text/bibtex'
            ]
        });
        monaco.languages.setLanguageConfiguration(this.BibTEX_LANGUAGE_ID, this.config);
        var biblatexGrammar = __webpack_require__(/*! ../../data/Bibtex.tmLanguage.json */ "../../packages/latex/data/Bibtex.tmLanguage.json");
        registry.registerTextmateGrammarScope('text.bibtex', {
            getGrammarDefinition: function () {
                return __awaiter(this, void 0, void 0, function () {
                    return __generator(this, function (_a) {
                        return [2 /*return*/, {
                                format: 'json',
                                content: biblatexGrammar
                            }];
                    });
                });
            }
        });
        registry.mapLanguageIdToTextmateGrammar(this.BibTEX_LANGUAGE_ID, 'text.bibtex');
    };
    BibTeXContribution = __decorate([
        inversify_1.injectable()
    ], BibTeXContribution);
    return BibTeXContribution;
}());
exports.BibTeXContribution = BibTeXContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/bibtex-grammar'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/latex-engine.js":
/*!**************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/latex-engine.js ***!
  \**************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var EngineStatus;
(function (EngineStatus) {
    EngineStatus[EngineStatus["Init"] = 1] = "Init";
    EngineStatus[EngineStatus["Ready"] = 2] = "Ready";
    EngineStatus[EngineStatus["Busy"] = 3] = "Busy";
    EngineStatus[EngineStatus["Error"] = 4] = "Error";
})(EngineStatus = exports.EngineStatus || (exports.EngineStatus = {}));
var ENGINE_PATH = '/bin/swiftlatex.js';
var CompileResult = /** @class */ (function () {
    function CompileResult() {
        this.pdf = undefined;
        this.status = -254;
        this.log = 'No log';
    }
    return CompileResult;
}());
exports.CompileResult = CompileResult;
var LaTeXEngine = /** @class */ (function () {
    function LaTeXEngine() {
        this.latexWorker = undefined;
        this.latexWorkerStatus = EngineStatus.Init;
        this.loadEngine().then(function (_) {
            console.log('Engine is ready to go now');
        }).catch(function (e) {
            console.error('Engine fails to load.');
        });
    }
    LaTeXEngine.prototype.isReady = function () {
        return this.latexWorkerStatus === EngineStatus.Ready;
    };
    LaTeXEngine.prototype.loadEngine = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.latexWorker !== undefined) {
                            throw new Error('Other instance is running, abort()');
                        }
                        this.latexWorkerStatus = EngineStatus.Init;
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker = new Worker(ENGINE_PATH);
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    if (cmd === 'postRun') {
                                        _this.latexWorkerStatus = EngineStatus.Ready;
                                        resolve();
                                    }
                                    else {
                                        _this.latexWorkerStatus = EngineStatus.Error;
                                        reject();
                                    }
                                };
                                _this.latexWorker.onerror = function (ev) {
                                    reject();
                                };
                            })];
                    case 1:
                        _a.sent();
                        this.latexWorker.onmessage = function (ev) {
                        };
                        this.latexWorker.onerror = function (ev) {
                        };
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXEngine.prototype.checkEngineStatus = function () {
        if (this.latexWorkerStatus !== EngineStatus.Ready) {
            throw Error('Engine is still spinning or not ready yet!');
        }
    };
    LaTeXEngine.prototype.compileLaTeX = function () {
        return __awaiter(this, void 0, void 0, function () {
            var start_compile_time, res;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        this.checkEngineStatus();
                        this.latexWorkerStatus = EngineStatus.Busy;
                        start_compile_time = performance.now();
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    var result = data['result'];
                                    var log = data['log'];
                                    var status = data['status'];
                                    var pdf = new Uint8Array(data['pdf']);
                                    if (cmd === 'compile') {
                                        _this.latexWorkerStatus = EngineStatus.Ready;
                                        if (result === 'failed') {
                                            console.error('Engine crushed terribly. Log: ' + log);
                                            _this.latexWorkerStatus = EngineStatus.Error;
                                            reject(log);
                                        }
                                        else {
                                            console.log('Engine compilation finish ' + (performance.now() - start_compile_time));
                                            var nice_report = new CompileResult();
                                            nice_report.status = status;
                                            nice_report.log = log;
                                            nice_report.pdf = pdf;
                                            resolve(nice_report);
                                        }
                                    }
                                };
                                _this.latexWorker.postMessage({ 'cmd': 'compilelatex' });
                            })];
                    case 1:
                        res = _a.sent();
                        this.latexWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/, res];
                }
            });
        });
    };
    /* Internal Use */
    LaTeXEngine.prototype.compileFormat = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        this.checkEngineStatus();
                        this.latexWorkerStatus = EngineStatus.Busy;
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    if (cmd === 'compile') {
                                        if (data['status'] === 0) {
                                            var formatArray = data['pdf']; /* PDF for result */
                                            var formatBlob = new Blob([formatArray], { type: 'application/octet-stream' });
                                            var formatURL_1 = URL.createObjectURL(formatBlob);
                                            setTimeout(function () { URL.revokeObjectURL(formatURL_1); }, 30000);
                                            console.log('Download format file via ' + formatURL_1);
                                        }
                                        else {
                                            reject(data['log']);
                                        }
                                    }
                                };
                                _this.latexWorker.postMessage({ 'cmd': 'compileformat' });
                            })];
                    case 1:
                        _a.sent();
                        this.latexWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXEngine.prototype.setEngineMainFile = function (filename) {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'setmainfile', 'url': filename });
        }
    };
    LaTeXEngine.prototype.writeMemFSFile = function (srccode, filename) {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'writefile', 'url': filename, 'src': srccode });
        }
    };
    LaTeXEngine.prototype.makeMemFSFolder = function (folder) {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            if (folder === '' || folder === '/') {
                return;
            }
            this.latexWorker.postMessage({ 'cmd': 'mkdir', 'url': folder });
        }
    };
    LaTeXEngine.prototype.flushCache = function () {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            console.warn('Flushing');
            this.latexWorker.postMessage({ 'cmd': 'flushcache' });
        }
    };
    LaTeXEngine.prototype.closeWorker = function () {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'grace' });
            this.latexWorker = undefined;
        }
    };
    LaTeXEngine = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], LaTeXEngine);
    return LaTeXEngine;
}());
exports.LaTeXEngine = LaTeXEngine;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/latex-engine'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/latex-grammar.js":
/*!***************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/latex-grammar.js ***!
  \***************************************************************************************/
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
var TeXContribution = /** @class */ (function () {
    function TeXContribution() {
        this.TEX_LANGUAGE_ID = 'latex';
        this.config = {
            'comments': {
                'lineComment': '%'
            },
            'brackets': [
                ['{', '}'],
                ['[', ']'],
                ['(', ')']
            ],
            'autoClosingPairs': [
                { 'open': '{', 'close': '}', 'notIn': ['string'] },
                { 'open': '[', 'close': ']', 'notIn': ['string'] },
                { 'open': '(', 'close': ')', 'notIn': ['string'] },
                { 'open': '`', 'close': '`', 'notIn': ['string', 'comment'] },
                { 'open': '"', 'close': '"', 'notIn': ['string', 'comment'] },
                { 'open': '$', 'close': '$', 'notIn': ['string', 'comment'] }
            ],
            'surroundingPairs': [
                { 'open': '{', 'close': '}' },
                { 'open': '[', 'close': ']' },
                { 'open': '(', 'close': ')' },
                { 'open': '`', 'close': '`' },
                { 'open': '"', 'close': '"' },
                { 'open': '$', 'close': '$' }
            ]
        };
    }
    TeXContribution.prototype.registerTextmateLanguage = function (registry) {
        monaco.languages.register({
            id: this.TEX_LANGUAGE_ID,
            'aliases': [
                'LaTeX',
                'latex'
            ],
            'extensions': [
                '.tex'
            ],
            'mimetypes': [
                'text/latex'
            ]
        });
        monaco.languages.setLanguageConfiguration(this.TEX_LANGUAGE_ID, this.config);
        var latexGrammar = __webpack_require__(/*! ../../data/LaTeX.tmLanguage.json */ "../../packages/latex/data/LaTeX.tmLanguage.json");
        registry.registerTextmateGrammarScope('text.tex.latex', {
            getGrammarDefinition: function () {
                return __awaiter(this, void 0, void 0, function () {
                    return __generator(this, function (_a) {
                        return [2 /*return*/, {
                                format: 'json',
                                content: latexGrammar
                            }];
                    });
                });
            }
        });
        registry.mapLanguageIdToTextmateGrammar(this.TEX_LANGUAGE_ID, 'text.tex.latex');
    };
    TeXContribution = __decorate([
        inversify_1.injectable()
    ], TeXContribution);
    return TeXContribution;
}());
exports.TeXContribution = TeXContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/latex-grammar'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/latex-log-parser.js":
/*!******************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/latex-log-parser.js ***!
  \******************************************************************************************/
/*! exports provided: LatexParserModule */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LatexParserModule", function() { return LatexParserModule; });
const LatexParserModule = (function() {
    var PAGE_REGEX, HBOX_WARNING_REGEX, LATEX_WARNING_REGEX, LINES_REGEX, LOG_WRAP_LIMIT, LatexParser, LogText, PACKAGE_REGEX, PACKAGE_WARNING_REGEX, state;
    LOG_WRAP_LIMIT = 79;
    LATEX_WARNING_REGEX = /^LaTeX Warning: (.*)$/;
    HBOX_WARNING_REGEX = /^(Over|Under)full \\(v|h)box/;
    PACKAGE_WARNING_REGEX = /^(Package \b.+\b Warning:.*)$/;
    LINES_REGEX = /lines? ([0-9]+)/;
    PACKAGE_REGEX = /^Package (\b.+\b) Warning/;
    PAGE_REGEX = /^Output written on "?.*\.[^ ."]+"? \(([0-9]+) page/;
    LogText = function(text) {
        var i, wrappedLines;
        this.text = text.replace(/(\r\n)|\r/g, '\n');
        wrappedLines = this.text.split('\n');
        this.lines = [wrappedLines[0]];
        i = 1;
        while (i < wrappedLines.length) {
            if (wrappedLines[i - 1].length === LOG_WRAP_LIMIT && wrappedLines[i - 1].slice(-3) !== '...') {
                this.lines[this.lines.length - 1] += wrappedLines[i];
            } else {
                this.lines.push(wrappedLines[i]);
            }
            i++;
        }
        this.row = 0;
    };
    (function() {
        this.nextLine = function() {
            this.row++;
            if (this.row >= this.lines.length) {
                return false;
            } else {
                return this.lines[this.row];
            }
        };
        this.rewindLine = function() {
            this.row--;
        };
        this.linesUpToNextWhitespaceLine = function() {
            return this.linesUpToNextMatchingLine(/^ *$/);
        };
        this.linesUpToNextMatchingLine = function(match) {
            var lines, nextLine;
            lines = [];
            nextLine = this.nextLine();
            if (nextLine !== false) {
                lines.push(nextLine);
            }
            while (nextLine !== false && !nextLine.match(match) && nextLine !== false) {
                nextLine = this.nextLine();
                if (nextLine !== false) {
                    lines.push(nextLine);
                }
            }
            return lines;
        };
    }).call(LogText.prototype);
    state = {
        NORMAL: 0,
        ERROR: 1
    };
    LatexParser = function(text, options) {
        this.log = new LogText(text);
        this.state = state.NORMAL;
        options = options || {};
        this.fileBaseNames = options.fileBaseNames || [/compiles/, /\/usr\/local/];
        this.ignoreDuplicates = options.ignoreDuplicates;
        this.data = [];
        this.fileStack = [];
        this.currentFileList = this.rootFileList = [];
        this.openParens = 0;
    };
    (function() {
        this.parse = function() {
            var lineNo;
            while ((this.currentLine = this.log.nextLine()) !== false) {
                if (this.state === state.NORMAL) {
                    if (this.currentLineIsError()) {
                        this.state = state.ERROR;
                        this.currentError = {
                            line: null,
                            file: this.currentFilePath,
                            level: 'error',
                            message: this.currentLine.slice(2),
                            content: '',
                            raw: this.currentLine + '\n'
                        };
                    } else if (this.currentLineIsPageNumber()) {
                        this.parsePageNumber();
                    } else if (this.currentLineIsRunawayArgument()) {
                        this.parseRunawayArgumentError();
                    } else if (this.currentLineIsWarning()) {
                        this.parseSingleWarningLine(LATEX_WARNING_REGEX);
                    } else if (this.currentLineIsHboxWarning()) {
                        this.parseHboxLine();
                    } else if (this.currentLineIsPackageWarning()) {
                        this.parseMultipleWarningLine();
                    } else {
                        this.parseParensForFilenames();
                    }
                }
                if (this.state === state.ERROR) {
                    this.currentError.content += this.log.linesUpToNextMatchingLine(/^l\.[0-9]+/).join('\n');
                    this.currentError.content += '\n';
                    this.currentError.content += this.log.linesUpToNextWhitespaceLine().join('\n');
                    this.currentError.content += '\n';
                    this.currentError.content += this.log.linesUpToNextWhitespaceLine().join('\n');
                    this.currentError.raw += this.currentError.content;
                    lineNo = this.currentError.raw.match(/l\.([0-9]+)/);
                    if (lineNo) {
                        this.currentError.line = parseInt(lineNo[1], 10);
                    }
                    this.data.push(this.currentError);
                    this.state = state.NORMAL;
                }
            }
            return this.postProcess(this.data);
        };
        this.currentLineIsError = function() {
            return this.currentLine[0] === '!';
        };
        this.currentLineIsRunawayArgument = function() {
            return this.currentLine.match(/^Runaway argument/);
        };
        this.currentLineIsPageNumber = function() {
            return this.currentLine.match('^Output written on');
        };
        this.currentLineIsWarning = function() {
            return !!this.currentLine.match(LATEX_WARNING_REGEX);
        };
        this.currentLineIsPackageWarning = function() {
            return !!this.currentLine.match(PACKAGE_WARNING_REGEX);
        };
        this.currentLineIsHboxWarning = function() {
            return !!this.currentLine.match(HBOX_WARNING_REGEX);
        };
        this.parseRunawayArgumentError = function() {
            var lineNo;
            this.currentError = {
                line: null,
                file: this.currentFilePath,
                level: 'error',
                message: this.currentLine,
                content: '',
                raw: this.currentLine + '\n'
            };
            this.currentError.content += this.log.linesUpToNextWhitespaceLine().join('\n');
            this.currentError.content += '\n';
            this.currentError.content += this.log.linesUpToNextWhitespaceLine().join('\n');
            this.currentError.raw += this.currentError.content;
            lineNo = this.currentError.raw.match(/l\.([0-9]+)/);
            if (lineNo) {
                this.currentError.line = parseInt(lineNo[1], 10);
            }
            return this.data.push(this.currentError);
        };
        this.parseSingleWarningLine = function(prefix_regex) {
            var line, lineMatch, warning, warningMatch;
            warningMatch = this.currentLine.match(prefix_regex);
            if (!warningMatch) {
                return;
            }
            warning = warningMatch[1];
            lineMatch = warning.match(LINES_REGEX);
            line = lineMatch ? parseInt(lineMatch[1], 10) : null;
            this.data.push({
                line: line,
                file: this.currentFilePath,
                level: 'warning',
                message: warning,
                raw: warning
            });
        };
        this.parseMultipleWarningLine = function() {
            var line, lineMatch, packageMatch, packageName, prefixRegex, raw_message, warningMatch, warning_lines;
            warningMatch = this.currentLine.match(PACKAGE_WARNING_REGEX);
            if (!warningMatch) {
                return;
            }
            warning_lines = [warningMatch[1]];
            lineMatch = this.currentLine.match(LINES_REGEX);
            line = lineMatch ? parseInt(lineMatch[1], 10) : null;
            packageMatch = this.currentLine.match(PACKAGE_REGEX);
            packageName = packageMatch[1];
            prefixRegex = new RegExp('(?:\\(' + packageName + '\\))*[\\s]*(.*)', 'i');
            while (!!(this.currentLine = this.log.nextLine())) {
                lineMatch = this.currentLine.match(LINES_REGEX);
                line = lineMatch ? parseInt(lineMatch[1], 10) : line;
                warningMatch = this.currentLine.match(prefixRegex);
                warning_lines.push(warningMatch[1]);
            }
            raw_message = warning_lines.join(' ');
            this.data.push({
                line: line,
                file: this.currentFilePath,
                level: 'warning',
                message: raw_message,
                raw: raw_message
            });
        };
        this.parseHboxLine = function() {
            var line, lineMatch;
            lineMatch = this.currentLine.match(LINES_REGEX);
            line = lineMatch ? parseInt(lineMatch[1], 10) : null;
            this.data.push({
                line: line,
                file: this.currentFilePath,
                level: 'typesetting',
                message: this.currentLine,
                raw: this.currentLine
            });
        };
        this.parsePageNumber = function() {
            var line, lineMatch;
            lineMatch = this.currentLine.match(PAGE_REGEX);
            line = parseInt(lineMatch[1], 10);
            this.data.push({
                page: line,
                file: this.currentFilePath,
                level: 'pagenumber',
                message: this.currentLine,
                raw: this.currentLine
            });
        };
        this.parseParensForFilenames = function() {
            var filePath, newFile, pos, previousFile, token;
            pos = this.currentLine.search(/\(|\)/);
            if (pos !== -1) {
                token = this.currentLine[pos];
                this.currentLine = this.currentLine.slice(pos + 1);
                if (token === '(') {
                    filePath = this.consumeFilePath();
                    if (filePath) {
                        this.currentFilePath = filePath;
                        newFile = {
                            path: filePath,
                            files: []
                        };
                        this.fileStack.push(newFile);
                        this.currentFileList.push(newFile);
                        this.currentFileList = newFile.files;
                    } else {
                        this.openParens++;
                    }
                } else if (token === ')') {
                    if (this.openParens > 0) {
                        this.openParens--;
                    } else {
                        if (this.fileStack.length > 1) {
                            this.fileStack.pop();
                            previousFile = this.fileStack[this.fileStack.length - 1];
                            this.currentFilePath = previousFile.path;
                            this.currentFileList = previousFile.files;
                        }
                    }
                }
                this.parseParensForFilenames();
            }
        };
        this.consumeFilePath = function() {
            var endOfFilePath, path;
            if (!this.currentLine.match(/^\/?([^ \)]+\/)+/)) {
                return false;
            }
            endOfFilePath = this.currentLine.search(RegExp(' |\\)'));
            path = void 0;
            if (endOfFilePath === -1) {
                path = this.currentLine;
                this.currentLine = '';
            } else {
                path = this.currentLine.slice(0, endOfFilePath);
                this.currentLine = this.currentLine.slice(endOfFilePath);
            }
            return path;
        };
        return this.postProcess = function(data) {
            var all, errors, hashEntry, hashes, i, typesetting, warnings, pages;
            all = [];
            errors = [];
            warnings = [];
            typesetting = [];
            pages = [];
            hashes = [];
            hashEntry = function(entry) {
                return entry.raw;
            };
            i = 0;
            while (i < data.length) {
                if (this.ignoreDuplicates && hashes.indexOf(hashEntry(data[i])) > -1) {
                    i++;
                    continue;
                }
                if (data[i].level === 'error') {
                    errors.push(data[i]);
                } else if (data[i].level === 'typesetting') {
                    typesetting.push(data[i]);
                } else if (data[i].level === 'warning') {
                    warnings.push(data[i]);
                } else if (data[i].level === 'pagenumber') {
                    pages.push(data[i]);
                }

                all.push(data[i]);
                hashes.push(hashEntry(data[i]));
                i++;
            }
            return {
                pagenumber: pages,
                errors: errors,
                warnings: warnings,
                typesetting: typesetting,
                all: all,
                files: this.rootFileList
            };
        };
    }).call(LatexParser.prototype);
    LatexParser.parse = function(text, options) {
        return new LatexParser(text, options).parse();
    };
    return LatexParser;
})();

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/latex-log-parser'] = undefined;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/latex-preview-widget.js":
/*!**********************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/latex-preview-widget.js ***!
  \**********************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
* Copyright (C) 2019 Elliott Wen.
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
var widget_1 = __webpack_require__(/*! @theia/core/lib/browser/widgets/widget */ "../../packages/core/lib/browser/widgets/widget.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
__webpack_require__(/*! ../../src/browser/style/index.css */ "../../packages/latex/src/browser/style/index.css");
var xdv_parser_1 = __webpack_require__(/*! ./xdv-parser */ "../../packages/latex/lib/browser/xdv-parser.js");
var xdv_machine_1 = __webpack_require__(/*! ./xdv-machine */ "../../packages/latex/lib/browser/xdv-machine.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var $ = __webpack_require__(/*! jquery */ "../../node_modules/jquery/dist/jquery.js");
var LaTeXPreviewWidget = /** @class */ (function (_super) {
    __extends(LaTeXPreviewWidget, _super);
    function LaTeXPreviewWidget() {
        var _this = _super.call(this) || this;
        _this.xdvBuffer = undefined;
        _this.currentPage = 1;
        _this.totalPage = 1;
        _this.user_zoom_ratio = 1.0;
        _this.initFromViewer = false;
        _this.cachedFileList = [];
        _this.onDidChangePositionEmitter = new core_1.Emitter();
        _this.toDispose.push(_this.onDidChangePositionEmitter);
        _this.setupViewer();
        _this.update();
        return _this;
    }
    LaTeXPreviewWidget.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    Object.defineProperty(LaTeXPreviewWidget.prototype, "onDidChangePosition", {
        get: function () {
            return this.onDidChangePositionEmitter.event;
        },
        enumerable: true,
        configurable: true
    });
    LaTeXPreviewWidget.prototype.createButton = function (parent, title) {
        var _a;
        var className = [];
        for (var _i = 2; _i < arguments.length; _i++) {
            className[_i - 2] = arguments[_i];
        }
        var button = document.createElement('div');
        button.title = title;
        (_a = button.classList).add.apply(_a, __spread(className, ['swiftlatex-preview-toolbar-button']));
        parent.appendChild(button);
        return button;
    };
    LaTeXPreviewWidget.prototype.setupViewer = function () {
        /* self */
        this.title.label = 'preview';
        this.title.caption = this.title.label;
        this.title.closable = false;
        this.id = 'latex-preview';
        this.title.iconClass = 'fa fa-eye';
        this.addClass('swiftlatex-preview-widget');
        this.node.tabIndex = 0;
        var parent = this.node;
        /* toolbar */
        var toolbar = document.createElement('div');
        toolbar.classList.add('swiftlatex-preview-toolbar');
        parent.appendChild(toolbar);
        /* toolbar buttons */
        var previous_btn = this.createButton(toolbar, 'Show The Previous Page', 'swiftlatex-preview-toolbar-previous');
        this.toDispose.push(widget_1.addEventListener(previous_btn, 'click', this.handleBack.bind(this)));
        var next_btn = this.createButton(toolbar, 'Show The Next Page', 'swiftlatex-preview-toolbar-next');
        this.toDispose.push(widget_1.addEventListener(next_btn, 'click', this.handleForward.bind(this)));
        var zoomin_btn = this.createButton(toolbar, 'Zoom In', 'swiftlatex-preview-toolbar-zoomin');
        this.toDispose.push(widget_1.addEventListener(zoomin_btn, 'click', this.handleZoomin.bind(this)));
        var zoomout_btn = this.createButton(toolbar, 'Zoom Out', 'swiftlatex-preview-toolbar-zoomout');
        this.toDispose.push(widget_1.addEventListener(zoomout_btn, 'click', this.handleZoomOut.bind(this)));
        /* toolbar input */
        var input = document.createElement('input');
        input.type = 'text';
        input.id = 'swiftlatex-preview-pagecontrol';
        input.classList.add('swiftlatex-preview-toolbar-input');
        this.toDispose.push(widget_1.addEventListener(input, 'keydown', this.handleInputChanged.bind(this)));
        toolbar.appendChild(input);
        /* Viewer area, maintained by cashdom/jquery */
        var viewer = document.createElement('div');
        viewer.id = 'swiftlatex_preview_viewer';
        parent.appendChild(viewer);
    };
    LaTeXPreviewWidget.prototype.updateXDV = function (buffer) {
        this.xdvBuffer = buffer;
        this.showPage();
    };
    LaTeXPreviewWidget.prototype.handleBack = function () {
        if (this.currentPage - 1 >= 1) {
            this.currentPage -= 1;
            this.showPage();
        }
    };
    LaTeXPreviewWidget.prototype.handleZoomin = function () {
        console.log('Zoom in');
        this.user_zoom_ratio = this.user_zoom_ratio + 0.1;
        this.doZoom();
    };
    LaTeXPreviewWidget.prototype.doZoom = function () {
        var viewer = $('.page_decoration');
        viewer.css('-webkit-transform', "scale(" + this.user_zoom_ratio + ")");
        viewer.css('-ms-transform', "scale(" + this.user_zoom_ratio + ")");
        viewer.css('-transform', "scale(" + this.user_zoom_ratio + ")");
    };
    LaTeXPreviewWidget.prototype.handleZoomOut = function () {
        console.log('Zoom Out');
        this.user_zoom_ratio = this.user_zoom_ratio - 0.1;
        this.doZoom();
    };
    LaTeXPreviewWidget.prototype.handleForward = function () {
        if (this.currentPage + 1 <= this.totalPage) {
            this.currentPage += 1;
            this.showPage();
        }
    };
    LaTeXPreviewWidget.prototype.showPage = function () {
        if (this.xdvBuffer) {
            var xdvMachine = new xdv_machine_1.XDVMachine();
            var parse_ok = false;
            try {
                xdv_parser_1.parseXDV(this.xdvBuffer, xdvMachine, this.currentPage);
                parse_ok = true;
            }
            catch (err) {
                console.log(err);
                /* Handle Error */
            }
            if (!parse_ok) {
                return;
            }
            this.cachedFileList = xdvMachine.fileList;
            this.totalPage = xdvMachine.totalPages;
            var htmlContent = xdvMachine.getBody();
            var styleContent = '<style>' + xdvMachine.getStyle() + '</style>\n';
            this.unbindEvents();
            $('#swiftlatex_preview_viewer').html('');
            $('#swiftlatex_preview_viewer').append(styleContent);
            $('#swiftlatex_preview_viewer').append(htmlContent);
            this.doZoom();
            this.bindEvents();
            this.doShowPageNumber();
        }
    };
    LaTeXPreviewWidget.prototype.unbindEvents = function () {
        $('.pf-line > span').off();
    };
    LaTeXPreviewWidget.prototype.bindEvents = function () {
        var _this = this;
        $('.pf-line > span').on('click', function (event_obj) {
            _this.initFromViewer = true;
            var span_obj = $(event_obj.target);
            var beforeOrAfter = event_obj.pageX > span_obj.offset().left + span_obj.width() / 2;
            _this.doShowCursor(span_obj, beforeOrAfter);
        });
    };
    LaTeXPreviewWidget.prototype.clearCursor = function () {
        $('.viewercursor').remove();
    };
    LaTeXPreviewWidget.prototype.doShowCursor = function (span_obj, beforeOrAfter) {
        this.clearCursor();
        // const is_space = span_obj.hasClass('pf-space');
        if (this.getRow(span_obj) !== 0) {
            if (this.getEcs(span_obj) === 0) {
                var cursor_html = "<span class='viewercursor' l=" + this.getRow(span_obj) + " c=" + (this.getCol(span_obj) + 1) + " f=" + this.getFid(span_obj) + "></span>";
                if (beforeOrAfter) {
                    span_obj.after(cursor_html);
                }
                else {
                    span_obj.before(cursor_html);
                }
            }
            /* Fire event */
            if (this.initFromViewer) {
                this.onDidChangePositionEmitter.fire({
                    line: this.getRow(span_obj),
                    column: this.getCol(span_obj),
                    url: this.fidToUrl(this.getFid(span_obj)),
                    cs: this.getEcs(span_obj)
                });
            }
        }
    };
    LaTeXPreviewWidget.prototype.handleInputChanged = function (e) {
        if (!this.xdvBuffer) {
            return;
        }
        var key = browser_1.KeyCode.createKeyCode(e).key;
        if (key && browser_1.Key.ENTER.keyCode === key.keyCode) {
            var srcElement = e.srcElement;
            if (srcElement instanceof HTMLInputElement) {
                var input_val = $(srcElement).val();
                if (input_val.includes('/')) {
                    input_val = input_val.split('/')[0];
                }
                var vv = parseInt(input_val);
                if (!isNaN(vv)) {
                    if ((vv >= 1 && vv <= this.totalPage) && vv !== this.currentPage) {
                        this.currentPage = vv;
                        this.showPage();
                    }
                }
                else {
                    this.doShowPageNumber();
                }
            }
        }
    };
    LaTeXPreviewWidget.prototype.doShowPageNumber = function () {
        $('#swiftlatex-preview-pagecontrol').val(this.currentPage + "/" + this.totalPage);
    };
    LaTeXPreviewWidget.prototype.getRow = function (obj) {
        var inp = obj.attr('l');
        if (!inp) {
            return 0;
        }
        return parseInt(inp);
    };
    ;
    LaTeXPreviewWidget.prototype.getCol = function (obj) {
        var inp = obj.attr('c');
        if (!inp) {
            return 0;
        }
        return parseInt(inp);
    };
    ;
    LaTeXPreviewWidget.prototype.getEcs = function (obj) {
        var inp = obj.attr('cs');
        if (!inp) {
            return 0;
        }
        return parseInt(inp);
    };
    ;
    LaTeXPreviewWidget.prototype.getFid = function (obj) {
        var inp = obj.attr('f');
        if (!inp) {
            return 0;
        }
        return parseInt(inp);
    };
    ;
    LaTeXPreviewWidget.prototype.fidToUrl = function (fid) {
        if (fid > this.cachedFileList.length) {
            return undefined;
        }
        return this.cachedFileList[fid - 1];
    };
    LaTeXPreviewWidget.prototype.urlToFid = function (url) {
        for (var j = 0; j < this.cachedFileList.length; j++) {
            if (this.cachedFileList[j] === url) {
                return j + 1;
            }
        }
        return -1;
    };
    LaTeXPreviewWidget.prototype.isCursorShowing = function () {
        return ($('.viewercursor').length === 1);
    };
    LaTeXPreviewWidget.prototype.handleEditorCursorMoved = function (line, column, uri) {
        var _this = this;
        this.initFromViewer = false;
        if (uri.startsWith('file:///')) {
            uri = uri.substr(8);
        }
        var fid = this.urlToFid(uri);
        if (fid === -1) {
            return;
        }
        /* Compatiablilty */
        line += 1;
        if (this.isCursorShowing()) {
            /* Todo Consider Adding It */
            var cursorObj = $('.viewercursor');
            var orirow = this.getRow(cursorObj);
            var oricol = this.getCol(cursorObj);
            var orifid = this.getFid(cursorObj);
            /* Cursor is always one bit ahead */
            if (orirow === line && oricol === column + 1 && orifid === fid) {
                return;
            }
            this.clearCursor();
        }
        console.log("looking for span[l='" + line + "' c='" + column + "' f='" + fid + "']");
        var candicates = $(".pf-line > span[l='" + line + "']");
        var filteredOnes = [];
        candicates.each(function (index, element) {
            var elementJquery = $(element);
            if (_this.getCol(elementJquery) === column && _this.getEcs(elementJquery) === 0 && _this.getFid(elementJquery) === fid) {
                filteredOnes.push(elementJquery);
            }
        });
        if (filteredOnes.length > 0) {
            this.doShowCursor(filteredOnes[0], true);
            return;
        }
        var min_distance = 0xffff;
        var heuristElement = undefined;
        var beforeOrAfter = true;
        candicates.each(function (index, element) {
            var elementJquery = $(element);
            if (_this.getEcs(elementJquery) !== 0) {
                return;
            }
            if (_this.getFid(elementJquery) !== fid) {
                return;
            }
            var distance = _this.getCol(elementJquery) - column;
            if (Math.abs(distance) <= min_distance) {
                min_distance = Math.abs(distance);
                heuristElement = elementJquery;
                beforeOrAfter = !(distance > 0);
            }
        });
        if (heuristElement !== undefined && heuristElement.length > 0) {
            this.doShowCursor(heuristElement, beforeOrAfter);
        }
    };
    LaTeXPreviewWidget.prototype.handleCharacterInserted = function (nchar) {
        this.initFromViewer = false;
        if (!this.isCursorShowing()) {
            return;
        }
        var cursorObj = $('.viewercursor');
        var orirow = this.getRow(cursorObj);
        var oricol = this.getCol(cursorObj);
        var orifid = this.getFid(cursorObj);
        if (nchar === ' ') {
            nchar = '&nbsp';
        }
        cursorObj.before("<span l=" + orirow + " c=" + oricol + " f=" + orifid + " cs=0>" + nchar + "</span>");
        cursorObj.attr('c', oricol + 1);
        /* Fix the following */
        var nextobj = cursorObj;
        var stepCount = 0;
        while (stepCount < 255) {
            stepCount += 1;
            nextobj = nextobj.next();
            if (nextobj.length === 0) {
                break;
            }
            var tmpcol = this.getCol(nextobj) + 1;
            nextobj.attr('c', tmpcol);
        }
    };
    LaTeXPreviewWidget.prototype.handleCharacterDeleted = function (nchar) {
        this.initFromViewer = false;
        if (!this.isCursorShowing()) {
            return;
        }
        var cursorObj = $('.viewercursor');
        var oricol = this.getCol(cursorObj);
        if (oricol === 1) {
            return;
        }
        var prevspan = cursorObj.prev();
        if (prevspan.length === 0) {
            return;
        }
        var isRemoved = false;
        if (nchar === ' ' && prevspan.hasClass('pf-space')) {
            prevspan.remove();
            isRemoved = true;
        }
        else if (prevspan.html() === nchar) {
            prevspan.remove();
            isRemoved = true;
        }
        /* Fix the following */
        if (isRemoved) {
            var nextobj = cursorObj;
            var stepCount = 0;
            while (stepCount < 255) {
                stepCount += 1;
                nextobj = nextobj.next();
                if (nextobj.length === 0) {
                    break;
                }
                var tmpcol = this.getCol(nextobj) - 1;
                nextobj.attr('c', tmpcol);
            }
        }
    };
    return LaTeXPreviewWidget;
}(widget_1.BaseWidget));
exports.LaTeXPreviewWidget = LaTeXPreviewWidget;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/latex-preview-widget'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/monaco-yjs.js":
/*!************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/monaco-yjs.js ***!
  \************************************************************************************/
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
var Y = __webpack_require__(/*! yjs */ "../../node_modules/yjs/dist/yjs.mjs");
var y_websocket_1 = __webpack_require__(/*! y-websocket */ "../../node_modules/y-websocket/src/y-websocket.js");
/* eslint-disable @typescript-eslint/no-explicit-any */
var createMutex = function () {
    var token = true;
    return function (f, g) {
        if (token) {
            token = false;
            try {
                f();
            }
            finally {
                token = true;
            }
            ;
        }
        else if (g !== undefined) {
            g();
        }
        ;
    };
};
var MonacoYJSBinding = /** @class */ (function () {
    function MonacoYJSBinding(monacoModel) {
        this.monacoModel = monacoModel;
        this.hasInited = false; /* Prevent multiple sync events */
        this.mux = createMutex();
        this.init_yjs(monacoModel.textEditorModel);
    }
    MonacoYJSBinding.prototype.init_yjs = function (monacoModel) {
        var _this = this;
        console.log('Opening url ' + monacoModel.uri.toString());
        var ydoc = new Y.Doc();
        var unique_id = (window.location.pathname + monacoModel.uri.toString()).replace(/\//gm, '_').replace(':', '_').replace(/\./gm, '_');
        var provider = new y_websocket_1.WebsocketProvider('wss://share.swiftlatex.com', unique_id, ydoc);
        var ytext = ydoc.getText('document');
        provider.on('sync', function () {
            if (_this.hasInited) {
                return;
            }
            _this.hasInited = true;
            var remote_source = ytext.toString();
            var local_source = monacoModel.getValue();
            if (remote_source === local_source) {
                console.log('All synced');
            }
            else {
                if (remote_source.length === 0) {
                    ytext.insert(0, local_source);
                    console.log('Using local source');
                }
                else {
                    monacoModel.setValue(remote_source);
                    console.log('Trusting remote source');
                }
            }
            console.log('Start binding editor events');
            _this.monacoHander = monacoModel.onDidChangeContent(function (event) {
                _this.mux(function () {
                    ydoc.transact(function () {
                        event.changes.sort(function (change1, change2) { return change2.rangeOffset - change1.rangeOffset; }).forEach(function (change) {
                            ytext.delete(change.rangeOffset, change.rangeLength);
                            ytext.insert(change.rangeOffset, change.text);
                        });
                    }, _this);
                });
            });
            console.log('Start binding yjs events');
            ytext.observe(function (event) {
                _this.mux(function () {
                    var index = 0;
                    event.delta.forEach(function (op) {
                        if (op.retain !== undefined) {
                            index += op.retain;
                        }
                        else if (op.insert !== undefined) {
                            var pos = monacoModel.getPositionAt(index);
                            var range = new monaco.Selection(pos.lineNumber, pos.column, pos.lineNumber, pos.column);
                            /* eslint-disable */
                            monacoModel.pushEditOperations([], [{ range: range, text: op.insert }], function () { return null; });
                            index += op.insert.length;
                            /* eslint-enable */
                        }
                        else if (op.delete !== undefined) {
                            var pos = monacoModel.getPositionAt(index);
                            var endPos = monacoModel.getPositionAt(index + op.delete);
                            var range = new monaco.Selection(pos.lineNumber, pos.column, endPos.lineNumber, endPos.column);
                            /* eslint-disable */
                            monacoModel.pushEditOperations([], [{ range: range, text: '' }], function () { return null; });
                            /* eslint-enable */
                        }
                        else {
                            throw Error('Unexpected sync protocol');
                        }
                    });
                    monacoModel.pushStackElement();
                });
            });
        });
        monacoModel.onWillDispose(function () {
            /* To debounce */
            _this.hasInited = true;
            console.log('Disposing editor event handlers');
            if (_this.monacoHander) {
                _this.monacoHander.dispose();
            }
            ytext._eH.l.length = 0;
            ydoc.destroy();
            // provider.disconnect();
            provider.destroy();
        });
        provider.connect();
    };
    return MonacoYJSBinding;
}());
exports.MonacoYJSBinding = MonacoYJSBinding;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/monaco-yjs'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/theia-latex-contribution.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/theia-latex-contribution.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var core_1 = __webpack_require__(/*! @theia/core */ "../../packages/core/lib/common/index.js");
var latex_engine_1 = __webpack_require__(/*! ./latex-engine */ "../../packages/latex/lib/browser/latex-engine.js");
var xdv_exporter_1 = __webpack_require__(/*! ./xdv-exporter */ "../../packages/latex/lib/browser/xdv-exporter.js");
var s3storagesystem_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/s3storagesystem */ "../../packages/filesystem/lib/browser/s3storagesystem.js");
var pdfjs_dist_1 = __webpack_require__(/*! pdfjs-dist */ "../../node_modules/pdfjs-dist/build/pdf.js");
var monaco_workspace_1 = __webpack_require__(/*! @theia/monaco/lib/browser/monaco-workspace */ "../../packages/monaco/lib/browser/monaco-workspace.js");
var monaco_yjs_1 = __webpack_require__(/*! ./monaco-yjs */ "../../packages/latex/lib/browser/monaco-yjs.js");
var output_channel_1 = __webpack_require__(/*! @theia/output/lib/common/output-channel */ "../../packages/output/lib/common/output-channel.js");
var problem_manager_1 = __webpack_require__(/*! @theia/markers/lib/browser/problem/problem-manager */ "../../packages/markers/lib/browser/problem/problem-manager.js");
var latex_log_parser_1 = __webpack_require__(/*! ./latex-log-parser */ "../../packages/latex/lib/browser/latex-log-parser.js");
var filesystem_watcher_1 = __webpack_require__(/*! @theia/filesystem/lib/browser/filesystem-watcher */ "../../packages/filesystem/lib/browser/filesystem-watcher.js");
var uri_1 = __webpack_require__(/*! @theia/core/lib/common/uri */ "../../packages/core/lib/common/uri.js");
var browser_1 = __webpack_require__(/*! @theia/core/lib/browser */ "../../packages/core/lib/browser/index.js");
var latex_preview_widget_1 = __webpack_require__(/*! ./latex-preview-widget */ "../../packages/latex/lib/browser/latex-preview-widget.js");
var uri_command_handler_1 = __webpack_require__(/*! @theia/core/lib/common/uri-command-handler */ "../../packages/core/lib/common/uri-command-handler.js");
var selection_service_1 = __webpack_require__(/*! @theia/core/lib/common/selection-service */ "../../packages/core/lib/common/selection-service.js");
var navigator_contribution_1 = __webpack_require__(/*! @theia/navigator/lib/browser/navigator-contribution */ "../../packages/navigator/lib/browser/navigator-contribution.js");
var browser_2 = __webpack_require__(/*! @theia/editor/lib/browser */ "../../packages/editor/lib/browser/index.js");
var output_contribution_1 = __webpack_require__(/*! @theia/output/lib/browser/output-contribution */ "../../packages/output/lib/browser/output-contribution.js");
var diff_match_patch_typescript_1 = __webpack_require__(/*! diff-match-patch-typescript */ "../../node_modules/diff-match-patch-typescript/dist/es/index.js");
/* eslint-disable @typescript-eslint/no-explicit-any */
var LaTeXMenus;
(function (LaTeXMenus) {
    LaTeXMenus.LATEX = __spread(core_1.MAIN_MENU_BAR, ['7_latex']);
    LaTeXMenus.LATEX_QUICK_BUILD = __spread(LaTeXMenus.LATEX, ['1_latex']);
    LaTeXMenus.LATEX_BUILD = __spread(LaTeXMenus.LATEX, ['1_latex']);
    LaTeXMenus.LATEX_EXPORT = __spread(LaTeXMenus.LATEX, ['1_latex']);
    LaTeXMenus.LATEX_CLEAN = __spread(LaTeXMenus.LATEX, ['2_latex']);
    LaTeXMenus.ENGINE_RESTART = __spread(LaTeXMenus.LATEX, ['2_latex']);
})(LaTeXMenus = exports.LaTeXMenus || (exports.LaTeXMenus = {}));
var LaTeXCommands;
(function (LaTeXCommands) {
    var LATEX_CATEGORY = 'LaTeX';
    LaTeXCommands.BUILD_QUICK = {
        id: 'latex:quickbuild',
        category: LATEX_CATEGORY,
        label: 'Preview'
    };
    LaTeXCommands.BUILD_LINK = {
        id: 'latex:build',
        category: LATEX_CATEGORY,
        label: 'Compile'
    };
    LaTeXCommands.PROJECT_EXPORT = {
        id: 'latex:export',
        category: LATEX_CATEGORY,
        label: 'Export PDF'
    };
    LaTeXCommands.PROJECT_CLEAN = {
        id: 'latex:clean',
        category: LATEX_CATEGORY,
        label: 'Clean Project'
    };
    LaTeXCommands.ENGINE_RESTART = {
        id: 'latex:restart',
        category: LATEX_CATEGORY,
        label: 'Restart Engine'
    };
    LaTeXCommands.ENGINE_SET_ENTRY = {
        id: 'latex:setentry',
        category: LATEX_CATEGORY,
        label: 'Set Main Entry File'
    };
})(LaTeXCommands = exports.LaTeXCommands || (exports.LaTeXCommands = {}));
var LaTeXMenuContribution = /** @class */ (function () {
    function LaTeXMenuContribution() {
    }
    LaTeXMenuContribution.prototype.registerMenus = function (menus) {
        menus.registerSubmenu(LaTeXMenus.LATEX, 'LaTeX');
        menus.registerMenuAction(LaTeXMenus.LATEX_QUICK_BUILD, {
            commandId: LaTeXCommands.BUILD_QUICK.id,
            order: '0'
        });
        menus.registerMenuAction(LaTeXMenus.LATEX_BUILD, {
            commandId: LaTeXCommands.BUILD_LINK.id,
            order: '1'
        });
        menus.registerMenuAction(LaTeXMenus.LATEX_EXPORT, {
            commandId: LaTeXCommands.PROJECT_EXPORT.id,
            order: '2'
        });
        menus.registerMenuAction(LaTeXMenus.LATEX_CLEAN, {
            commandId: LaTeXCommands.PROJECT_CLEAN.id,
            order: '3'
        });
        menus.registerMenuAction(LaTeXMenus.ENGINE_RESTART, {
            commandId: LaTeXCommands.ENGINE_RESTART.id,
            order: '4'
        });
        /* Navigator  */
        menus.registerMenuAction(navigator_contribution_1.NavigatorContextMenu.CLIPBOARD, {
            commandId: LaTeXCommands.ENGINE_SET_ENTRY.id,
            order: 'z'
        });
    };
    LaTeXMenuContribution = __decorate([
        inversify_1.injectable()
    ], LaTeXMenuContribution);
    return LaTeXMenuContribution;
}());
exports.LaTeXMenuContribution = LaTeXMenuContribution;
var RETRY_INTERVAL = 2000;
var LaTeXCommandContribution = /** @class */ (function () {
    function LaTeXCommandContribution(s3filesystem, messageService, latexEngine, xdvExporter, workspace, outputChannelManager, problemManager, fileSystemWatcher, app, selectionService, openerService, editorManager, outputContribution) {
        var _this = this;
        this.s3filesystem = s3filesystem;
        this.messageService = messageService;
        this.latexEngine = latexEngine;
        this.xdvExporter = xdvExporter;
        this.workspace = workspace;
        this.outputChannelManager = outputChannelManager;
        this.problemManager = problemManager;
        this.fileSystemWatcher = fileSystemWatcher;
        this.app = app;
        this.selectionService = selectionService;
        this.openerService = openerService;
        this.editorManager = editorManager;
        this.outputContribution = outputContribution;
        this.cachedXDV = undefined;
        this.hasFileSyncedToEngine = false;
        this.toDispose = new core_1.DisposableCollection();
        this.toDisposePerEditor = new core_1.DisposableCollection();
        this.opened_yjs_handles = new Map();
        this.lastCompileTime = 0;
        this.retryHandle = -1;
        this.savedEditContext = undefined;
        this.toDispose.push(this.workspace.onDidOpenTextDocument(function (model) { return _this.fireDidOpenDocument(model); }));
        this.toDispose.push(this.workspace.onDidChangeTextDocument(function (param) { return _this.fireDidChangeContents(param); }));
        this.toDispose.push(this.workspace.onDidCloseTextDocument(function (model) { return _this.fireDidCloseDocument(model); }));
        this.listenOnDirChange();
        this.output_channel = this.outputChannelManager.getChannel('LaTeX');
        this.preview_widget = new latex_preview_widget_1.LaTeXPreviewWidget();
        this.app.shell.addWidget(this.preview_widget, { area: 'right' });
        this.bindPreviewWidgetEvents();
    }
    LaTeXCommandContribution.prototype.bindPreviewWidgetEvents = function () {
        var _this = this;
        /* Cursor Event from Preview Widget */
        this.toDispose.push(this.preview_widget.onDidChangePosition(function (e) {
            var openOptions = {
                selection: {
                    start: {
                        line: e.line - 1,
                        character: e.column - 1
                    },
                    end: {
                        line: e.line - 1,
                        character: e.column - 1
                    }
                }
            };
            browser_1.open(_this.openerService, new uri_1.default(e.url), openOptions).catch(function (err) {
                /* Todo Handle Error */
            });
        }));
        /* Cursor Event from Editor */
        this.toDispose.push(this.editorManager.onCurrentEditorChanged(function (editorWidget) {
            _this.toDisposePerEditor.dispose();
            if (editorWidget) {
                var editor_1 = editorWidget.editor;
                if (editor_1) {
                    _this.toDisposePerEditor.push(editor_1.onCursorPositionChanged(function () {
                        var cursor = editor_1.cursor;
                        var currentURI = editor_1.document.uri;
                        _this.preview_widget.handleEditorCursorMoved(cursor.line, cursor.character, currentURI);
                        // console.log(`${cursor.line}-${cursor.character}-${currentURI}`);
                    }));
                }
            }
        }));
    };
    LaTeXCommandContribution.prototype.getCurrentContext = function () {
        var currentEditorWidget = this.editorManager.currentEditor;
        if (currentEditorWidget) {
            var currentEditor = currentEditorWidget.editor;
            if (currentEditor) {
                var cursor = currentEditor.cursor;
                var currentURI = currentEditor.document.uri;
                var text = currentEditor.document.getLineContent(cursor.line);
                return {
                    line: cursor.line,
                    column: cursor.character,
                    uri: currentURI,
                    text: text
                };
            }
        }
        console.log('Fail to get context?');
        return undefined;
    };
    LaTeXCommandContribution.prototype.generateContextPatch = function (saved, current) {
        if (saved.line === current.line) {
            var dmp = new diff_match_patch_typescript_1.DiffMatchPatch();
            var diff = dmp.diff_main(saved.text, current.text);
            var delcount = 0;
            var inscount = 0;
            var inscontent = '';
            var delcontent = '';
            for (var tindex in diff) {
                if (diff[tindex][0] === 1) {
                    inscount += 1;
                    inscontent += diff[tindex][1];
                }
                else if (diff[tindex][0] === -1) {
                    delcount += 1;
                    delcontent += diff[tindex][1];
                }
            }
            if (inscount === 1 && delcount === 0) {
                return '+' + inscontent;
            }
            else if (inscount === 0 && delcount === 1) {
                return '-' + delcontent.split('').reverse().join('');
            }
        }
        return '';
    };
    LaTeXCommandContribution.prototype.applyContextPatch = function (patch) {
        if (patch.startsWith('+')) {
            for (var tmpk = 1; tmpk < patch.length; tmpk++) {
                this.preview_widget.handleCharacterInserted(patch[tmpk]);
            }
        }
        else if (patch.startsWith('-')) {
            for (var tmpk = 1; tmpk < patch.length; tmpk++) {
                this.preview_widget.handleCharacterDeleted(patch[tmpk]);
            }
        }
    };
    LaTeXCommandContribution.prototype.registerCommands = function (registry) {
        var _this = this;
        registry.registerCommand(LaTeXCommands.BUILD_QUICK, {
            execute: function () { return _this.quickBuild(true); },
            isEnabled: function () { return _this.latexEngine.isReady(); },
        });
        registry.registerCommand(LaTeXCommands.BUILD_LINK, {
            execute: function () { return _this.fullBuild(); },
            isEnabled: function () { return _this.latexEngine.isReady(); },
        });
        registry.registerCommand(LaTeXCommands.PROJECT_EXPORT, {
            execute: function () { return _this.exportPDF(); },
            isEnabled: function () { return _this.latexEngine.isReady() && _this.cachedXDV !== undefined; }
        });
        registry.registerCommand(LaTeXCommands.PROJECT_CLEAN, {
            execute: function () { return _this.cleanProject(); },
            isEnabled: function () { return _this.latexEngine.isReady(); },
        });
        registry.registerCommand(LaTeXCommands.ENGINE_RESTART, {
            execute: function () { return _this.restartEngine(); }
        });
        registry.registerCommand(LaTeXCommands.ENGINE_SET_ENTRY, new uri_command_handler_1.UriAwareCommandHandler(this.selectionService, {
            execute: function (uris) { return _this.setMainFile(uris); },
            isEnabled: function (uris) { return _this.canSetAsMainFile(uris); },
        }, { multi: false }));
    };
    LaTeXCommandContribution.prototype.quickBuild = function (shouldShowPreviewWidget) {
        if (shouldShowPreviewWidget === void 0) { shouldShowPreviewWidget = false; }
        return __awaiter(this, void 0, void 0, function () {
            var mainTempFiles, compileResult, currentContext, patch, activate, reveal;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!this.latexEngine.isReady()) {
                            return [2 /*return*/, false];
                        }
                        return [4 /*yield*/, this.resolveMainFile()];
                    case 1:
                        mainTempFiles = _a.sent();
                        if (mainTempFiles.length === 0) {
                            this.messageService.error('Cannot locate the main LaTeX entry file. Please create a main entry file in root directory before compilation.', { timeout: 15000 });
                            return [2 /*return*/, false];
                        }
                        if (mainTempFiles.length > 1) {
                            this.messageService.error("Compiler cannot tell which file is the main entry file, either " + mainTempFiles[0] + " or " + mainTempFiles[1] + ".", { timeout: 15000 });
                            return [2 /*return*/, false];
                        }
                        if (!!this.hasFileSyncedToEngine) return [3 /*break*/, 3];
                        this.hasFileSyncedToEngine = true;
                        return [4 /*yield*/, this._syncFileToEngine('/')];
                    case 2:
                        _a.sent();
                        _a.label = 3;
                    case 3:
                        // this.messageService.info('Building preview', { timeout: 3000 });
                        this.latexEngine.setEngineMainFile(mainTempFiles[0]);
                        /* Todo */
                        this.savedEditContext = this.getCurrentContext();
                        return [4 /*yield*/, this.latexEngine.compileLaTeX()];
                    case 4:
                        compileResult = _a.sent();
                        /* Write log to output and parse log */
                        this.processCompileLog(compileResult.log);
                        if (!(compileResult.status === 1 || compileResult.status === 0)) return [3 /*break*/, 5];
                        /* Successful compilation */
                        this.cachedXDV = compileResult.pdf;
                        this.preview_widget.updateXDV(this.cachedXDV);
                        if (this.savedEditContext) {
                            currentContext = this.getCurrentContext();
                            patch = this.generateContextPatch(this.savedEditContext, currentContext);
                            console.log(patch);
                            this.preview_widget.handleEditorCursorMoved(this.savedEditContext.line, this.savedEditContext.column, this.savedEditContext.uri);
                            this.applyContextPatch(patch);
                        }
                        if (shouldShowPreviewWidget) {
                            this.app.shell.revealWidget(this.preview_widget.id);
                        }
                        return [2 /*return*/, true];
                    case 5:
                        activate = false;
                        reveal = true;
                        return [4 /*yield*/, this.outputContribution.openView({ activate: activate, reveal: reveal })];
                    case 6:
                        _a.sent();
                        // console.log('error detected ' + compileResult.status);
                        return [2 /*return*/, false];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.processCompileLog = function (log) {
        return __awaiter(this, void 0, void 0, function () {
            var parseRes, diags, allStuff, j, error, uri, diag, temps;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        this.output_channel.clear();
                        this.output_channel.append(log);
                        parseRes = latex_log_parser_1.LatexParserModule.parse(log);
                        diags = new Map();
                        allStuff = parseRes.errors.concat(parseRes.warnings);
                        j = 0;
                        _a.label = 1;
                    case 1:
                        if (!(j < allStuff.length)) return [3 /*break*/, 5];
                        error = allStuff[j];
                        uri = error.file;
                        diag = {
                            range: {
                                start: {
                                    line: error.line - 1,
                                    character: 0
                                },
                                end: {
                                    line: error.line - 1,
                                    character: 0
                                }
                            },
                            message: error.message,
                            severity: error.level === 'error' ? 1 : 2
                        };
                        /* Log file has the format ./xxx/xxx.tex, convert to them to standard */
                        uri = uri.substr(1);
                        if (!!uri.endsWith('.tex')) return [3 /*break*/, 3];
                        return [4 /*yield*/, this.resolveMainFile()];
                    case 2:
                        temps = _a.sent();
                        uri = '/' + temps[0];
                        _a.label = 3;
                    case 3:
                        if (!diags.has(uri)) {
                            diags.set(uri, []);
                        }
                        diags.get(uri).push(diag);
                        _a.label = 4;
                    case 4:
                        j++;
                        return [3 /*break*/, 1];
                    case 5:
                        diags.forEach(function (diag, key) {
                            _this.problemManager.setMarkers(new uri_1.default(key), 'Engine', diag);
                        });
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype._syncFileToEngine = function (url) {
        return __awaiter(this, void 0, void 0, function () {
            var objs, j, obj, content;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.s3filesystem.readdir(url)];
                    case 1:
                        objs = _a.sent();
                        j = 0;
                        _a.label = 2;
                    case 2:
                        if (!(j < objs.length)) return [3 /*break*/, 9];
                        obj = objs[j];
                        if (!obj.isDir) return [3 /*break*/, 4];
                        if (obj.uri.includes('.swiftlatex') || obj.uri.includes('.theia')) {
                            return [3 /*break*/, 8];
                        }
                        this.latexEngine.makeMemFSFolder(obj.uri);
                        return [4 /*yield*/, this._syncFileToEngine(obj.uri)];
                    case 3:
                        _a.sent();
                        return [3 /*break*/, 8];
                    case 4: return [4 /*yield*/, this.s3filesystem.readFile(obj.uri)];
                    case 5:
                        content = _a.sent();
                        if (!(obj.uri.endsWith('.pdf') || obj.uri.endsWith('.jpg')
                            || obj.uri.endsWith('.png') || obj.uri.endsWith('.bmp'))) return [3 /*break*/, 7];
                        return [4 /*yield*/, this.measureImageDim(content, obj.uri)];
                    case 6:
                        content = _a.sent();
                        _a.label = 7;
                    case 7:
                        this.latexEngine.writeMemFSFile(content, obj.uri);
                        console.log('Writing file to engine ' + obj.uri);
                        _a.label = 8;
                    case 8:
                        j++;
                        return [3 /*break*/, 2];
                    case 9: return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.measureImageDim = function (content, name) {
        return __awaiter(this, void 0, void 0, function () {
            var tempPDFURL, pdfDoc, page, viewport, tmpWidth, tmpHeight, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        if (!name.endsWith('pdf')) return [3 /*break*/, 7];
                        tempPDFURL = URL.createObjectURL(new Blob([content], { type: 'application/octet-stream' }));
                        _b.label = 1;
                    case 1:
                        _b.trys.push([1, 4, 5, 6]);
                        return [4 /*yield*/, pdfjs_dist_1.getDocument(tempPDFURL).promise];
                    case 2:
                        pdfDoc = _b.sent();
                        return [4 /*yield*/, pdfDoc.getPage(1)];
                    case 3:
                        page = _b.sent();
                        viewport = page.getViewport({ scale: 1 });
                        tmpWidth = viewport.width;
                        tmpHeight = viewport.height;
                        pdfDoc.destroy();
                        return [2 /*return*/, "swiftlatex\n" + tmpWidth + "\n" + tmpHeight + "\n"];
                    case 4:
                        _a = _b.sent();
                        console.error("Unable to measure the pdf document " + name + ", return a dummy dimension ");
                        return [2 /*return*/, 'swiftlatex\n300\n400\n'];
                    case 5:
                        URL.revokeObjectURL(tempPDFURL);
                        return [7 /*endfinally*/];
                    case 6: return [3 /*break*/, 8];
                    case 7: return [2 /*return*/, new Promise(function (resolve, reject) {
                            var tempImageURL = URL.createObjectURL(new Blob([content], { type: 'application/octet-stream' }));
                            var tmpImgElement = new Image();
                            tmpImgElement.onload = (function (_) {
                                URL.revokeObjectURL(tempImageURL);
                                resolve("swiftlatex\n" + tmpImgElement.width + "\n" + tmpImgElement.height + "\n");
                            });
                            tmpImgElement.onerror = (function (_) {
                                console.error("Unable to measure the image document " + name + ", return a dummy dimension ");
                                URL.revokeObjectURL(tempImageURL);
                                resolve('swiftlatex\n300\n400\n');
                            });
                            tmpImgElement.src = tempImageURL;
                        })];
                    case 8: return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.fullBuild = function () {
        return __awaiter(this, void 0, void 0, function () {
            var try_time, j, res;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        /* Force a sync */
                        this.hasFileSyncedToEngine = false;
                        this.messageService.info('Building...', { timeout: 3000 });
                        try_time = 3;
                        j = 0;
                        _a.label = 1;
                    case 1:
                        if (!(j < try_time)) return [3 /*break*/, 4];
                        return [4 /*yield*/, this.quickBuild(true)];
                    case 2:
                        res = _a.sent();
                        if (!res) {
                            return [3 /*break*/, 4];
                        }
                        _a.label = 3;
                    case 3:
                        j++;
                        return [3 /*break*/, 1];
                    case 4: return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.cleanProject = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                this.messageService.info('Cleaning build cache...', { timeout: 3000 });
                if (!this.latexEngine.isReady()) {
                    return [2 /*return*/];
                }
                this.hasFileSyncedToEngine = false;
                this.latexEngine.flushCache();
                return [2 /*return*/];
            });
        });
    };
    LaTeXCommandContribution.prototype.exportPDF = function () {
        return __awaiter(this, void 0, void 0, function () {
            var res, tempPDFURL, anchor;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!this.cachedXDV) {
                            return [2 /*return*/];
                        }
                        this.messageService.info('Exporting PDF...', { timeout: 3000 });
                        return [4 /*yield*/, this.xdvExporter.loadExporter()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, this._syncFileToExporter('/')];
                    case 2:
                        _a.sent();
                        this.xdvExporter.writeMainEntryFile(this.cachedXDV);
                        return [4 /*yield*/, this.xdvExporter.exportPDF()];
                    case 3:
                        res = _a.sent();
                        this.xdvExporter.closeWorker();
                        this.output_channel.clear();
                        this.output_channel.append(res.log);
                        if (res.status === 1 || res.status === 0) {
                            tempPDFURL = URL.createObjectURL(new Blob([res.pdf], { type: 'application/octet-stream' }));
                            anchor = document.createElement('a');
                            try {
                                anchor.href = tempPDFURL;
                                anchor.style.display = 'none';
                                anchor.download = 'export.pdf';
                                document.body.appendChild(anchor);
                                anchor.click();
                            }
                            finally {
                                // make sure anchor is removed from parent
                                if (anchor && anchor.parentNode) {
                                    anchor.parentNode.removeChild(anchor);
                                }
                                URL.revokeObjectURL(tempPDFURL);
                                anchor = undefined;
                            }
                        }
                        else {
                            /* Failed */
                            this.messageService.error('Unexpected error detected when generating PDF.', { timeout: 3000 });
                        }
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.restartEngine = function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        this.messageService.info('Restarting Engine...', { timeout: 3000 });
                        this.latexEngine.closeWorker();
                        this.xdvExporter.closeWorker();
                        this.hasFileSyncedToEngine = false;
                        return [4 /*yield*/, this.latexEngine.loadEngine()];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.fireDidChangeContents = function (param) {
        if (this.shouldIgnore(param.textDocument)) {
            return;
        }
        var save_uri = param.textDocument.uri.substr(7);
        if (this.latexEngine.isReady()) {
            this.latexEngine.writeMemFSFile(param.textDocument.getText(), save_uri);
            /* Please fire up a new compilation */
        }
        /* To call previewer */
        var changeEvent = param.contentChanges;
        if (changeEvent.length === 1) {
            var affected = changeEvent[0].rangeLength;
            if (affected === 1) {
                /* Delete */
                var internal_model = param.textDocument.textEditorModel;
                var internal_stack = internal_model._commandManager;
                var last_operation_reverse = internal_stack.currentOpenStackElement;
                if (last_operation_reverse && last_operation_reverse.editOperations && last_operation_reverse.editOperations.length === 1) {
                    var rev_ops = last_operation_reverse.editOperations[0];
                    if (rev_ops && rev_ops.operations && rev_ops.operations.length === 1) {
                        var rev_op = rev_ops.operations[0];
                        if (rev_op && rev_op.text) {
                            this.preview_widget.handleCharacterDeleted(rev_op.text);
                        }
                    }
                }
            }
            else if (affected === 0) {
                /* Insert */
                if (changeEvent[0].text.length === 1) {
                    this.preview_widget.handleCharacterInserted(changeEvent[0].text);
                }
            }
            // console.dir(changeEvent[0]);
        }
        if (save_uri.endsWith('.tex')) {
            this.tryBuild();
        }
    };
    LaTeXCommandContribution.prototype.tryBuild = function () {
        var _this = this;
        if (this.retryHandle > 0) {
            clearTimeout(this.retryHandle);
            this.retryHandle = -1;
        }
        if (this.latexEngine.isReady() && Date.now() - this.lastCompileTime > RETRY_INTERVAL) {
            this.lastCompileTime = Date.now();
            this.quickBuild().catch(function (e) {
            });
        }
        else {
            this.retryHandle = window.setTimeout(function () {
                _this.tryBuild();
            }, RETRY_INTERVAL);
        }
    };
    LaTeXCommandContribution.prototype.fireDidOpenDocument = function (model) {
        if (this.shouldIgnore(model)) {
            return;
        }
        /* Kick off YJS */
        if (this.opened_yjs_handles.has(model.uri)) {
            return;
        }
        new monaco_yjs_1.MonacoYJSBinding(model);
    };
    LaTeXCommandContribution.prototype.fireDidCloseDocument = function (model) {
        if (this.shouldIgnore(model)) {
            return;
        }
        if (this.opened_yjs_handles.has(model.uri)) {
            this.opened_yjs_handles.delete(model.uri);
        }
    };
    LaTeXCommandContribution.prototype.shouldIgnore = function (model) {
        if (!model || !model.valid) {
            return true;
        }
        if (!model.uri.startsWith('file:///')) {
            return true;
        }
        if (model.uri.startsWith('file:///.theia') ||
            model.uri.startsWith('file:///.vscode') ||
            model.uri.startsWith('file:///.swiftlatex')) {
            return true;
        }
        return false;
    };
    LaTeXCommandContribution.prototype.resolveMainFile = function () {
        return __awaiter(this, void 0, void 0, function () {
            var saved_mainfile, objs, possibleCandicates, k;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        saved_mainfile = window.localStorage.getItem(window.location.pathname + 'main_config');
                        if (saved_mainfile) {
                            return [2 /*return*/, [saved_mainfile]];
                        }
                        return [4 /*yield*/, this.s3filesystem.readdir('/')];
                    case 1:
                        objs = _a.sent();
                        possibleCandicates = [];
                        /* See whether there is only one .tex file */
                        for (k = 0; k < objs.length; k++) {
                            if (!objs[k].isDir && objs[k].uri.endsWith('.tex')) {
                                possibleCandicates.push(objs[k].uri.substr(1));
                            }
                        }
                        /* cache it so we dont have to do it again */
                        if (possibleCandicates.length === 1) {
                            window.localStorage.setItem(window.location.pathname + 'main_config', possibleCandicates[0]);
                        }
                        return [2 /*return*/, possibleCandicates];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.setMainFile = function (uri) {
        if (!uri) {
            return;
        }
        var uri_str = uri.toString().substr(8);
        window.localStorage.setItem(window.location.pathname + 'main_config', uri_str);
        this.messageService.info("Set " + uri_str + " as the main entry.", { timeout: 3000 });
    };
    LaTeXCommandContribution.prototype.canSetAsMainFile = function (uri) {
        if (!uri) {
            return false;
        }
        var uri_string = uri.toString();
        if (!uri_string.endsWith('.tex')) {
            return false;
        }
        if (!uri_string.startsWith('file:///')) {
            return false;
        }
        return (uri_string.match(/\//g) || []).length === 3;
    };
    LaTeXCommandContribution.prototype._syncFileToExporter = function (url) {
        return __awaiter(this, void 0, void 0, function () {
            var objs, j, obj, content;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, this.s3filesystem.readdir(url)];
                    case 1:
                        objs = _a.sent();
                        j = 0;
                        _a.label = 2;
                    case 2:
                        if (!(j < objs.length)) return [3 /*break*/, 7];
                        obj = objs[j];
                        if (!obj.isDir) return [3 /*break*/, 4];
                        if (obj.uri.includes('.swiftlatex') || obj.uri.includes('.theia')) {
                            return [3 /*break*/, 6];
                        }
                        this.xdvExporter.makeMemFSFolder(obj.uri);
                        return [4 /*yield*/, this._syncFileToExporter(obj.uri)];
                    case 3:
                        _a.sent();
                        return [3 /*break*/, 6];
                    case 4: return [4 /*yield*/, this.s3filesystem.readFile(obj.uri)];
                    case 5:
                        content = _a.sent();
                        this.xdvExporter.writeMemFSFile(content, obj.uri);
                        console.log('Writing file to exporter ' + obj.uri);
                        _a.label = 6;
                    case 6:
                        j++;
                        return [3 /*break*/, 2];
                    case 7: return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.listenOnDirChange = function () {
        return __awaiter(this, void 0, void 0, function () {
            var fileUri, watcher, onFileChange;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        fileUri = new uri_1.default('file:///');
                        return [4 /*yield*/, this.fileSystemWatcher.watchFileChanges(fileUri)];
                    case 1:
                        watcher = _a.sent();
                        this.toDispose.push(watcher);
                        onFileChange = function (events) {
                            for (var k = 0; k < events.length; k++) {
                                /* Todo Import a more clear symbol */
                                /* We mean delete or add file */
                                if (events[k].type === 1 || events[k].type === 2) {
                                    console.log('Need to resync file');
                                    _this.hasFileSyncedToEngine = false;
                                    break;
                                }
                            }
                        };
                        this.toDispose.push(this.fileSystemWatcher.onFilesChanged(onFileChange));
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXCommandContribution.prototype.dispose = function () {
        this.toDispose.dispose();
    };
    LaTeXCommandContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(s3storagesystem_1.S3StorageSystem)),
        __param(1, inversify_1.inject(common_1.MessageService)),
        __param(2, inversify_1.inject(latex_engine_1.LaTeXEngine)),
        __param(3, inversify_1.inject(xdv_exporter_1.XDVExporter)),
        __param(4, inversify_1.inject(monaco_workspace_1.MonacoWorkspace)),
        __param(5, inversify_1.inject(output_channel_1.OutputChannelManager)),
        __param(6, inversify_1.inject(problem_manager_1.ProblemManager)),
        __param(7, inversify_1.inject(filesystem_watcher_1.FileSystemWatcher)),
        __param(8, inversify_1.inject(browser_1.FrontendApplication)),
        __param(9, inversify_1.inject(selection_service_1.SelectionService)),
        __param(10, inversify_1.inject(browser_1.OpenerService)),
        __param(11, inversify_1.inject(browser_2.EditorManager)),
        __param(12, inversify_1.inject(output_contribution_1.OutputContribution)),
        __metadata("design:paramtypes", [s3storagesystem_1.S3StorageSystem,
            common_1.MessageService,
            latex_engine_1.LaTeXEngine,
            xdv_exporter_1.XDVExporter,
            monaco_workspace_1.MonacoWorkspace,
            output_channel_1.OutputChannelManager,
            problem_manager_1.ProblemManager,
            filesystem_watcher_1.FileSystemWatcher,
            browser_1.FrontendApplication,
            selection_service_1.SelectionService, Object, browser_2.EditorManager,
            output_contribution_1.OutputContribution])
    ], LaTeXCommandContribution);
    return LaTeXCommandContribution;
}());
exports.LaTeXCommandContribution = LaTeXCommandContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/theia-latex-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/theia-latex-frontend-module.js":
/*!*****************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/theia-latex-frontend-module.js ***!
  \*****************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
var theia_latex_spellchecker_1 = __webpack_require__(/*! ./theia-latex-spellchecker */ "../../packages/latex/lib/browser/theia-latex-spellchecker.js");
var browser_1 = __webpack_require__(/*! @theia/languages/lib/browser */ "../../packages/languages/lib/browser/index.js");
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var theia_latex_contribution_1 = __webpack_require__(/*! ./theia-latex-contribution */ "../../packages/latex/lib/browser/theia-latex-contribution.js");
var common_1 = __webpack_require__(/*! @theia/core/lib/common */ "../../packages/core/lib/common/index.js");
var latex_engine_1 = __webpack_require__(/*! ./latex-engine */ "../../packages/latex/lib/browser/latex-engine.js");
var xdv_exporter_1 = __webpack_require__(/*! ./xdv-exporter */ "../../packages/latex/lib/browser/xdv-exporter.js");
var textmate_contribution_1 = __webpack_require__(/*! @theia/monaco/lib/browser/textmate/textmate-contribution */ "../../packages/monaco/lib/browser/textmate/textmate-contribution.js");
var latex_grammar_1 = __webpack_require__(/*! ./latex-grammar */ "../../packages/latex/lib/browser/latex-grammar.js");
var bibtex_grammar_1 = __webpack_require__(/*! ./bibtex-grammar */ "../../packages/latex/lib/browser/bibtex-grammar.js");
exports.default = new inversify_1.ContainerModule(function (bind) {
    // add your contribution bindings here
    bind(latex_engine_1.LaTeXEngine).toSelf().inSingletonScope();
    bind(xdv_exporter_1.XDVExporter).toSelf().inSingletonScope();
    bind(theia_latex_spellchecker_1.LaTeXClientContribution).toSelf().inSingletonScope();
    bind(browser_1.LanguageClientContribution).toDynamicValue(function (ctx) { return ctx.container.get(theia_latex_spellchecker_1.LaTeXClientContribution); });
    bind(theia_latex_contribution_1.LaTeXCommandContribution).toSelf().inSingletonScope();
    bind(common_1.CommandContribution).toService(theia_latex_contribution_1.LaTeXCommandContribution);
    bind(theia_latex_contribution_1.LaTeXMenuContribution).toSelf().inSingletonScope();
    bind(common_1.MenuContribution).toService(theia_latex_contribution_1.LaTeXMenuContribution);
    bind(latex_grammar_1.TeXContribution).toSelf().inSingletonScope();
    bind(textmate_contribution_1.LanguageGrammarDefinitionContribution).toService(latex_grammar_1.TeXContribution);
    bind(bibtex_grammar_1.BibTeXContribution).toSelf().inSingletonScope();
    bind(textmate_contribution_1.LanguageGrammarDefinitionContribution).toService(bibtex_grammar_1.BibTeXContribution);
});

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/theia-latex-frontend-module'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/theia-latex-spellchecker.js":
/*!**************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/theia-latex-spellchecker.js ***!
  \**************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var browser_1 = __webpack_require__(/*! @theia/languages/lib/browser */ "../../packages/languages/lib/browser/index.js");
var common_1 = __webpack_require__(/*! ../common */ "../../packages/latex/lib/common/index.js");
var LaTeXClientContribution = /** @class */ (function (_super) {
    __extends(LaTeXClientContribution, _super);
    function LaTeXClientContribution(workspace, languages, languageClientFactory) {
        var _this = _super.call(this, workspace, languages, languageClientFactory) || this;
        _this.workspace = workspace;
        _this.languages = languages;
        _this.languageClientFactory = languageClientFactory;
        _this.id = common_1.LATEX_LANGUAGE_ID;
        _this.name = common_1.LATEX_LANGUAGE_NAME;
        return _this;
    }
    Object.defineProperty(LaTeXClientContribution.prototype, "globPatterns", {
        get: function () {
            return [
                '**/*.tex',
                '**/*.bib'
            ];
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(LaTeXClientContribution.prototype, "documentSelector", {
        get: function () {
            return [this.id, 'bibtex'];
        },
        enumerable: true,
        configurable: true
    });
    LaTeXClientContribution = __decorate([
        inversify_1.injectable(),
        __param(0, inversify_1.inject(browser_1.Workspace)),
        __param(1, inversify_1.inject(browser_1.Languages)),
        __param(2, inversify_1.inject(browser_1.LanguageClientFactory)),
        __metadata("design:paramtypes", [Object, Object, browser_1.LanguageClientFactory])
    ], LaTeXClientContribution);
    return LaTeXClientContribution;
}(browser_1.BaseLanguageClientContribution));
exports.LaTeXClientContribution = LaTeXClientContribution;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/theia-latex-spellchecker'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/xdv-exporter.js":
/*!**************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/xdv-exporter.js ***!
  \**************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var inversify_1 = __webpack_require__(/*! inversify */ "../../node_modules/inversify/lib/inversify.js");
var ExporterStatus;
(function (ExporterStatus) {
    ExporterStatus[ExporterStatus["Init"] = 1] = "Init";
    ExporterStatus[ExporterStatus["Ready"] = 2] = "Ready";
    ExporterStatus[ExporterStatus["Busy"] = 3] = "Busy";
    ExporterStatus[ExporterStatus["Error"] = 4] = "Error";
    ExporterStatus[ExporterStatus["Finish"] = 5] = "Finish";
})(ExporterStatus = exports.ExporterStatus || (exports.ExporterStatus = {}));
var ENGINE_PATH = '/bin/dvipdfmx.js';
var ConvertResult = /** @class */ (function () {
    function ConvertResult() {
        this.pdf = undefined;
        this.status = -254;
        this.log = 'No log';
    }
    return ConvertResult;
}());
exports.ConvertResult = ConvertResult;
var XDVExporter = /** @class */ (function () {
    function XDVExporter() {
        this.xdvWorker = undefined;
        this.xdvWorkerStatus = ExporterStatus.Init;
    }
    XDVExporter.prototype.loadExporter = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.xdvWorker !== undefined) {
                            throw new Error('Other instance is running, abort()');
                        }
                        this.xdvWorkerStatus = ExporterStatus.Init;
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.xdvWorker = new Worker(ENGINE_PATH);
                                _this.xdvWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    if (cmd === 'postRun') {
                                        _this.xdvWorkerStatus = ExporterStatus.Ready;
                                        resolve();
                                    }
                                    else {
                                        _this.xdvWorkerStatus = ExporterStatus.Error;
                                        reject();
                                    }
                                };
                            })];
                    case 1:
                        _a.sent();
                        this.xdvWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/];
                }
            });
        });
    };
    XDVExporter.prototype.checkExporterStatus = function () {
        if (this.xdvWorkerStatus !== ExporterStatus.Ready) {
            throw Error('Engine is still spinning.');
        }
    };
    XDVExporter.prototype.exportPDF = function () {
        return __awaiter(this, void 0, void 0, function () {
            var start_compile_time, res;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        this.checkExporterStatus();
                        this.xdvWorkerStatus = ExporterStatus.Busy;
                        start_compile_time = performance.now();
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.xdvWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    var result = data['result'];
                                    var log = data['log'];
                                    var status = data['status'];
                                    var pdf = data['pdf'];
                                    if (cmd === 'export') {
                                        if (result === 'failed') {
                                            console.error('Exporter crushed terribly. Log: ' + log);
                                            _this.xdvWorkerStatus = ExporterStatus.Error;
                                            reject(log);
                                        }
                                        else {
                                            console.log('Exporter compilation finish ' + (performance.now() - start_compile_time));
                                            var nice_report = new ConvertResult();
                                            nice_report.status = status;
                                            nice_report.log = log;
                                            nice_report.pdf = pdf;
                                            _this.xdvWorkerStatus = ExporterStatus.Finish;
                                            resolve(nice_report);
                                        }
                                    }
                                };
                                _this.xdvWorker.postMessage({ 'cmd': 'export' });
                            })];
                    case 1:
                        res = _a.sent();
                        this.xdvWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/, res];
                }
            });
        });
    };
    XDVExporter.prototype.writeMemFSFile = function (srccode, filename) {
        this.checkExporterStatus();
        if (this.xdvWorker !== undefined) {
            this.xdvWorker.postMessage({ 'cmd': 'writefile', 'url': filename, 'src': srccode });
        }
    };
    XDVExporter.prototype.writeMainEntryFile = function (srccode) {
        this.writeMemFSFile(srccode, 'InP0t_SwIfTLoTEx_.xdv');
    };
    XDVExporter.prototype.makeMemFSFolder = function (folder) {
        this.checkExporterStatus();
        if (this.xdvWorker !== undefined) {
            if (folder === '' || folder === '/') {
                return;
            }
            this.xdvWorker.postMessage({ 'cmd': 'mkdir', 'url': folder });
        }
    };
    XDVExporter.prototype.closeWorker = function () {
        if (this.xdvWorker !== undefined) {
            this.xdvWorker.postMessage({ 'cmd': 'grace' });
            this.xdvWorker = undefined;
        }
    };
    XDVExporter = __decorate([
        inversify_1.injectable(),
        __metadata("design:paramtypes", [])
    ], XDVExporter);
    return XDVExporter;
}());
exports.XDVExporter = XDVExporter;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/xdv-exporter'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/xdv-machine.js":
/*!*************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/xdv-machine.js ***!
  \*************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
Object.defineProperty(exports, "__esModule", { value: true });
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var CharPosition = /** @class */ (function () {
    function CharPosition() {
        this.line = 0;
        this.column = 0;
        this.group = 0;
        this.cs = 0;
        this.fid = 0;
    }
    return CharPosition;
}());
exports.CharPosition = CharPosition;
var Position = /** @class */ (function () {
    function Position(properties) {
        if (properties) {
            this.h = properties.h;
            this.v = properties.v;
            this.w = properties.w;
            this.x = properties.x;
            this.y = properties.y;
            this.z = properties.z;
        }
        else {
            this.h = 0;
            this.v = 0;
            this.w = 0;
            this.x = 0;
            this.y = 0;
            this.z = 0;
        }
    }
    return Position;
}());
var DviFont = /** @class */ (function () {
    function DviFont() {
    }
    return DviFont;
}());
exports.DviFont = DviFont;
var XDVMachine = /** @class */ (function () {
    function XDVMachine() {
        this.body = '';
        this.style = '';
        this.pointsPerDviUnit = 0;
        this.svgDepth = 0;
        this.color = 'black';
        this.colorStack = [];
        this.paperwidth = 600;
        this.paperheight = 800;
        this.currentpage = 1;
        this.fonts = new Map();
        this.usedfonts = new Map();
        this.stack = [];
        this.h_offset = 4736287;
        this.v_offset = 4736287;
        this.inTextMode = false;
        this.totalPages = 0;
        this.lastSpacePos = undefined;
        this.fileList = [];
    }
    XDVMachine.prototype.getBody = function () {
        return this.body;
    };
    XDVMachine.prototype.getStyle = function () {
        return this.style;
    };
    XDVMachine.prototype.pushColor = function (c) {
        this.endTextMode();
        this.colorStack.push(this.color);
        this.color = c;
    };
    XDVMachine.prototype.popColor = function () {
        this.endTextMode();
        if (this.colorStack.length >= 1) {
            this.color = this.colorStack.pop();
        }
        else {
            throw Error('Color stack is empty');
        }
    };
    XDVMachine.prototype.setPapersize = function (width, height) {
        this.paperwidth = width;
        this.paperheight = height;
        this.style += "#page" + this.currentpage + " { position:relative; width:" + this.paperwidth + "px; height:" + this.paperheight + "px; border-width: thin; }\n";
    };
    XDVMachine.prototype.putSVG = function (svg) {
        this.endTextMode();
        var left = (this.position.h + this.h_offset) * this.pointsPerDviUnit;
        var top = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
        this.svgDepth += (svg.match(/<svg>/g) || []).length;
        this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;
        svg = svg.replace('<svg>', '<svg width="10px" height="10px" viewBox="-5 -5 10 10" style="overflow: visible; position: absolute;">');
        svg = svg.replace(/{\?x}/g, left.toString());
        svg = svg.replace(/{\?y}/g, top.toString());
        this.body += svg;
    };
    XDVMachine.prototype.push = function () {
        this.endTextMode();
        this.stack.push(new Position(this.position));
    };
    XDVMachine.prototype.pop = function () {
        this.endTextMode();
        if (this.stack.length >= 1) {
            this.position = this.stack.pop();
        }
        else {
            throw Error('Position stack is empty');
        }
    };
    XDVMachine.prototype.beginPage = function (page, h_offset, v_offset) {
        this.stack = [];
        this.position = new Position();
        this.currentpage = page + 1;
        this.body += "<div id='page" + this.currentpage + "' class='page_decoration'>";
    };
    XDVMachine.prototype.endPage = function () {
        this.endTextMode();
        this.body += '</div>';
    };
    XDVMachine.prototype.post = function (_p) {
    };
    XDVMachine.prototype.postPost = function (_p) {
    };
    XDVMachine.prototype.endTextMode = function () {
        if (this.inTextMode) {
            this.body += '</div>';
            this.inTextMode = false;
        }
    };
    XDVMachine.prototype.moveRight = function (distance) {
        this.position.h += distance;
        if (this.inTextMode) {
            var l = 0;
            var c = 0;
            var f = 0;
            var cs = 0;
            if (this.lastSpacePos) {
                l = this.lastSpacePos.line;
                c = this.lastSpacePos.column;
                f = this.lastSpacePos.fid;
                cs = this.lastSpacePos.cs;
                this.lastSpacePos = undefined;
            }
            var inpixel = Number(distance * this.pointsPerDviUnit).toFixed(2);
            this.body += "<span l='" + l + "' c='" + c + "' f='" + f + "' cs='" + cs + "' class='pf-space' style='width: " + inpixel + "px;'> </span>";
        }
    };
    XDVMachine.prototype.moveDown = function (distance) {
        this.position.v += distance;
    };
    XDVMachine.prototype.setFont = function (fontnum) {
        this.endTextMode();
        if (this.fonts.has(fontnum)) {
            this.font = this.fonts.get(fontnum);
            if (!(this.usedfonts.has(this.font.name))) {
                this.usedfonts.set(this.font.name, 1);
                if (this.font.isnative) {
                    if (this.font.name.endsWith('.ttf') || this.font.name.endsWith('.otf')) {
                        // Local font
                    }
                    else {
                        // Remote font
                        this.style += "@font-face { font-family:" + this.font.name + "; src:url(https://texlive.swiftlatex.com/" + this.font.name + ".otf); } \n\n";
                    }
                }
                else {
                    // Texfont
                    this.style += "@font-face { font-family:" + this.font.name + "; src:url(https://texlive.swiftlatex.com/fonts/" + this.font.name + ".woff); }\n";
                }
            }
        }
        else {
            throw Error("Could not find font " + fontnum + ".");
        }
    };
    XDVMachine.prototype.preamble = function (numerator, denominator, magnification, comment) {
        var dviUnit = magnification * numerator / 1000.0 / denominator;
        this.pointsPerDviUnit = dviUnit * 72.27 / 100000.0 / 2.54;
    };
    XDVMachine.prototype.putRule = function (rulea, ruleb, moved) {
        this.endTextMode();
        var a = rulea * this.pointsPerDviUnit;
        var b = ruleb * this.pointsPerDviUnit;
        var left = (this.position.h + this.h_offset) * this.pointsPerDviUnit;
        var bottom = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
        var top = bottom - a;
        this.body += "<span style=\"background: " + this.color + "; position: absolute; top: " + top + "px; left: " + left + "px; width:" + b + "px; height: " + a + "px;\"></span>\n";
        if (moved) {
            this.position.h += ruleb;
        }
    };
    XDVMachine.prototype._to_legal_unicode = function (c) {
        if ((c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c === 0xAD)) {
            return c + 0xE000;
        }
        return c;
    };
    XDVMachine.prototype.setChar = function (c, text_height, text_width, pos) {
        this.endTextMode();
        c = this._to_legal_unicode(c);
        var htmlText = String.fromCharCode(c);
        // console.log(htmlText + ' ' + text_width);
        var cssleft = (this.position.h + this.h_offset) * this.pointsPerDviUnit;
        var cssheight = text_height * this.pointsPerDviUnit;
        var csstop = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
        // console.log(this.font.scaleFactor);
        var fontsize = this.font.designSize / 65536.0;
        if (this.svgDepth === 0) {
            this.body += "<div style=\"line-height: 0; color: " + this.color + "; \n            font-family: " + this.font.name + "; font-size: " + fontsize + "px; \n            position: absolute; top: " + Number(csstop - cssheight).toFixed(2) + "px; vertical-align: baseline;\n            left: " + Number(cssleft).toFixed(2) + "px;\">" + htmlText + "<span style=\"display: inline-block;\n             vertical-align: " + Number(cssheight).toFixed(2) + "px; height: 0px; line-height: 0;\"></span></div>";
        }
        else {
            var bottom = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
            this.body += "<text alignment-baseline=\"baseline\"\n            y=\"" + bottom + "\" x=\"" + cssleft + "\" style=\"font-family: " + this.font.name + ";\"\n            font-size=\"" + fontsize + "\">" + htmlText + "</text>\n";
        }
        this.position.h += text_width;
    };
    XDVMachine.prototype.setSpace = function (pos) {
        if (pos.line !== 0) {
            this.lastSpacePos = pos;
        }
        // console.log(pos.line + ' ' + pos.column);
    };
    XDVMachine.prototype.setNativeText = function (text, textpos, width) {
        var cssleft = (this.position.h + this.h_offset) * this.pointsPerDviUnit;
        var csstop = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
        var fontsize = this.font.designSize * this.font.extend;
        var textheight = fontsize; /* Todo, not sure whether it is correct */
        if (!this.inTextMode) {
            this.inTextMode = true;
            this.body += "<div class=\"pf-line\"\n            style=\"color: " + this.color + "; font-family: " + this.font.name + ";\n            font-size: " + Number(fontsize).toFixed(2) + "px;\n            top: " + Number(csstop - textheight).toFixed(2) + "px;\n            left: " + Number(cssleft).toFixed(2) + "px;\">";
        }
        for (var j = 0; j < text.length; j++) {
            var pos = textpos[j];
            this.body += "<span l='" + pos.line + "'\n            c='" + pos.column + "' f='" + pos.fid + "' cs='" + pos.cs + "'>" + String.fromCharCode(text[j]) + "</span>";
        }
        this.position.h += width;
    };
    XDVMachine.prototype.putImage = function (width, height, url) {
        this.endTextMode();
        var cssleft = (this.position.h + this.h_offset) * this.pointsPerDviUnit;
        var csstop = (this.position.v + this.v_offset) * this.pointsPerDviUnit;
        this.body += "<div data-url=\"" + url + "\"\n        style=\"top: " + Number(csstop - height).toFixed(2) + "px;\n        left: " + Number(cssleft).toFixed(2) + "px;\n        position: absolute; height:" + Number(height).toFixed(2) + "px;\n        width:" + Number(width).toFixed(2) + "px;\n        background-color:grey;\"></div>";
    };
    XDVMachine.prototype.loadFont = function (properties, fontnumber, isnative) {
        var f = new DviFont();
        if (!isnative) {
            f.name = properties.name;
            f.checksum = properties.checksum;
            f.scaleFactor = properties.scaleFactor;
            f.designSize = properties.designSize;
            f.isnative = false;
        }
        else {
            f.name = properties.name;
            f.designSize = properties.fontsize;
            f.faceindex = properties.faceindex;
            f.height = properties.height;
            f.depth = properties.depth;
            f.rbga = properties.rgba;
            if (!f.rbga) {
                f.rbga = 0xffffffff;
            }
            f.extend = properties.extend;
            if (!f.extend) {
                f.extend = 1.0;
            }
            f.slant = properties.slant;
            if (!f.slant) {
                f.slant = 0;
            }
            f.embolden = properties.embolden;
            if (!f.embolden) {
                f.embolden = 0;
            }
            f.isnative = true;
        }
        this.fonts.set(fontnumber, f);
    };
    return XDVMachine;
}());
exports.XDVMachine = XDVMachine;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/xdv-machine'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/browser/xdv-parser.js":
/*!************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/browser/xdv-parser.js ***!
  \************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(Buffer, global) {
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
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
/* eslint-disable @typescript-eslint/no-explicit-any */
var xdv_machine_1 = __webpack_require__(/*! ./xdv-machine */ "../../packages/latex/lib/browser/xdv-machine.js");
var FILE_VERSION = 71;
var Opcode;
(function (Opcode) {
    Opcode[Opcode["set_char"] = 0] = "set_char";
    Opcode[Opcode["set1"] = 128] = "set1";
    Opcode[Opcode["set2"] = 129] = "set2";
    Opcode[Opcode["set3"] = 130] = "set3";
    Opcode[Opcode["set4"] = 131] = "set4";
    Opcode[Opcode["set_rule"] = 132] = "set_rule";
    Opcode[Opcode["put_char"] = 133] = "put_char";
    Opcode[Opcode["put2"] = 134] = "put2";
    Opcode[Opcode["put3"] = 135] = "put3";
    Opcode[Opcode["put4"] = 136] = "put4";
    Opcode[Opcode["put_rule"] = 137] = "put_rule";
    Opcode[Opcode["nop"] = 138] = "nop";
    Opcode[Opcode["bop"] = 139] = "bop";
    Opcode[Opcode["eop"] = 140] = "eop";
    Opcode[Opcode["push"] = 141] = "push";
    Opcode[Opcode["pop"] = 142] = "pop";
    Opcode[Opcode["right"] = 143] = "right";
    Opcode[Opcode["right2"] = 144] = "right2";
    Opcode[Opcode["right3"] = 145] = "right3";
    Opcode[Opcode["right4"] = 146] = "right4";
    Opcode[Opcode["w"] = 147] = "w";
    Opcode[Opcode["w1"] = 148] = "w1";
    Opcode[Opcode["w2"] = 149] = "w2";
    Opcode[Opcode["w3"] = 150] = "w3";
    Opcode[Opcode["w4"] = 151] = "w4";
    Opcode[Opcode["x"] = 152] = "x";
    Opcode[Opcode["x1"] = 153] = "x1";
    Opcode[Opcode["x2"] = 154] = "x2";
    Opcode[Opcode["x3"] = 155] = "x3";
    Opcode[Opcode["x4"] = 156] = "x4";
    Opcode[Opcode["down"] = 157] = "down";
    Opcode[Opcode["down2"] = 158] = "down2";
    Opcode[Opcode["down3"] = 159] = "down3";
    Opcode[Opcode["down4"] = 160] = "down4";
    Opcode[Opcode["y"] = 161] = "y";
    Opcode[Opcode["y1"] = 162] = "y1";
    Opcode[Opcode["y2"] = 163] = "y2";
    Opcode[Opcode["y3"] = 164] = "y3";
    Opcode[Opcode["y4"] = 165] = "y4";
    Opcode[Opcode["z"] = 166] = "z";
    Opcode[Opcode["z1"] = 167] = "z1";
    Opcode[Opcode["z2"] = 168] = "z2";
    Opcode[Opcode["z3"] = 169] = "z3";
    Opcode[Opcode["z4"] = 170] = "z4";
    Opcode[Opcode["fnt"] = 171] = "fnt";
    Opcode[Opcode["fnt1"] = 235] = "fnt1";
    Opcode[Opcode["fnt2"] = 236] = "fnt2";
    Opcode[Opcode["fnt3"] = 237] = "fnt3";
    Opcode[Opcode["fnt4"] = 238] = "fnt4";
    Opcode[Opcode["xxx"] = 239] = "xxx";
    Opcode[Opcode["xxx2"] = 240] = "xxx2";
    Opcode[Opcode["xxx3"] = 241] = "xxx3";
    Opcode[Opcode["xxx4"] = 242] = "xxx4";
    Opcode[Opcode["fnt_def"] = 243] = "fnt_def";
    Opcode[Opcode["fnt_def2"] = 244] = "fnt_def2";
    Opcode[Opcode["fnt_def3"] = 245] = "fnt_def3";
    Opcode[Opcode["fnt_def4"] = 246] = "fnt_def4";
    Opcode[Opcode["pre"] = 247] = "pre";
    Opcode[Opcode["post"] = 248] = "post";
    Opcode[Opcode["post_post"] = 249] = "post_post";
    Opcode[Opcode["define_native_font"] = 252] = "define_native_font";
    Opcode[Opcode["set_glyphs"] = 253] = "set_glyphs";
    Opcode[Opcode["set_text_and_glyphs"] = 254] = "set_text_and_glyphs";
})(Opcode || (Opcode = {}));
var DviCommand = /** @class */ (function () {
    function DviCommand(properties) {
        this.special = false;
        Object.assign(this, properties);
    }
    DviCommand.prototype.execute = function (machine) {
    };
    DviCommand.prototype.toString = function () {
        return 'DviCommand { }';
    };
    return DviCommand;
}());
exports.DviCommand = DviCommand;
var SetChar = /** @class */ (function (_super) {
    __extends(SetChar, _super);
    function SetChar(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_char;
        return _this;
    }
    SetChar.prototype.execute = function (machine) {
        machine.setChar(this.c, this.text_height, this.text_width, this.char_positon);
    };
    SetChar.prototype.toString = function () {
        return "SetChar { c: '" + String.fromCharCode(this.c) + "' }";
    };
    return SetChar;
}(DviCommand));
exports.SetChar = SetChar;
var SetSpace = /** @class */ (function (_super) {
    __extends(SetSpace, _super);
    function SetSpace(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set2;
        return _this;
    }
    SetSpace.prototype.execute = function (machine) {
        machine.setSpace(this.char_positon);
    };
    SetSpace.prototype.toString = function () {
        return 'SetSpace { _ }';
    };
    return SetSpace;
}(DviCommand));
exports.SetSpace = SetSpace;
var PutRule = /** @class */ (function (_super) {
    __extends(PutRule, _super);
    function PutRule(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.put_rule;
        return _this;
    }
    PutRule.prototype.execute = function (machine) {
        machine.putRule(this.a, this.b, false);
    };
    PutRule.prototype.toString = function () {
        return "PutRule { a: " + this.a + ", b: " + this.b + " }";
    };
    return PutRule;
}(DviCommand));
exports.PutRule = PutRule;
var SetRule = /** @class */ (function (_super) {
    __extends(SetRule, _super);
    function SetRule(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_rule;
        return _this;
    }
    SetRule.prototype.execute = function (machine) {
        machine.putRule(this.a, this.b, true);
    };
    SetRule.prototype.toString = function () {
        return "SetRule { a: " + this.a + ", b: " + this.b + " }";
    };
    return SetRule;
}(DviCommand));
exports.SetRule = SetRule;
// 138  nop     no operation
var Nop = /** @class */ (function (_super) {
    __extends(Nop, _super);
    function Nop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.nop;
        return _this;
    }
    Nop.prototype.toString = function () {
        return 'Nop { }';
    };
    return Nop;
}(DviCommand));
// 139  bop c_0[4]..c_9[4], p[4]    beginning of page
var Bop = /** @class */ (function (_super) {
    __extends(Bop, _super);
    function Bop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.bop;
        return _this;
    }
    Bop.prototype.execute = function (machine) {
        machine.beginPage(this.c_0, this.h_offset, this.v_offset);
    };
    Bop.prototype.toString = function () {
        return 'Bop { ... }';
    };
    return Bop;
}(DviCommand));
// 140  eop     ending of page
var Eop = /** @class */ (function (_super) {
    __extends(Eop, _super);
    function Eop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.eop;
        return _this;
    }
    Eop.prototype.execute = function (machine) {
        if (machine.stack.length) {
            throw new Error('Stack should be empty at the end of a page.');
        }
        machine.endPage();
    };
    Eop.prototype.toString = function () {
        return 'Eop { }';
    };
    return Eop;
}(DviCommand));
// 141  push        save the current positions
var Push = /** @class */ (function (_super) {
    __extends(Push, _super);
    function Push(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.push;
        return _this;
    }
    Push.prototype.execute = function (machine) {
        machine.push();
    };
    Push.prototype.toString = function () {
        return 'Push { }';
    };
    return Push;
}(DviCommand));
// 142  pop     restore previous positions
var Pop = /** @class */ (function (_super) {
    __extends(Pop, _super);
    function Pop(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.pop;
        return _this;
    }
    Pop.prototype.execute = function (machine) {
        machine.pop();
    };
    Pop.prototype.toString = function () {
        return 'Pop { }';
    };
    return Pop;
}(DviCommand));
var MoveRight = /** @class */ (function (_super) {
    __extends(MoveRight, _super);
    function MoveRight(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.right;
        return _this;
    }
    MoveRight.prototype.execute = function (machine) {
        machine.moveRight(this.b);
    };
    MoveRight.prototype.toString = function () {
        return "MoveRight { b: " + this.b + " }";
    };
    return MoveRight;
}(DviCommand));
// 147  w0      move right by w
// 148  w1  b[1]    move right and set w
// 149  w2  b[2]
// 150  w3  b[3]
// 151  w4  b[4]
var MoveW = /** @class */ (function (_super) {
    __extends(MoveW, _super);
    function MoveW(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.w;
        return _this;
    }
    MoveW.prototype.execute = function (machine) {
        if (this.length > 1) {
            machine.position.w = this.b;
        }
        machine.moveRight(machine.position.w);
    };
    MoveW.prototype.toString = function () {
        if (this.length > 1) {
            return "MoveW { b: " + this.b + " }";
        }
        return 'MoveW0 { }';
    };
    return MoveW;
}(DviCommand));
// 152  x0      move right by x
// 153  x1  b[1]    move right and set x
// 154  x2  b[2]
// 155  x3  b[3]
// 156  x4  b[4]
var MoveX = /** @class */ (function (_super) {
    __extends(MoveX, _super);
    function MoveX(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.x;
        return _this;
    }
    MoveX.prototype.execute = function (machine) {
        if (this.length > 1) {
            machine.position.x = this.b;
        }
        machine.moveRight(machine.position.x);
    };
    MoveX.prototype.toString = function () {
        if (this.length > 1) {
            return "MoveX { b: " + this.b + " }";
        }
        return 'MoveX0 { }';
    };
    return MoveX;
}(DviCommand));
// 157  down1   a[1]    move down
// 158  down2   a[2]
// 159  down3   a[3]
// 160  down4   a[4]
var MoveDown = /** @class */ (function (_super) {
    __extends(MoveDown, _super);
    function MoveDown(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.down;
        return _this;
    }
    MoveDown.prototype.execute = function (machine) {
        machine.moveDown(this.a);
    };
    MoveDown.prototype.toString = function () {
        return "MoveDown { a: " + this.a + " }";
    };
    return MoveDown;
}(DviCommand));
// 161  y0      move down by y
// 162  y1  a[1]    move down and set y
// 163  y2  a[2]
// 164  y3  a[3]
// 165  y4  a[4]
var MoveY = /** @class */ (function (_super) {
    __extends(MoveY, _super);
    function MoveY(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.y;
        return _this;
    }
    MoveY.prototype.execute = function (machine) {
        if (this.length > 1) {
            machine.position.y = this.a;
        }
        machine.moveDown(machine.position.y);
    };
    MoveY.prototype.toString = function () {
        if (this.length > 1) {
            return "MoveY { a: " + this.a + " }";
        }
        return 'MoveY0 { }';
    };
    return MoveY;
}(DviCommand));
// 166  z0      move down by z
// 167  z1  a[1]    move down and set z
// 168  z2  a[2]
// 169  z3  a[3]
// 170  z4  a[4]
var MoveZ = /** @class */ (function (_super) {
    __extends(MoveZ, _super);
    function MoveZ(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.z;
        return _this;
    }
    MoveZ.prototype.execute = function (machine) {
        if (this.length > 1) {
            machine.position.z = this.a;
        }
        machine.moveDown(machine.position.z);
    };
    MoveZ.prototype.toString = function () {
        if (this.length > 1) {
            return "MoveZ { a: " + this.a + " }";
        }
        return 'MoveZ0 { }';
    };
    return MoveZ;
}(DviCommand));
// 171...234    fnt_num_i       set current font to i
// 235  fnt1    k[1]    set current font
// 236  fnt2    k[2]
// 237  fnt3    k[3]
// 238  fnt4    k[4]
var SetFont = /** @class */ (function (_super) {
    __extends(SetFont, _super);
    function SetFont(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.fnt;
        return _this;
    }
    SetFont.prototype.execute = function (machine) {
        // console.log("Setting mainfont to " + this.k);
        machine.setFont(this.k);
    };
    SetFont.prototype.toString = function () {
        return "SetFont { k: " + this.k + " }";
    };
    return SetFont;
}(DviCommand));
// 239  xxx1    k[1], x[k]  extension to DVI primitives
// 240  xxx2    k[2], x[k]
// 241  xxx3    k[3], x[k]
// 242  xxx4    k[4], x[k]
function _intToHex(n) {
    return ("00" + Math.round(n).toString(16)).substr(-2);
}
function _texColor(name) {
    if (name === 'gray 0') {
        return 'black';
    }
    if (name === 'gray 1') {
        return 'white';
    }
    if (name.startsWith('rgb ')) {
        var out = name.split(' ').slice(1).map(function (x) { return _intToHex(parseFloat(x) * 255); }).join('');
        return "#" + out;
    }
    else if (name.startsWith('gray ')) {
        var x = name.split(' ')[1];
        return _texColor("rgb " + x + " " + x + " " + x);
    }
    else if (name.startsWith('cmyk ')) {
        var channels = name.split(' ');
        if (channels.length === 5) {
            var channel_c = parseFloat(channels[1]);
            var channel_m = parseFloat(channels[2]);
            var channel_y = parseFloat(channels[3]);
            var channel_k = parseFloat(channels[4]);
            var new_r = (1 - channel_c) * (1 - channel_k);
            var new_g = (1 - channel_m) * (1 - channel_k);
            var new_b = (1 - channel_y) * (1 - channel_k);
            return _texColor("rgb " + new_r + " " + new_g + " " + new_b);
        }
    }
    return 'black';
}
var Special = /** @class */ (function (_super) {
    __extends(Special, _super);
    function Special(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.xxx;
        _this.special = true;
        return _this;
    }
    Special.prototype.toString = function () {
        return "Special { x: '" + this.x + "' }";
    };
    Special.prototype.execute = function (machine) {
        if (this.x.startsWith('dvisvgm:raw ')) {
            var svg = this.x.replace(/^dvisvgm:raw /, '');
            machine.putSVG(svg);
        }
        else if (this.x.startsWith('pdf:pagesize')) {
            var papersize = this.x.replace(/^pdf:pagesize /, '');
            var regex = /width ([0-9\.]+)pt height ([0-9\.]+)pt/gm;
            var m = regex.exec(papersize);
            if (!m || m.length !== 3) {
                /* Fallback */
                machine.setPapersize(595, 842);
            }
            else {
                var paperWidth = Number(m[1]);
                var paperHeight = Number(m[2]);
                machine.setPapersize(paperWidth, paperHeight);
            }
        }
        else if (this.x.startsWith('color push ')) {
            var color = _texColor(this.x.replace(/^color push /, ''));
            machine.pushColor(color);
        }
        else if (this.x.startsWith('color pop')) {
            machine.popColor();
        }
        else if (this.x.startsWith('pdf:image bbox 0 0 ')) {
            var image = this.x.replace(/pdf:image bbox 0 0 /, '').split(' ');
            var imageWidth = Number(image[0]);
            var imageHeight = Number(image[1]);
            var url = image[6].substr(1).slice(0, -1);
            machine.putImage(imageWidth, imageHeight, url);
        }
        else {
            console.error('unhandled ' + this.x);
        }
    };
    return Special;
}(DviCommand));
var SetTextGlyph = /** @class */ (function (_super) {
    __extends(SetTextGlyph, _super);
    function SetTextGlyph(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.set_text_and_glyphs;
        return _this;
    }
    SetTextGlyph.prototype.execute = function (machine) {
        machine.setNativeText(this.text, this.textpos, this.width);
    };
    SetTextGlyph.prototype.toString = function () {
        return "SetGlyph { count: " + this.textcount + " }";
    };
    return SetTextGlyph;
}(DviCommand));
var FontDefinition = /** @class */ (function (_super) {
    __extends(FontDefinition, _super);
    function FontDefinition(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.fnt_def;
        return _this;
    }
    FontDefinition.prototype.execute = function (machine) {
        // console.log("Defining Local Font name: " + this.n + " index: " + this.k);
        machine.loadFont({
            name: this.n,
            checksum: this.c,
            scaleFactor: this.s,
            designSize: this.d,
        }, this.k, false);
    };
    FontDefinition.prototype.toString = function () {
        return "FontDefinition { k: " + this.k + ", n: '" + this.n + "', ... }";
    };
    return FontDefinition;
}(DviCommand));
// 247  pre i[1], num[4], den[4], mag[4],  k[1], x[k]   preamble
var Preamble = /** @class */ (function (_super) {
    __extends(Preamble, _super);
    function Preamble(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.pre;
        return _this;
    }
    Preamble.prototype.execute = function (machine) {
        if (this.num <= 0) {
            throw new Error('Invalid numerator (must be > 0)');
        }
        if (this.den <= 0) {
            throw new Error('Invalid denominator (must be > 0)');
        }
        if (this.i !== FILE_VERSION) {
            throw new Error('DVI format must be 71, which is SwiftLaTeX format ');
        }
        machine.preamble(this.num, this.den, this.mag, this.x);
    };
    Preamble.prototype.toString = function () {
        return "Preamble { i: " + this.i + ", num: " + this.num + ", den: " + this.den + ", mag: " + this.mag + ", x: '" + this.x + "' }";
    };
    return Preamble;
}(DviCommand));
// 248  post    p[4], num[4], den[4], mag[4], l[4], u[4], s[2], t[2]
// < font definitions > postamble beginning
var Post = /** @class */ (function (_super) {
    __extends(Post, _super);
    function Post(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.post;
        return _this;
    }
    Post.prototype.execute = function (machine) {
        machine.post(this);
    };
    Post.prototype.toString = function () {
        return "Post { p: " + this.p + ", num: " + this.num + ", den: " + this.den + ", mag: " + this.mag + ", ... }";
    };
    return Post;
}(DviCommand));
// 249  post_post   q[4], i[1]; 223's   postamble ending
var PostPost = /** @class */ (function (_super) {
    __extends(PostPost, _super);
    function PostPost(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.post_post;
        return _this;
    }
    PostPost.prototype.execute = function (machine) {
        machine.postPost(this);
    };
    PostPost.prototype.toString = function () {
        return "PostPost { q: " + this.q + ", i: " + this.i + " }";
    };
    return PostPost;
}(DviCommand));
// 252
var NativeFontDefinition = /** @class */ (function (_super) {
    __extends(NativeFontDefinition, _super);
    function NativeFontDefinition(properties) {
        var _this = _super.call(this, properties) || this;
        _this.opcode = Opcode.define_native_font;
        return _this;
    }
    NativeFontDefinition.prototype.execute = function (machine) {
        // console.log("Defining Native Font name: " + this.filename + " index: " + this.fontnumber);
        machine.loadFont({
            name: this.filename,
            fontsize: this.fontsize,
            faceindex: this.faceindex,
            height: this.height,
            depth: this.depth,
            rbga: this.rbga,
            extend: this.extend,
            slant: this.slant,
            embolden: this.embolden,
        }, this.fontnumber, true);
    };
    NativeFontDefinition.prototype.toString = function () {
        return "NativeFontDefinition { filename: " + this.filename + ", fontnumber: " + this.fontnumber + ", length: " + this.length + ", rbga: " + this.rbga + "}";
    };
    return NativeFontDefinition;
}(DviCommand));
function parseCommand(opcode, buffer) {
    var undefined_codes = [250, 251, 255];
    if (opcode in undefined_codes) {
        throw new Error("Undefined opcode " + opcode);
    }
    var unsupported_codes = [Opcode.set3, Opcode.set4, Opcode.put_char, Opcode.put2, Opcode.put3, Opcode.put4,
        Opcode.set_glyphs];
    if (opcode in undefined_codes) {
        throw new Error('SwiftLaTeX does not support opcode ' + opcode);
    }
    if ((opcode >= Opcode.set_char) && (opcode < Opcode.set1)) {
        throw new Error('SwiftLaTeX does not generate simple setchar');
    }
    if (opcode in unsupported_codes) {
        throw new Error('SwiftLaTeX does not support code:' + opcode);
    }
    if ((opcode >= Opcode.fnt) && (opcode < Opcode.fnt1)) {
        return new SetFont({ k: opcode - 171, length: 1 });
    }
    switch (opcode) {
        case Opcode.set1:
            if (buffer.length < 17) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var c = buffer.readUInt8(0);
            var char_p = new xdv_machine_1.CharPosition();
            char_p.line = buffer.readUInt16BE(1);
            char_p.column = buffer.readUInt16BE(3);
            char_p.cs = buffer.readUInt16BE(5);
            char_p.group = buffer.readUInt8(7);
            char_p.fid = buffer.readUInt8(8);
            var text_height = buffer.readUInt32BE(9);
            var text_width = buffer.readUInt32BE(13);
            return new SetChar({
                c: c,
                text_height: text_height,
                text_width: text_width,
                char_positon: char_p,
                length: 17 + 1
            });
        case Opcode.set2:
            if (buffer.length < 10) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var char_sp = new xdv_machine_1.CharPosition();
            char_sp.line = buffer.readUInt16BE(2);
            char_sp.column = buffer.readUInt16BE(4);
            char_sp.cs = buffer.readUInt16BE(6);
            char_sp.group = buffer.readUInt8(8);
            char_sp.fid = buffer.readUInt8(9);
            return new SetSpace({
                char_positon: char_sp,
                length: 10 + 1
            });
        case Opcode.set_rule:
            if (buffer.length < 8) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new SetRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9,
            });
        case Opcode.put_rule:
            if (buffer.length < 8) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new PutRule({
                a: buffer.readInt32BE(0),
                b: buffer.readInt32BE(4),
                length: 9,
            });
        case Opcode.nop:
            return new Nop({ length: 1 });
        case Opcode.bop:
            if (buffer.length < 44) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new Bop({
                c_0: buffer.readUInt32BE(0),
                h_offset: buffer.readUInt32BE(4),
                v_offset: buffer.readUInt32BE(8),
                p: buffer.readUInt32BE(40),
                length: 45,
            });
        case Opcode.eop:
            return new Eop({ length: 1 });
        case Opcode.push:
            return new Push({ length: 1 });
        case Opcode.pop:
            return new Pop({ length: 1 });
        case Opcode.right:
        case Opcode.right2:
        case Opcode.right3:
        case Opcode.right4:
            if (buffer.length < opcode - Opcode.right + 1) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveRight({
                b: buffer.readIntBE(0, opcode - Opcode.right + 1),
                length: opcode - Opcode.right + 1 + 1,
            });
        case Opcode.w:
            return new MoveW({ b: 0, length: 1 });
        case Opcode.w1:
        case Opcode.w2:
        case Opcode.w3:
        case Opcode.w4:
            if (buffer.length < opcode - Opcode.w) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveW({
                b: buffer.readIntBE(0, opcode - Opcode.w),
                length: opcode - Opcode.w + 1,
            });
        case Opcode.x:
            return new MoveX({ b: 0, length: 1 });
        case Opcode.x1:
        case Opcode.x2:
        case Opcode.x3:
        case Opcode.x4:
            if (buffer.length < opcode - Opcode.x) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveX({
                b: buffer.readIntBE(0, opcode - Opcode.x),
                length: opcode - Opcode.x + 1,
            });
        case Opcode.down:
        case Opcode.down2:
        case Opcode.down3:
        case Opcode.down4:
            if (buffer.length < opcode - Opcode.down + 1) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveDown({
                a: buffer.readIntBE(0, opcode - Opcode.down + 1),
                length: opcode - Opcode.down + 1 + 1,
            });
        case Opcode.y:
            return new MoveY({ a: 0, length: 1 });
        case Opcode.y1:
        case Opcode.y2:
        case Opcode.y3:
        case Opcode.y4:
            if (buffer.length < opcode - Opcode.y) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveY({
                a: buffer.readIntBE(0, opcode - Opcode.y),
                length: opcode - Opcode.y + 1,
            });
        case Opcode.z:
            return new MoveZ({ a: 0, length: 1 });
        case Opcode.z1:
        case Opcode.z2:
        case Opcode.z3:
        case Opcode.z4:
            if (buffer.length < opcode - Opcode.z) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new MoveZ({
                a: buffer.readIntBE(0, opcode - Opcode.z),
                length: opcode - Opcode.z + 1,
            });
        case Opcode.fnt1:
        case Opcode.fnt2:
        case Opcode.fnt3:
        case Opcode.fnt4:
            if (buffer.length < opcode - Opcode.fnt1 + 1) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new SetFont({
                k: buffer.readIntBE(0, opcode - Opcode.fnt1 + 1),
                length: opcode - Opcode.fnt1 + 1 + 1,
            });
        case Opcode.xxx:
        case Opcode.xxx2:
        case Opcode.xxx3:
        case Opcode.xxx4: {
            var i = opcode - Opcode.xxx + 1;
            if (buffer.length < i) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var k = buffer.readUIntBE(0, i);
            if (buffer.length < i + k) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new Special({
                x: buffer.slice(i, i + k).toString(),
                length: i + k + 1,
            });
        }
        case Opcode.fnt_def:
        case Opcode.fnt_def2:
        case Opcode.fnt_def3:
        case Opcode.fnt_def4: {
            var i = opcode - Opcode.fnt_def + 1;
            if (buffer.length < i) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var k = buffer.readIntBE(0, i);
            if (buffer.length < i + 14) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var cc = buffer.readUInt32BE(i + 0);
            var s = buffer.readUInt32BE(i + 4);
            var d = buffer.readUInt32BE(i + 8);
            var a = buffer.readUInt8(i + 12);
            var l = buffer.readUInt8(i + 13);
            if (buffer.length < i + 14 + a + l) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var n = buffer.slice(i + 14, i + 14 + a + l).toString();
            return new FontDefinition({
                k: k,
                cc: cc,
                s: s,
                d: d,
                a: a,
                l: l,
                n: n,
                length: i + 14 + a + l + 1,
            });
        }
        case Opcode.pre: {
            if (buffer.length < 14) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var i = buffer.readUInt8(0);
            var num = buffer.readUInt32BE(1);
            var den = buffer.readUInt32BE(5);
            var mag = buffer.readUInt32BE(9);
            var k = buffer.readUInt8(13);
            if (buffer.length < 14 + k + 8) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var comment = buffer.slice(14, 14 + k).toString();
            return new Preamble({
                i: i,
                num: num,
                den: den,
                mag: mag,
                x: comment,
                length: 14 + k + 1,
            });
        }
        case Opcode.post:
            if (buffer.length < 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new Post({
                p: buffer.readUInt32BE(0),
                num: buffer.readUInt32BE(4),
                den: buffer.readUInt32BE(8),
                mag: buffer.readUInt32BE(12),
                l: buffer.readUInt32BE(16),
                u: buffer.readUInt32BE(20),
                s: buffer.readUInt16BE(24),
                t: buffer.readUInt16BE(26),
                length: 29,
            });
        case Opcode.post_post:
            if (buffer.length < 5) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            return new PostPost({
                q: buffer.readUInt32BE(0),
                i: buffer.readUInt8(4),
                length: 6,
            });
        case Opcode.define_native_font: {
            if (buffer.length < 19) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var fontnum = buffer.readUInt32BE(0);
            var fontsize = buffer.readUInt32BE(4) / 65536;
            var flag = buffer.readUInt16BE(8);
            var filenamelen = buffer.readUInt8(10);
            var filename = buffer.slice(11, 11 + filenamelen).toString();
            var faceindex = buffer.readUInt32BE(11 + filenamelen);
            var height = buffer.readUInt32BE(11 + filenamelen + 4);
            var depth = buffer.readUInt32BE(11 + filenamelen + 8);
            var res = new NativeFontDefinition({
                fontnumber: fontnum,
                fontsize: fontsize,
                flag: flag,
                height: height,
                depth: depth,
                filenamelen: filenamelen,
                filename: filename,
                faceindex: faceindex,
            });
            var XDV_FLAGS_COLORED = 0x0200;
            var XDV_FLAG_EXTEND = 0x1000;
            var XDV_FLAG_SLANT = 0x2000;
            var XDV_FLAG_EMBOLDEN = 0x4000;
            var length_1 = 23 + filenamelen;
            if ((flag & XDV_FLAGS_COLORED) !== 0) {
                res.rbga = buffer.readUInt32BE(length_1);
                length_1 += 4;
            }
            if ((flag & XDV_FLAG_EXTEND) !== 0) {
                res.extend = buffer.readUInt32BE(length_1) / 65536.0;
                length_1 += 4;
            }
            if ((flag & XDV_FLAG_SLANT) !== 0) {
                res.slant = buffer.readUInt32BE(length_1) / 65536.0;
                length_1 += 4;
            }
            if ((flag & XDV_FLAG_EMBOLDEN) !== 0) {
                res.embolden = buffer.readUInt32BE(length_1) / 65536.0;
                length_1 += 4;
            }
            res.length = length_1 + 1;
            return res;
        }
        case Opcode.set_text_and_glyphs: {
            if (buffer.length < 28) {
                throw new Error("not enough bytes to process opcode " + opcode);
            }
            var textcount = buffer.readUInt16BE(0);
            if (buffer.length < 2 + textcount * 10) {
                throw new Error("not enough bytes to process opcode " + opcode + " for textcount");
            }
            var text = [];
            var char_ps = [];
            for (var j = 0; j < textcount; j++) {
                var n = buffer.readUInt16BE(2 + j * 10);
                text.push(n);
                var char_pp = new xdv_machine_1.CharPosition();
                char_pp.line = buffer.readUInt16BE(2 + j * 10 + 2);
                char_pp.column = buffer.readUInt16BE(2 + j * 10 + 4);
                char_pp.cs = buffer.readUInt16BE(2 + j * 10 + 6);
                char_pp.group = buffer.readUInt8(2 + j * 10 + 8);
                char_pp.fid = buffer.readUInt8(2 + j * 10 + 9);
                char_ps.push(char_pp);
            }
            var width = buffer.readUInt32BE(2 + textcount * 10);
            var glyphcount = buffer.readUInt16BE(2 + textcount * 10 + 4);
            // console.log(`How many count? ${glyphcount}`);
            if (buffer.length < 2 + textcount * 10 + 6 + glyphcount * 10) {
                throw new Error("not enough bytes to process opcode " + opcode + " " + buffer.length + " " + glyphcount);
            }
            var res = new SetTextGlyph({
                textcount: textcount,
                text: text,
                textpos: char_ps,
                width: width,
                glyphcount: glyphcount,
                length: 2 + textcount * 10 + 6 + glyphcount * 10 + 1,
            });
            return res;
        }
    }
    throw new Error("routine for " + opcode + " is not implemented");
}
function locatePagePtr(buffer, lastBop, page) {
    var offset = lastBop;
    while (offset > 0) {
        var bop_opcode = buffer.readUInt8(offset);
        if (bop_opcode !== 139) {
            throw new Error('Parse failed in reading bop preamble');
        }
        var boppage = buffer.readUInt32BE(offset + 1) + 1;
        if (boppage < page) {
            throw new Error('No such a page');
        }
        if (boppage === page) {
            // console.log(`Find page ${page}`);
            break;
        }
        var _prevPtr = buffer.readUInt32BE(offset + 41);
        if (_prevPtr > offset) {
            throw new Error('Parse failed in reading bop jump');
        }
        offset = _prevPtr;
    }
    return offset;
}
function parseHeader(buffer, machine) {
    var offset = 0;
    // Parse Header
    var preamble_code = buffer.readUInt8(offset);
    if (preamble_code !== Opcode.pre) {
        throw new Error("Invalid Header " + Opcode.pre + " " + preamble_code);
    }
    var command = parseCommand(preamble_code, buffer.slice(offset + 1));
    command.execute(machine);
    offset += command.length;
    // Parse Footer
    return offset;
}
function parseFileLists(buffer, machine, offset) {
    var skip_count = buffer.readUInt16BE(offset);
    // console.log('skip ' + (offset - skip_count));
    var fileListStr = buffer.slice(offset - skip_count, offset).toString();
    var fileList = fileListStr.split(';');
    fileList.pop();
    machine.fileList = fileList;
}
function parseFooter(buffer, machine) {
    var offset = buffer.length - 1;
    // Locate Post
    while (offset > 0) {
        var opcode = buffer.readUInt8(offset);
        if (opcode === 223) {
            offset -= 1;
        }
        else if (opcode === FILE_VERSION) {
            break;
        }
        else {
            throw new Error('Parse failed, signature check');
        }
    }
    offset -= 4;
    /* Two post post hack */
    parseFileLists(buffer, machine, offset - 2);
    var prevPtr = buffer.readUInt32BE(offset);
    if (prevPtr > offset) {
        throw new Error('Parse failed in reading post ending');
    }
    offset = prevPtr;
    // Post Sanity Check
    var post_opcode = buffer.readUInt8(offset);
    if (post_opcode !== Opcode.post) {
        throw new Error('Parse failed in reading post preamble');
    }
    var total_page = buffer.readUInt16BE(offset + 27);
    machine.totalPages = total_page;
    prevPtr = buffer.readUInt32BE(offset + 1);
    if (prevPtr > offset) {
        throw new Error('Parse failed in reading post preamble');
    }
    // Parse fonts definition
    while (offset < buffer.length) {
        var opcode = buffer.readUInt8(offset);
        var command = parseCommand(opcode, buffer.slice(offset + 1));
        command.execute(machine);
        offset += command.length;
        if (command.opcode === Opcode.post_post) {
            break;
        }
    }
    return prevPtr; // Last Bop
}
function parseXDV(dviContent, machine, page) {
    var buffer = Buffer.from(dviContent);
    var offset = 0;
    var firstBop = parseHeader(buffer, machine);
    var lastBop = parseFooter(buffer, machine);
    // Jump to Page
    if (page !== 0) {
        offset = locatePagePtr(buffer, lastBop, page);
    }
    else {
        offset = firstBop;
    }
    while (offset < buffer.length) {
        var opcode = buffer.readUInt8(offset);
        var command = parseCommand(opcode, buffer.slice(offset + 1));
        command.execute(machine);
        offset += command.length;
        if (page !== 0 && command.opcode === Opcode.eop) {
            break;
        }
        if (command.opcode === Opcode.post || command.opcode === Opcode.post_post) {
            break;
        }
    }
}
exports.parseXDV = parseXDV;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/browser/xdv-parser'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/node-libs-browser/node_modules/buffer/index.js */ "../../node_modules/node-libs-browser/node_modules/buffer/index.js").Buffer, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/lib/common/index.js":
/*!******************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/lib/common/index.js ***!
  \******************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(global) {
/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
exports.LATEX_SCHEME = 'latex';
exports.LATEX_LANGUAGE_ID = 'latex';
exports.LATEX_LANGUAGE_NAME = 'LaTeX';

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/latex/lib/common'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ "../../packages/latex/src/browser/style/index.css":
/*!**************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/latex/src/browser/style/index.css ***!
  \**************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {


var content = __webpack_require__(/*! !../../../../../node_modules/css-loader!./index.css */ "../../node_modules/css-loader/index.js!../../packages/latex/src/browser/style/index.css");

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

/***/ "../../packages/monaco/lib/browser/textmate/textmate-contribution.js":
/*!*********************************************************************************************************!*\
  !*** /home/elliott/playground/test/theia/packages/monaco/lib/browser/textmate/textmate-contribution.js ***!
  \*********************************************************************************************************/
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
/**
 * Callback for extensions to contribute language grammar definitions
 */
exports.LanguageGrammarDefinitionContribution = Symbol('LanguageGrammarDefinitionContribution');
function getEncodedLanguageId(languageId) {
    return monaco.languages.getEncodedLanguageId(languageId);
}
exports.getEncodedLanguageId = getEncodedLanguageId;

if (!global) global = {};
(global['theia'] = global['theia'] || {})['@theia/monaco/lib/browser/textmate/textmate-contribution'] = this;

/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../../../../../node_modules/webpack/buildin/global.js */ "../../node_modules/webpack/buildin/global.js")))

/***/ }),

/***/ 3:
/*!**********************!*\
  !*** zlib (ignored) ***!
  \**********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ }),

/***/ 4:
/*!********************!*\
  !*** fs (ignored) ***!
  \********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ }),

/***/ 5:
/*!**********************!*\
  !*** http (ignored) ***!
  \**********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ }),

/***/ 6:
/*!***********************!*\
  !*** https (ignored) ***!
  \***********************/
/*! no static exports found */
/***/ (function(module, exports) {

/* (ignored) */

/***/ })

}]);
//# sourceMappingURL=26.bundle.js.map