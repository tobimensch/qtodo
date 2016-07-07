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

#include <qdialog.h>

#ifndef QTODO_SORT_DIALOG
#define QTODO_SORT_DIALOG

enum QTodoSortCriteria
{
	SORT_NONE,
	SORT_PRIORITY,
	SORT_STATUS,
	SORT_DEADLINE,
	SORT_ASSIGNED,
	SORT_CREATED,
	SORT_DONE,
	SORT_NUM
};

enum QTodoSortOrder
{
	ORDER_DESCENDING,
	ORDER_ASCENDING,
	ORDER_NUM
};

typedef std::map<int,std::pair<QTodoSortCriteria,QTodoSortOrder> > QTodoSortCriteriaMap;

class QComboBox;
typedef std::pair<QComboBox*,QComboBox*> QTodoSortComboPair;

class QTodoSortDialog : public QDialog
{
	Q_OBJECT

	public:
	QTodoSortDialog();
	const QTodoSortCriteriaMap* getCriterias();

	private slots:
	void criteriaChanged(int);

	private:
	QString sortCriteriaToText(int);
	QString sortOrderToText(int);

	std::map<int,QTodoSortComboPair> criterias;
};

#endif

