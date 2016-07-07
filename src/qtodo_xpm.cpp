/***************************************************************************
 *   Copyright (C) 2005 by Tobias Glaesser                                 *
 *   tobi.web@gmx.de                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qimage.h>

#include "qtodo_xpm.h"

#include "rightarrow.xpm"
#include "todo_list.xpm"
#include "exit.xpm"
#include "help.xpm"
#include "configure.xpm"
#include "yellow_plus.xpm"
#include "yellow_minus.xpm"
#include "undo.xpm"
#include "print.xpm"
#include "not_dead.xpm"
#include "save_all.xpm"
#include "editcut.xpm"
#include "editcopy.xpm"
#include "editpaste.xpm"
#include "editdelete.xpm"
#include "sort.xpm"
#include "add_top.xpm"
#include "add_bottom.xpm"
#include "done.xpm"
#include "in_progress.xpm"
#include "not_available.xpm"
#include "planning.xpm"
#include "high.xpm"
#include "normal.xpm"
#include "low.xpm"
#include "todo_section.xpm"
#include "calendar.xpm"
#include "clock.xpm"
#include "any.xpm"
#include "not_not_available.xpm"
#include "not_low.xpm"
#include "not_normal.xpm"
#include "not_high.xpm"
#include "not_planning.xpm"
#include "not_in_progress.xpm"
#include "not_done.xpm"
#include "todo_branch.xpm"
#include "dead.xpm"
#include "show_more.xpm"
#include "show_less.xpm"
#include "add_sub.xpm"
#include "add_above.xpm"
#include "add_below.xpm"
#include "descending.xpm"
#include "ascending.xpm"
#include "transparency.xpm"
#include "export_list.xpm"
#include "folder.xpm"
#include "text_bold.xpm"
#include "text_italic.xpm"
#include "text_under.xpm"
#include "text_strike.xpm"
#include "hyperlink.xpm"
#include "edit.xpm"
#include "eraser.xpm"
#include "play.xpm"
#include "filefind.xpm"
#include "archiving.xpm"
#include "reload.xpm"
#include "close.xpm"
#include "select_all.xpm"
#include "deselect_all.xpm"
#include "hdd.xpm"

QTodoImage QTodoXPM::rightarrow = rightarrow_xpm;
QTodoImage QTodoXPM::todo_list = todo_list_xpm;
QTodoImage QTodoXPM::exit = exit_xpm;
QTodoImage QTodoXPM::help = help_xpm;
QTodoImage QTodoXPM::configure = configure_xpm;
QTodoImage QTodoXPM::yellow_plus = yellow_plus_xpm;
QTodoImage QTodoXPM::yellow_minus = yellow_minus_xpm;
QTodoImage QTodoXPM::undo = undo_xpm;
QTodoImage QTodoXPM::print = print_xpm;
QTodoImage QTodoXPM::not_dead = not_dead_xpm;
QTodoImage QTodoXPM::save_all = save_all_xpm;
QTodoImage QTodoXPM::editcut = editcut_xpm;
QTodoImage QTodoXPM::editcopy = editcopy_xpm;
QTodoImage QTodoXPM::editpaste = editpaste_xpm;
QTodoImage QTodoXPM::editdelete = editdelete_xpm;
QTodoImage QTodoXPM::sort = sort_xpm;
QTodoImage QTodoXPM::add_top = add_top_xpm;
QTodoImage QTodoXPM::add_bottom = add_bottom_xpm;
QTodoImage QTodoXPM::done = done_xpm;
QTodoImage QTodoXPM::in_progress = in_progress_xpm;
QTodoImage QTodoXPM::not_available = not_available_xpm;
QTodoImage QTodoXPM::planning = planning_xpm;
QTodoImage QTodoXPM::high = high_xpm;
QTodoImage QTodoXPM::normal = normal_xpm;
QTodoImage QTodoXPM::low = low_xpm;
QTodoImage QTodoXPM::todo_section = todo_section_xpm;
QTodoImage QTodoXPM::calendar = calendar_xpm;
QTodoImage QTodoXPM::clock = clock_xpm;
QTodoImage QTodoXPM::any = any_xpm;
QTodoImage QTodoXPM::not_not_available = not_not_available_xpm;
QTodoImage QTodoXPM::not_low = not_low_xpm;
QTodoImage QTodoXPM::not_normal = not_normal_xpm;
QTodoImage QTodoXPM::not_high = not_high_xpm;
QTodoImage QTodoXPM::not_planning = not_planning_xpm;
QTodoImage QTodoXPM::not_in_progress = not_in_progress_xpm;
QTodoImage QTodoXPM::not_done = not_done_xpm;
QTodoImage QTodoXPM::todo_branch = todo_branch_xpm;
QTodoImage QTodoXPM::dead = dead_xpm;
QTodoImage QTodoXPM::show_more = show_more_xpm;
QTodoImage QTodoXPM::show_less = show_less_xpm;
QTodoImage QTodoXPM::add_sub = add_sub_xpm;
QTodoImage QTodoXPM::add_above = add_above_xpm;
QTodoImage QTodoXPM::add_below = add_below_xpm;
QTodoImage QTodoXPM::descending = descending_xpm;
QTodoImage QTodoXPM::ascending = ascending_xpm;
QTodoImage QTodoXPM::transparency = transparency_xpm;
QTodoImage QTodoXPM::export_list = export_list_xpm;
QTodoImage QTodoXPM::folder = folder_xpm;
QTodoImage QTodoXPM::text_bold = text_bold_xpm;
QTodoImage QTodoXPM::text_italic = text_italic_xpm;
QTodoImage QTodoXPM::text_under = text_under_xpm;
QTodoImage QTodoXPM::text_strike = text_strike_xpm;
QTodoImage QTodoXPM::hyperlink = hyperlink_xpm;
QTodoImage QTodoXPM::edit = edit_xpm;
QTodoImage QTodoXPM::eraser = eraser_xpm;
QTodoImage QTodoXPM::play = play_xpm;
QTodoImage QTodoXPM::filefind = filefind_xpm;
QTodoImage QTodoXPM::archiving = archiving_xpm;
QTodoImage QTodoXPM::reload = reload_xpm;
QTodoImage QTodoXPM::close = close_xpm;
QTodoImage QTodoXPM::select_all = select_all_xpm;
QTodoImage QTodoXPM::deselect_all = deselect_all_xpm;
QTodoImage QTodoXPM::hdd = hdd_xpm;

QPixmap QTodoXPM::get(char** name)
{
	return QImage(const_cast<const char**>(name));
}

QPixmap QTodoXPM::get(char** name, int width, int height)
{
	return QImage(const_cast<const char**>(name)).smoothScale(width,height);
}

QTodoImage::QTodoImage(char** _xpm)
{
	m_xpm = _xpm;
}

QPixmap QTodoImage::s(int w,int h)
{
	QString size;
	size[0] = (char)w;
	size[1] = (char)h;
	if(!scaled[size.ascii()].isNull())
	{
		return scaled[size.ascii()];
	}
	else
	{
		scaled[size.ascii()] = imagePointer()->smoothScale(w,h);
		return scaled[size.ascii()];
	}	
}

QImage* QTodoImage::imagePointer()
{
	if(!m_image)
		m_image = new QImage(m_xpm);

	return m_image;
}

QPixmap* QTodoImage::pixmapPointer()
{
	if(!m_pixmap)
		m_pixmap = new QPixmap(*imagePointer());

	return m_pixmap;
}


