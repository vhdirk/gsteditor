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

#ifndef __GSTE_ELEMENT_UI_PROP_VIEW_H__
#define __GSTE_ELEMENT_UI_PROP_VIEW_H__

#include <gstreamermm.h>
#include <gtkmm.h>

namespace Gste
{

/*!
 * \brief small convenience adapter for GParamSpec
 */
struct ParamAdapter
{
  GParamSpec *param_spec;
  std::string name;
  GType spec_type;
  GType value_type;
  std::string blurb;
  bool readable;
  bool writable;
  bool construct_only;
};


/*!
 * Base class for creating widgets for Gst Object properties
 */
class ElementUIPropView : public Gtk::VBox
{
public:
  static ElementUIPropView* create(Glib::RefPtr<Gst::Object> &element, ParamAdapter & param);

  virtual ~ElementUIPropView();

  virtual void disable_construct_only(bool disable=true)=0;

protected:
  virtual void update_async();

  virtual void update() = 0;
  virtual void on_value_changed() = 0;


  void block_element_signals(bool block);

protected:
  ElementUIPropView(Glib::RefPtr<Gst::Object> element, ParamAdapter & param);

  Glib::RefPtr<Gst::Object> m_element;
  ParamAdapter m_param;
  sigc::connection m_conn_widget;
  sigc::connection m_conn_element;

  Glib::Mutex m_mutex;
  Glib::ValueBase m_value_async;

};


template <typename T>
class ElementUIPropViewNumber : public ElementUIPropView
{
public:
  ElementUIPropViewNumber(Glib::RefPtr<Gst::Object> element, ParamAdapter & param,
                          T minimum_value=0, T maximum_value=100, T default_value=1);

  virtual void disable_construct_only(bool disable=true);

protected:
  virtual void on_value_changed();
  virtual void update();

protected:
  Gtk::Label  m_label_lower;
  Gtk::Label  m_label_upper;
  Gtk::HScale m_hscale;

  Gtk::SpinButton m_spinbutton;
  Glib::RefPtr<Gtk::Adjustment> m_adjustment;
};

class ElementUIPropViewSwitch : public ElementUIPropView
{
public:
  ElementUIPropViewSwitch(Glib::RefPtr<Gst::Object> element, ParamAdapter & param);

  virtual void disable_construct_only(bool disable=true);

protected:
  virtual void on_value_changed();
  virtual void update();


protected:
  Gtk::Switch m_switch;
};


class ElementUIPropViewText : public ElementUIPropView
{
public:
  ElementUIPropViewText(Glib::RefPtr<Gst::Object> element, ParamAdapter & param);

  virtual void disable_construct_only(bool disable=true);

protected:
  virtual void on_value_changed();
  virtual void update();


protected:
  Gtk::Entry m_entry;
};


class ElementUIPropViewChoice : public ElementUIPropView
{
public:
  ElementUIPropViewChoice(Glib::RefPtr<Gst::Object> element, ParamAdapter & param);

  virtual void disable_construct_only(bool disable=true);

protected:
  virtual void on_value_changed();
  virtual void update();


protected:
  std::vector<int>  m_values;
  Gtk::ComboBoxText m_combo;
};


class ElementUIPropViewFile : public ElementUIPropView
{
public:
  ElementUIPropViewFile(Glib::RefPtr<Gst::Object> element, ParamAdapter & param);

  virtual void disable_construct_only(bool disable=true);

protected:
  virtual void on_value_changed();
  virtual void update();


protected:
  Gtk::FileChooserButton m_file_button;

};

}

#endif /* __GSTE_ELEMENT_UI_PROP_VIEW_H__ */
