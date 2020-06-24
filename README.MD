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

## SwiftLaTeX Architecture
This section is devoted for interesting developers only. If you want more help, you could considering opening an issue.

SwiftLaTeX consists of three major components.

### Engine 
SwiftLaTeX engine (a C++ reimplementation of XeTeX) supports UTF-8 and Opentype fonts out of box. 
The engine binary exposes two major APIs **_compileLaTeX** and **_compileBibtex**.
You may find the following source file useful as it showcases how to use these APIs.
(https://github.com/SwiftLaTeX/IDE/blob/master/src/client/swiftlatex/latexEngine.ts). 

The engine is not a faithful reimplementation of XeTeX. First, it does not support Chinese/Japanese/Korea/Arabic languages. Supporting these languages require a fully working ICU library, whose size may be unbearable. Secondly, the engine uses browsers' APIs to handle image files. 
Due to API limitation (or my laziness), our engine does not support multi-page PDF images yet. 
If you want to include a multi-page PDF images in your document, consider extracting the page you want to include and save it as an independent file first. 

You can find the engine source code in https://github.com/SwiftLaTeX/SwiftLaTeX-Engine. Compiling it requires an Emscripten toolchain. 
If you need more tech support on the engine or want to submit a feature request, you can contact g.weber(at)auckland.ac.nz.

### Live Viewer
Our engine now directly outputs SVG files, which can be directly rendered in browsers.
The generated SVGs contains SyncTeX information, which is the mapping from the HTML element to the source code position. The information allows us to implement WYSIWYG editing on the viewer.

### Font support
SwiftLaTeX supports most legacy TeX fonts behind the scene. 
SwiftLaTeX supports opentype fonts (ttf, otf), but you need to upload the fonts by yourself due to license issue. Once you upload your fonts, you can use
```
\usepackage{fontspec}
\setmainfont{foobar.ttf} 
```
To use your fonts.

### Open source components
SwiftLaTeX would not be here without the following amazing open source projects.
1) Tectonic and XeTeX https://github.com/tectonic-typesetting/tectonic
2) Digestif, a Lua-based LaTeX language server. https://github.com/astoff/digestif

### Change Logs:
2020-6-23 18:00 NZT. We update the logic to fetch CTAN files, now the viewer can now properly display legacy tex fonts. 

### Production use
SwiftLaTeX is almost for production use, with the following exceptions. These exceptions will be gradually removed.
1. Viewer cannot display pdf images yet, but shows a placeholder for now.
2. No graphite fonts and international line-breaking support. We so fardeem this functionalties not very useful for English-speaking people. 
3. Sharing is not fully implemented yet, but it is on the way. 
 
## License

SwiftLaTeX is released under AGPL3 license. 
