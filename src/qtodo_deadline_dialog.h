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
#include <qptrlist.h>

#ifndef QTODO_DEADLINE_DIALOG
#define QTODO_DEADLINE_DIALOG

#include "deadlines.h"

class QTodoItem;

class QTodoDeadlineDialog : public QTodoDeadlineDialogBase
{
	Q_OBJECT

	public:
	QTodoDeadlineDialog();
	~QTodoDeadlineDialog();
	static void showGlobally();
	static void reloadGlobally();
	static bool isOpen();
	static QDateTime lastWarning();

	public slots:
	void reload();
	void print();

	private:
	void init();

	private slots:
	void slotDoubleClicked(QListViewItem*,const QPoint&, int);
	void slotContextMenuRequested(QListViewItem*,const QPoint&, int);
	void activateWindow();
	void activated(QListViewItem*);
	void contextMenuActivated(int);

	private:
	static QPtrList<QTodoDeadlineDialog> instances;
	static QDateTime last_deadline_warning;
};

class QTodoDeadlineListViewItem : public QListViewItem
{
	public:
	QTodoDeadlineListViewItem(QListView*);
	int compare(QListViewItem*, int, bool) const;

	QTodoItem* todo;
};

#endif

