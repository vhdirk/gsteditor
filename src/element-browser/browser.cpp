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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "browser.h"
#include "element-tree.h"
#include "caps-tree.h"

using namespace Gste;

ElementBrowser::ElementBrowser()
  : Gtk::Dialog(),
    m_longname(), m_description(), m_author(),
    m_pads(), m_padtemplates()
{
  this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
  this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  this->set_default_response(Gtk::RESPONSE_CANCEL);

  this->set_resizable(true);
  this->set_default_size(640, 480);

  this->set_title("Select element...");

  Gtk::HPaned* hpaned(Gtk::manage(new Gtk::HPaned()));
  this->get_content_area()->pack_start(*hpaned);

  ElementTree * tree = Gtk::manage(new ElementTree());
  tree->set_size_request(250, -1);

  hpaned->pack1(*tree, Gtk::FILL);

  Gtk::VBox* vbox(Gtk::manage(new Gtk::VBox()));
  hpaned->pack2(*vbox, Gtk::EXPAND);

  Gtk::Frame* frame_details(Gtk::manage(new Gtk::Frame("Element Details")));
  vbox->pack_start(*frame_details);

  Gtk::Grid* grid_main(Gtk::manage(new Gtk::Grid()));
  grid_main->set_row_spacing(1);

  /* the long name of the element */
  Gtk::Label* label_longname(Gtk::manage(new Gtk::Label("Name:")));
  label_longname->set_alignment(1.0, 0.0);
  label_longname->set_valign(Gtk::ALIGN_START);
  grid_main->attach(*label_longname, 0, 0, 1, 1);

  m_longname.set_selectable(true);
  m_longname.set_line_wrap(true);
  m_longname.set_justify(Gtk::JUSTIFY_LEFT);
  m_longname.set_alignment(0.0, 0.0);
  grid_main->attach(m_longname, 1, 0, 2, 1);

  /* the description */
  Gtk::Label* label_description(Gtk::manage(new Gtk::Label("Description:")));
  label_description->set_alignment(1.0, 0.0);
  label_description->set_valign(Gtk::ALIGN_START);
  grid_main->attach(*label_description, 0, 1, 1, 1);

  m_description.set_selectable(true);
  m_description.set_line_wrap(true);
  m_description.set_justify(Gtk::JUSTIFY_LEFT);
  m_description.set_alignment(0.0, 0.0);
  grid_main->attach(m_description, 1, 1, 2, 1);

  /* the author */
  Gtk::Label* label_autor(Gtk::manage(new Gtk::Label("Author:")));
  label_autor->set_alignment(1.0, 0.0);
  label_autor->set_valign(Gtk::ALIGN_START);
  grid_main->attach(*label_autor, 0, 2, 1, 1);

  m_author.set_selectable(true);
  m_author.set_line_wrap(true);
  m_author.set_justify(Gtk::JUSTIFY_LEFT);
  m_author.set_alignment(0.0, 0.0);
  grid_main->attach(m_author, 1, 2, 2, 1);

  frame_details->add(*grid_main);

  Gtk::Frame* frame_pads(Gtk::manage(new Gtk::Frame("Pads")));
  vbox->pack_start(*frame_pads);
  frame_pads->add(m_pads);

  Gtk::Frame* frame_padtemplates(Gtk::manage(new Gtk::Frame("Pad templates")));
  vbox->pack_start(*frame_padtemplates);
  frame_padtemplates->add(m_padtemplates);

  tree->signal_element_selected().connect(sigc::mem_fun(*this, &ElementBrowser::on_tree_selection_changed));
  tree->signal_element_activated().connect(sigc::mem_fun(*this, &ElementBrowser::on_tree_activated));

  this->show_all();
  this->hide();
}


Glib::RefPtr<Gst::ElementFactory> ElementBrowser::pick_modal()
{
  int response = this->run();

  this->hide();

  if (response != Gtk::RESPONSE_ACCEPT)
    return Glib::RefPtr<Gst::ElementFactory>();
  else
    return this->m_factory;
}


void ElementBrowser::on_tree_selection_changed(Glib::RefPtr<Gst::ElementFactory>& factory)
{
  this->m_longname.set_text(factory->get_metadata("long-name"));
  this->m_description.set_text(factory->get_metadata("description"));
  this->m_author.set_text(factory->get_metadata("author"));

  this->m_factory = factory;
  this->m_element = factory->create_named_element("element");

  this->m_padtemplates.set_element_factory(factory);
  this->m_pads.set_element(m_element);
}


void ElementBrowser::on_tree_activated(Glib::RefPtr<Gst::ElementFactory>& factory)
{
  this->m_factory = factory;
  this->response(Gtk::RESPONSE_ACCEPT);
}
