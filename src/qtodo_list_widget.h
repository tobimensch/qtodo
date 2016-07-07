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

#include <qwidget.h>

#include "qtodo_parser.h"

#ifndef QTODO_LIST_WIDGET
#define QTODO_LIST_WIDGET

class QTodoList;
class QLineEdit;
class QTodoListHeaderWidget;
class QTodoListViewItem;
class QTodoListFilter;

///Widget that contains all parts
///of a TODOlist: the head, the properties
///the real TODOlist
class QTodoListWidget : public QWidget
{
	Q_OBJECT

	public:
	QTodoListWidget(QTodoListViewItem*);
	virtual ~QTodoListWidget();
	QTodoList* getList();
	void setFile(const QString&);
	QString getFile();
	bool load();
	void save();
	QTodoListHeaderWidget* getHeader();
	QTodoListViewItem* getListViewItem();
	QTodoListFilter* getListFilter();
	void configure();
	void setModified();
	bool isModified();

	private slots:
	void addTodo();
	void addSection();

	signals:
	void modified();
	
	private:	
	void createBackup();

	QTodoListFilter* filter;
	QTodoList* list;
	QTodoListHeaderWidget* header;
	QTodoListViewItem* lists_item;
	
	QString file;
	bool m_modified;
};

#endif

