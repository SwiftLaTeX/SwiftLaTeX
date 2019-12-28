@echo off
:: Copyright (c) 2011 The Chromium Authors. All rights reserved.
:: Use of this source code is governed by a BSD-style license that can be
:: found in the LICENSE file.

set THISDIR=%~dp0
set TOOL_NAME="drmemory_full"

:: Set up DRMEMORY_COMMAND to invoke Dr. Memory {{{1
set DRMEMORY_PATH=%THISDIR%..
set DRMEMORY_SFX=%DRMEMORY_PATH%\drmemory-windows-sfx.exe
if EXIST %DRMEMORY_SFX% GOTO DRMEMORY_BINARY_OK
echo "Can't find Dr. Memory executables."
echo "See http://www.chromium.org/developers/how-tos/using-valgrind/dr-memory"
echo "for the instructions on how to get them."
exit /B 1

:DRMEMORY_BINARY_OK
%DRMEMORY_SFX% -o%DRMEMORY_PATH%\unpacked -y
set DRMEMORY_COMMAND=%DRMEMORY_PATH%\unpacked\bin\drmemory.exe
:: }}}

:RUN_TESTS
python %THISDIR%/pdfium_tests.py %*
