"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var machine_1 = require("./machine");
var parser_1 = require("./parser");
function dvi2html(buf) {
    var machine = new machine_1.Machine();
    parser_1.parseDVI(buf, machine);
    return machine.output;
}
exports.dvi2html = dvi2html;
