SwiftLaTeX XeTeX Engine (WIP)

## Introduction
The TeX engine used by SwiftLaTeX. Usually you do not have to build the WebAssembly binary by yourself, because we will ship a pre-built binary in each release.

## Build Instructions 
Install Emscripen emsdk (>1.39.0) and type make.

We ship two prebuilt libraries harfbuzz and graphite2 under the prebuilt directory.
This binaries are generated using latest LLVM backend used by emsdk > 1.39.0.
They are not compatibile with fastcomp backend used by emsdk < 1.39.0
Please double check your emsdk compiler version before building. 

We are working to put the harfbuzz and graphite sources in this directory so that the build system won't make a big fuss about your emsdk version. But it is in a less priority so far.