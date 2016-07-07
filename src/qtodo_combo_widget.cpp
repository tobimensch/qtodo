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
#include <qcursor.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>

#include "qtodo_combo_widget.h"

QTodoComboWidget::QTodoComboWidget(QWidget* parent) : QLabel(parent), current(-1), last(-1), direct(false), frozen(false)
{
	setFocusPolicy(QWidget::StrongFocus);
}

void QTodoComboWidget::insert(const QPixmap& pixmap,const QString& text, int index)
{
	item[index] = new QTodoComboWidgetItem(pixmap,text);
}

void QTodoComboWidget::insertSeparator(int index)
{
	item[index] = new QTodoComboWidgetItem(QPixmap(),QString(),true);
}

int QTodoComboWidget::getCurrent()
{
	return current;
}

int QTodoComboWidget::getLast()
{
	return last;
}

void QTodoComboWidget::setCurrent(int i, bool emit_changed)
{
	if(current == i || (frozen && current != -1))
		return;

	last = current;
	current = i;
	if(item[current]->pixmap.isNull())
		setText(item[current]->text);
	else
		setPixmap(item[current]->pixmap);
	
	if(!emit_changed)
		return;

	emit changed(i);
	emit changed();
	//setText(item[current]->text);
}

void QTodoComboWidget::mousePressEvent(QMouseEvent*)
{
	showMenu();	
}

void QTodoComboWidget::showMenu()
{
	if(direct)
	{
		int t;
		if(item[current+1])
			t = current+1;
		else
			t = 0;
		setCurrent(t);
		return;
	}

	QPopupMenu* menu = new QPopupMenu(this);
	for(std::map<int,QTodoComboWidgetItem*>::iterator it = item.begin(); it != item.end(); ++it)
	{
		if((*it).second->separator)
			menu->insertSeparator();
		else
			menu->insertItem(((*it).second)->pixmap,((*it).second)->text,(*it).first);
	}

	connect(menu,SIGNAL(activated(int)),this,SLOT(menuActivated(int)));
	QPoint pos = mapToGlobal(QPoint(0,height()));

	menu->sizeHint(); //we need this call so that menu->height() and menu->width() work correctly, before the popup is shown

	if(menu->height()+pos.y() > QApplication::desktop()->height())
		pos = mapToGlobal(QPoint(0,-menu->height()));
	if(pos.x()+menu->width() > QApplication::desktop()->width())
		pos.setX(pos.x() - ((pos.x() + menu->width()) - QApplication::desktop()->width()));
	menu->move(pos);

	menu->exec(pos);
	delete menu;
}

void QTodoComboWidget::menuActivated(int id)
{
	if(id < 0)
	{
		QPopupMenu* menu = dynamic_cast<QPopupMenu*>(const_cast<QObject*>(sender()));
		for(std::map<int,QTodoComboWidgetItem*>::iterator it = item.begin(); it != item.end(); ++it)
		{
			if(((*it).second)->text = menu->text(id))
			{
				id = (*it).first;
				break;
			}
		}
	}
	if(item[id])
		setCurrent(id);
}

void QTodoComboWidget::setDirectMode(bool b)
{
	direct = b;
}

void QTodoComboWidget::replacePixmap(const QPixmap& pixmap, int index)
{
	item[index]->pixmap = pixmap;
	if(index == current)
		setPixmap(item[index]->pixmap);
}

void QTodoComboWidget::setFrozen(bool b)
{
	frozen = b;
}

void QTodoComboWidget::keyPressEvent(QKeyEvent* e)
{
	switch(e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			showMenu();
			return;
			break;
		default:
			break;
	}

	QLabel::keyPressEvent(e);
}

void QTodoComboWidget::paintEvent(QPaintEvent* e)
{
	if (hasFocus())
	{
		if(!e->erased())
			erase();

	QPainter paint;
	paint.begin(this);
        style().drawPrimitive(QStyle::PE_FocusRect, &paint, rect(), colorGroup());
	paint.end();
	}

	QLabel::paintEvent(e);
}



