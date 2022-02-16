
# SwiftLaTeX

Demo Website https://www.swiftlatex.com

[![Building Assets](https://github.com/SwiftLaTeX/SwiftLaTeX/actions/workflows/github-actions-build.yml/badge.svg?branch=master)](https://github.com/SwiftLaTeX/SwiftLaTeX/actions/workflows/github-actions-build.yml)

## Introduction for SwiftLaTeX

SwiftLaTeX, LaTeX Engines in Browsers with optional WYSIWYG support. We are a big fan of WebAssembly and all computation is done locally.

## Main Features

1. __Speed__. SwiftLaTeX engines run in browsers, all computation is done strictly locally.
2. __Library Support__. Simply include a script tag and use PdfTeX or XeTeX in your own webpage.
3. __Compatibility__. Produce exact same output you would get from TexLive or MikTeX.
4. __WYSIWYG__. SwiftLaTeX provides an optional WYSIWYG editor, allowing users to editing PDF output directly (WIP).

## Quick Installation Guide
1. Download latest release from Github. Extract the files and put them into your webpage directory.
2. Include the src tag in your page.
```
<script src="PdfTeXEngine.js"></script>
```
3. Initialize the engine.
```
const engine = new PdfTeXEngine();
await engine.loadEngine();
```
4. Load the tex files/pictures/fonts to engines.
```
engine.writeMemFSFile("main.tex", "\documentclass{...");
```
5. Set the main file and fire the engine.
```
engine.setEngineMainFile("main.tex");
let r = await engine.compileLaTeX(); // r contains PDF binray and compilation log.
```

## APIs
1. async loadEngine(): Promise<void>. \
Load the webassembly engine.
2. isReady(): boolean. \
Check whether the engine is ready to compile documents.
3. writeMemFSFile(filename: string, srccode: string | Uint8Array). \
Upload source codes / assets to the engine.
4. makeMemFSFolder(folder: string). \
Create a directory.
5. setEngineMainFile(filename: string). \
Tell the engine which latex file is the entry file.
6. compileLaTeX():Promise<CompileResult>. \
Start compiling LaTeX documents and return CompileResult.
7. flushCache(). \
Purge all the files uploaded.
8. closeWorker(). \
Shutdown the engine.
9. setTexliveEndpoint(url: string). \
If you host your own texlive server, you can ask the engine to fetch files from the server.
10. compileFormat(). \
If you host your own texlive server, you may wanna generate the tex engine format file via this function.

## Compile Engines by yourself
SwiftLaTeX compiles XeTeX and PdfTeX engines into WebAssembly. We recommend the XeTeX engine as it supports UTF-8 and Opentype fonts out of box.
The engine is almost 100% identical to XeTeX except SwiftLaTeX does not include a full ICU dataset. As a result, the locale linebreaking may not function as expected. This issue is easy to fix: we just need to initialize the ICU library with the correct dataset.  If you just need to handle English, the PdfTeX is also a nice option. It is less compilcated, thus faster and less buggy.

1. Get the emsdk repo
```
git clone https://github.com/emscripten-core/emsdk.git
```
2. Enter that directory
```
cd emsdk
```
3. Fetch the latest version of the emsdk (not needed the first time you clone)
```
git pull
```
4. Download and install the latest SDK tools.
```
./emsdk install latest
```
5. Make the latest" SDK "active" for the current user. (writes .emscripten file)
```
./emsdk activate latest
```
6. Activate PATH and other environment variables in the current terminal
```
source ./emsdk_env.sh
```
7. Compile XeTeX or PdfTeX
```
cd pdftex.wasm or xetex.wasm
make
```





## CTAN files
LaTeX is not a stand-alone typesetting program in itself, but document preparation software that runs on top of Donald E. Knuth's TeX typesetting system. TeX distributions usually bundle together all the parts needed for a working TeX system and they generally add to this both configuration and maintenance utilities. Nowadays LaTeX, and many of the packages built on it, form an important component of any major TeX distribution. 
In SwiftLaTeX, all required files are fetched from CTAN (Comprehensive TeX Archive Network), https://www.ctan.org, or our mirror server https://texlive.swiftlatex.com. 
You can checkout the repo https://github.com/SwiftLaTeX/Texlive-Ondemand and host your own server. 

## WYSIWYG Editor
We are also working hard to bring back our WYSIWYG latex editor. The editor is pretty similar to overleaf, except users are allowed to edit pdf output directly. 

## License
The code in this repository is released under the GNU AFFERO GENERAL PUBLIC LICENSE, version 3. A copy can be found in the LICENSE file.
Copyright (c) SwiftLab, 2018-2022.
