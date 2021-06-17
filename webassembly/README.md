This is the engine used by SwiftLaTeX. It was based on XeTeX and patched to output HTMLs instead of PDFs. 

Also, it is using an in-built SyncTeX-like plugin. It maps each character in HTMLs to its source code location (including line and column). 
Conventional SyncTeX only allows for line-level accuracy. 

How to build

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

make
```
