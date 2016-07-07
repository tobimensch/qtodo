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
#include <qtoolbutton.h>
#include <qhbox.h>

#include "qtodo_list_filter.h"
#include "qtodo_list_widget.h"
#include "qtodo_list.h"
#include "qtodo_priority_widget.h"
#include "qtodo_status_widget.h"
#include "qtodo_datepicker.h"
#include "qtodo_combo_widget.h"
#include "qtodo_misc.h"
#include "qtodo_font.h"
#include "qtodo_xpm.h"
#include "qtodo_push_button.h"

QTodoListFilter::QTodoListFilter(QTodoListWidget* parent) : QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	QHBox* hbox_std = new QHBox(this);
	layout->addWidget(hbox_std);
	
	new QLabel(tr("Filter:"),hbox_std);
	text = new QLineEdit(hbox_std);

	connect(text,SIGNAL(textChanged(const QString&)),todoListWidget()->getList(),SLOT(filter(const QString&)));

	//hbox_ext = new QHBox(this);
	new QLabel(tr("Priority:"),hbox_std);
	priority = new QTodoPriorityWidget(hbox_std);
	priority->insert(QTP::any.s(16,16),tr("Any"),ANY);
	priority->insertSeparator(-1);
	priority->insertSeparator(PRIO_NUM);
	priority->insert(QTP::not_not_available.s(16,16),tr("Not N/A"),PRIO_NOT_NA);
	priority->insert(QTP::not_low.s(16,16),tr("Not low"),PRIO_NOT_LOW);
	priority->insert(QTP::not_normal.s(16,16),tr("Not normal"),PRIO_NOT_NORMAL);
	priority->insert(QTP::not_high.s(16,16),tr("Not high"),PRIO_NOT_HIGH);

	new QLabel(tr("Status:"),hbox_std);
	status = new QTodoStatusWidget(hbox_std);
	status->insert(QTP::any.s(16,16),tr("Any"),ANY);
	status->insertSeparator(-1);
	status->insertSeparator(STAT_NUM);
	status->insert(QTP::not_not_available.s(16,16),tr("Not N/A"),STAT_NOT_NA);
	status->insert(QTP::not_planning.s(16,16),tr("Not planning"),STAT_NOT_PLANNING);
	status->insert(QTP::not_in_progress.s(16,16),tr("Not in progress"),STAT_NOT_IN_PROGRESS);
	status->insert(QTP::not_done.s(16,16),tr("Not done"),STAT_NOT_DONE);
	//QWidget* spacer = new QWidget(hbox_ext);
	//spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
	new QLabel(tr("Deadline:"),hbox_std);

	deadline_filter_modes = new QTodoComboWidget(hbox_std);
	deadline_filter_modes->insert(QTP::any.s(16,16),tr("Any"),ANY);
	deadline_filter_modes->insertSeparator(-1);
	deadline_filter_modes->insert(QPixmap(),tr("="),DLFM_EQ);
	deadline_filter_modes->insert(QPixmap(),tr("<="),DLFM_LESS_OR_EQ);
	deadline_filter_modes->insert(QPixmap(),tr("<"),DLFM_LESS);
	deadline_filter_modes->insert(QPixmap(),tr(">="),DLFM_GREATER_OR_EQ);
	deadline_filter_modes->insert(QPixmap(),tr(">"),DLFM_GREATER);

	deadline = new QTodoDateInput(hbox_std,QDate::currentDate());
	//deadline->setFont(QF::get()->small);
	//deadline->setMaximumHeight(20);
	deadline->hide();

	refresh_button = new QTodoPushButton(hbox_std);
	refresh_button->setPixmap(QTP::reload);
	refresh_button->setFlat(true);
	refresh_button->setEnabled(false);

	setDefaults();

	connect(text,SIGNAL(textChanged(const QString&)),this,SLOT(processChange(const QString&)));
	connect(priority,SIGNAL(changed()),this,SLOT(processChange()));
	connect(status,SIGNAL(changed()),this,SLOT(processChange()));
	connect(deadline_filter_modes,SIGNAL(changed(int)),this,SLOT(deadlineFilterModeChanged(int)));
	connect(deadline,SIGNAL(dateChanged()),this,SLOT(processChange()));
	connect(refresh_button,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
	connect(todoListWidget(),SIGNAL(modified()),this,SLOT(enableRefreshButton()));

	//setDefaultWidget(hbox_std);
	//setExtendWidget(hbox_ext);
	//disable(true);
}

void QTodoListFilter::setDefaults()
{
	text->setText(QString());
	priority->setCurrent(ANY);
	status->setCurrent(ANY);
	deadline_filter_modes->setCurrent(ANY);
}

QTodoListWidget* QTodoListFilter::todoListWidget()
{
	return dynamic_cast<QTodoListWidget*>(parent());
}

int QTodoListFilter::getPriority()
{
	return priority->getCurrent();
}

int QTodoListFilter::getStatus()
{
	return status->getCurrent();
}

void QTodoListFilter::processChange()
{
	todoListWidget()->getList()->filter(text->text(),getPriority(),getStatus(),getDeadlineFilterMode(),getDeadline());
}

void QTodoListFilter::processChange(const QString&)
{
	processChange();
}

void QTodoListFilter::deadlineFilterModeChanged(int id)
{
	switch(id)
	{
		case ANY:
			deadline->hide();
			break;
		default:
			deadline->show();
	}
	processChange();
}

int QTodoListFilter::getDeadlineFilterMode()
{
	return deadline_filter_modes->getCurrent();
}

QDate QTodoListFilter::getDeadline()
{
	return deadline->getQDate();
}

void QTodoListFilter::enableRefreshButton()
{
	if(isFilter())
		refresh_button->setEnabled(true);
}

void QTodoListFilter::refreshButtonClicked()
{
	refresh_button->setEnabled(false);
	processChange();
}

bool QTodoListFilter::isFilter()
{
	if(!text->text().isEmpty() ||
	    getPriority() != ANY ||
	    getStatus() != ANY ||
	    getDeadlineFilterMode() != ANY)
		return true;
	return false;
}

void QTodoListFilter::focusFilter()
{
	text->setFocus();
}

void QTodoListFilter::refresh()
{
	refreshButtonClicked();
}

void QTodoListFilter::reset()
{
	setDefaults();
	refresh();
}



