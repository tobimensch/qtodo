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

#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qgroupbox.h>
#include <qwidgetstack.h>
#include <qinputdialog.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qimage.h>
#include <qhbox.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qdir.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtimer.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <qaccel.h>

#include "qtodo_widget.h"
#include "qtodo_list_widget.h"
#include "qtodo_list.h"
#include "qtodo_list_view_item.h"
#include "qtodo_settings_container.h"
#include "qtodo_settings.h"
#include "qtodo_lists.h"
#include "qtodo_add_list.h"
#include "qtodo_help.h"
#include "qtodo_about.h"
#include "qtodo_trayicon.h"
#include "qtodo_xpm.h"
#include "qtodo_misc.h"
#include "qtodo_deadline_dialog.h"
#include "qtodo_print_preview.h"
#include "qtodo_item.h"
#include "qtodo_font.h"
#include "qtodo_push_button.h"
#include "qtodo_undo_manager.h"
#include "qtodo_exporter.h"
#include "qtodo_recovery_dialog.h"
#include "qtodo_list_filter.h"

QPalette QTodoWidget::system_palette;

enum
{
	FMID_SAVE_ALL,
	FMID_EXPORT_LIST,
	FMID_PRINT,
	FMID_QUIT
};

QTodoWidget::QTodoWidget(WFlags f) : QWidget(0,0,f)
{
	setCaption("QTodo - ToDo manager");

	if(QSC::get()->save_main_geom)
	{
		setGeometry(QSC::get()->main_x,QSC::get()->main_y,QSC::get()->main_width,QSC::get()->main_height);
	}

	QGridLayout* grid = new QGridLayout(this);
	
	QGroupBox* gb_todo_lists = new QGroupBox(this);
	gb_todo_lists->setColumnLayout(0, Qt::Vertical );
	gb_todo_lists->layout()->setSpacing( 4 );
	gb_todo_lists->layout()->setMargin( 5 );
	QGridLayout* grid_todo_lists = new QGridLayout(gb_todo_lists->layout());
	
	todo_lists = new QTodoLists(gb_todo_lists);
	todo_lists->addColumn(tr("Todo-Lists"));
	todo_lists->setResizeMode(QListView::AllColumns);
	QTodoPushButton* add_todo_list = new QTodoPushButton(QIconSet(QTP::yellow_plus.s(12,12),QIconSet::Small),tr("&Add"),gb_todo_lists);
	add_todo_list->setMaximumWidth(80);
	add_todo_list->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	rem_todo_list = new QTodoPushButton(QIconSet(QTP::yellow_minus.s(12,12),QIconSet::Small),tr("&Remove"),gb_todo_lists);
	rem_todo_list->setMaximumWidth(90);
	rem_todo_list->setEnabled(false);
	rem_todo_list->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	
	connect(todo_lists,SIGNAL(listChanged()),this,SLOT(todoListChanged()));
	connect(add_todo_list,SIGNAL(clicked()),this,SLOT(addList()));
	connect(rem_todo_list,SIGNAL(clicked()),this,SLOT(remList()));

	grid_todo_lists->addMultiCellWidget(todo_lists,1,1,0,1);
	grid_todo_lists->addWidget(add_todo_list,2,0);
	grid_todo_lists->addWidget(rem_todo_list,2,1);

	todo_list_view = new QWidgetStack(this);
	todo_list_view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	QLabel* background = new QLabel(todo_list_view);
	background->setPaletteBackgroundColor(palette().color(QPalette::Active,QColorGroup::Mid));
	todo_list_view->addWidget(background);

	file_menu = new QPopupMenu(this);
	file_menu->insertItem(QTP::save_all.s(16,16),tr("Save all"),this,SLOT(saveAll()),0,FMID_SAVE_ALL);
	file_menu->setAccel(CTRL+Key_S,FMID_SAVE_ALL);
	file_menu->insertItem(QTP::export_list.s(16,16),tr("Export"),this,SLOT(exportList()),0,FMID_EXPORT_LIST);
	file_menu->insertSeparator();
	file_menu->insertItem(QTP::print.s(16,16),tr("&Print"),this,SLOT(printPreview()),0,FMID_PRINT);
	file_menu->setAccel(CTRL+Key_P,FMID_PRINT);
	file_menu->insertSeparator();
	file_menu->insertItem(QTP::exit.s(16,16),tr("Quit"),qApp,SLOT(quit()),0,FMID_QUIT);	
	file_menu->setAccel(CTRL+Key_Q,FMID_QUIT);

	QMenuBar* menu_bar = new QMenuBar(this);

	menu_bar->insertItem(tr("&File"),file_menu);
	connect(file_menu,SIGNAL(aboutToShow()),this,SLOT(updateFileMenu()));
	connect(file_menu,SIGNAL(aboutToHide()),this,SLOT(enableFileMenu()));

	edit_menu = new QPopupMenu(this);
	connect(edit_menu,SIGNAL(aboutToShow()),this,SLOT(updateEditMenu()));
	connect(edit_menu,SIGNAL(aboutToHide()),this,SLOT(disableEditMenu()));
	menu_bar->insertItem(tr("&Edit"),edit_menu);

	lists_menu = new QPopupMenu(this);
	connect(lists_menu,SIGNAL(aboutToShow()),this,SLOT(updateListsMenu()));
	connect(lists_menu,SIGNAL(activated(int)),this,SLOT(listsMenuActivated(int)));

	menu_bar->insertItem(tr("&Lists"),lists_menu);

	settings_menu = new QPopupMenu(this);
	settings_menu->insertItem(QTP::configure.s(16,16),tr("Configure list"),this,SLOT(configureList()));
	settings_menu->insertItem(QTP::configure.s(16,16),tr("Configure QTodo"),this,SLOT(configure()));
	connect(settings_menu,SIGNAL(aboutToShow()),this,SLOT(updateSettingsMenu()));
	
	menu_bar->insertItem(tr("&Settings"),settings_menu);

	QPopupMenu* help_menu = new QPopupMenu(this);
	help_menu->insertItem(QTP::help.s(16,16),tr("Help"),this,SLOT(showHelp()),Key_F1);
	help_menu->insertItem(QTP::todo_list.s(16,16),tr("About"),this,SLOT(showAbout()));

	menu_bar->insertItem(tr("&Help"),help_menu);

	QAccel *a = new QAccel(this);
	a->connectItem(a->insertItem(CTRL+Key_S),
			this,
			SLOT(saveAll()));
	a->connectItem(a->insertItem(CTRL+Key_P),
			this,
			SLOT(printPreview()));
	a->connectItem(a->insertItem(CTRL+Key_F),
			this,
			SLOT(focusFilter()));
	a->connectItem(a->insertItem(Key_F3),
			this,
			SLOT(refreshFilter()));
	a->connectItem(a->insertItem(Key_F4),
			this,
			SLOT(resetFilter()));
	a->connectItem(a->insertItem(ALT+Key_Z),
			this,
			SLOT(undo()));
	a->connectItem(a->insertItem(Key_F2),
			this,
			SLOT(showDeadlines()));
	a->connectItem(a->insertItem(Key_F11),
			this,
			SLOT(jumpToFirst()));
	a->connectItem(a->insertItem(Key_F12),
			this,
			SLOT(jumpToLast()));
	a->connectItem(a->insertItem(ALT+Key_Prior),
			this,
			SLOT(addTodoAtTop()));
	a->connectItem(a->insertItem(ALT+Key_Next),
			this,
			SLOT(addTodoAtBottom()));
	a->connectItem(a->insertItem(ALT+Key_Minus),
			this,
			SLOT(deleteSelectedTodos()));
	a->connectItem(a->insertItem(ALT+Key_7),
			this,
			SLOT(cutSelectedTodos()));
	a->connectItem(a->insertItem(ALT+Key_8),
			this,
			SLOT(copySelectedTodos()));
	a->connectItem(a->insertItem(ALT+Key_9),
			this,
			SLOT(pasteTodos()));
	a->connectItem(a->insertItem(CTRL+Key_O),
			this,
			SLOT(sortTodos()));
	a->connectItem(a->insertItem(CTRL+Key_O),
			this,
			SLOT(sortTodos()));
	a->connectItem(a->insertItem(CTRL+Key_A),
			this,
			SLOT(selectAllTodos()));
	a->connectItem(a->insertItem(CTRL+SHIFT+Key_A),
			this,
			SLOT(deselectAllTodos()));

	grid->setMenuBar(menu_bar);

	grid->addWidget(gb_todo_lists,0,0);
	grid->addWidget(todo_list_view,0,1);
	grid->setColStretch(1,2);

	QTodoRecoveryDialog* recovery_dialog = 0;
	for(QStringList::Iterator it = QSC::get()->todo_list_files.begin(); it != QSC::get()->todo_list_files.end(); ++it)
	{
		QTodoListViewItem* item = new QTodoListViewItem(todo_lists,(*it));
		todo_list_view->addWidget(item->listWidget());
		if(!item->load())
		{
			if(!recovery_dialog)
				recovery_dialog = new QTodoRecoveryDialog();
			recovery_dialog->add(item);
		}	
	}
	if(recovery_dialog)
	{
		qApp->setMainWidget(recovery_dialog);
		if(recovery_dialog->exec() == QDialog::Accepted)
			recovery_dialog->recover();
		else
			abort();
	}

	todo_lists->setCurrent(QSC::get()->last_todo_list,QSC::get()->last_todo_list_pos);

	spontaneous_hidden = false;

	QTimer::singleShot(2000,this,SLOT(deadlinesWarning()));

	if(QSC::get()->autosave_lists)
		prepareAutoSave();
}

