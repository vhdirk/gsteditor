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

#include <gtk/gtk.h>

#include "config.h"
#include "element-tree.h"


using namespace Gste;

//TODO: move to utils
template <typename T>
std::vector<T> split(const T &s, char delim) {
  std::vector<T> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}


ElementTree::ElementTree()
  : Gtk::VBox()
{
  //path = gste_get_ui_file ("editor.glade2");
  std::string ui_path = "../data/editor.glade";
  try
  {
    m_builder = Gtk::Builder::create_from_file(ui_path, "quicklaunch_palette");
  }
  catch (const Glib::FileError & ex)
  {
    std::cerr << ex.what() << std::endl;
    return;
  }

  m_builder->get_widget("quicklaunch_palette", m_palette);
  m_builder->get_widget("element-tree", m_view);
  m_builder->get_widget("find-entry", m_filter_entry);

  m_view->append_column("Element", m_columns.name);


  //Create the Tree model:
  m_model = Gtk::TreeStore::create(m_columns);
  m_filter_model = Gtk::ListStore::create(m_columns);

  this->m_curr_model = m_model;

  m_view->set_model(m_curr_model);


  //this->show_all();

  m_view->set_search_entry(*m_filter_entry);
  m_view->set_search_column(0);


  this->show_all();
  this->add(*m_palette);


  std::vector<ElementClassTreeNode> classtree = get_class_tree();
  ElementClassTreeNode::ClassIter treewalk = classtree.begin();
  ElementClassTreeNode::ClassIter treewalkend = classtree.end();

  for(treewalk; treewalk != treewalkend; treewalk++)
  {
    this->populate_store(m_model->children(), *treewalk);
  }


  this->m_view->signal_row_activated().connect(sigc::mem_fun(*this, &ElementTree::tree_activate));
  this->m_view->signal_cursor_changed().connect(sigc::mem_fun(*this, &ElementTree::tree_select));

  this->m_filter_entry->signal_changed().connect(sigc::mem_fun(*this, &ElementTree::filter_text_changed));

}

void ElementTree::tree_activate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
  typedef Gtk::TreeModel::Children::iterator ChildIter;

  ChildIter iter = this->m_model->get_iter(path);

  if(!iter) return;

  Gtk::TreeModel::Row row = *iter;
  Glib::RefPtr<Gst::ElementFactory> factory = row[m_columns.factory];

  if(!factory) return;

  this->m_selected = factory;
  this->m_signal_element_activated.emit(factory);
}

void ElementTree::tree_select()
{
  Glib::RefPtr<Gtk::TreeSelection> selection = this->m_view->get_selection();

  Gtk::TreeModel::iterator iter = selection->get_selected();
  if(!iter) return;


  Gtk::TreeModel::Row row = *iter;
  Glib::RefPtr<Gst::ElementFactory> factory = row[m_columns.factory];

  Glib::ustring name = row[m_columns.name];

  if(!factory) return;

  this->m_selected = factory;
  this->m_signal_element_selected.emit(factory);
}


void ElementTree::filter_text_changed()
{
  Glib::signal_idle().connect_once(sigc::mem_fun(*this, &ElementTree::filter_elements));
}


/*
 * Retrieve the text from the filter text and:
 * If the text is the previous text + a suffix, whittle down the filter_model
entries
 * If the text is not, clear the filter_model and rebuild from the tree model
 * If the text is 0 length, redisplay the tree model
 */
void ElementTree::filter_elements()
{
  /* Get the text from the edit box */
  Glib::ustring filter_text;

  filter_text = this->m_filter_entry->get_text();

  if (filter_text.compare(m_current_filter_text) == 0){
    //    tree->filter_idle_id = 0;
    return;
  }

  /* If the new text is blank, reinstate the treemodel and clear the
  filter_model */
  if(filter_text.empty()){
    this->set_tree_model(this->m_model);
    return;
  }

  this->m_old_filter_text = this->m_current_filter_text;
  this->m_current_filter_text = filter_text;

  /* Determine if the new text is just the old text with extra tacked on */
  /* if so, we filter down the existing filter_model to save time, otherwise
  build a new filter_model list */

  if(this->m_old_filter_text.compare(filter_text) != 0 ){
    this->build_filter_by_tree();
  }else{
    this->build_filter_by_existing_tree();
  }

  this->set_tree_model(this->m_filter_model);

}

