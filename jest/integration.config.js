module.exports = {
  displayName: 'snack-integration',
  rootDir: '..',
  transform: { '^.+\\.tsx?$': 'ts-jest' },
  moduleFileExtensions: ['ts', 'tsx', 'js', 'json'],
  testRegex: '/__integration-tests__/.*\\.test\\.(js|tsx?)$',
  testPathIgnorePatterns: ['/node_modules/', '/dist/', '/build/'],
  testEnvironment: '<rootDir>/jest/integration-environment.js',
  setupTestFrameworkScriptFile: '<rootDir>/jest/integration-setup-framework.js',
  globalSetup: '<rootDir>/jest/integration-global-setup.js',
  globalTeardown: '<rootDir>/jest/integration-global-teardown.js',
  globals: {
    'ts-jest': {
      diagnostics: false,
    },
  },
};
