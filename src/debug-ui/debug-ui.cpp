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

#include <iostream>

#include <gstreamermm.h>
#include <gtkmm.h>
#include <glibmm/i18n.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/utils.h"
#include "debug-ui.h"

using namespace Gste;


Glib::ustring debug_level_get_name(GstDebugLevel debug_level)
{
  //FIXME: how to disable the warning here?
  Glib::ustring debug_name = gst_debug_level_get_name(debug_level);

  if(!debug_name.empty())
    return debug_name;

  if(debug_level == 0)
    return "NONE";

  return "##";
}


DebugUI::DebugUI()
  : Gtk::Bin()
{
  std::string ui_path = Gste::get_data_file("editor.glade");
  Glib::RefPtr<Gtk::Builder> builder;
  try
  {
    builder = Gtk::Builder::create_from_file(ui_path, "debug_palette");
  }
  catch (const Glib::FileError & ex)
  {
    std::cerr << ex.what() << std::endl;
    return;
  }

  Gtk::Widget * box = 0;
  Gtk::Button *add_button = 0, *remove_button = 0, *refresh_button = 0;

  builder->get_widget("debug_palette", box);
  builder->get_widget("custom-levels-treeview", m_view);
  builder->get_widget("default-level-hscale", m_default_hscale);
  builder->get_widget("default-level-label", m_default_label);
  builder->get_widget("custom-level-hscale", m_custom_hscale);
  builder->get_widget("custom-level-label", m_custom_label);
  builder->get_widget("custom-box", m_custom_box);

  builder->get_widget("add-button", add_button);
  builder->get_widget("remove-button", remove_button);
  builder->get_widget("refresh-button", refresh_button);

  //bail out if a widget was not constructed
  if (! (box && add_button && remove_button && refresh_button &&
         m_view && m_default_hscale && m_default_label && m_custom_hscale
         && m_custom_label && m_custom_box) )
    return;

  m_store = Gtk::ListStore::create(m_columns);
  m_add_store = Gtk::ListStore::create(m_columns);

  m_view->set_model(m_store);

  m_default_adjustment = m_default_hscale->get_adjustment();
  m_default_adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &DebugUI::set_default_level));

  m_default_adjustment->clamp_page(0, GST_LEVEL_COUNT-1);
  m_default_adjustment->set_step_increment(1);
  m_default_adjustment->set_lower(0);
  m_default_adjustment->set_upper(GST_LEVEL_COUNT-1);

  //TODO: wrap gst debug?
  m_default_adjustment->set_value(gst_debug_get_default_threshold());

  m_custom_adjustment = m_custom_hscale->get_adjustment();
  m_custom_adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &DebugUI::set_custom_level));
  m_custom_adjustment->set_step_increment(1);
  m_custom_adjustment->set_lower(0);
  m_custom_adjustment->set_upper(GST_LEVEL_COUNT-1);


  m_view->append_column(_("Level"), m_columns.level);
  m_view->append_column(_("Name"), m_columns.name);
  m_view->append_column(_("Description"), m_columns.description);

  this->set_default_level();
  this->init_custom_levels();

  Glib::RefPtr<Gtk::TreeSelection> selection = m_view->get_selection();
  selection->set_mode(Gtk::SELECTION_MULTIPLE);
  selection->signal_changed().connect(sigc::mem_fun(*this, &DebugUI::tree_select));


  add_button->signal_clicked().connect(sigc::mem_fun(*this, &DebugUI::show_add_window));
  remove_button->signal_clicked().connect(sigc::mem_fun(*this, &DebugUI::remove_custom_cats));
  refresh_button->signal_clicked().connect(sigc::mem_fun(*this, &DebugUI::refresh_categories));

  this->add(*box);

}



void DebugUI::set_default_level()
{
  GstDebugLevel debug_level = static_cast<GstDebugLevel>(m_default_adjustment->get_value());

  gst_debug_set_default_threshold(debug_level);

  m_default_label->set_text(debug_level_get_name(debug_level));

  this->refresh_categories();
}



