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

#ifndef __GST_ELEMENT_BROWSER_H__
#define __GST_ELEMENT_BROWSER_H__

#include <gstreamermm.h>
#include <gtkmm.h>

#include "caps-tree.h"


namespace Gste
{


class ElementBrowser : public Gtk::Dialog
{ 
public:
    ElementBrowser();

    Glib::RefPtr<Gst::ElementFactory> pick_modal();


private:
    void on_tree_selection_changed(Glib::RefPtr<Gst::ElementFactory>& factory);
    void on_tree_activated(Glib::RefPtr<Gst::ElementFactory> &factory);


private:
    Glib::RefPtr<Gst::ElementFactory> m_factory;
    Glib::RefPtr<Gst::Element>        m_element;

    ElementCapsTree        m_pads;
    ElementFactoryCapsTree m_padtemplates;


private:
    Gtk::Label m_longname;
    Gtk::Label m_description;
    Gtk::Label m_author;
};

} //namespace Gste

#endif // __GST_ELEMENT_BROWSER_H__
