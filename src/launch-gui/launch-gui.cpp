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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "launch-gui.h"

using namespace Gste;


LaunchGUI::LaunchGUI(int argc, char *argv[])
  : Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    m_pipe_combo(true), m_status(), m_element_ui(ELEMENT_UI_VIEW_MODE_FULL),
    m_parse_but(), m_start_but(), m_pause_but()
{
  /***** set up the GUI *****/
  //this->signal_delete_event().connect(sigc::mem_fun(*this, &LaunchGUI::quit_live));
  this->set_default_size(500,300);

  Gtk::VBox * vbox = Gtk::manage(new Gtk::VBox());
  this->add(*vbox);

  try{
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file("/home/dvhaeren/development/gstreamer/gsteditor/data/gst-launch.png");
    this->set_icon(icon);
  }catch(Glib::FileError ex) {
    std::cerr << ex.what() << std::endl;
  }catch(Gdk::PixbufError ex) {
    std::cerr << ex.what() << std::endl;
  }

  Gtk::HBox * parse_line = Gtk::manage(new Gtk::HBox(false, 3));
  vbox->pack_start(*parse_line, Gtk::PACK_SHRINK);

  //gtk_combo_set_value_in_list (GTK_COMBO (m_pipe_combo), FALSE, FALSE);

  //load_history (pipe_combo);

  m_parse_but.set_label("Parse");
  m_start_but.set_label("Play");
  m_pause_but.set_label("Pause");
  m_start_but.set_sensitive(false);
  m_pause_but.set_sensitive(false);

  parse_line->pack_start(m_pipe_combo);
  parse_line->pack_start(m_parse_but, Gtk::PACK_SHRINK);
  parse_line->pack_start(m_start_but, Gtk::PACK_SHRINK);
  parse_line->pack_start(m_pause_but, Gtk::PACK_SHRINK);

  m_start_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_start));
  m_pause_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_pause));
  m_parse_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_parse));

  m_store = Gtk::TreeStore::create(m_columns);
  m_view.set_model(m_store);
  m_view.set_headers_visible(false);
  m_view.append_column("Mix", m_columns.name);

  Glib::RefPtr<Gtk::TreeSelection> selection = m_view.get_selection();
  selection->set_mode(Gtk::SELECTION_SINGLE);
  selection->signal_changed().connect(sigc::mem_fun(*this, &LaunchGUI::on_selection_changed));


  Gtk::VBox * prop_box = Gtk::manage(new Gtk::VBox());
  Gtk::Notebook * notebook = Gtk::manage(new Gtk::Notebook());

  build_debug_page (*notebook);

  Gtk::ScrolledWindow * page_scroll = Gtk::manage(new Gtk::ScrolledWindow());
  page_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  page_scroll->add(*prop_box);

  notebook->append_page(*page_scroll, "Properties");

  Gtk::ScrolledWindow * list_scroll = Gtk::manage(new Gtk::ScrolledWindow());
  list_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  list_scroll->add(m_view);

  Gtk::HPaned * pane = Gtk::manage(new Gtk::HPaned());
  pane->pack1(*list_scroll);
  pane->pack2(*notebook);

  vbox->pack_start(*pane);

  m_status.push("Stopped");
  vbox->pack_end(m_status, false, false);

  this->show_all();

  prop_box->pack_start(m_element_ui, Gtk::PACK_EXPAND_WIDGET);
  m_element_ui.show();

  if (argc > 1) {
    std::stringstream pipeline_desc;
    for(int i = 1; i < argc; ++i )
      pipeline_desc << argv[i] << " ";

    m_pipe_combo.append(pipeline_desc.str());
    m_pipe_combo.set_active_text(pipeline_desc.str());
    this->on_parse();
  }

}


void LaunchGUI::build_debug_page(Gtk::Notebook &notebook)
{
  //  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), gst_debug_ui_new (),
  //      gtk_label_new ("Debug"));
}

void LaunchGUI::quit_live()
{
  this->close();
}

void LaunchGUI::on_parse()
{
  Glib::ustring try_pipe = m_pipe_combo.get_active_text();
  std::clog << "trying pipeline: " << try_pipe << std::endl;

  try{
    m_pipeline = Gst::Parse::launch(try_pipe);

  }catch(Gst::ParseError & ex){
    m_status.push(ex.what());
    return;
  }catch(Gst::CoreError & ex){
    m_status.push(ex.what());
    return;
  }
  if(!m_pipeline){
    m_status.push("unknown parse error");
    return;
  }

  m_element_ui.clear_element();

  m_start_but.set_sensitive(true);

  m_store->clear();

  //FIXME: too much C here.
  if(GST_IS_CHILD_PROXY(m_pipeline->gobj()))
  {
    Glib::RefPtr<Gst::ChildProxy> childproxy(new Gst::ChildProxy(GST_CHILD_PROXY(m_pipeline->gobj())));
    childproxy->reference();
    this->build_tree(m_store->children(), childproxy);
  }

  m_view.expand_all();





  //  if (last_pipe == NULL || strcmp (last_pipe, try_pipe) != 0) {
  //    gchar *write_pipe = g_strdup_printf ("%s\n", try_pipe);

  //    g_object_set_data (G_OBJECT (widget), "last_pipe", try_pipe);
  //    fwrite (write_pipe, sizeof (gchar), strlen (write_pipe), history_file);
  //    fflush (history_file);
  //    history = g_list_prepend (history, try_pipe);
  //    gtk_combo_set_popdown_strings (GTK_COMBO (pipe_combo), history);
  //    g_free (write_pipe);
  //    g_free (last_pipe);
  //  }
}


