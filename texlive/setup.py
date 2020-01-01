from distutils.core import setup, Extension

hello_module = Extension('pykpathsea', sources = ['pykpathsea.c'], libraries=["kpathsea"])

setup(name='pykpathsea',
      version='0.1.0',
      description='Kpathsea',
      ext_modules=[hello_module])