# SwiftLaTeX

## Introduction for SwiftLaTeX

SwiftLaTeX, a WYSIWYG WebAssembly client-side LaTeX Editor. All computation is done locally and files are stored in user-provided cloud storage. 

![Screenshot](https://www.swiftlatex.com/images/screenshot-light.png)

## What are the differences to Overleaf or ShareLaTeX?

1. __Speed__. SwiftLaTeX engines run in browsers, all computation is done strictly locally. It makes SwiftLaTeX more responsive (i.e., faster compilation) and scalable.

2. __WYSIWYG__. SwiftLaTeX provides a WYSIWYG viewer, allowing users to editing PDF output directly.

3. __Flexible Storage Backend__. SwiftLaTeX stores files in user-provided cloud storage or potentially self-hosting S3/Minio. 

4. __Realtime collaboration__. Send a secret link to your friend to start real-time collaboration. 

## Great, I wanna have a try!
Try it in https://www.swiftlatex.com

If you want to host SwiftLaTeX by yourself, you could compile it by yourself with a Node 12 toolchain

```
git clone https://github.com/SwiftLaTeX/SwiftLaTeX
cd SwiftLaTeX
yarn
yarn start
```
The server will be up and running in http://localhost:3011

The Google and Dropbox storage options should work out of the box. For playgroud to work, you should set up a Minio server locally. More details to come.

## SwiftLaTeX Technical Details
This section is devoted for developers only. If you want more help, you could consider opening an issue.

### XeTeX Engine (Experimental)
SwiftLaTeX engine is based on Tectonic (a C++ reimplementation of XeTeX), which supports UTF-8 and Opentype fonts out of box.
The engine is not 100% faithful to XeTeX. 
First, though XeTeX does support Chinese/Japanese/Korea/Arabic languages, the locale line breaking function is not available yet. This function is rarely used but supporting it requires a fully working ICU library, whose size may be unbearable in browser environments.
Secondly, XeTeX does not support graphite fonts, which are rarely used.
You can find the engine source code in https://github.com/SwiftLaTeX/SwiftLaTeX-Engine. Nevertheless, compiling it requires an Emscripten toolchain and some basic knowledge about WebAssembly. We would strongly recommend you to use the prebuilt binaries in this repo. 
XeTeX engine now directly outputs SVG files, which can be directly rendered in browsers.
XeTeX currently does not faithfully display Tikz and PGF. (We are not very sure how many people are using these technologies nowadays). But if you insist, you should select a correct pgf driver as follows before including the tikz package. 
```
% SwiftLaTeX provides a beta support for Tikz and PGF, provided that you use a correct driver
\def\pgfsysdriver{pgfsys-swiftlatex.def}
\documentclass{article}
\usepackage{tikz}
\usepackage{verbatim}
```
![Screenshot](https://s3.swiftlatex.com/swiftlatex2/9FSa0rkYnHCWlA9ThUbeUYWKyE2A4yrK/asset/cn9tf1YLHmlocalhost_3011_editor.html_p_3D0KaL0q7np8(iPad_20Pro)_20(2).png)

SwiftLaTeX supports most legacy TeX fonts automatically. 
SwiftLaTeX also supports opentype fonts (ttf, otf), but you need to upload the fonts by yourself due to licensing issues. Once you upload your fonts, you can use fonts as follows.
```
\usepackage{fontspec}
\setmainfont{foobar.ttf} 
```

### PDFTeX Engine
Despite XeTeX engine is considered to be much modern, many users may still prefer the good-old PdfTeX engine.
In fact, the first SwiftLaTeX prototype was based on PdfTeX. 
PdfTeX possesses some advantages such as 3x faster compilation, faithful PDF display (means full support to Tikz and PGF) and a broader LaTeX package support.
Therefore, our team is working hard to bring the PdfTeX engine online. Users will be able to choose from PdfTeX and XeTeX then.
If Unicode and Freetype is not your concern, feel free to choose PDFTeX as it usually contains fewer bugs.

### CTAN files.
LaTeX is not a stand-alone typesetting program in itself, but document preparation software that runs on top of Donald E. Knuth's TeX typesetting system. TeX distributions usually bundle together all the parts needed for a working TeX system and they generally add to this both configuration and maintenance utilities. Nowadays LaTeX, and many of the packages built on it, form an important component of any major TeX distribution. 
In SwiftLaTeX, all required files are fetched from CTAN (Comprehensive TeX Archive Network), https://www.ctan.org, or our mirror server. 
### Language Server
SwiftLaTeX is using Digestif as a LaTeX language server. It is amazingly small and powerful. You could check out the project at https://github.com/astoff/digestif.

### Change Logs:
You can check the detailed change logs in http://www.swiftlatex.com/changelogs.html

### Production Use.
SwiftLaTeX is almost for production use, with the following exceptions. These exceptions will be gradually removed.
1. Viewer cannot display pdf images yet, but shows a placeholder for now.
2. No graphite fonts and international line-breaking support. We so fardeem this functionalties not very useful for English-speaking people. 
3. Sharing is not fully implemented yet, but it is on the way. 
 
## License

SwiftLaTeX is released under AGPL3 license. 
