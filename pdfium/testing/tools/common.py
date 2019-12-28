#!/usr/bin/env python
# Copyright 2015 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import glob
import os
import subprocess
import sys

def os_name():
  if sys.platform.startswith('linux'):
    return 'linux'
  if sys.platform.startswith('win'):
    return 'win'
  if sys.platform.startswith('darwin'):
    return 'mac'
  raise Exception('Confused, can not determine OS, aborting.')


def RunCommand(cmd):
  try:
    subprocess.check_call(cmd)
    return None
  except subprocess.CalledProcessError as e:
    return e

# Adjust Dr. Memory wrapper to have separate log directory for each test
# for better error reporting.
def DrMemoryWrapper(wrapper, pdf_name):
  if not wrapper:
    return []
  # convert string to list
  cmd_to_run = wrapper.split()

  # Do nothing if using default log directory.
  if cmd_to_run.count("-logdir") == 0:
    return cmd_to_run
  # Usually, we pass "-logdir" "foo\bar\spam path" args to Dr. Memory.
  # To group reports per test, we want to put the reports for each test into a
  # separate directory. This code can be simplified when we have
  # https://github.com/DynamoRIO/drmemory/issues/684 fixed.
  logdir_idx = cmd_to_run.index("-logdir")
  old_logdir = cmd_to_run[logdir_idx + 1]
  wrapper_pid = str(os.getpid())

  # We are using the same pid of the same python process, so append the number
  # of entries in the logdir at the end of wrapper_pid to avoid conflict.
  wrapper_pid += "_%d" % len(glob.glob(old_logdir + "\\*"))

  cmd_to_run[logdir_idx + 1] += "\\testcase.%s.logs" % wrapper_pid
  os.makedirs(cmd_to_run[logdir_idx + 1])

  f = open(old_logdir + "\\testcase.%s.name" % wrapper_pid, "w")
  print >>f, pdf_name + ".pdf"
  f.close()

  return cmd_to_run


class DirectoryFinder:
  '''A class for finding directories and paths under either a standalone
  checkout or a chromium checkout of PDFium.'''

  def __init__(self, build_location):
    # |build_location| is typically "out/Debug" or "out/Release".
    # Expect |my_dir| to be .../pdfium/testing/tools.
    self.my_dir = os.path.dirname(os.path.realpath(__file__))
    self.testing_dir = os.path.dirname(self.my_dir)
    if (os.path.basename(self.my_dir) != 'tools' or
        os.path.basename(self.testing_dir) != 'testing'):
      raise Exception('Confused, can not find pdfium root directory, aborting.')
    self.pdfium_dir = os.path.dirname(self.testing_dir)
    # Find path to build directory.  This depends on whether this is a
    # standalone build vs. a build as part of a chromium checkout. For
    # standalone, we expect a path like .../pdfium/out/Debug, but for
    # chromium, we expect a path like .../src/out/Debug two levels
    # higher (to skip over the third_party/pdfium path component under
    # which chromium sticks pdfium).
    self.base_dir = self.pdfium_dir
    one_up_dir = os.path.dirname(self.base_dir)
    two_up_dir = os.path.dirname(one_up_dir)
    if (os.path.basename(two_up_dir) == 'src' and
        os.path.basename(one_up_dir) == 'third_party'):
      self.base_dir = two_up_dir
    self.build_dir = os.path.join(self.base_dir, build_location)
    self.os_name = os_name()

  def ExecutablePath(self, name):
    '''Finds compiled binaries under the build path.'''
    result = os.path.join(self.build_dir, name)
    if self.os_name == 'win':
      result = result + '.exe'
    return result

  def ScriptPath(self, name):
    '''Finds other scripts in the same directory as this one.'''
    return os.path.join(self.my_dir, name)

  def WorkingDir(self, other_components=''):
    '''Places generated files under the build directory, not source dir.'''
    result = os.path.join(self.build_dir, 'gen', 'pdfium')
    if other_components:
      result = os.path.join(result, other_components)
    return result

  def TestingDir(self, other_components=''):
    '''Finds test files somewhere under the testing directory.'''
    result = self.testing_dir
    if other_components:
      result = os.path.join(result, other_components)
    return result
