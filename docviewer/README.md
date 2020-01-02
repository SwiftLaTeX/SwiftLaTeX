
This viewer is a fork of dvi parser from Dr Jim Fowler. 

Several modifications are applied. 
1. We implement support for XDV.
2. We remove bakoma TTF fonts to avoid license issues.
3. Fix a bug in the function mergeText which leads to misalignment of texts.

# DVI Parser

A live demo is available at https://people.math.osu.edu/fowler.291/latex/

Built using https://github.com/FabriceSalvaire/PyDVI and the work of @tmanderson

The goal with this project is to eventually have a complete [DVI](https://en.wikipedia.org/wiki/Device_independent_file_format) toolset
written in node. Once complete, this will hopefully be used to implement
TeX document conversions completely in node (particularly a highly configurable
HTML/CSS/JS output).

I'll also be updating the Wiki for those that are interested. Currently, I there's
a [DVI Specification Explained](https://github.com/tmanderson/dvi-parser/wiki/DVI-Specification-Explained) section that I
put together for my own benefit (sourced from a few different online pages).



