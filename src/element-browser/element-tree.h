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

#ifndef __GST_ELEMENT_BROWSER_ELEMENT_TREE_H__
#define __GST_ELEMENT_BROWSER_ELEMENT_TREE_H__

#include <gtkmm.h>
#include <gstreamermm.h>


namespace Gste
{

struct ElementClassTreeNode
{
  typedef std::vector<ElementClassTreeNode>::iterator ClassIter;
  typedef std::vector<Glib::RefPtr<Gst::ElementFactory> >::iterator FactIter;

  Glib::ustring                                   name;
  std::vector<ElementClassTreeNode>               subclasses;
  std::vector<Glib::RefPtr<Gst::ElementFactory> > factories;
};



class ElementTree : public Gtk::VBox
{
public: //constructors
  ElementTree();


public: //signals
  typedef sigc::signal<void, Glib::RefPtr<Gst::ElementFactory>& > type_signal_element_selected;
  typedef sigc::signal<void, Glib::RefPtr<Gst::ElementFactory>& > type_signal_element_activated;

  type_signal_element_selected  signal_element_selected();
  type_signal_element_activated signal_element_activated();

public: //static functions
  static std::vector<ElementClassTreeNode> & get_class_tree();

protected:
  void populate_store(Gtk::TreeModel::Children parent, ElementClassTreeNode &node);

  void tree_activate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
  void tree_select();

  void filter_text_changed();
  void filter_elements();

  void set_tree_model(Glib::RefPtr<Gtk::TreeModel> model);
  void set_tree_selection_by_factory (Glib::RefPtr<Gst::ElementFactory> &factory);

  bool set_select_by_factory_subtree (Gtk::TreeNodeChildren::iterator childiter, Glib::RefPtr<Gst::ElementFactory> &factory);

  void build_filter_by_tree();

  void build_filter_by_existing_tree();

  void filter_subtree(Gtk::TreeNodeChildren::iterator childiter);


protected:
  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns() {
      this->add(name);
      this->add(factory);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gst::ElementFactory> > factory;
  };

  ModelColumns m_columns;


protected:
  type_signal_element_selected  m_signal_element_selected;
  type_signal_element_activated m_signal_element_activated;


private:
  Glib::RefPtr<Gtk::TreeStore>      m_model;
  Glib::RefPtr<Gtk::ListStore>      m_filter_model;
  Glib::RefPtr<Gtk::TreeModel>      m_curr_model;
  Glib::RefPtr<Gst::ElementFactory> m_selected;

  Glib::ustring   m_current_filter_text;
  Glib::ustring   m_old_filter_text;

  Glib::RefPtr<Gtk::Builder>   m_builder;
  // the following are owned by m_builder.
  Gtk::VBox *     m_palette;
  Gtk::TreeView * m_view;
  Gtk::Entry*     m_filter_entry;
};

} //namespace Gste
#endif /* __GST_ELEMENT_BROWSER_ELEMENT_TREE_H__ */
