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


#ifndef __GSTE_EDITOR_PROPERTY_H__
#define __GSTE_EDITOR_PROPERTY_H__

#include <glibmm.h>
#include <gtkmm.h>

#include "element-ui/element-ui.h"
#include "element-browser/caps-tree.h"

namespace Gste{

class EditorProperty : public Gtk::Window
{
public:
  EditorProperty(Gtk::Window & parent);

  void set_parent_window(Gtk::Window & window);
private:
  ElementUI        m_element_ui;
  ElementCapsTree  m_element_caps_tree;

};



}


//#include <gst/gst.h>
//#include <glade/glade.h>
//#include <gst/editor/editor.h>

//#define GST_TYPE_EDITOR_PROPERTY (gst_editor_property_get_type())
//#define GST_EDITOR_PROPERTY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_EDITOR_PROPERTY, GstEditorProperty))
//#define GST_EDITOR_PROPERTY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_EDITOR_PROPERTY, GstEditorPropertyClass))
//#define GST_IS_EDITOR_PROPERTY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_EDITOR_PROPERTY))
//#define GST_IS_EDITOR_PROPERTY_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_EDITOR_PROPERTY))
//#define GST_EDITOR_PROPERTY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_EDITOR_PROPERTY, GstEditorPropertyClass))

//#define GST_EDITOR_PROPERTY_SET_OBJECT(item,object) \
//  (g_object_set_data (G_OBJECT(item), "gsteditorproperty", (object)))
//#define GST_EDITOR_PROPERTY_GET_OBJECT(item) \
//  (g_object_get_data (G_OBJECT(item),"gsteditorproperty"))

//typedef struct _GstEditorProperty GstEditorProperty;
//typedef struct _GstEditorPropertyClass GstEditorPropertyClass;

//struct _GstEditorProperty
//{
//  GObject object;

//  GstElement *shown_element;

//  GladeXML *xml;
//  GtkWidget *window;
//  GtkWidget *element_ui;
//  GtkWidget *caps_browser;
//};

//struct _GstEditorPropertyClass
//{
//  GObjectClass parent_class;

//  void (*element_selected) (GstEditorProperty * property,
//      GstEditorElement * element);
//  void (*in_selection_mode) (GstEditorProperty * property,
//      GstEditorElement * element);
//};

//GType gst_editor_property_get_type ();
//GstEditorProperty *gst_editor_property_get ();

//void gst_editor_property_show (GstEditorProperty * property,
//    GstEditorElement * element);


#endif /* __GSTE_EDITOR_PROPERTY_H__ */
