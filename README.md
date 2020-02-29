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

```
docker-compose up
```

## Known Bugs in SwiftLaTeX?

SwiftLaTeX is a quick evolving project and may consist of a small number of bugs. We are currently working hard on the following issues. We expect to fix them in the next few weeks.

1. Previewer may fail to display certain documents. (e.g., \usepackage{2up} will screw the viewer)

2. Previewer may fail to display images. 

3. WYSIWYG functionality is not available for formulas yet.

## SwiftLaTeX Architecture
This section is devoted for interesting developers only. If you want more help, you could considering opening an issue.

SwiftLaTeX consists of five major components.

### Engine 
SwiftLaTeX engine (a C++ reimplementation of XeTeX) supports UTF-8 and Opentype fonts out of box. 
The engine binary exposes two major APIs **_compileLaTeX** and **_compileBibtex**.
You may find the following source file useful as it showcases how to use these APIs.
(https://github.com/SwiftLaTeX/IDE/blob/master/packages/latex/src/browser/latex-engine.ts). 

The engine is not a faithful reimplementation. First, it does not support Chinese/Japanese/Korea/Arabic languages. Supporting these languages require a fully working ICU library, whose size may be unbearable. Secondly, the engine uses browsers' APIs to handle image files. 
Due to API limitation (or my laziness), our engine does not support multi-page PDF images yet.

Currently, the engine is under a proprietary license, while our team is happy to grant permission for you or your organization to use the binary for non-commercial purposes (e.g., individual or tertiary education). 
If you need more tech support on the engine, you can contact g.weber(at)auckland.ac.nz for further details.

### Theia Extension
Theia is a fork of VS code and designed to have high extensibility. SwiftLaTeX implements an extension for Theia, which mainly provides live previewer and WYSIWYG editing functionalities. The live previewer takes the engine output and converts it into HTML for display. 

### S3 Storage.
SwiftLaTeX stores users files in self-hosting Minio server or public Amazon S3 cloud. To do that, we rewrite the filesystem subsystem of Theia. https://github.com/SwiftLaTeX/IDE/blob/master/packages/filesystem/src/browser/s3storagesystem.ts. Optionally, we also provide a backend service to sync the files to different cloud storage like Google, Dropbox, and Github. 

### Realtime Collaboration
Todo

### Language Server
Todo
 
## License

SwiftLaTeX is released under AGPL3 license. 
