import * as fs from "fs";
import { dvi2html } from "./src";


let fonts = "";
fs.readdirSync('./fonts/output').forEach(file => {
    let name = file.replace(/.woff/, '');
    fonts = fonts + `@font-face { font-family: ${name}; src: url('fonts/output/${file}'); }\n`;
});
fonts +=  `@font-face { font-family: lmroman10-regular; src: url('lmroman10-regular.otf'); }\n`;
fonts +=  `@font-face { font-family: lmroman9-regular; src: url('lmroman9-regular.otf'); }\n`;

let bufContent = fs.readFileSync("test.xdv");

let html = "";
html = html + "<!doctype html>\n";
html = html + "<html lang=en>\n";
html = html + "<head>\n";
html = html + "<style>\n";
html = html + fonts;
html = html + "</style>"
html = html + "</head>\n";
html = html + '<body>\n';
html = html + '<div style="position: absolute;">\n';
html = html + dvi2html(bufContent);
html = html + '</div>\n';
html = html + '</body>\n';
html = html + "</html>\n";
fs.writeFileSync("index.html", html);2
console.log("DONE");
