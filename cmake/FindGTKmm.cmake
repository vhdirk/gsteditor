# - Try to find GTKmm
# Once done, this will define
#
#  GTKMM_FOUND - system has GTKmm
#  GTKMM_LIBRARIES - link these to use GTKmm
#  GTKMM_INCLUDE_DIRS - the GTKmm include directories
#  GTKMM_LIBRARY_DIRS - the GTKmm library directories
#
# Note: this does not work on Windows
#
# Copyright (c) 2013, Dirk Van Haerenborgh
#


include(LibFindMacros)


if(NOT DEFINED GTKmm_FIND_VERSION)
    set(GTKmm_FIND_VERSION "0.10")
endif(NOT DEFINED GTKmm_FIND_VERSION)


find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH} "${GTKmm_ROOT}/lib/pkgconfig:${GTKmm_ROOT}/lib64/pkgconfig")
pkg_check_modules(GTKMM gtkmm-${GTKmm_FIND_VERSION_MAJOR}.${GTKmm_FIND_VERSION_MINOR})

