/* Common utility functions
 *
 * Copyright (C) <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
 *
 * This file is part of GstEditor.
 *
 * GstEditor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GstEditor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GSTE_UTILS_H__
#define __GSTE_UTILS_H__

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "config.h"
#include <gstreamermm.h>
#include <gtkmm.h>


namespace Gste
{


/* given the name of the glade file, return the newly allocated full path to it
 * if it exists and NULL if not. */
Glib::ustring get_data_file (Glib::ustring filename);


}

#endif // __GSTE_UTILS_H__
