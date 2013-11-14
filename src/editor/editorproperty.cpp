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


#include <iostream>

#include <glibmm/i18n.h>

#include "editorproperty.h"
#include "common/utils.h"

using namespace Gste;


EditorProperty::EditorProperty(Gtk::Window &parent)
  : Gtk::Window(), m_element_ui(ELEMENT_UI_VIEW_MODE_FULL), m_element_caps_tree()
{
  std::string ui_path = Gste::get_data_file("editor.glade");
  Glib::RefPtr<Gtk::Builder> builder;
  try
  {
    builder = Gtk::Builder::create_from_file(ui_path, "property_window_notebook");
  }
  catch (const Glib::FileError & ex)
  {
    std::cerr << ex.what() << std::endl;
    return;
  }

  this->set_transient_for(parent);

  this->set_title(_("Properties: ..."));
  this->set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);
  this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);


  //autoconnecting signals cannot be done in gtkmm
  //  glade_xml_signal_autoconnect_full (property->xml,
  //      gst_editor_property_connect_func, &data);

  Gtk::Notebook * notebook;
  builder->get_widget("property_window_notebook", notebook);
  this->add(*notebook);
  notebook->show();

  Gtk::ScrolledWindow * scroll;

  m_element_ui.show();
  builder->get_widget("scrolledwindow-element-ui", scroll);
  scroll->add(m_element_ui);

  m_element_caps_tree.show();
  builder->get_widget("scrolledwindow-caps-browser", scroll);
  scroll->add(m_element_caps_tree);


  //  property->shown_element = NULL;

  //  g_signal_connect (property->window, "delete-event",
  //      G_CALLBACK (on_delete_event), property);

}



//#include <gtk/gtk.h>
//#include <gst/gst.h>
//#include "gsteditorproperty.h"
//#include "../common/gste-common.h"
//#include "../element-ui/gst-element-ui.h"
//#include "../element-browser/caps-tree.h"
//    typedef struct
//{
//  GstEditorProperty *property;
//  GModule *symbols;
//}
//connect_struct;

//enum
//{
//  PROP_0,
//  PROP_ELEMENT,
//  PROP_PARENT
//};

//enum
//{
//  LAST_SIGNAL
//};


///* class functions */
//static void gst_editor_property_class_init (GstEditorPropertyClass * klass);
//static void gst_editor_property_init (GstEditorProperty * property);

//static void gst_editor_property_set_property (GObject * object, guint prop_id,
//    const GValue * value, GParamSpec * pspec);
//static void gst_editor_property_get_property (GObject * object, guint prop_id,
//    GValue * value, GParamSpec * pspec);
//static void gst_editor_property_dispose (GObject * object);
//static void gst_editor_property_connect_func (const gchar * handler_name,
//    GObject * object,
//    const gchar * signal_name,
//    const gchar * signal_data,
//    GObject * connect_object, gboolean after, gpointer user_data);
//static gint on_delete_event (GtkWidget * widget, GdkEvent * event,
//    gpointer data);


//static GtkObjectClass *parent_class;

///* static guint gst_editor_property_signals[LAST_SIGNAL] = { 0 }; */


//GType
//gst_editor_property_get_type (void)
//{
//  static GType property_type = 0;

//  if (!property_type) {
//    static const GTypeInfo property_info = {
//      sizeof (GstEditorPropertyClass),
//      (GBaseInitFunc) NULL,
//      (GBaseFinalizeFunc) NULL,
//      (GClassInitFunc) gst_editor_property_class_init,
//      NULL,
//      NULL,
//      sizeof (GstEditorProperty),
//      0,
//      (GInstanceInitFunc) gst_editor_property_init,
//    };

//    property_type =
//        g_type_register_static (G_TYPE_OBJECT, "GstEditorProperty",
//        &property_info, 0);
//  }
//  return property_type;
//}

//static void
//gst_editor_property_class_init (GstEditorPropertyClass * klass)
//{
//  GObjectClass *object_class;

//  object_class = G_OBJECT_CLASS (klass);

//  parent_class = g_type_class_ref (G_TYPE_OBJECT);

//  object_class->set_property = gst_editor_property_set_property;
//  object_class->get_property = gst_editor_property_get_property;
//  object_class->dispose = gst_editor_property_dispose;

//  g_object_class_install_property (object_class, PROP_ELEMENT,
//      g_param_spec_object ("element", "Element", "Element",
//          gst_element_get_type (), G_PARAM_READWRITE));

//  g_object_class_install_property (object_class, PROP_PARENT,
//      g_param_spec_object ("parent", "Parent", "Parent window",
//          gtk_window_get_type (), G_PARAM_WRITABLE));
//}

//static void
//gst_editor_property_init (GstEditorProperty * property)
//{
//  connect_struct data;
//  GModule *symbols;
//  gchar *path;

//  symbols = g_module_open (NULL, 0);

//  data.property = property;
//  data.symbols = symbols;