void QTodoWidget::addList()
{
	QTodoAddList* add_list = new QTodoAddList(this);
	if(add_list->exec() == QDialog::Accepted)
	{
		QString name, file;

		if(add_list->new_button->isOn())
		{
			name = add_list->todoname->text();
			QString filename = add_list->filename->text();
			QString directory = add_list->directory->text();
			if(directory.isEmpty())
				directory = QDir::homeDirPath();
			file = directory + QDir::separator();
			if(filename.isEmpty())
				file += name;
			else
				file += filename;
		}
		else if(add_list->existing_button->isOn())
		{
			file = add_list->file->text();
		}
		else
		{
			return;
		}

		QTodoListViewItem* item = new QTodoListViewItem(todo_lists,file,name);

		if(QFileInfo(file).exists())
		{
			item->load();
		}
		
		todo_list_view->addWidget(item->listWidget());
		todo_lists->setCurrent(item->list());
	}
}

void QTodoWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key_Insert:
			//addTodo(); //FIXME
			break;
		default:
			break;
	}
	//e->ignore();

	QWidget::keyPressEvent(e);	
}

void QTodoWidget::remList()
{
	todo_lists->removeCurrent();
}

void QTodoWidget::todoListChanged()
{
	if(todo_lists->currentListWidget())
		todo_list_view->raiseWidget(todo_lists->currentListWidget());
	else	
		todo_list_view->raiseWidget(0);
	rem_todo_list->setEnabled(todo_lists->currentListItem());
}

