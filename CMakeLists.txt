#------------------------------------------------------------------------
# Copyright (C) <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
# This file is part of GstEditor.
#
# GstEditor is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# GstEditor is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#------------------------------------------------------------------------


cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
#------------------------------------------------------------------------
# project name
#------------------------------------------------------------------------
project(GstEditor)

#------------------------------------------------------------------------
# build defenitions (on top for fast access)
#------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE INTERNAL
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)


#------------------------------------------------------------------------
# library dependencies
#------------------------------------------------------------------------
## cmake module path (make sure to pull the cmake subrepository)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)


find_package(Glibmm 2.4 REQUIRED)
add_definitions(${GLIBMM_DEFINITIONS})

# GStreamer
find_package(GStreamer 1.0 REQUIRED)
add_definitions(${GSTREAMER_DEFINITIONS})


# GTKmm
set(GTKmm_ROOT "/opt/gnome")
find_package(GTKmm 3.0 REQUIRED)

# GStreamermm
set(GStreamermm_ROOT "/opt/gnome")
find_package(GStreamermm 1.0 REQUIRED)
add_definitions(${GSTREAMERMM_DEFINITIONS})


#------------------------------------------------------------------------
# include & link directories
#------------------------------------------------------------------------

## from library dependencies
include_directories(${GSTREAMER_INCLUDE_DIRS}
		            ${GSTREAMERMM_INCLUDE_DIRS}
		            ${GTKMM_INCLUDE_DIRS}
                ${GLIBMM_INCLUDE_DIRS}
)

link_directories( ${GSTREAMER_LIBRARY_DIRS}
                  ${GSTREAMERMM_LIBRARY_DIRS}
                  ${GTKMM_LIBRARY_DIRS}
                  ${GLIBMM_LIBRARY_DIRS}
)


## from this project
include_directories(
    "${PROJECT_SOURCE_DIR}/libs"   
    "${PROJECT_SOURCE_DIR}/include"   
    "${PROJECT_SOURCE_DIR}/src"       
)


#------------------------------------------------------------------------
# macros
#------------------------------------------------------------------------

# A macro to append to the global source property
set_property(GLOBAL PROPERTY gsteditor_global_SRC "")

macro(add_gsteditor_source sources)
  foreach(_SRC ${ARGV})
    get_filename_component(_ABS_SRC ${_SRC} ABSOLUTE)
    set_property(GLOBAL APPEND PROPERTY gsteditor_global_SRC ${_ABS_SRC})
  endforeach()
  unset(_SRC)
  unset(_ABS_SRC)
endmacro()




#------------------------------------------------------------------------
# build
#------------------------------------------------------------------------
#set(HEADERS )

set(PACKAGE "gst-editor")
set(UNINSTALLED_DATA_DIR "${PROJECT_SOURCE_DIR}/data")
set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h)
add_definitions("-D HAVE_CONFIG_H")

add_subdirectory(src/common)
add_subdirectory(src/element-browser)
add_subdirectory(src/debug-ui)
add_subdirectory(src/element-ui)
add_subdirectory(src/editor)
add_subdirectory(src/launch-gui)


get_property(gsteditor_global_SRC GLOBAL PROPERTY gsteditor_global_SRC)



add_executable(gst-inspect-gui src/gst-inspect-gui.cpp
                               ${gsteditor_global_SRC}
)
target_link_libraries(gst-inspect-gui ${GSTREAMER_LIBRARIES}
                                      ${GSTREAMERMM_LIBRARIES}
                                      ${GTKMM_LIBRARIES}
                                      ${GLIBMM_LIBRARIES}
)

add_executable(gst-launch-gui src/gst-launch-gui.cpp
                               ${gsteditor_global_SRC}
)
target_link_libraries(gst-launch-gui ${GSTREAMER_LIBRARIES}
                                      ${GSTREAMERMM_LIBRARIES}
                                      ${GTKMM_LIBRARIES}
                                      ${GLIBMM_LIBRARIES}
)

add_executable(gst-editor src/editor.cpp
                          ${gsteditor_global_SRC}
)

target_link_libraries(gst-editor ${GSTREAMER_LIBRARIES}
                                 ${GSTREAMERMM_LIBRARIES}
                                 ${GTKMM_LIBRARIES}
                                 ${GLIBMM_LIBRARIES}
)



