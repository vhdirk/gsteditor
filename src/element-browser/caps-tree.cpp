/* Copyright (C) <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
 * This file is part of GstEditor.
 *
 * GstEditor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "caps-tree.h"


using namespace Gste;


CapsTree::CapsTree()
  : Gtk::TreeView()
{
  //Create the Tree model:
  m_store = Gtk::TreeStore::create(m_columns);
  this->set_model(m_store);

  this->append_column("Name", m_columns.name);
  this->append_column("Info", m_columns.name);

  this->show();
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

    structure.foreach(sigc::mem_fun(*this, &CapsTree::add_field_to_tree));
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
  while(iter.next())
  {
    //TODO
    Glib::RefPtr<Gst::Caps> anycaps = Gst::Caps::create_any();
    Glib::RefPtr<Gst::Caps> caps = iter->query_caps(anycaps);

    if (!caps){
      ++iter;
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
    this->expand_row(path, false);

  ++iter;
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

  typedef Glib::ListHandle< Glib::RefPtr<Gst::PadTemplate> > PadTemplateList;

  PadTemplateList padtemplates = m_factory->get_static_pad_templates();


  for(PadTemplateList::iterator iter = padtemplates.begin();
      iter != padtemplates.end(); ++iter )
  {

      if (!*iter) continue;

      Glib::RefPtr<Gst::PadTemplate> padtempl = (*iter); //->get();
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
      this->expand_row(path, false);

  }
}



bool CapsTree::add_field_to_tree(const Glib::ustring& name, const Glib::ValueBase& value)
{
  std::cout << "foreach called" << std::endl;
}


//static gboolean
//add_field_to_tree (GQuark field_id, const GValue * value, gpointer userdata)
//{
//  CapsTreeTarget *target = (CapsTreeTarget *) userdata;
//  GtkTreeIter iter;

//  gtk_tree_store_append (target->store, &iter, target->parent);
//  gtk_tree_store_set (target->store, &iter,
//      NAME_COLUMN, g_quark_to_string (field_id),
//      INFO_COLUMN, print_value (value), -1);
//  return TRUE;
//}

///* this should really be a GtkTreeCellDataFunc */
//static gchar *
//print_value (const GValue * value)
//{
//  GType type = G_VALUE_TYPE (value);

//  switch (type) {
//    case G_TYPE_INT:
//    {
//      gint val;

//      val = g_value_get_int (value);
//      return g_strdup_printf ("Integer: %d", val);
//      break;
//    }
//    case G_TYPE_DOUBLE:
//    {
//      gdouble val;

//      val = g_value_get_double (value);
//      return g_strdup_printf ("Double: %g", val);
//      break;
//    }
//    case G_TYPE_BOOLEAN:
//    {
//      gboolean val;

//      val = g_value_get_boolean (value);
//      return g_strdup_printf ("Boolean: %s", val ? "TRUE" : "FALSE");
//      break;
//    }
//    case G_TYPE_STRING:
//    {
//      const gchar *val;

//      val = g_value_get_string (value);
//      return g_strdup_printf ("String: %s", val);
//      break;
//    }
//    default:
//      if (type == GST_TYPE_INT_RANGE) {
//        gint min, max;

//        min = gst_value_get_int_range_min (value);
//        max = gst_value_get_int_range_max (value);
//        return g_strdup_printf ("Integer range: %d - %d", min, max);
//      } else if (type == GST_TYPE_DOUBLE_RANGE) {
//        gdouble min, max;

//        min = gst_value_get_double_range_min (value);
//        max = gst_value_get_double_range_max (value);
//        return g_strdup_printf ("Double range: %g - %g", min, max);
//      } else if (type == GST_TYPE_FOURCC) {
//        guint32 val;

//        val = gst_value_get_fourcc (value);
//        return g_strdup_printf ("FourCC: '%c%c%c%c'",
//            (gchar) (val & 0xff),
//            (gchar) ((val >> 8) & 0xff),
//            (gchar) ((val >> 16) & 0xff), (gchar) ((val >> 24) & 0xff));
//      } else if (type == GST_TYPE_LIST) {
//        GArray *array;
//        GValue *list_value;
//        gchar *ret;
//        guint count = 0;

//        array = (GArray *) g_value_peek_pointer (value);
//        ret = g_strdup_printf ("List: ");
//        for (count = 0; count < array->len; count++) {
//          gchar *s;

//          list_value = &g_array_index (array, GValue, count);
//          s = print_value (list_value);
//          ret = g_strconcat (ret, count ? ", " : "", s, NULL);
//          g_free (s);
//        }
//        return ret;
//      }
//      return g_strdup_printf ("unknown caps field type %s", g_type_name (type));
//  }
//}
