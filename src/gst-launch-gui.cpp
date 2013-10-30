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


//GtkWidget *start_but, *pause_but, *parse_but, *status;
//GtkWidget *window;
//GtkWidget *element_ui;
//GtkWidget *prop_box, *dparam_box;
//GstElement *pipeline;
//GtkTreeStore *store = NULL;
//GtkTreeView *view = NULL;

//typedef void (*found_handler) (GstElement * element, gint xid, void *priv);

//static gint
//quit_live (GtkWidget * window, GdkEventAny * e, gpointer data)
//{
//  gtk_main_quit ();
//  return FALSE;
//}

//gboolean
//idle_func (gpointer data)
//{
//  //return gst_bin_iterate (GST_BIN (pipeline));
//  return TRUE;
//}

//void
//load_history (GtkWidget * pipe_combo)
//{

//  gchar *history_filename, *history_string, **split, **walk;
//  gint num_entries = 0, entries_limit = 50;
//  GList *history = NULL;
//  FILE *history_file;

//  history_filename =
//      g_strdup_printf ("%s/.gstreamer-guilaunch.history", g_get_home_dir ());

//  if (!g_file_get_contents (history_filename, &history_string, NULL, NULL)) {
//    history_string = g_strdup ("");
//  }

//  split = g_strsplit (history_string, "\n", 0);

//  /* go to the end of the list and append entries from there */
//  walk = split;
//  while (*walk)
//    walk++;
//  while (--walk >= split && num_entries++ < entries_limit)
//    history = g_list_append (history, *walk);

//  gtk_combo_set_popdown_strings (GTK_COMBO (pipe_combo), history);

//  history_file = fopen (history_filename, "a");
//  if (history_file == NULL) {
//    perror ("couldn't open history file");
//  }
//  g_object_set_data (G_OBJECT (pipe_combo), "history", history);
//  g_object_set_data (G_OBJECT (pipe_combo), "history_file", history_file);
//}

//void
//build_debug_page (GtkWidget * notebook)
//{
//#ifndef _MSC_VER
//  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), gst_debug_ui_new (),
//      gtk_label_new ("Debug"));

//#endif
//}

//void
//handle_have_size (GstElement * element, int width, int height)
//{
//  g_print ("setting window size\n");
//  gtk_widget_set_usize (GTK_WIDGET (g_object_get_data (G_OBJECT (element),
//              "gtk_socket")), width, height);
//  gtk_widget_show_all (GTK_WIDGET (g_object_get_data (G_OBJECT (element),
//              "vid_window")));
//}

//void
//xid_handler (GstElement * element, gint xid, void *priv)
//{
//  GtkWidget *gtk_socket, *vid_window;

//  g_print ("handling xid %d\n", xid);
//  vid_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

//  gtk_socket = gtk_socket_new ();
//  gtk_widget_show (gtk_socket);

//  gtk_container_add (GTK_CONTAINER (vid_window), gtk_socket);

//  gtk_widget_realize (gtk_socket);
//  gtk_socket_steal (GTK_SOCKET (gtk_socket), xid);

//  gtk_object_set (GTK_OBJECT (vid_window), "allow_grow", TRUE, NULL);
//  gtk_object_set (GTK_OBJECT (vid_window), "allow_shrink", TRUE, NULL);

//  g_signal_connect (G_OBJECT (element), "have_size",
//      G_CALLBACK (handle_have_size), element);
//  g_object_set_data (G_OBJECT (element), "vid_window", vid_window);
//  g_object_set_data (G_OBJECT (element), "gtk_socket", gtk_socket);
//}





//int main (int argc, char *argv[])
//{
//  Gtk::Main kit(argc, argv);
//  Gst::init(argc, argv);




//  /* dparam_box = gtk_vbox_new(FALSE, 0); */

//  notebook = gtk_notebook_new ();

//  page_scroll = gtk_scrolled_window_new (NULL, NULL);
//  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (page_scroll),
//      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
//  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (page_scroll),
//      prop_box);
//  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page_scroll,
//      gtk_label_new ("Properties"));

///*
//  page_scroll = gtk_scrolled_window_new (NULL, NULL);
//  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (page_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
//  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (page_scroll), dparam_box);
//  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page_scroll, gtk_label_new ("Dynamic Params"));
//*/
//#ifndef _MSC_VER
//      build_debug_page (notebook);
//#endif
//  page_scroll = gtk_scrolled_window_new (NULL, NULL);
//  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (page_scroll),
//      GTK_WIDGET (view));
//  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (page_scroll),
//      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

//  pane = gtk_hpaned_new ();
//  gtk_paned_pack1 (GTK_PANED (pane), page_scroll, TRUE, TRUE);
//  gtk_paned_pack2 (GTK_PANED (pane), notebook, TRUE, TRUE);

//  gtk_box_pack_start (GTK_BOX (vbox), pane, TRUE, TRUE, 0);

//  status = gtk_label_new ("stopped");
//  gtk_box_pack_start (GTK_BOX (vbox), status, FALSE, FALSE, 0);

//  gtk_widget_show_all (window);

//  element_ui = g_object_new (gst_element_ui_get_type (), "view-mode",
//      GST_ELEMENT_UI_VIEW_MODE_FULL, NULL);
//  gtk_box_pack_start (GTK_BOX (prop_box), element_ui, FALSE, FALSE, 0);

//  gtk_widget_show (element_ui);

//  if (argc) {
//    argvn = g_new0 (char *, argc);
//    memcpy (argvn, argv + 1, sizeof (char *) * (argc - 1));
//    joined = g_strjoinv (" ", argvn);
//    g_object_set (GTK_COMBO (pipe_combo)->entry, "text", joined, NULL);
//    parse_callback (parse_but, pipe_combo);
//  }

//  gtk_main ();

//  return 0;
//}

//TODO: this!
//struct MyApp : Gtk::Application {
//    MyApp() : Gtk::Application("my.app",
//      Gio::APPLICATION_HANDLES_COMMAND_LINE) {}
//    int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd) {
//        // parse arguments:
//        Glib::OptionContext ctx;
//        Glib::OptionGroup group("options", "main options");
//        bool show_gui = false;
//        Glib::OptionEntry entry;
//        entry.set_long_name("gui");
//        entry.set_description("show the gui.");
//        group.add_entry(entry, show_gui);
//        ctx.add_group(group);
//        // add GTK options, --help-gtk, etc
//        Glib::OptionGroup gtkgroup(gtk_get_option_group(true));
//        ctx.add_group(gtkgroup);
//        int argc;
//        char **argv = cmd->get_arguments(argc);
//        ctx.parse(argc, argv);
//        // maybe show the gui
//        if(show_gui)
//            activate();
//        return 0;
//    }
//    Gtk::ApplicationWindow *main;
//    void on_activate() {
//        // can't use Gtk::manage, so we have to keep
//        // the reference or the main loop quits.
//        main = new Gtk::ApplicationWindow();
//        add_window(*main);
//        main->show();
//    }
//};
//int main(int argc, char **argv) {
//    return MyApp().run(argc, argv);
//}

int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Gst::init(argc, argv);

  Glib::OptionContext ctx(PACKAGE);

  Glib::OptionGroup gstoptiongroup = Gst::get_option_group();
  ctx.add_group(gstoptiongroup);

  Gste::LaunchGUI launchgui(argc, argv);

  kit.run();

  return 0;
}
