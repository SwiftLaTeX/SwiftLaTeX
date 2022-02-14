#========================================================================
#
# splash/CMakeLists.txt
#
# CMake script for the splash library.
#
# Copyright 2015 Glyph & Cog, LLC
#
#========================================================================

if (HAVE_SPLASH)
  include_directories("${PROJECT_SOURCE_DIR}")
  include_directories("${PROJECT_BINARY_DIR}")
  include_directories("${PROJECT_SOURCE_DIR}/goo")
  include_directories("${PROJECT_SOURCE_DIR}/fofi")
  include_directories("${FREETYPE_INCLUDE_DIRS}")

  if (HAVE_DTYPE4_H)
    include_directories("${DTYPE_INCLUDE_DIR}")
    set(DTYPE_SRCS
        SplashDT4Font.cc SplashDT4FontEngine.cc SplashDT4FontFile.cc)
  endif ()

  add_library(splash_objs OBJECT
    Splash.cc
    SplashBitmap.cc
    SplashClip.cc
    SplashFTFont.cc
    SplashFTFontEngine.cc
    SplashFTFontFile.cc
    SplashFont.cc
    SplashFontEngine.cc
    SplashFontFile.cc
    SplashFontFileID.cc
    SplashPath.cc
    SplashPattern.cc
    SplashScreen.cc
    SplashState.cc
    SplashXPath.cc
    SplashXPathScanner.cc
    ${DTYPE_SRCS}
  )

  add_library(splash
    $<TARGET_OBJECTS:splash_objs>
  )
endif ()
