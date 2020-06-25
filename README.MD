# SwiftLaTeX

## One-liner introduction for SwiftLaTeX

SwiftLaTeX, a WYSIWYG WebAssembly-powered LaTeX Editor.

![Screenshot](https://www.swiftlatex.com/images/screenshot-light.png)

## What are the differences to Overleaf or ShareLaTeX?

1. __Speed__. SwiftLaTeX engines run in browsers. It makes SwiftLaTeX more responsive (i.e., faster compilation) and scalable.

2. __WYSIWYG__. SwiftLaTeX provides a WYSIWYG viewer, allowing users to editing PDF output directly.

3. __Flexible Storage Backend__. SwiftLaTeX stores files in user-provider cloud storage or self-hosting Minio. 

4. __Realtime collaboration__. Send a secret link to your friend to start real-time collaboration. 

## Great, I wanna have a try!
Try it in https://www.swiftlatex.com

Or host it by yourself.

```
git clone https://github.com/SwiftLaTeX/SwiftLaTeX
yarn
yarn start
```
It should work out of the box for Google and Dropbox storage. For playgroud to work, you should consider setting up a Minio server locally. More details to come.

## SwiftLaTeX Architecture
This section is devoted for interesting developers only. If you want more help, you could considering opening an issue.

### Live Viewer
Our engine now directly outputs SVG files, which can be directly rendered in browsers.
The generated SVGs contains SyncTeX information, which is the mapping from the HTML element to the source code position. The information allows us to implement WYSIWYG editing on the viewer.
However, our viewer does not support Tikz and PGF yet.  

### Font support
SwiftLaTeX supports most legacy TeX fonts behind the scene. 
SwiftLaTeX supports opentype fonts (ttf, otf), but you need to upload the fonts by yourself due to license issue. Once you upload your fonts, you can use fonts with the following snippets.
```
\usepackage{fontspec}
\setmainfont{foobar.ttf} 
```

### Engine 
SwiftLaTeX engine is based on Tectonic (a C++ reimplementation of XeTeX), which supports UTF-8 and Opentype fonts out of box. 
The engine binary exposes two major APIs **_compileLaTeX** and **_compileBibtex**.
You may find the following source file useful as it showcases how to use these APIs.
(https://github.com/SwiftLaTeX/IDE/blob/master/src/client/swiftlatex/latexEngine.ts). 

The engine is not 100% faithful to XeTeX. 
First, it does not support Chinese/Japanese/Korea/Arabic languages.
Supporting these languages require a fully working ICU library, whose size may be unbearable.
Secondly, it does not support graphite fonts, which are rarely used.
You can find the engine source code in https://github.com/SwiftLaTeX/SwiftLaTeX-Engine. Nevertheless, compiling it requires an Emscripten toolchain and some basic knowledge about WebAssembly. We would strongly recommend you to use the prebuilt binaries in this repo. 

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
