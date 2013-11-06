/* Gstreamer Debug Viewer
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

#ifndef __GSTE_DEBUG_UI_H__
#define __GSTE_DEBUG_UI_H__

#include <gstreamermm.h>
#include <gtkmm.h>

namespace Gste
{

class DebugUI : public Gtk::Bin
{
public:
  DebugUI();


protected:
  void set_default_level();
  void set_custom_level();
  void init_custom_levels();
  void tree_select();
  void refresh_categories();
  void populate_add_categories();
  void show_add_window();
  bool find_cat_in_customlist(GstDebugCategory * cat);

  void add_custom_cats();
  void remove_custom_cats();


protected:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns() {
      this->add(level);
      this->add(name);
      this->add(description);
      this->add(category);
    }
    Gtk::TreeModelColumn<Glib::ustring> level;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> description;
    Gtk::TreeModelColumn< GstDebugCategory* > category;
  };

  ModelColumns m_columns;

private:
  Glib::RefPtr<Gtk::Adjustment> m_default_adjustment;
  Glib::RefPtr<Gtk::Adjustment> m_custom_adjustment;

  Glib::RefPtr<Gtk::ListStore> m_store;
  Glib::RefPtr<Gtk::ListStore> m_add_store;

  // managed by Gtk
  Gtk::TreeView * m_view;
  Gtk::HScale *   m_default_hscale;
  Gtk::Label *    m_default_label;
  Gtk::HScale *   m_custom_hscale;
  Gtk::Label *    m_custom_label;
  Gtk::Widget *   m_custom_box;
  Gtk::Window *   m_add_window;
  Gtk::TreeView * m_add_cats_treeview;
};


}

#endif // __GSTE_DEBUG_UI_H__
