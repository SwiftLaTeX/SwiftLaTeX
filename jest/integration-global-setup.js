const puppeteer = require('puppeteer');
const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

module.exports = async function setup() {
  const server = child_process.spawn('node', ['build/server'], { detached: true });

  await Promise.race([
    new Promise((resolve, reject) => setTimeout(reject, 5000)),
    new Promise((resolve, reject) => {
      server.stdout.on('data', data => {
        if (/The Snack web server is listening/.test(data.toString())) {
          resolve(data.toString());
        }
      });

      server.stderr.on('data', data => {
        reject(data.toString());
      });
    }),
  ]);

  const browser = await puppeteer.launch();

  fs.writeFileSync(path.join('node_modules', '.snack-ws-endpoint'), browser.wsEndpoint());

  global.__SERVER__ = server;
  global.__BROWSER__ = browser;
};
