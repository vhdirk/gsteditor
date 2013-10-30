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

#ifndef __GSTE_ELEMENT_BROWSER_CAPS_TREE_H__
#define __GSTE_ELEMENT_BROWSER_CAPS_TREE_H__

#include <gtkmm.h>
#include <gstreamermm.h>


namespace Gste{

/*!
 * \brief Provides a treeview filled with gstcaps information
 */
class CapsTree : public Gtk::ScrolledWindow
{
public:
  /*!
   * \brief Default constructor
   */
  CapsTree();

protected:

  void add_caps_to_tree (Glib::RefPtr<Gst::Caps> caps, Gtk::TreeModel::Row row);

  virtual void update_caps_tree() = 0;

  static Glib::ustring field_value_to_string(Glib::ValueBase & value_base);

  static void field_value_to_string(Glib::ValueBase & value_base, Glib::ustring &valuestr, Glib::ustring &typestr);

protected:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns() {
      this->add(name);
      this->add(info);
    }
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> info;
  };

  ModelColumns m_columns;

protected:
  Glib::RefPtr<Gtk::TreeStore>  m_store;
  Gtk::TreeView                 m_view;
};


class ElementCapsTree : public CapsTree
{
public:
  ElementCapsTree();

public:
  void set_element(Glib::RefPtr<Gst::Element> element);

protected:
  virtual void update_caps_tree();

protected:
  Glib::RefPtr<Gst::Element>        m_element;
};


class ElementFactoryCapsTree : public CapsTree
{
public:
  ElementFactoryCapsTree();

public:
  void set_element_factory(Glib::RefPtr<Gst::ElementFactory> factory);

protected:
  virtual void update_caps_tree();

protected:
  Glib::RefPtr<Gst::ElementFactory> m_factory;
};

} //namespace Gste

#endif // __GSTE_ELEMENT_BROWSER_CAPS_TREE_H__
