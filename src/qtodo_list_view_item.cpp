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
 
#include "qtodo_list_view_item.h"
#include "qtodo_list_widget.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_section_widget.h"
#include "qtodo_xpm.h"
#include "qtodo_list.h"

QTodoListViewItem::QTodoListViewItem(QListView* parent, const QString& file, const QString& name) : QListViewItem(parent,name)
{
	setPixmap(0,QTP::todo_list.s(32,32));
	
	todo_list_widget = new QTodoListWidget(this);
	todo_list_widget->setFile(file);
	if(!name.isEmpty())
		todo_list_widget->getHeader()->setName(name);
}

bool QTodoListViewItem::load()
{
	bool r = todo_list_widget->load();
	if(r)
		update();
	return r;
}

void QTodoListViewItem::save()
{
	todo_list_widget->save();
}

QTodoList* QTodoListViewItem::list()
{
	return todo_list_widget->getList();
}

QTodoListWidget* QTodoListViewItem::listWidget()
{
	return todo_list_widget;
}

void QTodoListViewItem::update()
{
	setText(0,todo_list_widget->getHeader()->getName());
}

QTodoListViewItemSection::QTodoListViewItemSection(QTodoListViewItem* parent) : QListViewItem(parent,"")
{
 	setPixmap(0,QTP::todo_section.s(16,16));
}

void QTodoListViewItemSection::setSection(QTodoSectionWidget* _todo_section_widget)
{
	todo_section_widget = _todo_section_widget;
}

QTodoSectionWidget* QTodoListViewItemSection::section()
{
	return todo_section_widget;
}

void QTodoListViewItemSection::update()
{
	setText(0,section()->getName());
}

void QTodoListViewItem::preserveContentsYPos()
{
	list()->preserveContentsYPos();
}

void QTodoListViewItem::restoreContentsYPos()
{
	list()->restoreContentsYPos();
}


