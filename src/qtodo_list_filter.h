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

#include <qwidget.h>
#include <qdatetime.h>

#ifndef QTODO_LIST_FILTER
#define QTODO_LIST_FILTER

enum{
	DLFM_EQ,
	DLFM_LESS_OR_EQ,
	DLFM_LESS,
	DLFM_GREATER_OR_EQ,
	DLFM_GREATER
};

class QTodoListWidget;
class QTodoPriorityWidget;
class QTodoStatusWidget;
class QTodoComboWidget;
class QHBox;
class QLineEdit;
class QTodoDateInput;
class QPushButton;

class QTodoListFilter : public QWidget
{
	Q_OBJECT

	public:
	QTodoListFilter(QTodoListWidget*);
	int getPriority();
	int getStatus();
	int getDeadlineFilterMode();
	QDate getDeadline();
	void focusFilter();
	void refresh();
	void reset();
	
	private slots:
	void deadlineFilterModeChanged(int);
	void processChange();
	void processChange(const QString&);
	void enableRefreshButton();
	void refreshButtonClicked();

	private:
	bool isFilter();
	void setDefaults();

	QTodoListWidget* todoListWidget();
	QHBox* hbox_ext;
	QTodoPriorityWidget* priority;
	QTodoStatusWidget* status;
	QTodoComboWidget* deadline_filter_modes;
	QTodoDateInput* deadline;
	QLineEdit* text;
	QPushButton* refresh_button;
};

#endif 
