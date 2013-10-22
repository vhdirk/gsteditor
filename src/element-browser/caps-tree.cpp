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

#include "caps-tree.h"


template <typename T>
Glib::ustring to_string(T value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

using namespace Gste;


CapsTree::CapsTree()
  : Gtk::ScrolledWindow(),
    m_view()
{
  //Create the Tree model:
  m_store = Gtk::TreeStore::create(m_columns);
  m_view.set_model(m_store);

  m_view.append_column("Name", m_columns.name);
  m_view.append_column("Info", m_columns.info);

  this->add(m_view);
  this->show();
}

Glib::ustring CapsTree::field_value_to_string(Glib::ValueBase & value_base)
{
  Glib::ustring typestr;
  Glib::ustring valuestr;
  field_value_to_string(value_base, valuestr, typestr);

  std::stringstream ss;
  ss << "(" << typestr << ") " << valuestr;
  return ss.str();
}



void CapsTree::field_value_to_string(Glib::ValueBase & value_base, Glib::ustring & valuestr, Glib::ustring & typestr)
{
  //Note: I dislike the amount of C that is exposed here, though there currently seems
  //      no way around that.
  GType value_type = G_VALUE_TYPE(value_base.gobj());
  //Note: reinterpret_cast is used, since the compiler seems to think Glib::Value<>
  //      and Glib::ValueBase are not polymorphic, meaning that dynamic_cast does not work.
  if (value_type == G_TYPE_INT)
  {
    Glib::Value<int> value = *reinterpret_cast<Glib::Value<int>*>(&value_base);
    valuestr = to_string(value.get());
    typestr = "int";
  }
  else if (value_type == G_TYPE_DOUBLE)
  {
    Glib::Value<double> value = *reinterpret_cast<Glib::Value<double>*>(&value_base);
    valuestr = to_string(value.get());
    typestr = "double";
  }
  else if (value_type == G_TYPE_BOOLEAN)
  {
    Glib::Value<bool> value = *reinterpret_cast<Glib::Value<bool>*>(&value_base);
    valuestr = (value.get() ?  "TRUE" : "FALSE");
    typestr = "bool";
  }
  else if (value_type == G_TYPE_STRING)
  {
    Glib::Value<Glib::ustring> value = *reinterpret_cast<Glib::Value<Glib::ustring>* >(&value_base);
    valuestr = to_string(value.get());
    typestr = "string";
  }
  else if (value_type == GST_TYPE_FRACTION)
  {
    Glib::Value<Gst::Fraction> value = *reinterpret_cast<Glib::Value<Gst::Fraction>*>(&value_base);
    std::stringstream ss;
    ss << value.get().num << "/" << value.get().denom;
    valuestr = ss.str();
    typestr = "fraction";
  }
  else if (value_type == GST_TYPE_INT_RANGE)
  {
    Glib::Value<Gst::IntRange> value = *reinterpret_cast<Glib::Value<Gst::IntRange>*>(&value_base);
    std::stringstream ss;
    ss << "[" << value.get().min << " - " << value.get().max << "]";
    valuestr = ss.str();
    typestr = "int";
  }
  else if (value_type == GST_TYPE_DOUBLE_RANGE)
  {
    Glib::Value<Gst::DoubleRange> value = *reinterpret_cast<Glib::Value<Gst::DoubleRange>*>(&value_base);
    std::stringstream ss;
    ss << "[" << value.get().min << " - " << value.get().max << "]";
    valuestr = ss.str();
    typestr = "double";
  }
  else if (value_type == GST_TYPE_FRACTION_RANGE)
  {
    Glib::Value<Gst::FractionRange> value = *reinterpret_cast<Glib::Value<Gst::FractionRange>*>(&value_base);
    std::stringstream ss;
    ss << "[" << value.get().min.num <<"/"<< value.get().min.denom;
    ss << " - " << value.get().max.num <<"/"<< value.get().max.denom << "]";
    valuestr = ss.str();
    typestr = "fraction";
  }
  else if (value_type == GST_TYPE_LIST)
  {
    Gst::ValueList valuelist = *reinterpret_cast<Gst::ValueList*>(&value_base);

    Glib::ustring listvalstr;

    std::stringstream ss;
    ss << "{";
    for (int i = 0; i < valuelist.size(); i++)
    {
      Glib::ValueBase listval;
      valuelist.get(i, listval);

      //it is expected that all values in the list have the same type
      field_value_to_string(listval, listvalstr, typestr);

      ss << listvalstr;
      if (i+1 < valuelist.size())
        ss << ", ";
    }
    ss << "}";
    valuestr = ss.str();
  }
  else{
    std::stringstream ss;
    ss << "unknown caps field type " << g_type_name (value_type);
    valuestr = ss.str();
  }
}



void CapsTree::add_caps_to_tree (Glib::RefPtr<Gst::Caps> caps, Gtk::TreeModel::Row row)
{

  Gtk::TreeModel::Children parent = row.children();

  for (unsigned int i = 0; i < caps->size(); i++)
  {
    Gst::Structure structure = caps->get_structure(i);

    if(!structure) continue;

    Glib::ustring mime = structure.get_name();

    Gtk::TreeModel::Row nrow = *(m_store->append(parent));
    nrow[m_columns.name] = mime;

    Gtk::TreeModel::Children nparent = nrow.children();
    for(int i = 0; i < structure.size(); i++)
    {
      //convert field value to string
      Glib::ustring name = structure.get_nth_field_name(i);

      Glib::ValueBase value_base;
      structure.get_field(name, value_base);

      Glib::ustring info = field_value_to_string(value_base);

      Gtk::TreeModel::Row row = *(this->m_store->append(nparent));
      row[m_columns.name] = name;
      row[m_columns.info] = info;
    }
  }

}


ElementCapsTree::ElementCapsTree()
  : CapsTree()
{

}


void ElementCapsTree::set_element(Glib::RefPtr<Gst::Element> element)
{
  this->m_element = element;
  this->update_caps_tree();
}




void ElementCapsTree::update_caps_tree()
{
  this->m_store->clear();

  if(!m_element) return;

  typedef Gst::Iterator<Gst::Pad> PadIter;

  PadIter iter = m_element->iterate_pads();

  //iter.begin();
  while(iter.next())
  {
    //TODO
    Glib::RefPtr<Gst::Caps> anycaps = Gst::Caps::create_any();
    Glib::RefPtr<Gst::Caps> caps = iter->query_caps(anycaps);

    if (!caps){
      iter++;
      continue;
    }

    Gtk::TreeModel::Row row = *(m_store->append());

    /* Add a sub-tree of caps entries */
    this->add_caps_to_tree(caps, row);

    //TODO
    row[m_columns.name] = iter->get_name();

    switch(iter->get_direction())
    {
    case Gst::PAD_SRC:
      row[m_columns.info] = "Source";
      break;
    case Gst::PAD_SINK:
      row[m_columns.info] = "Sink";
      break;
    default:
      row[m_columns.info] = "Unknown pad direction";
      break;
    }
    Gtk::TreeModel::Path path = this->m_store->get_path(row);
    this->m_view.expand_row(path, false);

    iter++;
  }

}



ElementFactoryCapsTree::ElementFactoryCapsTree()
  : CapsTree()
{

}


void ElementFactoryCapsTree::set_element_factory(Glib::RefPtr<Gst::ElementFactory> factory)
{
  this->m_factory = factory;
  this->update_caps_tree();
}



void ElementFactoryCapsTree::update_caps_tree()
{
  this->m_store->clear();

  if(!m_factory) return;

  std::vector<Gst::StaticPadTemplate> padtemplates = m_factory->get_static_pad_templates();

  for(std::vector<Gst::StaticPadTemplate>::iterator iter = padtemplates.begin();
      iter != padtemplates.end(); ++iter )
  {
    Glib::RefPtr<Gst::PadTemplate> padtempl = iter->get();
    Glib::RefPtr<Gst::Caps> caps = padtempl->get_caps();

    Gtk::TreeModel::Row row = *(m_store->append());

    /* Add a sub-tree of caps entries */
    this->add_caps_to_tree(caps, row);

    row[m_columns.name] = padtempl->get_name_template();

    switch(padtempl->get_direction())
    {
    case Gst::PAD_SRC:
      row[m_columns.info] = "Source";
      break;
    case Gst::PAD_SINK:
      row[m_columns.info] = "Sink";
      break;
    default:
      row[m_columns.info] = "Unknown pad direction";
      break;
    }
    Gtk::TreeModel::Path path = this->m_store->get_path(row);
    this->m_view.expand_row(path, false);

  }
}
