/* GStreamer Element View and Controller
 *
 * Copyright (C) <2002> Andy Wingo <wingo@pobox.com>
 *               <2013> Dirk Van Haerenborgh <vhdirk@gmail.com>
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
#include <iomanip>

#include <glibmm/value.h>
#include <glibmm/value_custom.h>

#include "element-ui.h"
#include "element-ui-prop-view.h"

using namespace Gste;


ElementUIPropView* ElementUIPropView::create(Glib::RefPtr<Gst::Object> &element, ParamAdapter & param)
{
  if (param.spec_type == G_TYPE_PARAM_INT)
  {
     GParamSpecInt* spec = G_PARAM_SPEC_INT(param.param_spec);
     return new ElementUIPropViewNumber<gint>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_UINT)
  {
    GParamSpecUInt* spec = G_PARAM_SPEC_UINT(param.param_spec);
    return new ElementUIPropViewNumber<guint>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_LONG)
  {
    GParamSpecLong* spec = G_PARAM_SPEC_LONG(param.param_spec);
    return new ElementUIPropViewNumber<glong>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_ULONG)
  {
    GParamSpecULong* spec = G_PARAM_SPEC_ULONG(param.param_spec);
    return new ElementUIPropViewNumber<gulong>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_INT64)
  {
    GParamSpecInt64* spec = G_PARAM_SPEC_INT64(param.param_spec);
    return new ElementUIPropViewNumber<gint64>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_UINT64)
  {
    GParamSpecUInt64* spec = G_PARAM_SPEC_UINT64(param.param_spec);
    return new ElementUIPropViewNumber<guint64>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_FLOAT)
  {
    GParamSpecFloat* spec = G_PARAM_SPEC_FLOAT(param.param_spec);
    return new ElementUIPropViewNumber<gfloat>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_DOUBLE)
  {
    GParamSpecDouble* spec = G_PARAM_SPEC_DOUBLE(param.param_spec);
    return new ElementUIPropViewNumber<gdouble>(element, param, spec->minimum, spec->maximum, spec->default_value);
  }
  if (param.spec_type == G_TYPE_PARAM_BOOLEAN)
  {
    return new ElementUIPropViewSwitch(element, param);
  }
  if(param.spec_type == G_TYPE_PARAM_STRING)
  {
    if(param.name == "location" ||
       param.name == "filename" ||
       param.name == "file" ||
       param.name == "uri"){
      return new ElementUIPropViewFile(element, param);
    }else{
      return new ElementUIPropViewText(element, param);
    }
  }
  if(param.spec_type == G_TYPE_PARAM_ENUM)
  {
    return new ElementUIPropViewChoice(element, param);
  }

  return NULL;
}


ElementUIPropView::ElementUIPropView(Glib::RefPtr<Gst::Object> element, ParamAdapter & param)
  : Gtk::VBox(),
    m_element(element), m_param(param)
{
  m_value_async.init(m_param.value_type);

  if (m_param.readable){
    m_conn_element = m_element->connect_property_changed_with_return(param.name, sigc::mem_fun(*this, &ElementUIPropView::update_async));
  }
}

ElementUIPropView::~ElementUIPropView()
{
  if(m_conn_element) m_conn_element.disconnect();
  if(m_conn_widget) m_conn_widget.disconnect();

}


/* It is assumed that this function is called from the element's thread, so we
   can just go ahead and use g_object_get here */
void ElementUIPropView::update_async()
{
  //  GST_DEBUG ("async property update: %s", pview->param->name);
  m_mutex.lock();

  if(m_conn_widget){
    m_conn_widget.block();
  }

  m_value_async.reset();
  m_element->get_property_value(m_param.name, m_value_async);

  Glib::signal_idle().connect_once(sigc::mem_fun(*this, &ElementUIPropView::update));

  if(m_conn_widget){
    m_conn_widget.unblock();
  }

  m_mutex.unlock();
}

void ElementUIPropView::block_element_signals(bool block)
{
  if(m_conn_element){
    m_conn_element.block(block);
  }
}

//------------------------------------------------------------------------------
// Number
//------------------------------------------------------------------------------

template <typename T>
ElementUIPropViewNumber<T>::ElementUIPropViewNumber(Glib::RefPtr<Gst::Object> element, ParamAdapter & param,
                                                    T minimum_value, T maximum_value, T default_value)
  : ElementUIPropView(element, param),
    m_label_lower(), m_label_upper(), m_adjustment(), m_spinbutton(), m_hscale()
{
  /* spin button stuff */
  Gtk::Grid * grid_spin = Gtk::manage(new Gtk::Grid());
  this->add(*grid_spin);
  this->show();

  std::stringstream ss;
  ss << std::setprecision(5) << std::scientific << minimum_value;
  m_label_lower.set_text(ss.str());
  grid_spin->attach(m_label_lower, 0, 0, 1, 1);
  m_label_lower.set_justify(Gtk::JUSTIFY_LEFT);

  m_adjustment = Gtk::Adjustment::create(default_value, minimum_value, maximum_value, 1, 10, 10);
  m_spinbutton.configure(m_adjustment, 1, 0);
  grid_spin->attach(m_spinbutton, 1, 0, 2, 1);

  ss.str("");
  ss << std::setprecision(5) <<  std::scientific << maximum_value;
  m_label_upper.set_text(ss.str());
  grid_spin->attach(m_label_upper, 3, 0, 1, 1);
  m_label_upper.set_justify(Gtk::JUSTIFY_RIGHT);

  m_hscale.set_adjustment(m_adjustment);
  grid_spin->attach(m_hscale, 0, 1, 4, 1);
  m_hscale.set_draw_value(false);
  m_hscale.set_digits(2);

  //make parameter inactive is not writable
  if(m_param.writable){
    m_conn_widget = m_adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &ElementUIPropViewNumber::on_value_changed));
  }

  this->update_async();
  this->show_all();
}

