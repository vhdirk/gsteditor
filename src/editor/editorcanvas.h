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


#ifndef __GSTE_EDITOR_CANVAS_H__
#define __GSTE_EDITOR_CANVAS_H__

#include <gtkmm.h>
#include <goocanvasmm.h>
#include <gstreamermm.h>

#include "editorproperty.h"

namespace Gste
{

class EditorCanvas : public Goocanvas::Canvas
{
public:
  EditorCanvas(Gtk::Window &parent);
  virtual ~EditorCanvas(){}

public:
  void set_properties_visible(bool visible);

protected:


private:
  bool m_autosize;

  Glib::StaticRWLock m_globallock;
  EditorProperty     m_property_window;

};

}


//#include <gtk/gtk.h>
//#include <goocanvas.h>
//#include <gst/editor/gsteditorbin.h>
//#include "gsteditor.h"

//#define GST_TYPE_EDITOR_CANVAS (gst_editor_canvas_get_type())
//#define GST_EDITOR_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_EDITOR_CANVAS, GstEditorCanvas))
//#define GST_EDITOR_CANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_EDITOR_CANVAS, GstEditorCanvasClass))
//#define GST_IS_EDITOR_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_EDITOR_CANVAS))
//#define GST_IS_EDITOR_CANVAS_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_EDITOR_CANVAS))
//#define GST_EDITOR_CANVAS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_EDITOR_CANVAS, GstEditorCanvasClass))


//struct _GstEditorCanvas
//{
//  GooCanvas canvas;
//  GstEditor* parent;
//  GstEditorBin *bin;
//  GstEditorElement *selection;
//  GtkWidget *property_window;
//  GtkWidget *palette;
//  gchar *status;
//  GData **attributes;		/* list of name -> GstEditorItemAttr mappings */
//  GStaticRWLock globallock;
//  gboolean autosize;
//  gdouble widthbackup,heightbackup;
//};

//struct _GstEditorCanvasClass
//{
//   GooCanvasClass parent_class;
//};


//GType gst_editor_canvas_get_type (void);


#endif /* __GSTE_EDITOR_CANVAS_H__ */