void DebugUI::set_custom_level()
{
  GstDebugLevel debug_level = static_cast<GstDebugLevel>(m_custom_adjustment->get_value());

  m_custom_label->set_text(debug_level_get_name(debug_level));

  /* Walk through selected items in the list and set the debug category */
  Glib::RefPtr<Gtk::TreeView::Selection> selection = this->m_view->get_selection();
  std::vector<Gtk::TreeModel::Path> selected_rows = selection->get_selected_rows();

  for(std::vector<Gtk::TreeModel::Path>::iterator it=selected_rows.begin();
      it != selected_rows.end(); ++it)
  {
    Gtk::TreeModel::Row row = *(m_store->get_iter(*it));
    GstDebugCategory* cat = row[m_columns.category];

    //gst_debug_category_reset_threshold (cat);
    gst_debug_category_set_threshold (cat, debug_level);
  }

  this->refresh_categories();
}

/* Add any starting debug levels to the tree */
void DebugUI::init_custom_levels ()
{
  /*
   * Get the current default threshold. Add all categories set to a different level to the custom list
   * and refresh the tree
   */

  //FIXME: Gst Debug is not wrapped in Gstreamermm at all.
  GstDebugLevel default_thresh = gst_debug_get_default_threshold ();
  GSList *slist = gst_debug_get_all_categories ();

  while (slist)
  {
    GstDebugCategory *cat = (GstDebugCategory *) slist->data;
    GstDebugLevel thresh = gst_debug_category_get_threshold (cat);

    if (thresh != default_thresh) {

      //FIXME: how to disable the warning here?
      Glib::ustring level = debug_level_get_name (gst_debug_category_get_threshold (cat));

      Gtk::TreeModel::Row row = *(m_store->append());
      row[m_columns.level] = level;
      row[m_columns.name] = gst_debug_category_get_name (cat);
      row[m_columns.description] = gst_debug_category_get_description (cat);
      row[m_columns.category] = cat;
    }
    slist = slist->next;
  }

  //  /* I don't really grok the sorting stuff atm, and I have to do other work
  //   * now */
  //  gtk_tree_view_set_model (GTK_TREE_VIEW (debug_ui->treeview),
  //      gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (lstore)));
}


/*
 * Walk the tree model of categories and update the 'level' column
 * to reflect the current threshold
 */
void DebugUI::refresh_categories()
{
  for (Gtk::TreeIter iter = m_store->children().begin(); iter !=  m_store->children().end(); ++iter)
  {
    Gtk::TreeModel::Row row = *iter;
    GstDebugCategory* cat = row[m_columns.category];
    row[m_columns.level] = debug_level_get_name (gst_debug_category_get_threshold (cat));
  }

  populate_add_categories();
}


void DebugUI::populate_add_categories()
{
  if(!m_add_window) return;

  m_add_store->clear();

  //FIXME: Gst Debug is not wrapped in Gstreamermm at all.
  GSList *slist = gst_debug_get_all_categories ();
  for (; slist != NULL; slist = g_slist_next (slist))
  {
    GstDebugCategory *cat = (GstDebugCategory *) slist->data;

    /* Don't add the category if it is already in the custom cats list */
    if (find_cat_in_customlist(cat))
      continue;

    Gtk::TreeModel::Row row = *(m_add_store->append());
    row[m_columns.name] = gst_debug_category_get_name (cat);
    row[m_columns.description] = gst_debug_category_get_description (cat);
    row[m_columns.category] = cat;
  }
}



bool DebugUI::find_cat_in_customlist(GstDebugCategory * cat)
{

  for (Gtk::TreeIter iter = m_store->children().begin(); iter !=  m_store->children().end(); ++iter)
  {
    Gtk::TreeModel::Row row = *iter;
    GstDebugCategory* cur_cat = row[m_columns.category];

    if(cur_cat == cat){
      return true;
    }
  }

  return false;
}


void DebugUI::tree_select ()
{
  /* Walk through selected items in the list and set the debug category */
  Glib::RefPtr<Gtk::TreeView::Selection> selection = this->m_view->get_selection();

  if(selection->count_selected_rows() == 0){
    m_custom_box->set_sensitive(false);
    return;
  }

  m_custom_box->set_sensitive(true);


  std::vector<Gtk::TreeModel::Path> selected_rows = selection->get_selected_rows();

  for(std::vector<Gtk::TreeModel::Path>::iterator it=selected_rows.begin();
      it != selected_rows.end(); ++it)
  {
    Gtk::TreeModel::Row row = *(m_add_store->get_iter(*it));
    GstDebugCategory* cat = row[m_columns.category];


    m_custom_hscale->get_adjustment()->set_value(gst_debug_category_get_threshold (cat));
  }

}