/*
 * Clear the filter_model, walk the entire element tree adding
 * matching entries
 */
void ElementTree::build_filter_by_tree()
{
  this->m_filter_model->clear();

  Gtk::TreeNodeChildren::iterator iter = this->m_model->children().begin();

  if(iter){
    this->filter_subtree(iter);
  }

  if(this->m_selected){
    this->set_tree_selection_by_factory(this->m_selected);
  }


}

/*
 * Walk the existing filter list model, removing non-matching entries
 */
void ElementTree::build_filter_by_existing_tree()
{
  typedef Gtk::TreeModel::Children::iterator ChildIter;

  ChildIter iter = this->m_filter_model->children().begin();

  while (iter != this->m_filter_model->children().end())
  {
    Gtk::TreeModel::Row row = *iter;
    Glib::RefPtr<Gst::ElementFactory> factory = row[m_columns.factory];

    if(this->m_current_filter_text.compare(factory->get_name()) != 0)
    {
      iter = this->m_filter_model->erase(iter);
    }else{
      ++iter;
    }

  }
}


/* Walk through the tree model and filter entries into the filter list_store */
void ElementTree::filter_subtree(Gtk::TreeNodeChildren::iterator childiter)
{

  while(childiter)
  {
    Gtk::TreeModel::Row row = *childiter;
    Glib::RefPtr<Gst::ElementFactory> factory = row[m_columns.factory];

    if (factory)
    {
      Glib::ustring factory_name = factory->get_name();

      if(factory_name.find(this->m_current_filter_text) !=std::string::npos )
      {
        /* Add the matching row to the filtered list */
        Gtk::TreeModel::Row row = *(m_filter_model->append());
        row[m_columns.name]  = factory_name;
        row[m_columns.factory] = factory;
      }

    }
    filter_subtree(childiter->children().begin());

    childiter++;
  }

}



void ElementTree::set_tree_model(Glib::RefPtr<Gtk::TreeModel> model)
{
  if(this->m_curr_model != model){

    this->m_curr_model = model;
    this->m_view->set_model(this->m_curr_model);

    /* If there's a selection in the filtered list, expand that item in the tree
     view */
    if(this->m_selected){
      this->set_tree_selection_by_factory(this->m_selected);
    }
  }
}

void ElementTree::set_tree_selection_by_factory (Glib::RefPtr<Gst::ElementFactory>& factory)
{
  if (!this->m_curr_model->children().empty()){
    this->set_select_by_factory_subtree(this->m_curr_model->children().begin(), factory);
  }

}


bool ElementTree::set_select_by_factory_subtree (Gtk::TreeNodeChildren::iterator childiter, Glib::RefPtr<Gst::ElementFactory>& factory)
{
  /* Walk forward searching for the element factory */

  while(childiter)
  {
    Gtk::TreeModel::Row row = *childiter;

    Glib::RefPtr<Gst::ElementFactory> curr_factory = row[m_columns.factory];

    if(curr_factory == factory)
    {
      Glib::RefPtr<Gtk::TreeView::Selection> selection = this->m_view->get_selection();

      Gtk::TreeModel::Path path = this->m_curr_model->get_path(childiter);

      if(path){
        this->m_view->collapse_all();
        this->m_view->expand_to_path(path);
        this->m_view->scroll_to_row(path, 0);
      }
      selection->select(childiter);
      return true;
    }

    if(set_select_by_factory_subtree(childiter->children().begin(), factory))
      return true;

    childiter++;
  }

}


