"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.PdfTeXEngine = exports.CompileResult = exports.EngineStatus = void 0;
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
const {PdfTeXEngine} = require("./PdfTeXEngine");
const fs = require('fs');
const data = fs.readFileSync('https://texlive2.swiftlatex.com/pdftex/26/pdflatex.ini',
            {encoding:'utf8', flag:'r'});

console.log(data);