void QTodoWidget::save()
{
	QSC::get()->todo_list_files.clear();
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item; item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QSC::get()->todo_list_files += item->listWidget()->getFile();
	}
}

QTodoLists * QTodoWidget::todoLists()
{
	return todo_lists;
}

void QTodoWidget::showHelp()
{
	QTodoHelp* help = new QTodoHelp();
	help->show();
}

void QTodoWidget::trayIconClicked(const QPoint&, int)
{
	trayIconHideShowAction();
}

void QTodoWidget::trayIconDoubleClicked(const QPoint&)
{
	trayIconHideShowAction();
}

void QTodoWidget::trayIconHideShowAction()
{
	if(isShown() && !spontaneous_hidden)
	{
		if(isVisible())
		{
			saveAll();
			QSC::get()->save();
			hide();
		}
		else
		{
			show();
		}
	}
	else
	{
		advShow();
	}
}

bool QTodoWidget::isQTodoVisible() //FIXME: doesn't work, since spontanous hidden events sometimes miss
{
	if(isShown() && !spontaneous_hidden)
	{
		if(isVisible())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void QTodoWidget::trayMenuActivated(int id)
{
	switch(id)
	{
		case TMID_OPEN:
			advShow();
			break;
		case TMID_HIDE:
			saveAll();
			QSC::get()->save();
			hide();
			break;
		case TMID_DEADLINES:
			showDeadlines();
			break;
		case TMID_DEADLINE_ALERT_SWITCH:
			QSC::get()->warn_before_deadlines_expire = !QSC::get()->warn_before_deadlines_expire;
			QSC::get()->save();
			updateTrayIcon(true);
			break;
		case TMID_QUIT:
			qApp->quit();
			break;
	}	
}

void QTodoWidget::hideEvent(QHideEvent* event)
{
	if(event->spontaneous())
	{
		spontaneous_hidden = true;
	}
}

void QTodoWidget::showEvent(QShowEvent* event)
{
	if(event->spontaneous())
	{
		spontaneous_hidden = false;
	}
}

void QTodoWidget::updateListsMenu()
{
	lists_menu->clear();

	QListViewItem* item = todo_lists->firstChild();
	for(int i = 0;item;item = item->nextSibling(), ++i)
	{
		lists_menu->insertItem(QTP::todo_list.s(16,16),item->text(0),i);
	}
}

void QTodoWidget::listsMenuActivated(int id)
{
	QListViewItem* item = todo_lists->firstChild();
	for(;id > 0; --id)
	{
		item = item->nextSibling();
	}
	advShow();
	todo_lists->setSelected(item,true);
}

void QTodoWidget::configureList()
{
	todo_lists->currentListWidget()->configure();
}

void QTodoWidget::showAbout()
{
	QTodoAbout* about = new QTodoAbout();
	about->exec();
	delete about;
}

void QTodoWidget::toCurrentDisplay()
{
	QTodoMisc::mainWidgetToCurrentDisplay();
}

void QTodoWidget::configure()
{
	QTodoSettings settings;
	settings.exec();
}

void QTodoWidget::trayMenuAboutToShow()
{
	QPopupMenu* tray_menu = dynamic_cast<QPopupMenu*>(const_cast<QObject*>(sender()));
	tray_menu->clear();

	if(isQTodoVisible())
		tray_menu->insertItem(tr("Hide"),TMID_HIDE);
	else
		tray_menu->insertItem(tr("Open"),TMID_OPEN);
	tray_menu->insertItem(tr("&Lists"),lists_menu);
	tray_menu->insertItem(QTP::not_dead.s(16,16),tr("Deadlines"),TMID_DEADLINES);
	tray_menu->setItemEnabled(TMID_DEADLINES,activeDeadline());
	tray_menu->setCheckable(true);
	int id = tray_menu->insertItem(tr("Alert"),TMID_DEADLINE_ALERT_SWITCH);
	tray_menu->setItemChecked(id,QSC::get()->warn_before_deadlines_expire);
	tray_menu->insertSeparator();
	tray_menu->insertItem(QTP::exit.s(16,16),tr("Quit"),TMID_QUIT);
}

void QTodoWidget::closeEvent(QCloseEvent* ce)
{
	if(QSC::get()->dont_quit_on_window_close_button)
	{
		if(QSC::get()->tray_icon)
			hide();
		else
			ce->ignore();
		saveAll();
		QSC::get()->save();
	}
	else
		ce->accept();
}

void QTodoWidget::showDeadlines()
{
	QTodoDeadlineDialog::showGlobally();
}

void QTodoWidget::setTrayIcon(QTodoTrayIcon* _tray_icon)
{
	tray_icon = _tray_icon;
}

void QTodoWidget::setTrayIconEnabled(bool b)
{
	if(b)
		tray_icon->show();
	else
		tray_icon->hide();
}

void QTodoWidget::updateTrayIcon(bool force)
{
	if(QSC::get()->warn_before_deadlines_expire && QSC::get()->warn_by_trayicon)
		trayiconDeadlinesWarning();
	else if(force)
		tray_icon->setIcon(QTP::todo_list.s(16,16));
}

void QTodoWidget::deadlinesWarning()
{
	if(!QSC::get()->warn_before_deadlines_expire || QTodoDeadlineDialog::lastWarning().addSecs(QSC::get()->minutes_between_deadline_warnings) > QDateTime::currentDateTime())
	{
		QTimer::singleShot(60000,this,SLOT(deadlinesWarning()));
		return;
	}

	QTodoLists* todo_lists = todoLists();
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QTodoListIterator list_it(item->list());
		for(;list_it.current();++list_it)
		{
			if(QTodoItem* item = dynamic_cast<QTodoItem*>(list_it.current()))
			{
				if(item->getStatus() != STAT_DONE && item->getDeadlineDateTime().isValid())
				{
				 	if(QDateTime::currentDateTime().addSecs(QSC::get()->minutes_to_warn_before_deadline_expire*60) >= item->getDeadlineDateTime())
					{
						executeDeadlinesWarning();
						QTimer::singleShot(60000*QSC::get()->minutes_between_deadline_warnings,this,SLOT(deadlinesWarning()));
						return;
					}
				}
			}
		}
	}

	QTimer::singleShot(60000,this,SLOT(deadlinesWarning()));
}

QTodoItem* QTodoWidget::mostExpiredTodo()
{
	QTodoItem* most_expired_item = 0;

	QTodoLists* todo_lists = todoLists();
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QTodoListIterator list_it(item->list());
		for(;list_it.current();++list_it)
		{
			if(QTodoItem* item = dynamic_cast<QTodoItem*>(list_it.current()))
			{
				if(item->getStatus() != STAT_DONE && item->getDeadlineDateTime().isValid())
				{
				 	if(QDateTime::currentDateTime().addSecs(QSC::get()->minutes_to_warn_before_deadline_expire*60) >= item->getDeadlineDateTime())
					{
						if(!most_expired_item)
							most_expired_item = item;
						else if(most_expired_item->getDeadlineDateTime() > item->getDeadlineDateTime())
							most_expired_item = item;
					}
				}
			}
		}
	}

	return most_expired_item;
}

