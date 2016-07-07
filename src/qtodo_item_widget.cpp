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
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qhbox.h>
#include <qtimer.h>
#include <qaccel.h>

#include "qtodo_settings_container.h"
#include "qtodo_item_widget.h"

QTodoItemWidget::QTodoItemWidget(QWidget* parent) : QFrame(parent)
{
	init();
}

void QTodoItemWidget::init()
{
	setMouseTracking(true);
	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}

void QTodoItemWidget::keyPressEvent(QKeyEvent* event)
{
	QFrame::keyPressEvent(event);
}

void QTodoItemWidget::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == LeftButton)
	{
		emitSelectToggled();
	}
}

void QTodoItemWidget::mouseDoubleClickEvent(QMouseEvent*)
{
	emitSelectDoubleToggled();
}

void QTodoItemWidget::emitSelectToggled()
{
	emit selectToggled();
}

void QTodoItemWidget::emitSelectDoubleToggled()
{
	emit selectDoubleToggled();
}

void QTodoItemWidget::mouseMoveEvent(QMouseEvent*)
{
}

void QTodoItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint pos;
	switch(event->reason())
	{
		case QContextMenuEvent::Mouse:
			pos = event->globalPos();
			break;
		case QContextMenuEvent::Keyboard:
		case QContextMenuEvent::Other:
			pos = QPoint(-1,-1);
			break;
	}

	emit contextMenuRequested(pos);
}

void QTodoItemWidget::enterEvent(QEvent*)
{
	if(!QSC::get()->extend_todo_on_mouse_over)
		return;

	QTimer::singleShot(1250,this,SLOT(emitShowHBoxBelowTrigger()));
}

void QTodoItemWidget::leaveEvent(QEvent*)
{
	if(!QSC::get()->extend_todo_on_mouse_over)
		return;

	if(cursorInsideWidget())
		return;

	emit hideHBoxBelowTrigger();
}

bool QTodoItemWidget::cursorInsideWidget()
{
	return rect().contains(mapFromGlobal(QCursor::pos()));
}

void QTodoItemWidget::emitShowHBoxBelowTrigger()
{
	emit showHBoxBelowTrigger();
}

void QTodoItemWidget::setActive(bool b)
{
	if(b)
		setFrameStyle(QFrame::Panel | QFrame::Raised);
	else
		setFrameStyle(QFrame::Panel | QFrame::Plain);
}



