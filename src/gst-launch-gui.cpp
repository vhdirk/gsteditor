/* Copyright (C) <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <glibmm.h>
#include <gtkmm.h>
#include <gstreamermm.h>

#include "launch-gui/launch-gui.h"

namespace Gste
{

class LaunchGUIApp : public Gtk::Application
{
public:
  LaunchGUIApp()
    : Gtk::Application("org.gste.gst-launch-gui",
                       Gio::APPLICATION_HANDLES_COMMAND_LINE)
  {}

  int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd)
  {
    Glib::OptionContext ctx(PACKAGE);

    // add local options
//    Glib::OptionGroup group("options", "main options");
//    bool show_gui = false;
//    Glib::OptionEntry entry;
//    entry.set_long_name("gui");
//    entry.set_description("show the gui.");
//    group.add_entry(entry, show_gui);
//    ctx.add_group(group);

    // add GTK options, --help-gtk, etc
    Glib::OptionGroup gtkgroup(gtk_get_option_group(true));
    ctx.add_group(gtkgroup);

    // add GStreamer options
    Glib::OptionGroup gstoptiongroup = Gst::get_option_group();
    ctx.add_group(gstoptiongroup);

    int argc;
    char **argv = cmd->get_arguments(argc);

    ctx.parse(argc, argv);

    // show the gui
    main = new LaunchGUI(argc, argv);

    activate();
    return 0;
  }


protected:

  void on_activate()
  {
    add_window(*main);
    main->show();
  }

protected:
  LaunchGUI *main;
};

}



int main (int argc, char *argv[])
{
  // init Gstreamer. Has to be called first!
  Gst::init(argc, argv);

  return Gste::LaunchGUIApp().run(argc, argv);
}
