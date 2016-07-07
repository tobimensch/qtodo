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

#include <qfont.h>

#ifndef QTODO_FONT
#define QTODO_FONT

class QTodoFont
{
	public:
	~QTodoFont();
	static QTodoFont* get() { _instance ? _instance : _instance = new QTodoFont(); return _instance; }

	QFont bold;
	QFont small_bold;
	QFont small;
	QFont big;
	QFont underline;

	static QWidget* setFont(QWidget*,const QFont&);

	private:
	QTodoFont();
	
	static QTodoFont* _instance;
};

typedef QTodoFont QF;

#endif

