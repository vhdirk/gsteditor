# - Try to find GooCanvasmm
# Once done, this will define
#
#  GOOCANVASMM_FOUND - system has GooCanvasmm
#  GOOCANVASMM_LIBRARIES - link these to use GooCanvasmm
#  GOOCANVASMM_INCLUDE_DIRS - the GooCanvasmm include directories
#  GOOCANVASMM_LIBRARY_DIRS - the GooCanvasmm library directories
#
# Note: this does not work on Windows
#
# Copyright (c) 2013, Dirk Van Haerenborgh
#


include(LibFindMacros)


if(NOT DEFINED GooCanvasmm_FIND_VERSION)
    set(GooCanvasmm_FIND_VERSION "2.0")
endif(NOT DEFINED GooCanvasmm_FIND_VERSION)


find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH} "${GooCanvasmm_ROOT}/lib/pkgconfig:${GooCanvasmm_ROOT}/lib64/pkgconfig")
pkg_check_modules(GOOCANVASMM goocanvasmm-${GooCanvasmm_FIND_VERSION_MAJOR}.${GooCanvasmm_FIND_VERSION_MINOR})

