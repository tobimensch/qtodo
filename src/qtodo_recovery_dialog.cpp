/***************************************************************************
 *   Copyright (C) 2006 by Tobias Glaesser                                 *
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
#include <qfileinfo.h>
#include <qdir.h>

#include "qtodo_recovery_dialog.h"
#include "qtodo_list_view_item.h"
#include "qtodo_list_widget.h"
#include "qtodo_list_header_widget.h"

class QTodoRecoveryListViewItem : public QListViewItem
{
	public:
	QTodoRecoveryListViewItem(QListView* parent) : QListViewItem(parent) {};

	QTodoListViewItem* list_view_item;
};

QTodoRecoveryDialog::QTodoRecoveryDialog(QWidget* parent, const char* name) : QTodoRecoveryDialogBase(parent,name)
{
}

void QTodoRecoveryDialog::add(QTodoListViewItem* list_view_item)
{
	QString last_save;
	QString last_backup;

	QFileInfo file_info(list_view_item->listWidget()->getFile());
	if(file_info.exists())
		last_save = file_info.lastModified().toString(Qt::LocalDate);

	file_info.setFile(list_view_item->listWidget()->getFile()+'~');
	if(file_info.exists())
		last_backup = file_info.lastModified().toString(Qt::LocalDate);

	QTodoRecoveryListViewItem* item = new QTodoRecoveryListViewItem(list);
	item->setText(0,list_view_item->listWidget()->getFile());
	item->setText(1,last_save);
	item->setText(2,last_backup);
	item->list_view_item = list_view_item;
}

void QTodoRecoveryDialog::recover()
{
	QFileInfo file_info;
	QDir dir;

	QListViewItem* item = list->firstChild();
	for(;item; item = item->nextSibling())
	{
		if(item->text(2).isEmpty())
		{ //we can't recover from a backup file, so we at least (try to) restore the todo-list-name :(
			file_info.setFile(item->text(0));
			dynamic_cast<QTodoRecoveryListViewItem*>(item)->list_view_item->listWidget()->getHeader()->setName(file_info.fileName());
			dynamic_cast<QTodoRecoveryListViewItem*>(item)->list_view_item->update();
			dynamic_cast<QTodoRecoveryListViewItem*>(item)->list_view_item->listWidget()->setModified();
			dynamic_cast<QTodoRecoveryListViewItem*>(item)->list_view_item->save();
		}
		else
		{
			dir.rename(item->text(0)+'~',item->text(0),true);
			dynamic_cast<QTodoRecoveryListViewItem*>(item)->list_view_item->load();
		}
	}
}