void QTodoWidget::executeDeadlinesWarning()
{
	if(QSC::get()->warn_by_deadline_dialog)
		showDeadlines();
	if(QSC::get()->warn_by_command_execution)
	{
		QProcess proc;
		proc.setArguments(QStringList::split(" ",QSC::get()->warning_execute_command));
		proc.start();
		if(!proc.isRunning())
			QMessageBox::information(this,tr("QTodo deadline warning failed"),tr("You see this message, because the command to execute as deadline warning didn't work."));
	}
	if(QSC::get()->warn_by_trayicon)
	{
		trayiconDeadlinesWarning();
	}
}


void QTodoWidget::trayiconDeadlinesWarning()
{
	if(!QSC::get()->warn_by_trayicon)
		return;

	QTodoItem* most_expired_item = mostExpiredTodo();
	if(most_expired_item == 0)
	{
		tray_icon->setIcon(QTP::todo_list.s(16,16));
		return;
	}

	QColor col = most_expired_item->getCurrentDeadlineColor();
	QImage image;
	image = QTP::dead;
	image = image.scale(16,16);
	int y = image.height() - 1;
	for(;y >= 0; --y)
	{
		int x = image.width() - 1;
		for(;x >= 0;--x)
		{
		uint* p = (uint*)image.scanLine(y)+x;
		if(QColor(*p) == Qt::white)
			*p = col.rgb();
		else if(QColor(*p) == Qt::black)
			*p = col.rgb()/2;
		}
	}
	QPixmap pixmap(image);
	tray_icon->setIcon(pixmap);
}

