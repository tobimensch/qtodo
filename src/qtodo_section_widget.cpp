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
#include <qlineedit.h>
#include <qlabel.h>

#include "qtodo_section_widget.h"
#include "qtodo_list_view_item.h"
#include "qtodo_settings_container.h"

QTodoSectionWidget::QTodoSectionWidget(QWidget*parent,QTodoListViewItemSection* _lists_item) : QFrame(parent)
{
	lists_item = _lists_item;

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(5);
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	setMaximumWidth(200);

	QHBoxLayout* hboxlay = new QHBoxLayout(this);
	hboxlay->setMargin(6);

	QLabel* label = new QLabel(tr("Section:"),this);
	label->setPaletteBackgroundColor(QColor(QSC::get()->todo_item_head_color));

	edit = new QLineEdit(this);
	edit->setFrame(false);
	
	QToolButton* actions = new QToolButton(Qt::RightArrow,this);
	actions->setMinimumWidth(16);

	hboxlay->addWidget(label);
	hboxlay->addWidget(edit,1);
	hboxlay->addWidget(actions);
}

void QTodoSectionWidget::save(QTextStream& stream)
{
	stream << endl << edit->text() << ":" << endl;
	stream << QString().fill('-',edit->text().length()+1);
	stream << endl;
}

void QTodoSectionWidget::setName(const QString& name)
{
	edit->setText(name);
}

QString QTodoSectionWidget::getName()
{
	return edit->text();
}

void QTodoSectionWidget::nameChanged(const QString&)
{
	lists_item->update();
}

