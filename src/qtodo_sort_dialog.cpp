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
#include <qcombobox.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qframe.h>

#include "qtodo_sort_dialog.h"
#include "qtodo_xpm.h"

QTodoSortDialog::QTodoSortDialog()
{
	setCaption(tr("Sort"));
	setIcon(QTP::sort.s(16,16));

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(3);
	grid->setSpacing(6);

	grid->addMultiCellWidget(new QLabel(tr("Set order and options of sort criterias:"),this),0,0,0,2);
	QFrame* hline = new QFrame(this);
	hline->setFrameStyle(QFrame::HLine | QFrame::Raised);
	grid->addMultiCellWidget(hline,1,1,0,2);

	int i;
	for(i = 0; i < SORT_NUM-1; ++i)
	{
		QComboBox* criteria = new QComboBox(this);
		for(int j = 0; j < SORT_NUM; ++j)
		{
			criteria->insertItem(sortCriteriaToText(j),j);
		}
		QComboBox* order = new QComboBox(this);
		order->setEnabled(false);
		for(int j = 0; j < ORDER_NUM; ++j)
		{
			QPixmap pix;
			if(j == ORDER_ASCENDING)
				pix = QTP::ascending.s(16,16);
			else
				pix = QTP::descending.s(16,16);
			order->insertItem(pix,sortOrderToText(j),j);
		}

		criterias[i] = QTodoSortComboPair(criteria,order);

		connect(criteria,SIGNAL(activated(int)),this,SLOT(criteriaChanged(int)));

		grid->addWidget(new QLabel(QString::number(i+1)+". "+tr("criteria"),this),i+2,0);
		grid->addWidget(criteria,i+2,1);
		grid->addWidget(order,i+2,2);
	}
	
	QHBox* hbox = new QHBox(this);
	QPushButton* sort_button = new QPushButton(QTP::sort.s(16,16),tr("Sort"),hbox);
	QPushButton* cancel_button = new QPushButton(tr("Cancel"),hbox);

	connect(sort_button,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel_button,SIGNAL(clicked()),this,SLOT(reject()));

	i+=3;
	grid->addMultiCellWidget(hbox,i,i,0,2);
}

QString QTodoSortDialog::sortCriteriaToText(int id)
{
	switch(id)
	{
		case SORT_NONE:
			return tr("None");
		case SORT_PRIORITY:
			return tr("Priority");
		case SORT_STATUS:
			return tr("Status");
		case SORT_DEADLINE:
			return tr("Deadline");
		case SORT_ASSIGNED:
			return tr("Assigned to");
		case SORT_CREATED:
			return tr("Created");
		case SORT_DONE:
			return tr("Done");
		default:
			return tr("N/A");
	}
}

QString QTodoSortDialog::sortOrderToText(int id)
{
	switch(id)
	{
		case ORDER_ASCENDING:
			return tr("Ascending");
		case ORDER_DESCENDING:
			return tr("Descending");
		default:
			return tr("N/A");
	}
}

void QTodoSortDialog::criteriaChanged(int id)
{
	QComboBox* combo = dynamic_cast<QComboBox*>(const_cast<QObject*>(sender()));

	for(std::map<int,QTodoSortComboPair>::iterator it = criterias.begin(); it != criterias.end(); ++it)
	{
		if((*it).second.first != combo && (*it).second.first->currentItem() == id)
		{
			(*it).second.first->setCurrentItem(0);
			(*it).second.second->setEnabled(false);
			combo->setFocus();
		}
		else if((*it).second.first == combo)
		{
			(*it).second.second->setEnabled(id);
		}
	}
}

const QTodoSortCriteriaMap* QTodoSortDialog::getCriterias()
{
	QTodoSortCriteriaMap* map = new QTodoSortCriteriaMap;
	for(std::map<int,QTodoSortComboPair>::iterator it = criterias.begin(); it != criterias.end(); ++it)
	{
		if((*it).second.first->currentItem() != SORT_NONE)
			(*map)[(*it).first] = std::pair<QTodoSortCriteria,QTodoSortOrder>(static_cast<QTodoSortCriteria>((*it).second.first->currentItem()),static_cast<QTodoSortOrder>((*it).second.second->currentItem()));
	}
	return map;
}

