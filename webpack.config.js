/* eslint-disable import/no-commonjs */
const webpack = require('webpack');
const path = require('path');
const TerserPlugin = require('terser-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');


module.exports = {
  mode: process.env.NODE_ENV === 'production' ? 'production' : 'development',
  devtool: 'source-map',
  entry: {
    // Editor bundle
    editor: './src/client/editor.tsx',

    // Storage Library
    storage: './src/client/storage/index.tsx',

    // Editor Worker
    'editor.worker': 'monaco-editor/esm/vs/editor/editor.worker.js',
  },
  output: {
    globalObject: 'self',
    path: path.resolve(__dirname, 'dist'),
    publicPath: '/dist/',
    filename: '[name].bundle.js',
    chunkFilename: '[id].[hash].chunk.js',
  },
  optimization: {
    noEmitOnErrors: true,
    minimize: process.env.NODE_ENV === 'production',
    minimizer: [
      new TerserPlugin({
        exclude: /eslint_bundle/,
        parallel: true,
      }),
    ],
  },
  plugins: [
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify(process.env.NODE_ENV),
    }),
    new webpack.IgnorePlugin(
      /^((fs)|(path)|(os)|(crypto)|(source-map-support))$/,
      /vs(\/|\\)language(\/|\\)typescript(\/|\\)lib/,
    ),
    new MiniCssExtractPlugin(),
  ],
  module: {
    rules: [
      {
        // graphql-request includes this polyfill
        test: path.resolve(__dirname, 'node_modules/cross-fetch/dist/browser-polyfill.js'),
        use: 'null-loader',
      },
      {
        test: /\.(js|tsx?)$/,
        exclude: /(node_modules|(vendor\/.+.bundle\.js))/,
        use: {
          loader: 'babel-loader',
        },
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader'],
      },
      {
        test: /\.(png|jpg|gif|svg|ttf)$/,
        use: {
          loader: 'file-loader',
          options: {
            outputPath: 'assets/',
          },
        },
      },
    ],
  },
  resolve: {
    extensions: ['.tsx', '.ts', '.js'],
    alias: {
      aphrodite: 'aphrodite/no-important',
    },
  },
};
