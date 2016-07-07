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

#include <map>

#include <qpixmap.h>
#include <qimage.h>

#ifndef QTODO_XPM
#define QTODO_XPM

class QTodoImage
{
	public:
	QTodoImage(char**);
	QPixmap s(int,int);
	operator QPixmap() {return *pixmapPointer();};
	
	QPixmap pixmap() {return *pixmapPointer();};
	QImage* imagePointer();
	QPixmap* pixmapPointer();
	char** xpm();

	private:
	std::map<const char*,QPixmap> scaled;

	QPixmap* m_pixmap;
	QImage* m_image;
	char** m_xpm;
};

class QTodoXPM
{
	public:
	static QPixmap get(char**);
	static QPixmap get(char**,int,int);

	static QTodoImage rightarrow;
	static QTodoImage todo_list;
	static QTodoImage exit;
	static QTodoImage help;
	static QTodoImage configure;
	static QTodoImage yellow_plus;
	static QTodoImage yellow_minus;
	static QTodoImage undo;
	static QTodoImage print;
	static QTodoImage not_dead;
	static QTodoImage save_all;
	static QTodoImage editcut; 
	static QTodoImage editcopy;
	static QTodoImage editpaste;
	static QTodoImage editdelete;
	static QTodoImage sort;
	static QTodoImage add_top;
	static QTodoImage add_bottom;
	static QTodoImage done;
	static QTodoImage in_progress;
	static QTodoImage not_available;
	static QTodoImage planning;
	static QTodoImage high;
	static QTodoImage normal;
	static QTodoImage low;
	static QTodoImage todo_section;
	static QTodoImage calendar;
	static QTodoImage clock;
	static QTodoImage any;
	static QTodoImage not_not_available;
	static QTodoImage not_low;
	static QTodoImage not_normal;
	static QTodoImage not_high;
	static QTodoImage not_planning;
	static QTodoImage not_in_progress;
	static QTodoImage not_done;
	static QTodoImage todo_branch;
	static QTodoImage dead;
	static QTodoImage show_more;
	static QTodoImage show_less;
	static QTodoImage add_sub;
	static QTodoImage add_above;
	static QTodoImage add_below;
	static QTodoImage descending;
	static QTodoImage ascending;
	static QTodoImage transparency;
	static QTodoImage export_list;
	static QTodoImage folder;
	static QTodoImage text_bold;
	static QTodoImage text_italic;
	static QTodoImage text_under;
	static QTodoImage text_strike;
	static QTodoImage hyperlink;
	static QTodoImage eraser;
	static QTodoImage edit;
	static QTodoImage play;
	static QTodoImage filefind;
	static QTodoImage archiving;
	static QTodoImage reload;
	static QTodoImage close;
	static QTodoImage select_all;
	static QTodoImage deselect_all;
	static QTodoImage hdd;
};

typedef QTodoXPM QTP;

#endif

