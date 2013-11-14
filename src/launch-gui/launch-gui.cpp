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
#include <fstream>
#include <algorithm>
#include <set>


#include <glibmm/i18n.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "launch-gui.h"
#include "common/utils.h"
#include "debug-ui/debug-ui.h"

using namespace Gste;


LaunchGUI::LaunchGUI(int argc, char *argv[])
  : Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    m_pipe_combo(true), m_last_pipe(), m_status(), m_element_ui(ELEMENT_UI_VIEW_MODE_FULL),
    m_parse_but(), m_start_but(), m_pause_but()
{
  /***** set up the GUI *****/
  this->set_default_size(640,480);

  Gtk::VBox * vbox = Gtk::manage(new Gtk::VBox());
  this->add(*vbox);

  try{
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(Gste::get_data_file("gst-launch.png"));
    this->set_icon(icon);
  }catch(Glib::FileError ex) {
    std::cerr << ex.what() << std::endl;
  }catch(Gdk::PixbufError ex) {
    std::cerr << ex.what() << std::endl;
  }
  // status bar
  vbox->pack_end(m_status, false, false);

  //parse line
  Gtk::HBox * parse_line = Gtk::manage(new Gtk::HBox(false, 3));
  vbox->pack_start(*parse_line, Gtk::PACK_SHRINK);

  this->combo_load_history();

  m_parse_but.set_label(_("Parse"));

  parse_line->pack_start(m_pipe_combo);
  parse_line->pack_start(m_parse_but, Gtk::PACK_SHRINK);

  m_parse_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_parse));

  // media control
  Gtk::HBox * media_line = Gtk::manage(new Gtk::HBox(false, 3));
  vbox->pack_end(*media_line, Gtk::PACK_SHRINK);
  m_start_but.set_image_from_icon_name("media-playback-start");
  m_pause_but.set_image_from_icon_name("media-playback-pause");
  m_start_but.set_sensitive(false);
  m_pause_but.set_sensitive(false);
  m_progress_bar.set_draw_value(false);
  m_progress_bar.get_adjustment()->set_lower(0);
  m_progress_bar.get_adjustment()->set_upper(0);

  m_progress_position.set_text("00:00");
  m_progress_duration.set_text("00:00");

  media_line->pack_start(m_start_but, Gtk::PACK_SHRINK);
  media_line->pack_start(m_pause_but, Gtk::PACK_SHRINK);
  media_line->pack_start(m_progress_position, Gtk::PACK_SHRINK);
  media_line->pack_start(m_progress_bar, Gtk::PACK_EXPAND_WIDGET);
  media_line->pack_end(m_progress_duration, Gtk::PACK_SHRINK);

  m_start_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_start));
  m_pause_but.signal_clicked().connect(sigc::mem_fun(*this, &LaunchGUI::on_pause));

  m_progress_conn_manual = m_progress_bar.signal_value_changed().connect(sigc::mem_fun(*this, &LaunchGUI::on_progress_changed));

  // element tree
  m_store = Gtk::TreeStore::create(m_columns);
  m_view.set_model(m_store);
  m_view.set_headers_visible(false);
  m_view.append_column(_("Mix"), m_columns.name);

  Glib::RefPtr<Gtk::TreeSelection> selection = m_view.get_selection();
  selection->set_mode(Gtk::SELECTION_SINGLE);
  selection->signal_changed().connect(sigc::mem_fun(*this, &LaunchGUI::on_selection_changed));


  Gtk::VBox * prop_box = Gtk::manage(new Gtk::VBox(true, 5));
  Gtk::Notebook * notebook = Gtk::manage(new Gtk::Notebook());

  build_debug_page (*notebook);

  Gtk::ScrolledWindow * page_scroll = Gtk::manage(new Gtk::ScrolledWindow());
  page_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  page_scroll->add(*prop_box);

  notebook->append_page(*page_scroll, _("Properties"));

  Gtk::ScrolledWindow * list_scroll = Gtk::manage(new Gtk::ScrolledWindow());
  list_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  list_scroll->add(m_view);

  Gtk::HPaned * pane = Gtk::manage(new Gtk::HPaned());
  pane->pack1(*list_scroll);
  pane->pack2(*notebook);

  vbox->pack_start(*pane);

  m_status.push(_("Stopped"));

  this->show_all();
  notebook->set_current_page(1);


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


bool LaunchGUI::update_progress_bar()
{
  if(!m_pipeline) return false;

  gint64 duration = 0, position = 0;

  m_progress_conn_manual.block();

  bool success = m_pipeline->query_duration(Gst::FORMAT_TIME, duration) &&
     m_pipeline->query_position(Gst::FORMAT_TIME, position);

  m_progress_bar.set_sensitive(success);
  m_progress_bar.get_adjustment()->set_upper(duration);
  m_progress_bar.get_adjustment()->set_value(position);

  char buff[20];
  sprintf(buff, "%" GST_TIME_FORMAT "\n", GST_TIME_ARGS(duration));
  m_progress_duration.set_text(buff);

  sprintf(buff, "%" GST_TIME_FORMAT "\n", GST_TIME_ARGS(position));
  m_progress_position.set_text(buff);


  m_progress_conn_manual.unblock();

  return success;
}

