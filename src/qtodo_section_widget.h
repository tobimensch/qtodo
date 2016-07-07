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

#include <qframe.h>
 
#ifndef QTODO_SECTION_WIDGET
#define QTODO_SECTION_WIDGET

class QLineEdit;
class QTodoListViewItemSection;

///Widget representing the start
///of a section inside a TODOlist
class QTodoSectionWidget : public QFrame
{
	Q_OBJECT

	public:
	QTodoSectionWidget(QWidget*,QTodoListViewItemSection*);
	void setName(const QString&);
	QString getName();
	void save(QTextStream&);

	private slots:
	void nameChanged(const QString&);
	
	private:
	QLineEdit* edit;
	QTodoListViewItemSection* lists_item;
};

#endif

