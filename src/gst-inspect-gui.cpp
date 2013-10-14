/* Copyright (C) <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
 * This file is part of GstEditor.
 *
 * GstEditor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//TODO: create config.h
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <glibmm.h>
#include <gtkmm.h>
#include <gstreamermm.h>

//#include "gst/editor/editor.h"
#include "element-browser/browser.h"
//#include "gst/debug-ui/debug-ui.h"

int
main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Gst::init(argc, argv);


  Glib::OptionContext ctx(PACKAGE);

  Glib::OptionGroup gstoptiongroup = Gst::get_option_group();
  ctx.add_group(gstoptiongroup);


  //Gst::ElementFactory chosen;



  Gste::ElementBrowser brow;
  //brow.show_all();

  Glib::RefPtr<Gst::ElementFactory> chosen = brow.pick_modal();

  if (chosen){
    std::cout << "selected '" << chosen->get_name() << "'" << std::endl;
  }else{
    std::cout << "didn't choose any" << std::endl;
  }

  return 0;
}
