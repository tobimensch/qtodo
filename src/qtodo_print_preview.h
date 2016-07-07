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

#include <qdialog.h>

#ifndef QTODO_PRINT_PREVIEW
#define QTODO_PRINT_PREVIEW

class QTextBrowser;
class QCheckBox;
class QTodoListWidget;
class QTodoItem;

class QTodoPrintPreview : public QDialog
{
	Q_OBJECT
	
	public:
	QTodoPrintPreview(QTodoListWidget*);
	QTodoPrintPreview();
	~QTodoPrintPreview();

	private slots:
	void avoidSourceChange(const QString&);
	void updateView();
	void print();

	private:
	QString todoListHead();
	QString deadlinesHead();
	QString todoItemRichText(QTodoItem*);
	void init();

	QCheckBox* show_icons_checkbox;
	QCheckBox* use_filter_checkbox;
	QTextBrowser* textbrowser;
	QTodoListWidget* list_widget;

	int max_prio_width;
	int max_stat_width;
};

class QTodoMimeSourceFactory : public QObject, public QMimeSourceFactory
{
	Q_OBJECT
		
	public:
	QTodoMimeSourceFactory();
	const QMimeSource* data(const QString& abs_name) const;
};

#endif

