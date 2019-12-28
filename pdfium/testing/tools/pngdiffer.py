#!/usr/bin/env python
# Copyright 2015 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import sys

import common

class PNGDiffer():
  ACTUAL_TEMPLATE = '.pdf.%d.png'
  EXPECTED_TEMPLATE = '_expected' + ACTUAL_TEMPLATE
  PLATFORM_EXPECTED_TEMPLATE = '_expected_%s' + ACTUAL_TEMPLATE

  def __init__(self, finder):
    self.pdfium_diff_path = finder.ExecutablePath('pdfium_diff')
    self.os_name = finder.os_name

  def GetActualFiles(self, input_filename, source_dir, working_dir):
    actual_paths = []
    template_paths = self._GetTemplatePaths(
        input_filename, source_dir, working_dir)
    actual_path_template = template_paths[0];
    expected_path_template = template_paths[1]
    platform_expected_path_template = template_paths[2]
    i = 0
    while True:
      actual_path = actual_path_template % i
      expected_path = expected_path_template % i
      platform_expected_path = (
          platform_expected_path_template % (self.os_name, i))
      if os.path.exists(platform_expected_path):
        expected_path = platform_expected_path
      elif not os.path.exists(expected_path):
        break
      actual_paths.append(actual_path)
      i += 1
    return actual_paths

  def HasDifferences(self, input_filename, source_dir, working_dir):
    template_paths = self._GetTemplatePaths(
        input_filename, source_dir, working_dir)
    actual_path_template = template_paths[0];
    expected_path_template = template_paths[1]
    platform_expected_path_template = template_paths[2]
    i = 0
    while True:
      actual_path = actual_path_template % i
      expected_path = expected_path_template % i
      # PDFium tests should be platform independent. Platform based results are
      # used to capture platform dependent implementations.
      platform_expected_path = (
          platform_expected_path_template % (self.os_name, i))
      if (not os.path.exists(expected_path) and
          not os.path.exists(platform_expected_path)):
        if i == 0:
          print "WARNING: no expected results files for " + input_filename
        break
      print "Checking " + actual_path
      sys.stdout.flush()
      if os.path.exists(expected_path):
        error = common.RunCommand(
            [self.pdfium_diff_path, expected_path, actual_path])
      else:
        error = 1;
      if error:
        # When failed, we check against platform based results.
        if os.path.exists(platform_expected_path):
          error = common.RunCommand(
              [self.pdfium_diff_path, platform_expected_path, actual_path])
        if error:
          print "FAILURE: " + input_filename + "; " + str(error)
          return True
      i += 1
    return False

  def _GetTemplatePaths(self, input_filename, source_dir, working_dir):
    input_root, _ = os.path.splitext(input_filename)
    actual_path = os.path.join(working_dir, input_root + self.ACTUAL_TEMPLATE)
    expected_path = os.path.join(
        source_dir, input_root + self.EXPECTED_TEMPLATE)
    platform_expected_path = os.path.join(
        source_dir, input_root + self.PLATFORM_EXPECTED_TEMPLATE)
    return (actual_path, expected_path, platform_expected_path)
