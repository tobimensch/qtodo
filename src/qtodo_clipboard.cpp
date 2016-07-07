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

#include <qclipboard.h>
#include <qapplication.h>

#include "qtodo_item.h"
#include "qtodo_item_widget.h"
#include "qtodo_clipboard.h"

QTodoClipboard* QTodoClipboard::_instance = 0;


QTodoClipboard::QTodoClipboard()
{
	cut = false;
}

bool QTodoClipboard::isEmpty()
{
	return items.empty();
}

void QTodoClipboard::clear()
{
	if(cut)
		cut = false;

	for(QTodoItemVector::iterator it = items.begin(); it != items.end(); ++it)
		delete (*it);
	
	items.clear();
}

void QTodoClipboard::add(QTodoItem* item)
{
	if(cut)
		items.push_back(item);
	else
		items.push_back(new QTodoItem(*item));
}

void QTodoClipboard::add(std::vector<QTodoItem*> _items)
{
	if(cut)
	{
		items.insert(items.end(),_items.begin(),_items.end());
	}
	else
	{
		for(std::vector<QTodoItem*>::iterator it = _items.begin(); it != _items.end(); ++it)
			add((*it));
	}
}

QTodoItemVector* QTodoClipboard::getItems()
{
	return &items;
}

void QTodoClipboard::setCut(bool b)
{
	cut = b;
}

void QTodoClipboard::doExport()
{
	QString string;
	QTextStream stream(&string, IO_WriteOnly);
	for(QTodoItemVector::iterator it = items.begin(); it != items.end(); ++it)
		(*it)->save(stream);
	
	QClipboard* cp = QApplication::clipboard();
	cp->setText(string,QClipboard::Clipboard);
}

int QTodoClipboard::maxDepth()
{
	int max = -1;
	for(QTodoItemVector::iterator it = items.begin(); it != items.end(); ++it)
		if((*it)->getDepth() > max)
			max = (*it)->getDepth();
	return max;
}

int QTodoClipboard::minDepth()
{
	int min = -1;
	for(QTodoItemVector::iterator it = items.begin(); it != items.end(); ++it)
		if(min == -1 || (*it)->getDepth() < min)
			min = (*it)->getDepth();
	return min;
}

