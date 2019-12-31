import * as fs from "fs";
import { dvi2html } from "./src";
import { Writable } from 'stream';

let filename = 'test.xdv';

let stream = fs.createReadStream(filename, { highWaterMark: 256 });

let html = "";
html = html + "<!doctype html>\n";
html = html + "<html lang=en>\n";
html = html + "<head>\n";
html = html + "</head>\n";
html = html + '<body>\n';
html = html + '<div style="position: absolute;">\n';

const myWritable = new Writable({
  write(chunk, encoding, callback) {
    html = html + chunk;
    callback();
  }
});

async function main() {
  await dvi2html( stream, myWritable );
  
  html = html + '</div>\n';
  html = html + '</body>\n';
  html = html + "</html>\n";

  fs.writeFileSync("index.html", html);
}

main()
console.log("DONE");
