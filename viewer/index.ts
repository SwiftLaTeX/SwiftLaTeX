import * as fs from "fs";
import { Machine } from "./src/machine";
import { parseDVI } from "./src/parser";


let bufContent = fs.readFileSync("test.xdv");
let machine = new Machine();
parseDVI(bufContent, machine);

let html = "";
html = html + "<!doctype html>\n";
html = html + "<html lang=en>\n";
html = html + "<head><style>\n";
html = html + machine.getStyle();
html = html + "</style></head>\n";
html = html + '<body>\n';
html = html + machine.getBody();
html = html + '</body>\n';
html = html + "</html>\n";
fs.writeFileSync("index.html", html);
console.log("DONE");
