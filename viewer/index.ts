import * as fs from "fs";
import { dvi2html } from "./src";


let bufContent = fs.readFileSync("test.xdv");

let html = "";
html = html + "<!doctype html>\n";
html = html + "<html lang=en>\n";
html = html + "<head>\n";
html = html + "</head>\n";
html = html + '<body>\n';
html = html + '<div style="position: absolute;">\n';
html = html + dvi2html(bufContent);
html = html + '</div>\n';
html = html + '</body>\n';
html = html + "</html>\n";
fs.writeFileSync("index.html", html);
console.log("DONE");
