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

#include <map>

#include <qlabel.h>
#include <qpixmap.h>

#ifndef QTODO_COMBO_WIDGET
#define QTODO_COMBO_WIDGET

struct QTodoComboWidgetItem
{
	QTodoComboWidgetItem(const QPixmap& _pixmap,const QString& _text,bool _separator = false) : pixmap(_pixmap),text(_text),separator(_separator) {}

	QPixmap pixmap;
	QString text;
	bool separator;
};

class QTodoComboWidget : public QLabel
{
	Q_OBJECT

	public:
	QTodoComboWidget(QWidget*);
	void insert(const QPixmap&,const QString&,int index);
	void insertSeparator(int index);
	int getCurrent();
	void setCurrent(int,bool emit_changed=true);
	void setDirectMode(bool);
	void replacePixmap(const QPixmap&,int index);
	void setFrozen(bool);
	int getLast();
	void showMenu();

	signals:
	void changed(int);
	void changed();

	private slots:
	void menuActivated(int);

	private:
	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);

	std::map<int,QTodoComboWidgetItem*> item;
	int current;
	int last;
	bool direct;
	bool frozen;
};

#endif

