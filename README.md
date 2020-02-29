# SwiftLaTeX

## One-liner introduction for SwiftLaTeX

SwiftLaTeX, a WYSIWYG WebAssembly-powered LaTeX Editor.

![Screenshot](https://github.com/SwiftLaTeX/SwiftLaTeX/raw/master/screenshot.png)

## What are the differences to Overleaf or ShareLaTeX?

1. __Speed__. SwiftLaTeX engines run in browsers. It makes SwiftLaTeX more responsive (i.e., faster compilation) and scalable.

2. __WYSIWYG__. SwiftLaTeX provides a WYSIWYG viewer, allowing users to editing PDF output directly.

2. __VS Code__. SwiftLaTeX is built atop the famous VS code platform, supporting rich editing features like LaTeX language server (i.e., code completion and syntax highlighting), spell check, and live preview.

4. __Flexible Storage Backend__. SwiftLaTeX stores files in self-hosting Minio Object storage. SwiftLaTeX can also sync project files to different cloud storage backends such as Dropbox, Google Drive, OneDrive, and Github.

5. __Realtime collaboration__. Send a secret link to your friend to start real-time collaboration. 

## Great, I wanna have a try!
Try it in https://v2.swiftlatex.com

Or host it by yourself using docker-compose
``
docker-compose up
``

## Known Bugs in SwiftLaTeX?

SwiftLaTeX is a quick evolving project and may consist of a small number of bugs. We are currently working hard on the following issues.

1. Previewer may fail to display certain documents. (e.g., \usepackage{2up} will screw the viewer)

2. Previewer may fail to display images. 

3. WYSIWYG functionality is not available for formulas yet.

## SwiftLaTeX Architecture
This section is devoted for interesting developers only. If you want more help, you could considering opening an issue.

SwiftLaTeX consists of five major components.
### Engine 
SwiftLaTeX engin (fork from XeTeX) supports UTF-8 and opentype fonts out of box. The engine binary is available under the bin folder. It exposes two major APIs **_compileLaTeX** and **_compileBibtex** for the use of developers. For more implementation details, you can check this source file (https://github.com/SwiftLaTeX/IDE/blob/master/packages/latex/src/browser/latex-engine.ts).