void QTodoWidget::doAutoSave()
{
	if(last_auto_save.secsTo(QDateTime::currentDateTime()) >= QSC::get()->autosave_minutes*60 - 1)
	{
		saveAll();

		last_auto_save = QDateTime::currentDateTime();
		prepareAutoSave();
	}
}

void QTodoWidget::saveAll()
{
	todo_lists->save();
	save();
}

void QTodoWidget::prepareAutoSave()
{
	QTimer::singleShot(60000*QSC::get()->autosave_minutes,this,SLOT(doAutoSave()));
}

void QTodoWidget::printPreview()
{
	if(!todo_lists->currentListWidget())
		return;

	QTodoPrintPreview print_preview(todo_lists->currentListWidget());
	print_preview.exec();
}

void QTodoWidget::advShow()
{
	QTodoMisc::mainWidgetToCurrentDisplay();
	show();
}

void QTodoWidget::updateSettingsMenu()
{
	int id = settings_menu->idAt(0);
	settings_menu->setItemEnabled(id,todo_lists->currentListWidget());
}

void QTodoWidget::updateFileMenu()
{
	file_menu->setItemEnabled(FMID_PRINT,todo_lists->currentListWidget());
	file_menu->setItemEnabled(FMID_EXPORT_LIST,todo_lists->currentListWidget());
	//file_menu->setItemEnabled(FMID_SAVE_ALL,todo_lists->numLists());
	file_menu->setItemEnabled(FMID_SAVE_ALL,todo_lists->modifiedList());
}

