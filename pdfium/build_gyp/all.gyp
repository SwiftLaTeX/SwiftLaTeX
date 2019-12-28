# Copyright 2014 PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        '../pdfium.gyp:*',
        '../samples/samples.gyp:*',
        '../testing/libfuzzer/fuzzers.gyp:*',
      ],
    }
  ]
}
