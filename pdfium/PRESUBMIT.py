# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Presubmit script for pdfium.

See http://dev.chromium.org/developers/how-tos/depottools/presubmit-scripts
for more details about the presubmit API built into depot_tools.
"""

LINT_FILTERS = [
  # Rvalue ref checks are unreliable.
  '-build/c++11',
  # Need to fix header names not matching cpp names.
  '-build/include',
  # Need to fix header names not matching cpp names.
  '-build/include_order',
  # Too many to fix at the moment.
  '-readability/casting',
  # Need to refactor large methods to fix.
  '-readability/fn_size',
  # Need to fix errors when making methods explicit.
  '-runtime/explicit',
  # Lots of usage to fix first.
  '-runtime/int',
  # Need to fix two snprintf TODOs
  '-runtime/printf',
  # Lots of non-const references need to be fixed
  '-runtime/references',
  # We are not thread safe, so this will never pass.
  '-runtime/threadsafe_fn',
  # Figure out how to deal with #defines that git cl format creates.
  '-whitespace/indent',
]


def _CheckUnwantedDependencies(input_api, output_api):
  """Runs checkdeps on #include statements added in this
  change. Breaking - rules is an error, breaking ! rules is a
  warning.
  """
  import sys
  # We need to wait until we have an input_api object and use this
  # roundabout construct to import checkdeps because this file is
  # eval-ed and thus doesn't have __file__.
  original_sys_path = sys.path
  try:
    sys.path = sys.path + [input_api.os_path.join(
        input_api.PresubmitLocalPath(), 'buildtools', 'checkdeps')]
    import checkdeps
    from cpp_checker import CppChecker
    from rules import Rule
  except ImportError:
    return [output_api.PresubmitError(
        'Unable to run checkdeps, does pdfium/buildtools/checkdeps exist?')]
  finally:
    # Restore sys.path to what it was before.
    sys.path = original_sys_path

  added_includes = []
  for f in input_api.AffectedFiles():
    if not CppChecker.IsCppFile(f.LocalPath()):
      continue

    changed_lines = [line for line_num, line in f.ChangedContents()]
    added_includes.append([f.LocalPath(), changed_lines])

  deps_checker = checkdeps.DepsChecker(input_api.PresubmitLocalPath())

  error_descriptions = []
  warning_descriptions = []
  for path, rule_type, rule_description in deps_checker.CheckAddedCppIncludes(
      added_includes):
    description_with_path = '%s\n    %s' % (path, rule_description)
    if rule_type == Rule.DISALLOW:
      error_descriptions.append(description_with_path)
    else:
      warning_descriptions.append(description_with_path)

  results = []
  if error_descriptions:
    results.append(output_api.PresubmitError(
        'You added one or more #includes that violate checkdeps rules.',
        error_descriptions))
  if warning_descriptions:
    results.append(output_api.PresubmitPromptOrNotify(
        'You added one or more #includes of files that are temporarily\n'
        'allowed but being removed. Can you avoid introducing the\n'
        '#include? See relevant DEPS file(s) for details and contacts.',
        warning_descriptions))
  return results


def CheckChangeOnUpload(input_api, output_api):
  results = []
  results += _CheckUnwantedDependencies(input_api, output_api)
  results += input_api.canned_checks.CheckPatchFormatted(input_api, output_api)
  results += input_api.canned_checks.CheckChangeLintsClean(
      input_api, output_api, None, LINT_FILTERS)

  return results
