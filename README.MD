# SwiftLaTeX

## Introduction for SwiftLaTeX

SwiftLaTeX, a WYSIWYG WebAssembly client-side LaTeX Editor. All computation is done locally and files are stored in user-provided cloud storage. 

## Main Features

1. __Speed__. SwiftLaTeX engines run in browsers, all computation is done strictly locally. It makes SwiftLaTeX more responsive (i.e., faster compilation) and scalable.

2. __WYSIWYG__. SwiftLaTeX provides a WYSIWYG viewer, allowing users to editing PDF output directly.

3. __Flexible Storage Backend__. SwiftLaTeX stores files in user-provided cloud storage or potentially self-hosting S3/Minio. 

4. __Realtime collaboration__. Send a secret link to your friend to start real-time collaboration.

## Installation Guide

No installation needed, just try it in https://www.swiftlatex.com

#### Compile by yourself
If you do not have node and yarn yet, try the following commands first.
```
wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.3/install.sh | bash
nvm install 12 (you may need to restart your console to use nvm)
npm install yarn -g 
```

If you already have Node 12 and yarn, just run
```
git clone https://github.com/SwiftLaTeX/SwiftLaTeX
cd SwiftLaTeX
yarn
yarn start
```

## SwiftLaTeX Technical Details

### XeTeX Engine
SwiftLaTeX engine is based on Tectonic (a C++ reimplementation of XeTeX), which supports UTF-8 and Opentype fonts out of box.
The engine is not 100% faithful to XeTeX. 
First, though XeTeX does support Chinese/Japanese/Korea/Arabic languages, the locale line breaking function is not available yet. This function is rarely used and supporting it requires a fully working ICU library, whose size may be unbearable in browser environments.
Secondly, This engine not support graphite fonts, which are rarely used too.

![Screenshot](https://s3.swiftlatex.com/swiftlatex2/9FSa0rkYnHCWlA9ThUbeUYWKyE2A4yrK/asset/cn9tf1YLHmlocalhost_3011_editor.html_p_3D0KaL0q7np8(iPad_20Pro)_20(2).png)

SwiftLaTeX supports most legacy TeX fonts automatically. 
SwiftLaTeX also supports opentype fonts (ttf, otf), SwiftLaTeX provides a set of basic fonts (same as what you get from apt install texlive-full in Ubuntu), you could of cource upload your own opentype fonts.
```
\usepackage{fontspec}

\setmainfont{TeX Gyre Pagella}
\setsansfont{TeX Gyre Heros}[Scale=MatchLowercase]
\setmonofont{Inconsolata}[Scale=MatchLowercase]
```

![Screenshot](https://s3.swiftlatex.com/swiftlatex2/AWdSGuPe60mHwNXnzqDeuOLzWUgkaeaE/asset/localhost_3011_editor,html_p%3DMy_fancy_project,cCBkO57V(iPad_Pro).iBO26.png)

#### Build Engine
If you wanna build or enhance XeTeX engine by yourself, you need to install Emscripten first.
```
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git

# Enter that directory
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the latest SDK tools.
./emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes .emscripten file)
./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

Then go to the directory 'webassembly' and type make. After that, you will find the engine files swiftlatex.wasm and swiftlatex.js over there.
```
cd webassembly
make
```

#### Enhance Engine Display Backend
Our engine outputs html directly, you can find and hack the backend in this file(https://github.com/SwiftLaTeX/SwiftLaTeX/blob/aaaba052def4b0b7e770ac46126cdaf0b9fc39f1/webassembly/xetexdir/XeTeX_ext.c#L313). Currently, Tikz is still not fully supported. Any contributions will be appreciated. We are consider moving this logic to javascript instead.

### Storage Backends
SwiftLaTeX has few requirements on storage backends. A developer who wishes to add new storage backends just needs to implement the following three simple methods. 

```
abstract put(scope: string, itemKey: string, file: Blob): Promise<string>;
put a file to the (scope, itemKey) and return an accessible http url to the file. SwiftLaTeX stores files in a two-level key-val structure. 

abstract get(scope: string, itemKey: string): Promise<ArrayBuffer>;
get the file located in (scope, itemKey)

abstract list(scope: string): Promise<ItemEntry[]>;
list all files located in (scope)
```
For more concrete examples, please have a look at src/client/storage/minio.tsx.

### Project Sharing and Real-time collaboration 
SwiftLaTeX adopts a simple sharing mechanism, which is agnostic to storage backends, provided that the storage backends provide a public url to each file. (For example, a user can store his project in Google Drive and share the project to users who use Dropbox).
Exposing public urls is not a security risk, as they are usually very long and impractical to guess. 

Sharing is achieved by using the YJS, A CRDT framework with a powerful abstraction of shared data. Every changed is synced among users.
https://github.com/yjs/yjs

### CTAN files
LaTeX is not a stand-alone typesetting program in itself, but document preparation software that runs on top of Donald E. Knuth's TeX typesetting system. TeX distributions usually bundle together all the parts needed for a working TeX system and they generally add to this both configuration and maintenance utilities. Nowadays LaTeX, and many of the packages built on it, form an important component of any major TeX distribution. 
In SwiftLaTeX, all required files are fetched from CTAN (Comprehensive TeX Archive Network), https://www.ctan.org, or our mirror server. 

### Change Logs
You can check the detailed change logs in http://www.swiftlatex.com/changelogs.html

### Production Use
SwiftLaTeX is almost ready for production use, with the following exceptions. These exceptions will be gradually removed.
1. No graphite fonts and international line-breaking support. We so far deem this functionalty not very useful for English-speaking people. 
3. XeTeX engine cannot properly display standalone tikz pictures, we are still investigating this issue.


## License
The code in this repository is released under the GNU AFFERO GENERAL PUBLIC LICENSE, version 3. A copy can be found in the LICENSE file.
Copyright (c) SwiftLab, 2018-2021.
