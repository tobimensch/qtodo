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

#include <qframe.h>
#include <qdialog.h>
 
#ifndef QTODO_LIST_HEADER_WIDGET
#define QTODO_LIST_HEADER_WIDGET

class QLabel;
class QTodoLabel;
class QTextEdit;
class QLineEdit;
class QButton;
class QTodoTopNode;
class QTodoListWidget;

///Dialog to set properties of a TODOlist
class QTodoPropertiesDlg : public QDialog
{
	Q_OBJECT

	public:
	QTodoPropertiesDlg();
	void setFile(const QString&);
	void setName(const QString&);
	void setDescription(const QString&);
	QString getName();
	QString getDescription();
	
	private:
	QLineEdit* file;
	QLineEdit* name;
	QTextEdit* description;
};

///Head of a TODOlist
class QTodoListHeaderWidget : public QFrame
{
	Q_OBJECT

	public:
	QTodoListHeaderWidget(QTodoListWidget*);
	void load(QTodoTopNode*);
	void save(QTextStream&);
	QString getName();
	QString getDescription();
	void setName(const QString&);
	QTodoListWidget* todoListWidget();
	
	public slots:
	void propertiesDlg();

	private slots:
	void actionsClicked();
	
	private:
	QButton* actions;
	QLabel* name;
	QTodoLabel* description;
};

#endif