//  path = gste_get_ui_file ("editor.glade2");
//  if (!path)
//    g_error ("GStreamer Editor user interface file 'editor.glade2' not found.");
//  property->xml = glade_xml_new (path, "property_window", NULL);

//  if (!property->xml) {
//    g_error ("GStreamer Editor could not load property_window from %s", path);
//  }
//  g_free (path);

//  glade_xml_signal_autoconnect_full (property->xml,
//      gst_editor_property_connect_func, &data);

//  property->window = glade_xml_get_widget (property->xml, "property_window");
//  gtk_widget_show (property->window);

//  property->element_ui =
//      g_object_new (gst_element_ui_get_type (), "view-mode",
//      GST_ELEMENT_UI_VIEW_MODE_FULL, NULL);
//  gtk_widget_show (property->element_ui);
//  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW
//      (glade_xml_get_widget (property->xml, "scrolledwindow-element-ui")),
//      property->element_ui);

//  property->caps_browser =
//      g_object_new (gst_element_browser_caps_tree_get_type (), NULL);
//  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW
//      (glade_xml_get_widget (property->xml, "scrolledwindow-caps-browser")),
//      property->caps_browser);

//  property->shown_element = NULL;

//  g_signal_connect (property->window, "delete-event",
//      G_CALLBACK (on_delete_event), property);
//}

///* we need more control here so... */
//static void
//gst_editor_property_connect_func (const gchar * handler_name,
//    GObject * object,
//    const gchar * signal_name,
//    const gchar * signal_data,
//    GObject * connect_object, gboolean after, gpointer user_data)
//{
//  GCallback func;
//  gpointer *funcp = NULL;
//  connect_struct *data = (connect_struct *) user_data;

//  if (!g_module_symbol (data->symbols, handler_name, funcp))
//    g_warning ("gsteditorproperty: could not find signal handler '%s'.",
//        handler_name);
//  else {
//    func = (GCallback) * funcp;
//    if (after)
//      g_signal_connect_after (object, signal_name, func,
//          (gpointer) data->property);
//    else
//      g_signal_connect (object, signal_name, func, (gpointer) data->property);
//  }
//}

//static void
//gst_editor_property_set_property (GObject * object, guint prop_id,
//    const GValue * value, GParamSpec * pspec)
//{
//  GstEditorProperty *property;
//  GstElement *old_element;
//  gchar *title;
//  GdkWindow *window;
//  GdkAtom atoms[2] = { GDK_NONE, GDK_NONE };

//  /* get the major types of this object */
//  property = GST_EDITOR_PROPERTY (object);
//  old_element = property->shown_element;

//  switch (prop_id) {
//    case PROP_ELEMENT:
//      property->shown_element = g_value_get_object (value);
//      if (property->shown_element != old_element) {
//        g_object_set (property->element_ui, "element", property->shown_element,
//            NULL);
//        g_object_set (property->caps_browser, "element",
//            property->shown_element, NULL);
//        if (property->shown_element)
//          title =
//              g_strdup_printf ("Properties: %s",
//              GST_OBJECT_NAME (property->shown_element));
//        else
//          title = g_strdup ("Properties: (none)");
//        g_object_set (property->window, "title", title, NULL);
//        g_free (title);
//      }
//      break;

//    case PROP_PARENT:
//      gtk_window_set_transient_for (GTK_WINDOW (property->window),
//          GTK_WINDOW (g_value_get_object (value)));

//      /* we are assumed to be realized at this point.. */
//      window = property->window->window;

//      atoms[0] = gdk_atom_intern ("_NET_WM_WINDOW_TYPE", FALSE);

//      gdk_property_change (window,
//          gdk_atom_intern ("_NET_WM_WINDOW_TYPE_UTILITY", FALSE),
//          gdk_atom_intern ("ATOM", FALSE),
//          32, GDK_PROP_MODE_REPLACE, (guchar *) atoms, 1);
//      break;

//    default:
//      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
//      break;
//  }
//}

//static void
//gst_editor_property_get_property (GObject * object, guint prop_id,
//    GValue * value, GParamSpec * pspec)
//{
//  GstEditorProperty *property;

//  /* get the major types of this object */
//  property = GST_EDITOR_PROPERTY (object);

//  switch (prop_id) {
//    case PROP_ELEMENT:
//      g_value_set_object (value, (GObject *) property->shown_element);
//      break;

//    default:
//      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
//      break;
//  }
//}

//static void
//gst_editor_property_dispose (GObject * object)
//{
//  GstEditorProperty *property = GST_EDITOR_PROPERTY (object);

//  gtk_widget_destroy (property->window);

//  if (G_OBJECT_CLASS (parent_class)->dispose)
//    G_OBJECT_CLASS (parent_class)->dispose (object);
//}

//static gint
//on_delete_event (GtkWidget * widget, GdkEvent * event, gpointer data)
//{
//  GstEditorProperty *property = GST_EDITOR_PROPERTY (data);

//  g_object_unref (G_OBJECT (property));

//  return FALSE;
//}
