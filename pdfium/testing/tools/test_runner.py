#!/usr/bin/env python
# Copyright 2016 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import cStringIO
import functools
import multiprocessing
import optparse
import os
import re
import shutil
import subprocess
import sys

import common
import pngdiffer
import suppressor

class KeyboardInterruptError(Exception): pass

# Nomenclature:
#   x_root - "x"
#   x_filename - "x.ext"
#   x_path - "path/to/a/b/c/x.ext"
#   c_dir - "path/to/a/b/c"

def TestOneFileParallel(this, test_case):
  """Wrapper to call GenerateAndTest() and redirect output to stdout."""
  try:
    input_filename, source_dir = test_case
    result = this.GenerateAndTest(input_filename, source_dir);
    return (result, input_filename, source_dir)
  except KeyboardInterrupt:
    raise KeyboardInterruptError()


class TestRunner:
  def __init__(self, dirname):
    self.test_dir = dirname

  def GenerateAndTest(self, input_filename, source_dir):
    input_root, _ = os.path.splitext(input_filename)
    expected_txt_path = os.path.join(source_dir, input_root + '_expected.txt')

    pdf_path = os.path.join(self.working_dir, input_root + '.pdf')

    # Remove any existing generated images from previous runs.
    actual_images = self.image_differ.GetActualFiles(input_filename, source_dir,
                                                     self.working_dir)
    for image in actual_images:
      if os.path.exists(image):
        os.remove(image)

    sys.stdout.flush()

    raised_exception = self.Generate(source_dir, input_filename, input_root,
                                     pdf_path)

    if raised_exception != None:
      print "FAILURE: " + input_filename + "; " + str(raised_exception)
      return False

    if os.path.exists(expected_txt_path):
      raised_exception = self.TestText(input_root, expected_txt_path, pdf_path)
    else:
      raised_exception = self.TestPixel(input_root, pdf_path)

    if raised_exception != None:
      print "FAILURE: " + input_filename + "; " + str(raised_exception)
      return False

    if len(actual_images):
      if self.image_differ.HasDifferences(input_filename, source_dir,
                                          self.working_dir):
        return False

    return True

  def Generate(self, source_dir, input_filename, input_root, pdf_path):
    original_path = os.path.join(source_dir, input_filename)
    input_path = os.path.join(source_dir, input_root + '.in')

    input_event_path = os.path.join(source_dir, input_root + ".evt")
    if os.path.exists(input_event_path):
      output_event_path = os.path.splitext(pdf_path)[0] + ".evt"
      shutil.copyfile(input_event_path, output_event_path)

    if not os.path.exists(input_path):
      if os.path.exists(original_path):
        shutil.copyfile(original_path, pdf_path)
      return None

    sys.stdout.flush()

    return common.RunCommand(
        [sys.executable, self.fixup_path, '--output-dir=' + self.working_dir,
            input_path])


  def TestText(self, input_root, expected_txt_path, pdf_path):
    txt_path = os.path.join(self.working_dir, input_root + '.txt')

    with open(txt_path, 'w') as outfile:
      # add Dr. Memory wrapper if exist
      cmd_to_run = common.DrMemoryWrapper(self.drmem_wrapper, input_root)
      cmd_to_run.extend([self.pdfium_test_path, pdf_path])
      subprocess.check_call(cmd_to_run, stdout=outfile)

    cmd = [sys.executable, self.text_diff_path, expected_txt_path, txt_path]
    return common.RunCommand(cmd)


  def TestPixel(self, input_root, pdf_path):
    cmd_to_run = common.DrMemoryWrapper(self.drmem_wrapper, input_root)
    cmd_to_run.extend([self.pdfium_test_path, '--send-events', '--png',
                       pdf_path])
    return common.RunCommand(cmd_to_run)


  def HandleResult(self, input_filename, input_path, result):
    if self.test_suppressor.IsResultSuppressed(input_filename):
      if result:
        self.surprises.append(input_path)
    else:
      if not result:
        self.failures.append(input_path)


  def Run(self):
    parser = optparse.OptionParser()
    parser.add_option('--build-dir', default=os.path.join('out', 'Debug'),
                      help='relative path from the base source directory')
    parser.add_option('-j', default=multiprocessing.cpu_count(),
                      dest='num_workers', type='int',
                      help='run NUM_WORKERS jobs in parallel')
    parser.add_option('--wrapper', default='', dest="wrapper",
                      help='wrapper for running test under Dr. Memory')
    options, args = parser.parse_args()

    finder = common.DirectoryFinder(options.build_dir)
    self.fixup_path = finder.ScriptPath('fixup_pdf_template.py')
    self.text_diff_path = finder.ScriptPath('text_diff.py')

    self.drmem_wrapper = options.wrapper

    self.source_dir = finder.TestingDir()
    if self.test_dir != 'corpus':
      test_dir = finder.TestingDir(os.path.join('resources', self.test_dir))
    else:
      test_dir = finder.TestingDir(self.test_dir)

    self.pdfium_test_path = finder.ExecutablePath('pdfium_test')
    if not os.path.exists(self.pdfium_test_path):
      print "FAILURE: Can't find test executable '%s'" % self.pdfium_test_path
      print "Use --build-dir to specify its location."
      return 1

    self.working_dir = finder.WorkingDir(os.path.join('testing', self.test_dir))
    if not os.path.exists(self.working_dir):
      os.makedirs(self.working_dir)

    self.feature_string = subprocess.check_output([self.pdfium_test_path,
                                                   '--show-config'])
    self.test_suppressor = suppressor.Suppressor(finder, self.feature_string)
    self.image_differ = pngdiffer.PNGDiffer(finder)

    walk_from_dir = finder.TestingDir(test_dir);

    test_cases = []
    input_file_re = re.compile('^[a-zA-Z0-9_.]+[.](in|pdf)$')
    if len(args):
      for file_name in args:
        file_name.replace(".pdf", ".in")
        input_path = os.path.join(walk_from_dir, file_name)
        if not os.path.isfile(input_path):
          print "Can't find test file '%s'" % file_name
          return 1

        test_cases.append((os.path.basename(input_path),
                           os.path.dirname(input_path)))
    else:
      for file_dir, _, filename_list in os.walk(walk_from_dir):
        for input_filename in filename_list:
          if input_file_re.match(input_filename):
            input_path = os.path.join(file_dir, input_filename)
            if not self.test_suppressor.IsExecutionSuppressed(input_path):
              if os.path.isfile(input_path):
                test_cases.append((input_filename, file_dir))

    self.failures = []
    self.surprises = []

    if options.num_workers > 1 and len(test_cases) > 1:
      try:
        pool = multiprocessing.Pool(options.num_workers)
        worker_func = functools.partial(TestOneFileParallel, self)

        worker_results = pool.imap(worker_func, test_cases)
        for worker_result in worker_results:
          result, input_filename, source_dir = worker_result
          input_path = os.path.join(source_dir, input_filename)

          self.HandleResult(input_filename, input_path, result)

      except KeyboardInterrupt:
        pool.terminate()
      finally:
        pool.close()
        pool.join()
    else:
      for test_case in test_cases:
        input_filename, input_file_dir = test_case
        result = self.GenerateAndTest(input_filename, input_file_dir)
        self.HandleResult(input_filename,
                          os.path.join(input_file_dir, input_filename), result)

    if self.surprises:
      self.surprises.sort()
      print '\n\nUnexpected Successes:'
      for surprise in self.surprises:
        print surprise;

    if self.failures:
      self.failures.sort()
      print '\n\nSummary of Failures:'
      for failure in self.failures:
        print failure
      return 1

    return 0
