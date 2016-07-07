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
#include <qdatetime.h>
 
#ifndef QTODO_WIDGET
#define QTODO_WIDGET

enum 
{
	TMID_OPEN,
	TMID_HIDE,
	TMID_DEADLINE_ALERT_SWITCH,
	TMID_DEADLINES,
	TMID_QUIT
};

class QTodoLists;
class QWidgetStack;
class QTodoTrayIcon;
class QPopupMenu;
class QPushButton;
class QTodoItem;

///Main window of QTodo
class QTodoWidget : public QWidget
{
	Q_OBJECT

	public:
	QTodoWidget(WFlags f = 0);
	void save();
	QTodoLists* todoLists();
	void setTrayIcon(QTodoTrayIcon*);
	void setTrayIconEnabled(bool);
	void updateTrayIcon(bool force=false);
	void prepareAutoSave();
	void advShow();
	bool isQTodoVisible();
	QString trayIconToolTipText();
	
	private:
	void keyPressEvent(QKeyEvent*);
	void hideEvent(QHideEvent*);
	void showEvent(QShowEvent*);
	void closeEvent(QCloseEvent*);
	bool activeDeadline();
	QTodoItem* mostExpiredTodo();

	private slots:
	void saveAll();
	void doAutoSave();	
	void addList();
	void remList();
	void todoListChanged();
	void showHelp();
	void showAbout();
	void showDeadlines();
	void exportList();
	void trayIconClicked(const QPoint&,int);
	void trayIconDoubleClicked(const QPoint&);
	void trayIconHideShowAction();
	void trayMenuActivated(int);
	void trayMenuAboutToShow();
	void updateFileMenu();
	void updateListsMenu();
	void updateEditMenu();
	void updateSettingsMenu();
	void enableFileMenu();
	void disableEditMenu();
	void clearEditMenu();
	void listsMenuActivated(int);
	void configureList();
	void configure();
	void toCurrentDisplay();
	void deadlinesWarning();
	void executeDeadlinesWarning();
	void trayiconDeadlinesWarning();
	void printPreview();
	void undo();
	void focusFilter();
	void refreshFilter();
	void jumpToFirst();
	void jumpToLast();
	void addTodoAtTop();
	void addTodoAtBottom();
	void deleteSelectedTodos();
	void copySelectedTodos();
	void cutSelectedTodos();
	void pasteTodos();
	void resetFilter();
	void sortTodos();
	void selectAllTodos();
	void deselectAllTodos();

	public:
	static QPalette system_palette;
	
	private:
	QPopupMenu* file_menu;
	QPopupMenu* settings_menu;
	QTodoLists* todo_lists;
	QWidgetStack* todo_list_view;
	QPopupMenu* lists_menu;
	QPopupMenu* edit_menu;
	QPushButton* rem_todo_list;
	QTodoTrayIcon* tray_icon;
	QDateTime last_auto_save;

	bool spontaneous_hidden;
};

#endif

