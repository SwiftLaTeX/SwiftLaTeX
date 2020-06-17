const fs = require('fs');
const path = require('path');

module.exports = async function teardown() {
  fs.unlinkSync(path.join('node_modules', '.snack-ws-endpoint'), global.__BROWSER__.wsEndpoint());

  await global.__BROWSER__.close();
  await process.kill(-global.__SERVER__.pid);
};
