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
#include <qtoolbutton.h>
#include <qhbox.h>

#include "qtodo_extend_widget.h"

QTodoExtendWidget::QTodoExtendWidget(QWidget* parent) : QWidget(parent)
{
	default_widget = 0;
	extend_widget = 0;

	vbox = new QVBoxLayout(this);
	hbox_default = new QHBoxLayout();
	hbox_extend = new QHBoxLayout();

	vbox->addLayout(hbox_default);
	vbox->addLayout(hbox_extend);

	extend_button = new QToolButton(Qt::DownArrow,this);
	extend_button->setMaximumWidth(16);
	extend_button->setMinimumWidth(16);
	unextend_button = new QToolButton(Qt::UpArrow,this);
	unextend_button->setMaximumWidth(16);
	unextend_button->setMinimumWidth(16);
	unextend_button->hide();

	connect(extend_button,SIGNAL(clicked()),this,SLOT(extend()));
	connect(unextend_button,SIGNAL(clicked()),this,SLOT(unextend()));
}

void QTodoExtendWidget::extend()
{
	extend_button->hide();
	unextend_button->show();

	extend_widget->show();
}

void QTodoExtendWidget::unextend()
{
	extend_button->show();
	unextend_button->hide();

	extend_widget->hide();
}

void QTodoExtendWidget::setDefaultWidget(QWidget* _default_widget)
{
	default_widget = _default_widget;
	hbox_default->addWidget(default_widget);
	hbox_default->addWidget(extend_button);
}

void QTodoExtendWidget::setExtendWidget(QWidget* _extend_widget)
{
	extend_widget = _extend_widget;
	hbox_extend->addWidget(extend_widget);
	hbox_extend->addWidget(unextend_button);
	_extend_widget->hide();
}

void QTodoExtendWidget::disable(bool b)
{
	if(b)
	{
		extend_button->hide();
		unextend_button->hide();
	}
	else
	{
		extend_button->show();
		unextend_button->show();
	}
}