void QTodoWidget::enableFileMenu()
{
	file_menu->setItemEnabled(FMID_PRINT,true);
	file_menu->setItemEnabled(FMID_EXPORT_LIST,true);
	file_menu->setItemEnabled(FMID_SAVE_ALL,true);
}

void QTodoWidget::updateEditMenu()
{
	edit_menu->clear();
	int id = edit_menu->insertItem(QTP::undo.s(16,16),tr("Undo"),this,SLOT(undo()));
	edit_menu->setItemEnabled(id,QTUM::get()->isUndoPossible());
	edit_menu->setAccel(ALT+Key_Z,id);
	if(todo_lists->currentList())
	{
		edit_menu->insertSeparator();
		disconnect(edit_menu,SIGNAL(activated(int)),0,0);
		todo_lists->currentList()->contextMenu(edit_menu);
		todo_lists->currentList()->contextMenuUpdate(edit_menu);
	}
	edit_menu->insertSeparator();
	id = edit_menu->insertItem(QTP::not_dead.s(15,15),tr("Deadlines"),this,SLOT(showDeadlines()));
	edit_menu->setAccel(Key_F2,id);
	edit_menu->setItemEnabled(id,activeDeadline());
}

void QTodoWidget::disableEditMenu()
{
	QTimer::singleShot(1,this,SLOT(clearEditMenu()));
}

void QTodoWidget::clearEditMenu()
{
	edit_menu->clear();
}

bool QTodoWidget::activeDeadline()
{
	QTodoLists* todo_lists = todoLists();
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QTodoListIterator list_it(item->list());
		for(;list_it.current();++list_it)
			if(QTodoItem* item = dynamic_cast<QTodoItem*>(list_it.current()))
				if(item->getStatus() != STAT_DONE && item->getDeadlineDateTime().isValid())
				 	return true;
	}
	return false;
}