void DebugUI::show_add_window()
{

  if (!m_add_window)
  {
    std::string ui_path = Gste::get_data_file("editor.glade");
    Glib::RefPtr<Gtk::Builder> builder;
    try
    {
      builder = Gtk::Builder::create_from_file(ui_path, "add-debug-win");
    }
    catch (const Glib::FileError & ex)
    {
      std::cerr << ex.what() << std::endl;
      return;
    }

    builder->get_widget("add-debug-win", m_add_window);
    m_add_window->set_transient_for(*dynamic_cast<Gtk::Window*>(this->get_toplevel()));

    /* Connect the callbacks and populate the list of debug categories */
    builder->get_widget("categories-tree", m_add_cats_treeview);

    Gtk::Button *add_button, *cancel_button;

    builder->get_widget("add-cat-button", add_button);
    builder->get_widget("cancel-cat-button", cancel_button);

    add_button->signal_clicked().connect(sigc::mem_fun(*this, &DebugUI::add_custom_cats));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*m_add_window, &Gtk::Window::hide));

    /* Set up the tree view columns */
    m_add_cats_treeview->set_model(m_add_store);
    m_add_cats_treeview->append_column(_("Name"), m_columns.name);
    m_add_cats_treeview->append_column(_("Description"), m_columns.description);

    Glib::RefPtr<Gtk::TreeSelection> selection = m_add_cats_treeview->get_selection();
    selection->set_mode(Gtk::SELECTION_MULTIPLE);
  }
  m_add_window->show_all();
  populate_add_categories();
}



/*
 * Handle a click on the 'add' button inside the add categories window
 * Grab the selected categories from the list and add them to the main window
 */
void DebugUI::add_custom_cats()
{
  if (!m_add_window) return;
  m_add_window->hide();

  /* Walk through selected items in the list and set the debug category */
  Glib::RefPtr<Gtk::TreeView::Selection> selection = this->m_add_cats_treeview->get_selection();
  std::vector<Gtk::TreeModel::Path> selected_rows = selection->get_selected_rows();

  for(std::vector<Gtk::TreeModel::Path>::iterator it=selected_rows.begin();
      it != selected_rows.end(); ++it)
  {
    Gtk::TreeModel::Row srow = *(m_add_store->get_iter(*it));
    GstDebugCategory* cat = srow[m_columns.category];

    if (!cat)
      continue;

    /* Add the category to the list of custom entries */
    Gtk::TreeModel::Row row = *(m_store->append());
    row[m_columns.level] = debug_level_get_name (gst_debug_category_get_threshold (cat));
    row[m_columns.name] = gst_debug_category_get_name (cat);
    row[m_columns.description] = gst_debug_category_get_description (cat);
    row[m_columns.category] = cat;
  }
}


void DebugUI::remove_custom_cats()
{
  /* Walk through selected items in the list and set the debug category */
  Glib::RefPtr<Gtk::TreeView::Selection> selection = this->m_view->get_selection();
  std::vector<Gtk::TreeModel::Path> selected_rows = selection->get_selected_rows();

  std::vector<Gtk::TreeRowReference> row_references;
  for(std::vector<Gtk::TreeModel::Path>::iterator it=selected_rows.begin();
      it != selected_rows.end(); ++it) {
    row_references.push_back(Gtk::TreeRowReference(m_store, *it));
  }

  for(std::vector<Gtk::TreeRowReference>::iterator it=row_references.begin();
      it != row_references.end(); ++it)
  {
    Gtk::TreeModel::iterator iter = (m_store->get_iter(it->get_path()));
    Gtk::TreeModel::Row srow = *iter;
    GstDebugCategory* cat = srow[m_columns.category];

    gst_debug_category_reset_threshold (cat);
    m_store->erase(iter);
  }

  populate_add_categories();
}
