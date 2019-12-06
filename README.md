# SwiftLaTeX

A Browser-based Fast LaTeX Visual Editor. 

# Key features:
1. WYSIWYG
2. Fast compilation thanks to LaTeX checkpointing
3. Cloud file storage

Try it here: https://www.swiftlatex.com/oauth/login_oauth?type=sandbox


## Short Introduction

SwiftLaTeX is a Web-browser based editor to create PDF documents such as reports, term projects, slide decks, in the typesetting system LaTeX. In contrast to other web-based editors SwiftLaTeX is true WYSIWYG, What-you-see-is-what-you-get: You edit directly in a representation of the print output. You can import a LaTeX document at any stage of completeness into SwiftLaTeX. You can start a new document with SwiftLaTeX, or you can use SwiftLaTeX for final copy-editing. For advanced operation you can edit the so-called LaTeX source code, which gives you more fine-grained control. SwiftLaTeX is collaborative; you can share your project with others and work on it at the same time. SwiftLaTeX stores your data in the cloud under your account; currently it supports Google Drive and DropBox.

## Installation
You are welcome to host SwiftLaTeX by yourself according to AGPL licence. And you can also use our web https://www.swiftlatex.com.
### Run SwiftLaTeX using Docker in 3 Steps. (We will release the docker image in docker hub soon)
1. Install Docker
2. Run "docker build . -t swiftlatex/swiftlatex"
3. Run "docker-compose up"

### Run SwiftLaTeX using Python3 in 3 Steps

1. Install Python3 && Pip3
2. Run "pip3 install -r requirements.txt"
3. Run "python3 wsgi.py"

Open url "https://localhost:3000", and enjoy writing.

### Adding Google Drive and Dropbox Support
1. You first need to be a Google API Developer to retrieve a Google API Client ID and Secret. See here (https://developers.google.com/identity/protocols/OAuth2)
2. Edit config.py and put your Client ID and Secret Inside. (You can use environment variables instead.)

## FAQ

### About LaTeX packages
All packages are dynamically loaded from our file server and cached locally. Our file server has almost all the packages. 
If you want to host the file server by yourself, you can checkout another repo: https://github.com/elliott-wen/texlive-server

### About Engines
Currently, this engine is built atop pdftex. So no unicode supported. 
We are working to port xetex in future release.
The engine source code is hosted in https://github.com/SwiftLaTeX/PdfTeXLite.
It is unusable so far as we need more time to upload and tidy up the source codes. Stay tuned.


## Roadmap

### Bug Fixes
1. WYSIWYG
Formulas are absolute positioned, therefore, the correct display only comes after a compilation. Reductant spaces occurs between words.
2. Slow Upload to Google
Our system abstracts your cloud storage as a POSIX-like file system to simplify user interface implementation at the cost of a little bit performance. We are working hard to improve our implementation to reduce the network turn around time.
3. Sharing Files only works on Google
4. Checkpointing breaks certain projects.

### Pending Features
1. Vertical Spilt View
2. Adding XeTeX support. Clean up and Release Engine Source Codes. 
3. Tidy-up all the JS files.
4. Add Github and S3 storage support.

## Participate
As an open source project SwiftLaTeX strongly benefits from an active community. It is a good idea to announce your plans on the issue list. So everybody knows what's going on and there is no duplicate work.

### Spread the word
The easiest way to help on the development with SwiftLaTeX is to use it! Furthermore, if you like SwiftLaTeX, tell all your friends and colleagues about it.

### Bug Reports
User feedback is highly welcome. If you wanna report bugs regarding some TeX documentations not compiling. Please attach the snippets so that we can look into it.


### Contributon and Copyright
If you are sending PR requests, you own the copyright of your contribution, and that you must agree to give us a license to use it in both the open source version, and the version of SwiftLaTeX running at www.swiftlatex.com, which may include additional changes. For more details, you can see https://www.swiftlatex.com/contribute.html. 

## Research Paper
If you are interested in reading tech jargons, you could have a look at https://dl.acm.org/citation.cfm?id=3209522&dl=ACM&coll=DL
(Though some stuff in the paper is outdated.)