void LaunchGUI::build_tree (Gtk::TreeModel::Children parent, Glib::RefPtr<Gst::ChildProxy> &childproxy)
{

  //Glib::RefPtr<Gst::ChildProxy> childproxy(new Gst::ChildProxy(GST_CHILD_PROXY(object->gobj())));
  if(!childproxy) return;

  int n_elements = childproxy->get_children_count();

  //iterate child elements
  for (int i = n_elements-1; i >= 0; i--)
  {
    Glib::RefPtr<Glib::Object> gobject = childproxy->get_child(i);

    Glib::RefPtr<Gst::Object> gstobject = Glib::RefPtr<Gst::Object>::cast_dynamic(gobject);
    if (!gstobject) continue;

    this->add_child_to_tree(parent, gstobject);
  }

}


void LaunchGUI::add_child_to_tree (Gtk::TreeModel::Children parent, const Glib::RefPtr<Gst::Object> &gstobject)
{
  Gtk::TreeIter iter = m_store->append(parent);
  Gtk::TreeModel::Row row = *iter;
  row[m_columns.name]  = gstobject->get_name();
  row[m_columns.element]  = gstobject;

  //connect childproxy signals, and build subtree
  if (GST_IS_CHILD_PROXY(gstobject->gobj()))
  {
    // For some reason, we cannot simply cast to Gst::Childproxy, or use Glib::wrap as it will try to do the same.
    // Instead, we call the constructor and reference the element explicitely, since the wrapper will
    // by default assume ownership over the object.
    Glib::RefPtr<Gst::ChildProxy> newchildproxy(new Gst::ChildProxy(GST_CHILD_PROXY(gstobject->gobj())));
    newchildproxy->reference();

    row[m_columns.conn_child_added] = newchildproxy->signal_child_added()
        .connect(sigc::bind(sigc::mem_fun(*this, &LaunchGUI::on_child_added), iter));

    row[m_columns.conn_child_removed] = newchildproxy->signal_child_removed()
        .connect(sigc::bind(sigc::mem_fun(*this, &LaunchGUI::on_child_removed), iter));

    this->build_tree(row.children(), newchildproxy);
  }

}


void LaunchGUI::on_child_added(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name, Gtk::TreeIter iter)
{
  this->add_child_to_tree(iter->children(), object);
  m_view.expand_all();

}


void LaunchGUI::on_child_removed(const Glib::RefPtr<Gst::Object>& object, const Glib::ustring& name, Gtk::TreeIter iter)
{
  Gtk::TreeModel::Children children = iter->children();

  Gtk::TreeIter childiter = children.begin();
  while(childiter != children.end())
  {
    Gtk::TreeModel::Row childrow = *childiter;
    if (childrow[m_columns.name] == object->get_name())
    {
      //disconnect signals (if any)
      sigc::connection conn = childrow[m_columns.conn_child_added];
      if(conn) conn.disconnect();

      conn = childrow[m_columns.conn_child_removed];
      if(conn) conn.disconnect();

      //remove the element from the store
      childiter = m_store->erase(childiter);
    }else{
      childiter++;
    }
  }

}




void LaunchGUI::on_start()
{
  if(m_start_but.get_active())
  {
    m_pause_but.set_sensitive(true);
    m_pause_but.set_active(false);
    m_parse_but.set_sensitive(false);
    m_pipe_combo.set_sensitive(false);
    m_status.push("Playing");

    m_pipeline->set_state(Gst::STATE_PLAYING);
  }else{
    m_pause_but.set_sensitive(false);
    m_pause_but.set_active(false);
    m_parse_but.set_sensitive(true);
    m_pipe_combo.set_sensitive(true);
    m_status.push("Stopped");

    m_pipeline->set_state(Gst::STATE_NULL);

  }

}


void LaunchGUI::on_pause()
{
  if(m_pause_but.get_active()){
    m_pipeline->set_state(Gst::STATE_PAUSED);
    m_status.push("Paused");
  }else{
    m_pipeline->set_state(Gst::STATE_PLAYING);
    m_status.push("Playing");
  }
}


void LaunchGUI::on_selection_changed()
{
  Gtk::TreeModel::iterator iter = m_view.get_selection()->get_selected();

  if(!iter) return;

  Gtk::TreeModel::Row row = *iter;
  Glib::RefPtr<Gst::Object> elem = row[m_columns.element];

  m_element_ui.set_element(elem);
}
