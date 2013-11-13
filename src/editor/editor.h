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

#ifndef __GSTE_EDITOR_H__
#define __GSTE_EDITOR_H__

#include <gstreamermm.h>
#include <gtkmm.h>

#include "editor.h"
#include "editorcanvas.h"
#include "element-browser/element-tree.h"

namespace Gste
{

class Editor : public Gtk::Window
{
public:
  Editor( /*Gst::Element & element */);


protected:

  void on_element_tree_select(Glib::RefPtr<Gst::ElementFactory> &factory);

private:

  Gste::ElementTree m_element_tree;
  Gste::EditorCanvas m_canvas;

  Gtk::SpinButton * m_spin_width;
  Gtk::SpinButton * m_spin_height;



};







}


//#define GST_TYPE_EDITOR (gst_editor_get_type())
//#define GST_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_EDITOR, GstEditor))
//#define GST_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_EDITOR, GstEditorClass))
//#define GST_IS_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_EDITOR))
//#define GST_IS_EDITOR_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_EDITOR))
//#define GST_EDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_EDITOR, GstEditorClass))


//#define GST_EDITOR_SET_OBJECT(item,object) (g_object_set_data (G_OBJECT (item), "gsteditorobject", (object)))
//#define GST_EDITOR_GET_OBJECT(item) (g_object_get_data (G_OBJECT (item), "gsteditorobject"))


//struct _GstEditor
//{
//  GObject object;

//  GladeXML *xml;
//  GtkWidget *window;
//  GtkWidget *element_tree;
//  GtkSpinButton *sw,*sh;
  

//  gchar *filename;
//  gboolean changed;
//  gboolean need_name;

//  GstEditorCanvas *canvas;

//  GData *attributes;//one global attributes location all other point to
//  GMutex *outputmutex;//only used for time measurement outputs
//};

//struct _GstEditorClass
//{
//  GObjectClass parent_class;
//};

//GType gst_editor_get_type (void);
//GtkWidget *gst_editor_new (GstElement * element);
//void gst_editor_load (GstEditor * editor, const gchar * file_name);
//void gst_editor_on_spinbutton(GtkSpinButton * spinheight, GstEditor * editor);

#endif /* __GSTE_EDITOR_H__ */