bool find_element_class_tree(ElementClassTreeNode * out, ElementClassTreeNode * node, Glib::ustring & klassname)
{
  ElementClassTreeNode::ClassIter treewalk;

  for (treewalk = node->subclasses.begin();
       treewalk != node->subclasses.end(); treewalk++)
  {
    if (klassname.compare(treewalk->name) == 0)
    {
      out = &(*treewalk);
      return true;
    }
  }

  return false;

}

void print_element_class_tree(ElementClassTreeNode & node, int depth = 0)
{
  ElementClassTreeNode::ClassIter treewalk = node.subclasses.begin();
  ElementClassTreeNode::ClassIter treewalkend = node.subclasses.end();

  for (treewalk; treewalk != treewalkend; treewalk++)
  {
    for(int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << treewalk->name << std::endl;
    print_element_class_tree(*treewalk, depth+1);
  }

}


void ElementTree::populate_store(Gtk::TreeModel::Children parent, ElementClassTreeNode & node)
{
  Gtk::TreeModel::Row row = *(m_model->append(parent));
  row[m_columns.name]  = node.name;

  Gtk::TreeModel::Children newparent = row.children();

  ElementClassTreeNode::ClassIter treewalk = node.subclasses.begin();
  ElementClassTreeNode::ClassIter treewalkend = node.subclasses.end();

  for (treewalk; treewalk != treewalkend; treewalk++)
  {
    this->populate_store(newparent, *treewalk);
  }


  ElementClassTreeNode::FactIter efwalk = node.factories.begin();
  ElementClassTreeNode::FactIter efwalkend = node.factories.end();

  for(efwalk; efwalk != efwalkend; efwalk++)
  {
    Gtk::TreeModel::Row nrow = *(m_model->append(newparent));
    nrow[m_columns.name]  = (*efwalk)->get_name();
    nrow[m_columns.factory] = (*efwalk);
  }
}


std::vector<ElementClassTreeNode> &ElementTree::get_class_tree()
{
  typedef Glib::ListHandle< Glib::RefPtr<Gst::Plugin> > GstPluginList;
  typedef Glib::ListHandle< Glib::RefPtr<Gst::PluginFeature> > GstFeatureList;

  static ElementClassTreeNode classtree;

  Glib::RefPtr<Gst::Registry> reg = Gst::Registry::get();

  GstPluginList plugins = reg->get_plugin_list();

  /* loop over all plugins (.so) */
  for (GstPluginList::iterator it = plugins.begin(); it != plugins.end(); it++){

    GstFeatureList features = reg->get_feature_list( (*it)->get_name() );

    /* loop over all features within the plugin */
    for (GstFeatureList::iterator itt = features.begin(); itt != features.end(); itt++)
    {
      Glib::RefPtr<Gst::ElementFactory> ef = Glib::RefPtr<Gst::ElementFactory>::cast_dynamic(*itt);
      if (!ef) continue;

      std::vector<Glib::ustring> keys = ef->get_metadata_keys();

      //bail out of no klassname found
      if (std::find(keys.begin(), keys.end(), "klass") == keys.end())
        continue;

      Glib::ustring klasses = ef->get_metadata("klass");
      std::vector<Glib::ustring> klassnames = split(klasses, '/');

      ElementClassTreeNode *currtree = &classtree;

      /* loop over all klass names */
      for(std::vector<Glib::ustring>::iterator kit = klassnames.begin(); kit != klassnames.end(); kit++)
      {
        Glib::ustring klassname = *kit;
        bool found = find_element_class_tree(currtree, currtree, klassname);

        if (!found) {
          ElementClassTreeNode branch;
          branch.name = klassname;

          currtree->subclasses.push_back(branch);
          currtree = &(currtree->subclasses.back());
        }
      }
      currtree->factories.push_back(ef);
    }
  }

  //TODO: sort this list
  return classtree.subclasses;
}


ElementTree::type_signal_element_selected ElementTree::signal_element_selected()
{
  return m_signal_element_selected;
}

ElementTree::type_signal_element_activated ElementTree::signal_element_activated()
{
  return m_signal_element_activated;
}
