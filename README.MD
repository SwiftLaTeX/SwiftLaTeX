# SwiftLaTeX

## Introduction for SwiftLaTeX

SwiftLaTeX, a WYSIWYG WebAssembly client-side LaTeX Editor. All computation is done locally and files are stored in user-provided cloud storage. 

![Screenshot](https://www.swiftlatex.com/images/screenshot-light.png)

## What are the differences to Overleaf or ShareLaTeX?

1. __Speed__. SwiftLaTeX engines run in browsers, all computation is done strictly locally. It makes SwiftLaTeX more responsive (i.e., faster compilation) and scalable.

2. __WYSIWYG__. SwiftLaTeX provides a WYSIWYG viewer, allowing users to editing PDF output directly.

3. __Flexible Storage Backend__. SwiftLaTeX stores files in user-provided cloud storage or potentially self-hosting S3/Minio. 

4. __Realtime collaboration__. Send a secret link to your friend to start real-time collaboration. 

## Installation Guide

No installation needed, just try it in https://www.swiftlatex.com

### Host in Windows
Download the latest release from https://github.com/SwiftLaTeX/SwiftLaTeX/releases.

Unzip the file, and click the webserver.exe.

Open your browser, and go to http://localhost:3011.

This option allows you to store files in your local hard drive, Google, or Dropbox.

### Host in Ubuntu
If you want to host SwiftLaTeX by yourself, you could 1) use precompiled version, 2) compile source code by yourself, or 3) use Docker.

#### Use precompiled version 
Download the latest release from https://github.com/SwiftLaTeX/SwiftLaTeX/releases.
Unzip the file, and and enter the directory.
```
python3 webserver_src.py
```
Open your browser, and go to http://localhost:3011.

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

#### Use Docker
If you prefer using Docker, you could try the following commands.
```
git clone https://github.com/SwiftLaTeX/SwiftLaTeX
cd SwiftLaTeX
docker build . -t swiftlatex/swiftlatexv2
docker run -p 3011:3011 swiftlatex/swiftlatexv2
```
The server will be up and running in http://localhost:3011.
All storage options should work of the box. You could host your own Minio server and provide the credential info to src/server/minio.tsx, such that you can store playgroud files in your server. 

## SwiftLaTeX Technical Details

### XeTeX Engine
SwiftLaTeX engine is based on Tectonic (a C++ reimplementation of XeTeX), which supports UTF-8 and Opentype fonts out of box.
The engine is not 100% faithful to XeTeX. 
First, though XeTeX does support Chinese/Japanese/Korea/Arabic languages, the locale line breaking function is not available yet. This function is rarely used but supporting it requires a fully working ICU library, whose size may be unbearable in browser environments.
Secondly, This engine not support graphite fonts, which are rarely used.
You can find the engine source code in https://github.com/SwiftLaTeX/SwiftLaTeX-Engine. Nevertheless, compiling it requires an Emscripten toolchain and some basic knowledge about WebAssembly. We would strongly recommend you to use the prebuilt binaries in this repo. 

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

### PDFTeX Engine
Despite XeTeX engine is considered to be much modern, many users may still prefer the good-old PdfTeX engine.
In fact, the first SwiftLaTeX prototype was based on PdfTeX. 
PdfTeX possesses some advantages such as 3x faster compilation, faithful PDF display (means full support to Tikz and PGF) and a broader LaTeX package support.
Therefore, we now allow users to use the legacy PdfTeX engine. 
PDFTeX engine does not fully support WYSIWYG editing.  
Also Unicode and Freetype are not supported. You could switch the engine as shown below.
![Screenshot](https://s3.swiftlatex.com/swiftlatex2/7PWFAFkNUJsIqFbEj1rPdGWgqELHX1AH/asset/www,swiftlatex,com_editor,html_p%3D8hYjNAR9XQ.hhE3R.png)

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

(More technical details to come. Sharing is always a headache considering the differences among storage backends)


### CTAN files
LaTeX is not a stand-alone typesetting program in itself, but document preparation software that runs on top of Donald E. Knuth's TeX typesetting system. TeX distributions usually bundle together all the parts needed for a working TeX system and they generally add to this both configuration and maintenance utilities. Nowadays LaTeX, and many of the packages built on it, form an important component of any major TeX distribution. 
In SwiftLaTeX, all required files are fetched from CTAN (Comprehensive TeX Archive Network), https://www.ctan.org, or our mirror server. 

### Language Server
SwiftLaTeX is using Digestif as a LaTeX language server. It is amazingly small and powerful. You could check out the project at https://github.com/astoff/digestif.

### Change Logs
You can check the detailed change logs in http://www.swiftlatex.com/changelogs.html

### Production Use
SwiftLaTeX is almost for production use, with the following exceptions. These exceptions will be gradually removed.
1. No graphite fonts and international line-breaking support. We so far deem this functionalty not very useful for English-speaking people. 
2. Sharing is not fully implemented yet, but it is on the way. 
3. XeTeX engine cannot properly display standalone tikz pictures, we are still investigating this issue.
 
## License

The code in this repository is released under the GNU AFFERO GENERAL PUBLIC LICENSE, version 3. A copy can be found in the LICENSE file.
Copyright (c) SwiftLab, 2014-2020.