template <typename T>
void ElementUIPropViewNumber<T>::update()
{
  double value = static_cast<Glib::Value<T>*>(&m_value_async)->get();

  m_adjustment->set_value(value);
}

template <typename T>
void ElementUIPropViewNumber<T>::on_value_changed()
{
  this->block_element_signals(true);

  m_element->set_property(m_param.name, static_cast<T>(m_adjustment->get_value()));

  this->block_element_signals(false);
}

//------------------------------------------------------------------------------
// Toggle
//------------------------------------------------------------------------------

ElementUIPropViewSwitch::ElementUIPropViewSwitch(Glib::RefPtr<Gst::Object> element, ParamAdapter & param)
  : ElementUIPropView(element, param),
    m_switch()
{
  /* toggle button stuff */
  this->add(m_switch);

  m_conn_widget = m_switch.property_active().signal_changed().connect(sigc::mem_fun(*this, &ElementUIPropViewSwitch::on_value_changed));

  this->update_async();
  this->show_all();

}

void ElementUIPropViewSwitch::update()
{
  m_switch.set_active(static_cast<Glib::Value<bool>*>(&m_value_async)->get());
}

void ElementUIPropViewSwitch::on_value_changed()
{
  this->block_element_signals(true);

  m_element->set_property(m_param.name, m_switch.get_active());

  this->block_element_signals(false);
}

//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------

ElementUIPropViewText::ElementUIPropViewText(Glib::RefPtr<Gst::Object> element, ParamAdapter & param)
  : ElementUIPropView(element, param),
    m_entry()
{
  this->add(m_entry);
  m_conn_widget = m_entry.signal_changed().connect(sigc::mem_fun(*this, &ElementUIPropViewText::on_value_changed));

  this->update_async();
  this->show_all();
}

void ElementUIPropViewText::update()
{
  m_entry.set_text(static_cast<Glib::Value<Glib::ustring>*>(&m_value_async)->get());
}

void ElementUIPropViewText::on_value_changed()
{
  this->block_element_signals(true);

  m_element->set_property(m_param.name, m_entry.get_text());

  this->block_element_signals(false);
}

//------------------------------------------------------------------------------
// Choice
//------------------------------------------------------------------------------

ElementUIPropViewChoice::ElementUIPropViewChoice(Glib::RefPtr<Gst::Object> element, ParamAdapter & param)
  : ElementUIPropView(element, param),
    m_combo(), m_values()
{
  this->add(m_combo);

  //ugh, too much C here
  GEnumClass *klass = G_ENUM_CLASS (g_type_class_ref (param.value_type));
  for (int i = 0; i < klass->n_values; i++){
    GEnumValue *value = &klass->values[i];
    m_values.push_back(value->value);
    m_combo.append(value->value_name);
  }
  g_type_class_unref(klass);

  m_conn_widget = m_combo.signal_changed().connect(sigc::mem_fun(*this, &ElementUIPropViewChoice::on_value_changed));

  this->update_async();
  this->show_all();
}

void ElementUIPropViewChoice::update()
{
  int val = static_cast<Glib::Value_Enum<int> *>(&m_value_async)->get();

  for(int i= 0; i < m_values.size(); i++)
  {
    if (val == m_values[i]) {
      m_combo.set_active(i);
      break;
    }
  }

}

void ElementUIPropViewChoice::on_value_changed()
{
  this->block_element_signals(true);

  m_element->set_property(m_param.name, m_values[m_combo.property_active()]);

  this->block_element_signals(false);
}

//------------------------------------------------------------------------------
// File
//------------------------------------------------------------------------------

ElementUIPropViewFile::ElementUIPropViewFile(Glib::RefPtr<Gst::Object> element, ParamAdapter & param)
  : ElementUIPropView(element, param),
    m_file_button()
{
  this->add(m_file_button);

  m_conn_widget = m_file_button.signal_file_set().connect(sigc::mem_fun(*this, &ElementUIPropViewFile::on_value_changed));

  this->update_async();
  this->show_all();
}


void ElementUIPropViewFile::update()
{
  Glib::ustring value = reinterpret_cast<Glib::Value<Glib::ustring>*>(&m_value_async)->get();
  if(m_param.name == "uri"){
    m_file_button.set_uri(value);
  }else{
    m_file_button.set_filename(value);
  }
}


void ElementUIPropViewFile::on_value_changed()
{
  this->block_element_signals(true);

  Glib::ustring value;
  if(m_param.name == "uri"){
    value = m_file_button.get_uri();
  }else{
    value = m_file_button.get_filename();
  }

  m_element->set_property(m_param.name, value);

  this->block_element_signals(false);
}
