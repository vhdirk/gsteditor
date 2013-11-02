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

#include "utils.h"


Glib::ustring Gste::get_data_file (Glib::ustring filename)
{
  Glib::ustring path = "";

  /* looking for glade file in uninstalled dir */
  path = Glib::build_filename(GST_EDITOR_UNINSTALLED_DATA_DIR, filename);
  if (Glib::file_test(path, Glib::FILE_TEST_IS_REGULAR))
    return path;


  /* looking for glade file in installed dir */
  path = Glib::build_filename(GST_EDITOR_DATA_DIR, filename);
  if (Glib::file_test(path, Glib::FILE_TEST_IS_REGULAR))
    return path;

  /* not found */
  return path;
}

