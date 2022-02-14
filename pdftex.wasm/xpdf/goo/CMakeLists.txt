#========================================================================
#
# goo/CMakeLists.txt
#
# CMake script for the goo library.
#
# Copyright 2015 Glyph & Cog, LLC
#
#========================================================================

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")

add_library(goo_objs OBJECT
  FixedPoint.cc
  GHash.cc
  GList.cc
  GString.cc
  gfile.cc
  gmem.cc
  gmempp.cc
  parseargs.c
)

add_library(goo
  $<TARGET_OBJECTS:goo_objs>
)
