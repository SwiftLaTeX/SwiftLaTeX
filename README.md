# SwiftLaTeX

A Browser-based Fast LaTeX Visual Editor. 

# Key features:
1. WYSIWYG
2. Fast compilation thanks to LaTeX checkpointing
3. Cloud file storage

Try it here: https://www.swiftlatex.com/oauth/login_oauth?type=sandbox


## Short Introduction

SwiftLaTeX is a Web-browser based editor to create PDF documents such as reports, term projects, slide decks, in the typesetting system LaTeX. In contrast to other web-based editors SwiftLaTeX is true WYSIWYG, What-you-see-is-what-you-get: You edit directly in a representation of the print output. You can import a LaTeX document at any stage of completeness into SwiftLaTeX. You can start a new document with SwiftLaTeX, or you can use SwiftLaTeX for final copy-editing. For advanced operation you can edit the so-called LaTeX source code, which gives you more fine-grained control. SwiftLaTeX is collaborative; you can share your project with others and work on it at the same time. SwiftLaTeX stores your data in the cloud under your account; currently it supports Google Drive and DropBox.

## Run SwiftLaTeX using Docker in 3 Steps.

1. Install Docker
2. Run "docker build . -t swiftlatex/swiftlatex"
3. Run "docker-compose up"


## Run SwiftLaTeX using Python3 in 3 Steps

1. Install Python3 && Pip3
2. Run "pip3 install -r requirements.txt"
3. Run "python3 wsgi.py"

## Adding Google Drive and Dropbox Support

1. You first need to be a Google API Developer to retrieve a Google API Client ID and Secret. See here (https://developers.google.com/identity/protocols/OAuth2)
2. Edit config.py and put your Client ID and Secret Inside. (You can use environment variables instead.)

## About LaTeX packages
All packages are dynamically loaded from our file server and cached locally. Our file server has almost all the packages. 
If you want to host the file server by yourself, you can checkout another repo: https://github.com/elliott-wen/texlive-server

## About Engines
Currently, this engines are built atop pdftex. So no unicode supported. 
We are working to port xetex in future release.



## Known bug lists

1. WYSIWYG

Formulas are absolute positioned, therefore, the correct display only comes after a compilation. Reductant spaces occurs between words.

2. Slow Upload to Google

Our system abstracts your cloud storage as a POSIX-like file system to simplify user interface implementation at the cost of a little bit performance. We are working hard to improve our implementation to reduce the network turn around time.

## Contributor License Agreement
Before we can accept any contributions of code, we need you to agree to our Contributor License Agreement https://www.swiftlatex.com/contribute.html. This is to ensure that you own the copyright of your contribution, and that you agree to give us a license to use it in both the open source version, and the version of SwiftLaTeX running at www.swiftlatex.com, which may include additional changes.

## Research Paper
If you are interested in tech jargons, you could have a look at https://dl.acm.org/citation.cfm?id=3209522&dl=ACM&coll=DL
(Though some stuff in the paper is outdated.)
