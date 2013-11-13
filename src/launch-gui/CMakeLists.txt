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

set(launch_gui_HEADERS launch-gui.h )
                   

set(launch_gui_SOURCES launch-gui.cpp )


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


source_group("src" FILES ${launch_gui_SOURCES})
source_group("include" FILES ${launch_gui_HEADERS})


add_source_group(${launch_gui_SOURCES})

