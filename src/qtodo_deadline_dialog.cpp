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

#include <qapplication.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qlabel.h>

#include "qtodo_deadline_dialog.h"
#include "qtodo_widget.h"
#include "qtodo_lists.h"
#include "qtodo_list.h"
#include "qtodo_list_view_item.h"
#include "qtodo_item.h"
#include "qtodo_parser.h"
#include "qtodo_settings_container.h"
#include "qtodo_misc.h"
#include "qtodo_xpm.h"
#include "qtodo_print_preview.h"
#include "qtodo_font.h"

enum
{
	DMID_SHOW,
	DMID_SETDONE,
	DMID_DELAY_1HOUR,
	DMID_DELAY_1DAY,
	DMID_DELAY_1WEEK,
	DMID_DELAY_1MONTH
};

QPtrList<QTodoDeadlineDialog> QTodoDeadlineDialog::instances;
QDateTime QTodoDeadlineDialog::last_deadline_warning;

QTodoDeadlineListViewItem::QTodoDeadlineListViewItem(QListView* parent) : QListViewItem(parent), todo(0)
{
}

int QTodoDeadlineListViewItem::compare(QListViewItem* i,int col, bool ascending) const
{
	if(col == 0) //deadline expire time
	{
		return (QDateTime::currentDateTime().secsTo(todo->getDeadlineDateTime()) - QDateTime::currentDateTime().secsTo((dynamic_cast<QTodoDeadlineListViewItem*>(i))->todo->getDeadlineDateTime()));
	}
	else if(col == 2) //priority
	{
		return todo->getPriority() - (dynamic_cast<QTodoDeadlineListViewItem*>(i))->todo->getPriority();
	}
	else if(col == 3) //status
	{
		return todo->getStatus() - (dynamic_cast<QTodoDeadlineListViewItem*>(i))->todo->getStatus();
	}
	else
		return QListViewItem::compare(i,col,ascending);
}


QPixmap createPixmap(int width, int height, const QColor& col)
{
	QPixmap pixmap(width,height);
	pixmap.fill(col);
	return pixmap;
}

QTodoDeadlineDialog::QTodoDeadlineDialog()
{
	setIcon(QTP::not_dead.s(16,16));

	connect(list,SIGNAL(doubleClicked(QListViewItem*, const QPoint &, int)),this,SLOT(slotDoubleClicked(QListViewItem*, const QPoint &, int )));
	connect(list,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(activated(QListViewItem*)));
	connect(list,SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&,int)),this,SLOT(slotContextMenuRequested(QListViewItem*, const QPoint&,int)));
	connect(list,SIGNAL(spacePressed(QListViewItem*)),this,SLOT(activated(QListViewItem*)));
	connect(reload_button,SIGNAL(clicked()),this,SLOT(reload()));
	connect(print_button,SIGNAL(clicked()),this,SLOT(print()));
	list->setAllColumnsShowFocus(true);

	init();

	instances.append(this);
}

QTodoDeadlineDialog::~QTodoDeadlineDialog()
{
	instances.remove(this);
}

void QTodoDeadlineDialog::slotDoubleClicked(QListViewItem* _item, const QPoint&, int)
{
	activated(_item);
}

void QTodoDeadlineDialog::activated(QListViewItem* _item)
{
	QTodoDeadlineListViewItem* item = dynamic_cast<QTodoDeadlineListViewItem*>(_item);
	if(!item)
		return;

	QTodoWidget* todo_widget = dynamic_cast<QTodoWidget*>(qApp->mainWidget());
	QTodoLists* todo_lists = todo_widget->todoLists();
	todo_lists->setCurrent(item->todo->list());
	item->todo->list()->setContentsPos(0,item->todo->y());
	item->todo->list()->deselectAll();
	item->todo->setSelected(true);

	todo_widget->advShow();
	showNormal();
	QTodoMisc::windowToCurrentDisplay(winId());

	QPoint pos = item->todo->mapToGlobal(QPoint(0,item->todo->height()));
	if(pos.y() + height() > QApplication::desktop()->height())
	{
		pos = item->todo->mapToGlobal(QPoint(0,-height()-28));
		if(pos.y() < 28)
			pos = item->todo->mapToGlobal(QPoint(0,100));
	}
	move(pos);
}