void LaunchGUI::on_progress_changed()
{
  if(!m_pipeline) return;

  gint64 position = m_progress_bar.get_value();

  m_pipeline->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH | Gst::SEEK_FLAG_KEY_UNIT, position);

}

void LaunchGUI::build_debug_page(Gtk::Notebook &notebook)
{
  //TODO: handle debug page lifetime
  Gste::DebugUI* debug_ui = new DebugUI();
  notebook.append_page(*debug_ui, _("Debug"));
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
    m_status.push(_("unknown parse error"));
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

  //update the slider
  update_progress_bar();

  //append to history combo
  if(!m_last_pipe.empty() && m_last_pipe != try_pipe)
  {
    m_pipe_combo.append(try_pipe);
  }

  //append to history file
  this->append_history(try_pipe);
}


void LaunchGUI::build_tree (Gtk::TreeModel::Children parent, Glib::RefPtr<Gst::ChildProxy> &childproxy)
{

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
  if(m_progress_conn_auto) m_progress_conn_auto.disconnect();

  if(m_start_but.get_active())
  {
    m_pipeline->set_state(Gst::STATE_PLAYING);

    Gst::State state, pending;
    Gst::StateChangeReturn ret = m_pipeline->get_state(state, pending, Gst::CLOCK_TIME_NONE);

    if(ret != Gst::STATE_CHANGE_FAILURE){
      m_start_but.set_image_from_icon_name("media-playback-stop");

      m_pause_but.set_sensitive(true);
      m_pause_but.set_active(false);
      m_parse_but.set_sensitive(false);
      m_pipe_combo.set_sensitive(false);
      m_status.push(_("Playing"));

      m_element_ui.disable_construct_only(true);
      update_progress_bar();

      m_progress_conn_auto = Glib::signal_timeout().connect(sigc::mem_fun(*this, &LaunchGUI::update_progress_bar), 200);

    } else {
      m_start_but.set_active(false);
      m_status.push(_("Error while settings pipeline to playing state."));
    }

  }else{
    m_start_but.set_image_from_icon_name("media-playback-start");
    m_pause_but.set_sensitive(false);
    m_pause_but.set_active(false);
    m_parse_but.set_sensitive(true);
    m_pipe_combo.set_sensitive(true);
    m_status.push(_("Stopped"));

    m_element_ui.disable_construct_only(false);

    m_pipeline->set_state(Gst::STATE_NULL);
  }

}


void LaunchGUI::on_pause()
{
  if(m_pause_but.get_active()){
    m_pipeline->set_state(Gst::STATE_PAUSED);
    m_status.push(_("Paused"));
  }else{
    m_pipeline->set_state(Gst::STATE_PLAYING);
    m_status.push(_("Playing"));
  }
}


void LaunchGUI::on_selection_changed()
{
  Gtk::TreeModel::iterator iter = m_view.get_selection()->get_selected();

  if(!iter) return;

  Gtk::TreeModel::Row row = *iter;
  Glib::RefPtr<Gst::Object> elem = row[m_columns.element];

  m_element_ui.set_element(elem, m_start_but.get_active());
}


Glib::ustring LaunchGUI::get_history_filename()
{
  Glib::ustring history_path = Glib::build_filename( Glib::get_user_config_dir(), "gsteditor" );

  if (!Glib::file_test(history_path, Glib::FILE_TEST_IS_DIR)){
    Glib::RefPtr<Gio::File> history_dir = Gio::File::create_for_path(history_path);
    history_dir->make_directory_with_parents();
  }

  Glib::ustring history_file = Glib::build_filename( history_path, "gst-launch-gui.history");
  if (!Glib::file_test(history_file, Glib::FILE_TEST_IS_REGULAR))
  {
    Glib::RefPtr<Gio::File> history_f = Gio::File::create_for_path(history_file);
    history_f->create_file();
  }

  return history_file;
}


void LaunchGUI::combo_load_history()
{
  std::set<Glib::ustring> entries = load_history();

  for(std::set<Glib::ustring>::iterator it=entries.begin();
      it != entries.end(); ++it)
  {
    m_pipe_combo.prepend(*it);
  }
}




std::set<Glib::ustring> LaunchGUI::load_history()
{
  std::set<Glib::ustring> entries;

  Glib::ustring history_filename = get_history_filename();
  if (!Glib::file_test(history_filename, Glib::FILE_TEST_IS_REGULAR))
    return entries;

  std::ifstream histfile(history_filename.c_str());
  if(histfile.is_open())
  {
    std::string line;
    while (std::getline(histfile, line)){
      entries.insert(line);
    }

    histfile.close();
  }
  return entries;
}



void LaunchGUI::append_history(Glib::ustring & pipeline)
{
  Glib::ustring history_filename = get_history_filename();

  std::set<Glib::ustring> entries = load_history();
  entries.insert(pipeline);

  std::ofstream histfile(history_filename.c_str(), std::ios::out | std::ios::trunc);
  if(histfile.is_open())
  {
    for(std::set<Glib::ustring>::iterator it=entries.begin();
        it != entries.end(); ++it)
    {
      std::string tmp =  *it ;
      histfile << tmp << std::endl;
    }
    histfile.close();
  }

}
