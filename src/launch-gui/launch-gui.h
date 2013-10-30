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

#ifndef __GSTE_LAUNCH_GUI_H__
#define __GSTE_LAUNCH_GUI_H__

#include <gstreamermm.h>
#include <gtkmm.h>

#include "element-ui/element-ui.h"

namespace Gste
{

class LaunchGUI : public Gtk::Window
{
public:
  LaunchGUI(int argc, char *argv[]);

private: //slots
  void quit_live();
  void on_start();
  void on_pause();
  void on_parse();
  void on_selection_changed();

  /**
   * @par Slot Prototype:
   * <tt>void on_my_%child_added(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name)</tt>
   *
   * Will be emitted after the @a object was added to the @a child_proxy.
   * @param object The Object that was added.
   * @param name The name of the new child.
   */

  void on_child_added(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name, Gtk::TreeIter iter);

  /**
   * @par Slot Prototype:
   * <tt>void on_my_%child_removed(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name)</tt>
   *
   * Will be emitted after the @a object was removed from the @a child_proxy.
   * @param object The Object that was removed.
   * @param name The name of the old child.
   */
  void on_child_removed(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name, Gtk::TreeIter iter);


  void build_tree (Gtk::TreeModel::Children parent, Glib::RefPtr<Gst::ChildProxy> &childproxy);
  void add_child_to_tree (Gtk::TreeModel::Children parent, const Glib::RefPtr<Gst::Object> &gstobject);

  void watch_childproxies();
  void build_debug_page(Gtk::Notebook &notebook);

protected:
  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns() {
      this->add(name);
      this->add(element);
      this->add(conn_child_added);
      this->add(conn_child_removed);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gst::Object> > element;
    Gtk::TreeModelColumn<sigc::connection> conn_child_added;
    Gtk::TreeModelColumn<sigc::connection> conn_child_removed;
  };

  ModelColumns m_columns;

protected:
  Glib::RefPtr<Gtk::TreeStore>  m_store;
  Gtk::TreeView     m_view;

  Gtk::ComboBoxText m_pipe_combo;
  Gtk::Statusbar    m_status;

  Gtk::Button       m_parse_but;
  Gtk::ToggleButton m_start_but;
  Gtk::ToggleButton m_pause_but;

  Glib::RefPtr<Gst::Element>  m_pipeline;

  ElementUI m_element_ui;
};

} //namespace Gste

#endif // __GSTE_LAUNCH_GUI_H__
