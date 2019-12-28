// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/utils/path_service.h"

#ifdef _WIN32
#include <Windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else  // Linux
#include <linux/limits.h>
#include <unistd.h>
#endif  // _WIN32

#include <string>

#include "core/fxcrt/include/fx_system.h"

// static
bool PathService::EndsWithSeparator(const std::string& path) {
  return path.size() > 1 && path[path.size() - 1] == PATH_SEPARATOR;
}

// static
bool PathService::GetExecutableDir(std::string* path) {
// Get the current executable file path.
#ifdef _WIN32
  char path_buffer[MAX_PATH];
  path_buffer[0] = 0;

  if (GetModuleFileNameA(NULL, path_buffer, MAX_PATH) == 0)
    return false;
  *path = std::string(path_buffer);
#elif defined(__APPLE__)
  ASSERT(path);
  unsigned int path_length = 0;
  _NSGetExecutablePath(NULL, &path_length);
  if (path_length == 0)
    return false;

  path->reserve(path_length);
  path->resize(path_length - 1);
  if (_NSGetExecutablePath(&((*path)[0]), &path_length))
    return false;
#else   // Linux
  static const char kProcSelfExe[] = "/proc/self/exe";
  char buf[PATH_MAX];
  ssize_t count = ::readlink(kProcSelfExe, buf, PATH_MAX);
  if (count <= 0)
    return false;

  *path = std::string(buf, count);
#endif  // _WIN32

  // Get the directory path.
  std::size_t pos = path->size() - 1;
  if (EndsWithSeparator(*path))
    pos--;
  std::size_t found = path->find_last_of(PATH_SEPARATOR, pos);
  if (found == std::string::npos)
    return false;
  path->resize(found);
  return true;
}

// static
bool PathService::GetSourceDir(std::string* path) {
  if (!GetExecutableDir(path))
    return false;

  if (!EndsWithSeparator(*path))
    path->push_back(PATH_SEPARATOR);
  path->append("..");
  path->push_back(PATH_SEPARATOR);
  path->append("..");
  return true;
}

// static
bool PathService::GetTestDataDir(std::string* path) {
  if (!GetSourceDir(path))
    return false;

  if (!EndsWithSeparator(*path))
    path->push_back(PATH_SEPARATOR);
  path->append("testing");
  path->push_back(PATH_SEPARATOR);
  path->append("resources");
  return true;
}

// static
bool PathService::GetTestFilePath(const std::string& file_name,
                                  std::string* path) {
  if (!GetTestDataDir(path))
    return false;

  if (!EndsWithSeparator(*path))
    path->push_back(PATH_SEPARATOR);
  path->append(file_name);
  return true;
}
