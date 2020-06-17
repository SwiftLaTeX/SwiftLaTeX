module.exports = {
  displayName: 'snack',
  rootDir: '..',
  transform: { '^.+\\.tsx?$': 'ts-jest' },
  moduleFileExtensions: ['ts', 'tsx', 'js', 'json'],
  testPathIgnorePatterns: ['/node_modules/', '/dist/', '/build/'],
  testRegex: '/__tests__/.*\\.test\\.(js|tsx?)$',
  setupFiles: ['<rootDir>/jest/unit-setup.js'],
  testURL: 'https://snack.expo.io/',
  globals: {
    'ts-jest': {
      diagnostics: false,
    },
  },
};
