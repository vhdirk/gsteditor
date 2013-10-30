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

#include "element-ui.h"

#ifdef _DEBUG
#define debug(...) g_message(__VA_ARGS__)
#else
#define debug(...)
#endif

using namespace Gste;


// The following is a serious drawback in Glibmm: there seems to be nosimple way to query
// properties of an object.
std::vector<ParamAdapter> list_klass_parameters(Glib::RefPtr<Gst::Object> element)
{
  GType gtype = G_OBJECT_CLASS_TYPE(G_OBJECT_GET_CLASS(G_OBJECT(element->gobj())));
  std::vector<ParamAdapter> out;
  //Get the list of properties:
  GParamSpec** param_specs = 0;
  guint cnt = 0;
  if(G_TYPE_IS_OBJECT(gtype))
  {
    GObjectClass* klass = G_OBJECT_CLASS(g_type_class_ref(gtype));
    param_specs = g_object_class_list_properties (klass, &cnt);
    g_type_class_unref(klass);
  }
  else if (G_TYPE_IS_INTERFACE(gtype))
  {
    gpointer interface = g_type_default_interface_ref(gtype);
    if(interface) //We check because this fails for G_TYPE_VOLUME, for some reason.
    {
      param_specs = g_object_interface_list_properties(interface, &cnt);
      g_type_default_interface_unref(interface);
    }
  }

  //This extra check avoids an occasional crash, for instance for GVolume
  if(!param_specs){
    cnt = 0;
  }

  for(guint i = 0; i < cnt; i++)
  {
    GParamSpec* param_spec = param_specs[i];
    // Generate the property if the specified gtype actually owns the property.
    // (Generally all properties, including any base classes' properties are
    // retrieved by g_object_interface_list_properties() for a given gtype.
    // The base classes' properties should not be generated).
    if(param_spec && param_spec->owner_type == gtype)
    {
      out.push_back(ParamAdapter());
      out.back().param_spec = param_spec;

      // name and type:
      out.back().name = param_spec->name;
      out.back().spec_type = G_PARAM_SPEC_TYPE(param_spec);
      out.back().value_type = param_spec->value_type;

      // blurb
      const gchar* blurb = g_param_spec_get_blurb(param_spec);
      out.back().blurb = (blurb) ? blurb : "";

      // flags:
      GParamFlags flags = param_spec->flags;
      out.back().readable = (flags & G_PARAM_READABLE) == G_PARAM_READABLE;
      out.back().writable = (flags & G_PARAM_WRITABLE) == G_PARAM_WRITABLE;
      out.back().construct_only = (flags & G_PARAM_CONSTRUCT_ONLY) == G_PARAM_CONSTRUCT_ONLY;
    }
  }

  g_free(param_specs);
  return out;
}



ElementUI::ElementUI(ElementUIViewMode view_mode):
  Gtk::Grid(),
  m_name(), m_optionmenu(), m_viewmode(view_mode),
  m_message()
{
  m_name.set_text("No element selected");
  m_name.set_justify(Gtk::JUSTIFY_RIGHT);
  m_name.set_size_request(150, -1);
  m_name.set_padding(2, 0);
  this->attach(m_name, 0, 0, 1, 1 );
  this->attach(m_optionmenu, 1, 0, 1, 1 );

  m_name.show();

  m_optionmenu.set_visible(m_viewmode == ELEMENT_UI_VIEW_MODE_COMPACT);

}

bool ElementUI::should_hide_parameter(ParamAdapter & adapter)
{
  //  if ( (adapter.readable && adapter.writable) ||
  //       (adapter.readable && m_show_readonly)  ||
  //       (adapter.writable && m_show_writeonly) &&
  //       ! (adapter.name in m_exclude_params) )
  //  {

  //  }
  return false;
}

void ElementUI::set_element(Glib::RefPtr<Gst::Object> & element)
{
  this->clear_element();

  m_element = element;
  if (!m_element){
    m_name.set_text("No element selected");
    return;
  }

  m_params = list_klass_parameters(element);

  //  params.erase(std::remove_if(params.begin(), params.end(),
  //                              sigc::mem_fun(*this, &ElementUI::should_hide_parameter)));

  if (m_params.size())
  {
    if(m_viewmode == ELEMENT_UI_VIEW_MODE_COMPACT)
    {
      //compact layout. 1 entry per param
      for(std::vector<ParamAdapter>::iterator it = m_params.begin();
          it != m_params.end(); ++it)
      {
        m_optionmenu.append(it->name);
      }

      m_optionmenu.set_visible(true);

      //        g_signal_connect (G_OBJECT (menu), "deactivate",
      //            G_CALLBACK (on_optionmenu_deactivate), ui);
    }
    else
    {
      //full layout
      for(int i = 0; i < m_params.size(); ++i)
      {
        ElementUIPropView* param_view = ElementUIPropView::create(m_element, m_params[i]);

        if(!param_view) continue;

        //param_view = Gtk::manage(param_view);

        Gtk::Label* param_label = Gtk::manage(new Gtk::Label(m_params[i].name));
        param_label->show();
        param_label->set_tooltip_text(m_params[i].blurb);
        this->attach(*param_label, 0, i+2, 1, 1);
        m_param_labels.push_back(param_label);

        param_view->show();
        this->attach(*param_view, 1, i+2, 1, 1);
        m_param_views.push_back(param_view);
      }
    }

    //        if (g_object_class_find_property (G_OBJECT_GET_CLASS (ui->element),
    //                "filesize")
    //            && g_object_class_find_property (G_OBJECT_GET_CLASS (ui->element),
    //                "offset"))
    //          offset_hack (ui);

  }
  else // element has no properties
  {
    m_message.set_text("This element has no properties.");
    m_message.show();
    this->attach(m_message, 0, 2, 1, 1);
    
  }

  std::stringstream ss;
  //TODO
  //ss << m_element->get_factory()->get_metadata("long-name") << ": " << m_element->get_name();
  //m_name.set_text(ss.str());
  debug ("done setting element");
}

void ElementUI::clear_element()
{
  debug ("doing cleanup");

  for(int i = 0; i < m_param_views.size(); ++i)
  {
    this->remove(*(m_param_labels[i]));
    this->remove(*(m_param_views[i]));
    delete m_param_views[i];
  }
  m_params.clear();
  m_param_labels.clear();
  m_param_views.clear();

  m_message.hide();
}
