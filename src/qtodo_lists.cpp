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

#include <qpopupmenu.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qfile.h>

#include "qtodo_lists.h"
#include "qtodo_list_view_item.h"
#include "qtodo_list.h"
#include "qtodo_section_widget.h"
#include "qtodo_list_widget.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_radio_dialog.h"
#include "qtodo_xpm.h"
#include "qtodo_undo_manager.h"

QTodoLists::QTodoLists(QWidget* parent) : QListView(parent)
{
	connect(this,SIGNAL(contextMenuRequested(QListViewItem * , const QPoint & , int )),this,SLOT(contextMenu(QListViewItem * , const QPoint & , int )));
	connect(this,SIGNAL(selectionChanged()),this,SLOT(listChange()));
	connect(this,SIGNAL(listChanged()),this,SLOT(prepareList()));

	current = 0;
}

void QTodoLists::addSection(QTodoListViewItem* item)
{
	QString name = QInputDialog::getText("Add section","Name");
	if(name.isEmpty())
		return;

	QTodoSectionWidget* section = item->list()->addSection();
	QTodoListViewItemSection* _item = new QTodoListViewItemSection(item);
	_item->setSection(section);
	_item->section()->setName(name);
	_item->update();

	item->setOpen(true);
}

void QTodoLists::contextMenu(QListViewItem* item, const QPoint & pos, int col)
{
	if(dynamic_cast<QTodoListViewItem*>(item))
		contextMenuListItem(item,pos,col);
	else if(dynamic_cast<QTodoListViewItemSection*>(item))
		contextMenuSectionItem(item,pos,col);
}

void QTodoLists::contextMenuListItem(QListViewItem*, const QPoint & pos, int)
{
	return; //FIXME

	QPopupMenu* menu = new QPopupMenu(this);
	menu->insertItem(tr("Add section"),MNID_ADD_SECTION);
	connect(menu,SIGNAL(activated(int)),this,SLOT(listMenuActivated(int)));
	menu->exec(pos);
	delete menu;
}

void QTodoLists::contextMenuSectionItem(QListViewItem*, const QPoint& pos, int)
{
	QPopupMenu* menu = new QPopupMenu(this);
	menu->insertItem(tr("GoTo"),MNID_GOTO_SECTION);
	connect(menu,SIGNAL(activated(int)),this,SLOT(sectionMenuActivated(int)));
	menu->exec(pos);
	delete menu;
}

void QTodoLists::listMenuActivated(int id)
{
	switch(id)
	{
		case MNID_ADD_SECTION:
			addSection(dynamic_cast<QTodoListViewItem*>(selectedItem()));
			break;
		default:
			break;
	}
}

void QTodoLists::sectionMenuActivated(int id)
{
	switch(id)
	{
		case MNID_GOTO_SECTION:
			if(QTodoListViewItemSection* item = dynamic_cast<QTodoListViewItemSection*>(selectedItem()))
			{
				currentList()->setContentsPos(0,item->section()->y());
			}
			break;
		default:
			break;
	}
}

QTodoList* QTodoLists::currentList()
{
	if(QTodoListViewItem* item = currentListItem())
		return item->list();
	else
		return NULL;
}



void QTodoLists::listChange()
{
	if(current != currentListItem())
	{
		current = currentListItem();
		emit listChanged();
	}
	if(QTodoListViewItemSection* item = dynamic_cast<QTodoListViewItemSection*>(selectedItem()))
	{
		currentList()->setContentsPos(0,item->section()->y());
	}
}

QTodoListViewItem* QTodoLists::currentListItem()
{
	if(!selectedItem())
		return NULL;

	if(dynamic_cast<QTodoListViewItem*>(selectedItem()))
		return dynamic_cast<QTodoListViewItem*>(selectedItem());
	else if(dynamic_cast<QTodoListViewItem*>(selectedItem()->parent()))
		return dynamic_cast<QTodoListViewItem*>(selectedItem()->parent());
	else
		return NULL;
}

QTodoListWidget* QTodoLists::currentListWidget()
{
	if(QTodoListViewItem* item = currentListItem())
		return item->listWidget();
	else
		return NULL;
}

void QTodoLists::setCurrent(const QString& file, int pos)
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
		{
			if(dynamic_cast<QTodoListViewItem*>(it.current())->listWidget()->getFile() == file)
			{
				setSelected(it.current(),true);

				dynamic_cast<QTodoListViewItem*>(it.current())->list()->adjustSize();
				dynamic_cast<QTodoListViewItem*>(it.current())->list()->setContentsPos(0,pos);

				emit listChanged();
				return;
			}
		}
	}
}

void QTodoLists::setCurrent(QTodoList* _current)
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
		{
			if(dynamic_cast<QTodoListViewItem*>(it.current())->list() == _current)
			{
				setSelected(it.current(),true);

				emit listChanged();
				return;
			}
		}
	}
}

void QTodoLists::contentsWheelEvent(QWheelEvent* event)
{
	if(!firstChild())
		return;

	QListViewItem* item = selectedItem();
	if(event->delta() < 0)
	{
		if(!item)
			item = lastItem();

		item = item->itemBelow();
		if(!item)
			item = firstChild();
	}
	else
	{
		if(!item)
			item = firstChild();

		item = item->itemAbove();
		if(!item)
			item = lastItem();
	}
	setSelected(item,true);
}

enum
{
	LRID_FILESYSTEM,
	LRID_QTODO
};

void QTodoLists::removeCurrent()
{
	if(QTodoListViewItem* item = currentListItem())
	{
		QTodoRadioDialog radio_dialog(tr("Remove list"),tr("You are about to remove the ")+item->listWidget()->getHeader()->getName()+tr("-list."));
		radio_dialog.setIcon(QTP::yellow_minus.s(12,12));	

		radio_dialog.add(QTP::hdd,tr("Remove from filesystem"),LRID_FILESYSTEM);
		radio_dialog.add(QTP::todo_list.s(32,32),tr("Remove from QTodo"),LRID_QTODO);
		if(radio_dialog.exec() != QDialog::Accepted)
			return;

		QString file;
		switch(radio_dialog.selected())
		{
			case LRID_FILESYSTEM:
				file = item->listWidget()->getFile();
			case LRID_QTODO:
				QTUM::get()->deleteUndosRelatedToList(item->list());
				delete item->listWidget();
				delete item;
				if(firstChild())
					setSelected(firstChild(),true);
				else
					emit listChanged();
				if(!file.isEmpty())
					QFile::remove(file);
				break;
		}
	}
}

void QTodoLists::save()
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
			dynamic_cast<QTodoListViewItem*>(it.current())->save();
	}
}

void QTodoLists::prepareList()
{
	if(current)
		current->list()->prepareList();
}

int QTodoLists::numLists()
{
	QListViewItem* item = firstChild();
	int i = 0;
	for(;item;item = item->nextSibling(), ++i)
	{}
	return i;
}

bool QTodoLists::modifiedList()
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
		{
			if(dynamic_cast<QTodoListViewItem*>(it.current())->listWidget()->isModified())
				return true;
		}
	}
	return false;
}

void QTodoLists::preserveContentsPositions()
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
			dynamic_cast<QTodoListViewItem*>(it.current())->preserveContentsYPos();
	}
}

void QTodoLists::restoreContentsPositions()
{
	QListViewItemIterator it(this);
	for(;it.current();++it)
	{
		if (dynamic_cast<QTodoListViewItem*>(it.current()))
			dynamic_cast<QTodoListViewItem*>(it.current())->restoreContentsYPos();
	}
}


