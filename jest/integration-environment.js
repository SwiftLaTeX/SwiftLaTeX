const NodeEnvironment = require('jest-environment-node');
const fs = require('fs');
const path = require('path');
const puppeteer = require('puppeteer');

module.exports = class extends NodeEnvironment {
  async setup() {
    await super.setup();

    const endpoint = fs.readFileSync(path.join('node_modules', '.snack-ws-endpoint'), 'utf8');

    if (!endpoint) {
      throw new Error('Unable to connect to puppeteer: browserWSEndpoint not found');
    }

    this.global.browser = await puppeteer.connect({
      browserWSEndpoint: endpoint,
    });

    this.global.page = await this.global.browser.newPage();

    await this.global.page.goto('http://localhost:3011');
  }

  async teardown() {
    await this.global.page.close();
    await this.global.browser.disconnect();
    await super.teardown();
  }

  runScript(script) {
    return super.runScript(script);
  }
};