void QTodoWidget::undo()
{
	QTUM::get()->undo();
}

void QTodoWidget::exportList()
{
	QTodoExporter exporter(todo_lists->currentListWidget());
	exporter.exec();
}

void QTodoWidget::focusFilter()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentListWidget()->getListFilter()->focusFilter();
}

void QTodoWidget::refreshFilter()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentListWidget()->getListFilter()->refresh();
}


void QTodoWidget::resetFilter()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentListWidget()->getListFilter()->reset();
}

void QTodoWidget::jumpToFirst()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->jumpToFirst();
}

void QTodoWidget::jumpToLast()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->jumpToLast();
}

void QTodoWidget::addTodoAtTop()
{
	QTUM::get()->startRecording();
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->addTodoAtTop();
	QTUM::get()->stopRecording();
}

void QTodoWidget::addTodoAtBottom()
{
	QTUM::get()->startRecording();
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->addTodo();
	QTUM::get()->stopRecording();
}

void QTodoWidget::deleteSelectedTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->_delete();
}

void QTodoWidget::cutSelectedTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->cut();
}

void QTodoWidget::copySelectedTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->copy();
}

void QTodoWidget::sortTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->sort();
}

void QTodoWidget::selectAllTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->selectAll();
}

void QTodoWidget::deselectAllTodos()
{
	if(todo_lists->currentListWidget())
		todo_lists->currentList()->deselectAll();
}

void QTodoWidget::pasteTodos()
{
	if(todo_lists->currentListWidget())
	{
		QTodoList* list = todo_lists->currentList();
		QWidget* widget = list->focusWidget();
		for(;widget && dynamic_cast<QTodoList*>(widget) == 0;)
		{
			widget = dynamic_cast<QWidget*>(widget->parent());
			QTodoItem* item;
			if(widget && ( item = dynamic_cast<QTodoItem*>(widget)))
			{
				list->pasteTodo(item);
				return;
			}
		}

		list->paste();
	}
}

QString QTodoWidget::trayIconToolTipText()
{
	QString text;
	text += "<nobr><center><font>"+QString("QTodo - ")+tr("Quick deadline overview")+"</font></center></nobr>";

	bool deadlines = false;
	int unfinished = 0;
	std::multimap<QDateTime,QTodoItem*> items;
	QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
	for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
	{
		QTodoListIterator list_it(item->list());
		for(;list_it.current();++list_it)
		{
			if(QTodoItem* todo_item = dynamic_cast<QTodoItem*>(list_it.current()))
			{
				if(todo_item->getStatus() != STAT_DONE)
					++unfinished;
				if(todo_item->getDeadlineString().isEmpty() || todo_item->getStatus() == STAT_DONE)
					continue;
				
				deadlines = true;

				items.insert(std::pair<QDateTime,QTodoItem*>(todo_item->getDeadlineDateTime(),todo_item));
			}
		}
	}


	if(deadlines)
		text += "<table border=1 cellspacing=0 cellpadding=0 width=\"100%\">";

	int c = 0;
	for(std::multimap<QDateTime,QTodoItem*>::iterator it = items.begin(); it != items.end(); ++it)
	{
		QTodoItem* todo_item = (*it).second;
		text += "<tr><td width=\"10px\" bgcolor=\""+todo_item->getCurrentDeadlineColor().name()+"\"></td><td width\"20%\"><nobr>";
		text += "";
		text += QTodoMisc::timeSpanUntilDateTimeString(todo_item->getDeadlineDateTime());
		text += "</nobr></td><td width=\"80%\">";
		text += QTodoMisc::shortText(todo_item->getTask(),200);
		text += "</td></tr>";
		++c;
	}

	if(deadlines)
		text += "</table>";


	text += "<nobr>" + tr("Active deadlines:") + " " + QString::number(c);
	text += " " + tr("Unfinished tasks:") + " " + QString::number(unfinished) + "</nobr>";

	return text;
}







