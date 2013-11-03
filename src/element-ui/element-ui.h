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

#ifndef __GSTE_ELEMENT_UI_H__
#define __GSTE_ELEMENT_UI_H__

#include <gstreamermm.h>
#include <gtkmm.h>

#include "element-ui-prop-view.h"

namespace Gste
{

enum ElementUIViewMode{
  ELEMENT_UI_VIEW_MODE_COMPACT = 1,
  ELEMENT_UI_VIEW_MODE_FULL
};


class ElementUI : public Gtk::Grid
{
public:
  ElementUI(ElementUIViewMode view_mode);

public:
  void set_element(Glib::RefPtr<Gst::Object> &element, bool playing);
  void clear_element();

  //TODO: i'd like to replace this by an async task watching the element status
  void disable_construct_only(bool disable=true);

protected:
  void set_page_title(Glib::ustring title);

protected:
  ElementUIViewMode m_viewmode;

  Gtk::Label m_name;
  Gtk::ComboBoxText m_optionmenu;
  Glib::RefPtr<Gst::Object> m_element;

  std::vector<ElementUIPropView*>  m_param_views;
  std::vector<Gtk::Label*>  m_param_labels;

  Gtk::Label m_message;

protected:
  std::vector< ParamAdapter > m_params;

};

}

#endif // __GSTE_ELEMENT_UI_H__
