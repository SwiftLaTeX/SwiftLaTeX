#!/usr/bin/env python

from re import findall
from os import path
from io import open
try:
    from skbuild import setup
except ImportError:
    from setuptools import setup

here = path.abspath(path.dirname(__file__))
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()
with open(path.join(here, 'include/graphite2/Font.h')) as f:
    c_header = f.read()
major, minor, bug = findall('define GR2_VERSION_[A-X]+\s+(\d+)', c_header)
version = major + "." + minor + "." + bug

setup(
    name             = 'graphite2',
    version          = version,
    description      = 'SIL graphite2 smart font system python bindings',
    author           = 'SIL International',
    license          = 'LGPL-2.1+ OR MPL-1.1+',
    url              = 'https://github.com/silnrsi/graphite',
    zip_safe         = False,
    package_dir      = {'': 'python'},
    packages         = ['graphite2'],
    install_requires = ['future'],
    long_description = long_description,
    long_description_content_type = 'text/markdown',
    classifiers = [
        'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
        'License :: OSI Approved :: Mozilla Public License 1.1 (MPL 1.1)',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3'
    ]
)
