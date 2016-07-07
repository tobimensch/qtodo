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

#include <qlistview.h>
 
#ifndef QTodoListItem
#define QTodoListItem

class QIconViewItem;
class QTodoList;
class QTodoListWidget;
class QTodoSectionWidget;

///Represents a TODOlist in
///the list of TODOlists (QTodoLists).
class QTodoListViewItem : public QListViewItem
{
	public:
	QTodoListViewItem(QListView*, const QString&, const QString& = 0);
	QTodoList* list();
	QTodoListWidget* listWidget();
	bool load();
	void save();
	void update();
	void preserveContentsYPos();
	void restoreContentsYPos();
	
	private:
	QTodoListWidget* todo_list_widget;
	int preserved_contents_y_pos;
};

///Represents a section in
///the list of TODOlists (QTodoLists)
class QTodoListViewItemSection : public QListViewItem
{
	public:
	QTodoListViewItemSection(QTodoListViewItem*);
	void setSection(QTodoSectionWidget*);
	QTodoSectionWidget* section();
	void update();
	
	private:
	QTodoSectionWidget* todo_section_widget;
};

#endif

