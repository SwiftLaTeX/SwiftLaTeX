import * as fs from "fs";

import { dvi2html } from "./src";
import { Writable } from 'stream';



//execSync("latex sample/sample.tex");

let fonts = "";
fs.readdirSync('./fonts/output').forEach(file => {
    let name = file.replace(/.woff/, '');
    fonts = fonts + `@font-face { font-family: ${name}; src: url('fonts/output/${file}'); }\n`;
});
fonts +=  `@font-face { font-family: lmroman10-regular; src: url('lmroman10-regular.otf'); }\n`;
fonts +=  `@font-face { font-family: lmroman9-regular; src: url('lmroman9-regular.otf'); }\n`;
fs.writeFileSync("fonts.css", fonts);

let filename = 'test.xdv';

let stream = fs.createReadStream(filename, { highWaterMark: 4096 });

let html = "";
html = html + "<!doctype html>\n";
html = html + "<html lang=en>\n";
html = html + "<head>\n";
html = html + '<link rel="stylesheet" type="text/css" href="fonts.css">\n';
html = html + "</head>\n";
html = html + '<body>\n';
html = html + '<div style="position: absolute;">\n';

//html = html + dviParser( buffer );

const myWritable = new Writable({
    write(chunk, encoding, callback) {
        html = html + chunk;
        callback();
    }
});

async function main() {
    await dvi2html(stream, myWritable);

    html = html + '</div>\n';
    html = html + '</body>\n';
    html = html + "</html>\n";

    fs.writeFileSync("index.html", html);
}

main()
console.log("DONE");