void QTodoDeadlineDialog::init()
{
	QTodoLists* todo_lists = dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists();
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QTodoListIterator list_it(item->list());
		for(;list_it.current();++list_it)
		{
			if(QTodoItem* todo_item = dynamic_cast<QTodoItem*>(list_it.current()))
			{
				if(todo_item->getDeadlineString().isEmpty() || todo_item->getStatus() == STAT_DONE)
					continue;

				QColor state_col = todo_item->getCurrentDeadlineColor();
				QPixmap state_pixmap = createPixmap(32,16,state_col);

				QString time_to_deadline = QTodoMisc::timeSpanUntilDateTimeString(todo_item->getDeadlineDateTime());

				QTodoDeadlineListViewItem* list_item = new QTodoDeadlineListViewItem(list);
				list_item->todo = todo_item;
				list_item->setText(0,time_to_deadline);
				list_item->setText(1,item->text(0));
				list_item->setText(2,QTodo::priorityToString(todo_item->getPriority()));
				list_item->setText(3,QTodo::statusToString(todo_item->getStatus()));
				list_item->setText(4,todo_item->getTaskPlainText().replace("\n"," "));
				list_item->setPixmap(0,state_pixmap);
				list_item->setPixmap(2,QTodo::priorityToPixmap(todo_item->getPriority()));
				list_item->setPixmap(3,QTodo::statusToPixmap(todo_item->getStatus()));
			}
		}	
	}
	QTimer::singleShot(60000,this,SLOT(reload()));
}

void QTodoDeadlineDialog::reload()
{
	list->clear();
	init();
}

void QTodoDeadlineDialog::showGlobally()
{
	if(isOpen())
	{
		(instances.at(0))->reload();
		if((instances.at(0))->isVisible())
		{
			QTodoMisc::windowToCurrentDisplay((instances.at(0))->winId());
		}
		else
		{
			(instances.at(0))->showNormal();
			QTimer::singleShot(10,instances.at(0),SLOT(activateWindow())); //needed for WMs like KDE's, not needed in IceWM
		}
	}
	else
	{
		QTodoDeadlineDialog* new_dialog = new QTodoDeadlineDialog();
		new_dialog->show();
		QTimer::singleShot(10,new_dialog,SLOT(activateWindow())); //needed for WMs like KDE's, not needed in IceWM
	}
	last_deadline_warning = QDateTime::currentDateTime();
}

void QTodoDeadlineDialog::reloadGlobally()
{
	QPtrListIterator<QTodoDeadlineDialog> it(instances);
	for(;it.current();++it)
		it.current()->reload();
}

bool QTodoDeadlineDialog::isOpen()
{
	return instances.count();
}

QDateTime QTodoDeadlineDialog::lastWarning()
{
	return last_deadline_warning;
}

void QTodoDeadlineDialog::activateWindow()
{
	QTodoMisc::windowToCurrentDisplay(winId());
}

void QTodoDeadlineDialog::print()
{
	QTodoPrintPreview print_preview;
	print_preview.exec();
}

void QTodoDeadlineDialog::slotContextMenuRequested(QListViewItem*, const QPoint& pos,int)
{
	QPopupMenu* menu = new QPopupMenu(this);
	menu->insertItem(QTP::show_more.s(16,16),tr("Show"),DMID_SHOW);
	menu->insertItem(QTP::done.s(16,16),tr("Set done"),DMID_SETDONE);
	menu->insertSeparator();
	menu->insertItem(QF::setFont(new QLabel(tr("Delay:"),this),QF::get()->underline));
	menu->insertItem(tr("1 hour"),DMID_DELAY_1HOUR);
	menu->insertItem(tr("1 day"),DMID_DELAY_1DAY);
	menu->insertItem(tr("1 week"),DMID_DELAY_1WEEK);
	menu->insertItem(tr("1 month"),DMID_DELAY_1MONTH);

	connect(menu,SIGNAL(activated(int)),this,SLOT(contextMenuActivated(int)));	

	menu->exec(pos);
	delete menu;
}

void QTodoDeadlineDialog::contextMenuActivated(int id)
{
	QTodoDeadlineListViewItem* item = dynamic_cast<QTodoDeadlineListViewItem*>(list->currentItem());
	if(!item)
		return;

	QDateTime cur_deadline = item->todo->getDeadlineDateTime();
	if(cur_deadline < QDateTime::currentDateTime())
		cur_deadline = QDateTime::currentDateTime();
	switch(id)
	{
		case DMID_SHOW:
			activated(item);
			break;
		case DMID_SETDONE:
			item->todo->setStatus(STAT_DONE,true);
			break;
		case DMID_DELAY_1HOUR:
			item->todo->setDeadline(cur_deadline.addSecs(3600),false);
			break;
		case DMID_DELAY_1DAY:
			item->todo->setDeadline(cur_deadline.addDays(1),false);
			break;
		case DMID_DELAY_1WEEK:
			item->todo->setDeadline(cur_deadline.addDays(7),false);
			break;
		case DMID_DELAY_1MONTH:
			item->todo->setDeadline(cur_deadline.addMonths(1),false);
			break;
	}
}







