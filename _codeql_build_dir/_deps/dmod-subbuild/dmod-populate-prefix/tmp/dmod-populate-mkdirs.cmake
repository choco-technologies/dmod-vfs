# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-src")
  file(MAKE_DIRECTORY "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-src")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-build"
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix"
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/tmp"
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/src/dmod-populate-stamp"
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/src"
  "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/src/dmod-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/src/dmod-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/dmvfs/dmvfs/_codeql_build_dir/_deps/dmod-subbuild/dmod-populate-prefix/src/dmod-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
