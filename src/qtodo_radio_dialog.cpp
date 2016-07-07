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
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qhbox.h>

#include "qtodo_radio_dialog.h"

QButton* QTodoRadioButton::radio_dialog_ok_button = 0;

QTodoRadioDialog::QTodoRadioDialog(const QString& caption, const QString& text)
{
	setCaption(caption);

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(3);

	button_group = new QButtonGroup(1,Qt::Horizontal,this);

	QPushButton* cancel = new QPushButton(tr("&Cancel"),this);
	ok = new QPushButton(tr("&Ok"),this);
	ok->setEnabled(false);

	QTodoRadioButton::radio_dialog_ok_button = ok;

	connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
	connect(button_group,SIGNAL(clicked(int)),this,SLOT(enableOkButton(int)));

	grid->addMultiCellWidget(new QLabel(text,this),0,0,0,1);
	grid->addMultiCellWidget(button_group,1,1,0,1);
	grid->addWidget(ok,2,0);
	grid->addWidget(cancel,2,1);	
}

void QTodoRadioDialog::add(const QString& s, int id)
{
	add(QPixmap(),s,id);
}

void QTodoRadioDialog::add(const QPixmap& p,const QString& s,int id)
{
	QTodoRadioButton* new_button = new QTodoRadioButton(button_group);
	new_button->setText(s);
	if(p.isNull())
		new_button->label->hide();
	else
		new_button->setPixmap(p);
	button_group->insert(new_button->radio_button,id);
	if(button_group->count() == 1)
	{
		button_group->setButton(id);
		enableOkButton(true);
	}
	else if(button_group->selectedId() != -1)
	{
		dynamic_cast<QRadioButton*>(button_group->selected())->setChecked(false);
		new_button->radio_button->setChecked(false);
		ok->setEnabled(false);
	}
}

int QTodoRadioDialog::selected()
{
	return button_group->selectedId();
}

void QTodoRadioDialog::enableOkButton(int)
{
	ok->setEnabled(true);
}

QTodoRadioButton::QTodoRadioButton(QWidget* parent) : QHBox(parent)
{
	label = new QPushButton(this);
	radio_button = new QRadioButton(this); 
	setStretchFactor(radio_button,2);

	connect(label,SIGNAL(clicked()),this,SLOT(onLabelClick()));
}

void QTodoRadioButton::setText(const QString& text)
{
	radio_button->setText(text);
}

void QTodoRadioButton::setPixmap(const QPixmap& pixmap)
{
	label->setPixmap(pixmap);
}

QTodoRadioButton::~QTodoRadioButton()
{
}

void QTodoRadioButton::onLabelClick()
{
	radio_button->setChecked(true);
	radio_dialog_ok_button->setEnabled(true);
}


