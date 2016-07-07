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

#include <qstring.h>
#include <qtranslator.h>
#include <qdatetime.h>

#ifndef QTODO_MISC
#define QTODO_MISC

#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct QTodoMisc
{
	static void loadTranslator(const QString&);

	static int mainWidgetToCurrentDisplay();
	static int windowToCurrentDisplay(const Window&);
	static void debug(const QString& = 0, void* = 0);

	static QString dateTime2LocalString(const QDateTime&);
	static QString shortText(const QString&, uint len);
	static QString timeSpanUntilDateTimeString(const QDateTime&);

	static const QString ISODate;
	static const QString ISOTime;
	static const QString ISODateTime;

	static QString browseDirectory(const QString&);
	static QString browseFile(const QString&);

	private:
	/* declarations of static functions *//*{{{*/
	static int client_msg(Display *disp, Window win, char *msg, unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3, unsigned long data4);
	static int activate_window (Display *disp, Window win, bool switch_desktop);
	static int window_to_desktop (Display *disp, Window win, int desktop);
	static char *get_property (Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size);

	static const int MAX_PROPERTY_VALUE_LEN;

	static int debug_count;

	static QTranslator qt_translator;
	static QTranslator qtodo_translator;
};

#endif

