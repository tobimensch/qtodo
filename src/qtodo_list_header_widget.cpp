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
#include <qlineedit.h>
#include <qtextedit.h>
#include <qimage.h>
#include <qhbox.h>
#include <qtoolbutton.h>
#include <qcursor.h>

#include "qtodo_list_widget.h"
#include "qtodo_list_view_item.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_list.h"
#include "qtodo_parser.h"
#include "qtodo_xpm.h"
#include "qtodo_font.h"
#include "qtodo_misc.h"
#include "qtodo_push_button.h"
#include "qtodo_label.h"

QTodoListHeaderWidget::QTodoListHeaderWidget(QTodoListWidget* parent) : QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
	setLineWidth(2);

	QHBoxLayout* hbox = new QHBoxLayout(this);
	hbox->setMargin(3);
	
	name = new QLabel(this);
	name->hide();
	description = new QTodoLabel(this);
	description->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	QTodoPushButton* properties = new QTodoPushButton(tr("Properties"),this);
	properties->setPixmap(QTP::configure.s(16,16));
	//QSpacerItem* spacer = new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
	actions = new QTodoPushButton(this);
	actions->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
	actions->setMaximumWidth(12);
	actions->setPixmap(QTodoXPM::rightarrow.s(12,12));
	actions->setAutoRepeat(false);

	connect(properties,SIGNAL(clicked()),this,SLOT(propertiesDlg()));
	connect(actions,SIGNAL(clicked()),this,SLOT(actionsClicked()));

	//hbox->addWidget(QF::setFont(new QLabel(tr("Name:"),this),QF::get()->bold));
	QTodoLabel* description_label = new QTodoLabel(this);
	description_label->setText(tr("Description:"));
	description_label->setMaximumWidth(QFontMetrics(font()).width(description_label->text())+15);
	description_label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
	hbox->addWidget(description_label);
	hbox->addWidget(description);
	//hbox->addItem(spacer);
	hbox->addWidget(properties);
	hbox->addWidget(actions);
}

void QTodoListHeaderWidget::load(QTodoTopNode* header)
{
	name->setText(header->name);
	description->setText(header->description);
}

void QTodoListHeaderWidget::propertiesDlg()
{
	QTodoPropertiesDlg prop_dlg;
	prop_dlg.setFile(todoListWidget()->getFile());
	prop_dlg.setName(name->text());
	prop_dlg.setDescription(description->realText());
	if(prop_dlg.exec() == QDialog::Accepted)
	{
		name->setText(prop_dlg.getName());
		description->setText(prop_dlg.getDescription());
		todoListWidget()->getListViewItem()->update();
		todoListWidget()->setModified();
	}	
}

void QTodoListHeaderWidget::save(QTextStream& stream)
{
	stream << name->text() << " TODO" << endl;
	stream << QString().fill('-',name->text().length()+1+QString("TODO").length()) << endl;
	stream << description->realText() << endl << endl;
}

QString QTodoListHeaderWidget::getName()
{
	return name->text();
}

QString QTodoListHeaderWidget::getDescription()
{
	return description->realText();
}

QTodoListWidget* QTodoListHeaderWidget::todoListWidget()
{
	return dynamic_cast<QTodoListWidget*>(parent());
}

void QTodoListHeaderWidget::setName(const QString& _name)
{
	name->setText(_name);
}

QTodoPropertiesDlg::QTodoPropertiesDlg()
{
	setCaption(tr("List settings"));
	setIcon(QTP::configure.s(16,16));

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(3);
	
	file = new QLineEdit(this);
	file->setReadOnly(true);
	QPalette pal = file->palette();
	pal.setActive(file->palette().disabled());
	file->setPalette(pal);
	name = new QLineEdit(this);
	description = new QTextEdit(this);
	description->setTabChangesFocus(true);
	QFrame* hline = new QFrame(this);
	hline->setFrameStyle(QFrame::HLine | QFrame::Raised);

	name->setFocus();

	QHBox* hbox = new QHBox(this);

	QPushButton* ok = new QPushButton(tr("Ok"),hbox);
	QPushButton* cancel = new QPushButton(tr("Cancel"),hbox);
	
	connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
	
	grid->addWidget(new QLabel(tr("File:"),this),0,0);
	grid->addWidget(file,0,1);
	grid->addMultiCellWidget(hline,1,1,0,1);
	grid->addWidget(new QLabel(tr("Name:"),this),2,0);
	grid->addWidget(name,2,1);
	grid->addWidget(new QLabel(tr("Description:"),this),3,0);
	grid->addMultiCellWidget(description,4,4,0,1);
	grid->addMultiCellWidget(hbox,5,5,0,1);
}

void QTodoPropertiesDlg::setFile(const QString& _file)
{
	file->setText(_file);
}

void QTodoPropertiesDlg::setName(const QString& _name)
{
	name->setText(_name);
}

void QTodoPropertiesDlg::setDescription(const QString& _description)
{
	description->setText(_description);
}

QString QTodoPropertiesDlg::getName()
{
	return name->text();
}

QString QTodoPropertiesDlg::getDescription()
{
	return description->text();
}

void QTodoListHeaderWidget::actionsClicked()
{
	QPoint pos = actions->mapToGlobal(QPoint(0,actions->height()));
	todoListWidget()->getList()->contextMenu(pos);
}


