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

#include <glibmm.h>
#include <gtkmm.h>
#include <gstreamermm.h>

#include "element-browser/browser.h"
#include "editor/editor.h"

int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Gst::init(argc, argv);

  Glib::OptionContext ctx(PACKAGE);

  Glib::OptionGroup gstoptiongroup = Gst::get_option_group();
  ctx.add_group(gstoptiongroup);

//  Gste::Editor editor;

  kit.run();
  //brow.show_all();

  //int v = brow.pick_modal();


  //chosen = (GstElementFactory *) gst_element_browser_pick_modal ();
  //  if (chosen)
  //    g_print ("selected '%s'\n", GST_OBJECT_NAME (chosen));
  //  else
  //    g_print ("didn't choose any\n");
  return 0;
}









//int
//main (int argc, char *argv[])
//{

//GstEditor * editor;

//#if 0
//      GnomeProgram * p;

//#endif  /*
// */
//      gboolean launch = FALSE;

//const gchar **remaining_args = NULL;



//GOptionEntry options[] = {

// {
//    "launch", 'l', 0, G_OPTION_ARG_NONE, &launch,

//"Create pipeline from gst-launch(1) syntax", NULL},
//        /* last but not least a special option that collects filenames or
//           gst-launch arguments */
//    {
//    G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY,
//&remaining_args,

//"Special option that collects any remaining arguments for us", NULL},

// {
//  NULL}
//};

//GOptionContext * ctx;

//GError * err = NULL;


//#ifdef ENABLE_NLS
//      bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);

//bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

//textdomain (GETTEXT_PACKAGE);

//#endif  /*
// */

//if (!g_thread_supported ())
//    g_thread_init (NULL);

//g_type_init ();

//glade_init ();


//ctx = g_option_context_new (PACKAGE);

//g_option_context_add_main_entries (ctx, options, GETTEXT_PACKAGE);

//g_option_context_add_group (ctx, gst_init_get_option_group ());

//g_option_context_add_group (ctx, gtk_get_option_group (TRUE));


//if (!g_option_context_parse (ctx, &argc, &argv, &err)) {

//g_print ("Error initializing: %s\n", err->message);

//exit (1);

//}


//#if 0
//      if (!(p =
//          gnome_program_init (PACKAGE, VERSION,
//LIBGNOMEUI_MODULE,
//argc, argv,

//GNOME_PARAM_GOPTION_CONTEXT, ctx,
//GNOME_PARAM_APP_DATADIR,
//              DATADIR,
//"app-datadir", GST_EDITOR_DATA_DIR, NULL)))

//g_error ("gnome_progam_init() failed, aborting...");

//#endif  /*
// */

//gste_init ();


//if (remaining_args != NULL)

// {

//if (launch)

// {

//GError * error = NULL;

//GstElement * element;

//GstBin * bin;


//element = gst_parse_launchv (remaining_args, &error);

//if (!element)

// {

//g_print ("Error: %s\n", error->message);

//exit (1);

//}

//bin = GST_BIN (element);

//editor = (GstEditor *) gst_editor_new (GST_ELEMENT (bin));

//}

//    else

// {

//while (*remaining_args)

// {

//editor = (GstEditor *) gst_editor_new (NULL);

//gst_editor_load (editor, *remaining_args++);

//}

//}

//}

//  else

// {

//gst_editor_new (gst_element_factory_make ("pipeline", NULL));

//}


//gtk_main ();


//exit (0);

//}



//#if defined(_MSC_VER) && defined(NDEBUG)
//#include <windows.h>
//    int WINAPI
//WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
//LPSTR lpCmdLine,
//    int nCmdShow)
//{

//main (__argc, __argv);

//}
//#endif  /* */
