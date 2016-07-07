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

#include <qlabel.h>
#include <qdatetime.h>

#ifndef QTODO_DATETIME_LABEL
#define QTODO_DATETIME_LABEL

class QWidget;

enum
{
	DateTimeLabel,
	DateLabel,
	TimeLabel
};

class QTodoDatetimeLabel : public QLabel
{
	public:
	QTodoDatetimeLabel(QWidget*);
	void setFormat(const QString&);
	void setDateTime(const QDateTime&);
	void setType(int);
	QDateTime getDateTime();
	QString getDateTimeString();
	QString getDateTimeLocalString();
	QDate getDate();
	QTime getTime();

	private:
	QDateTime datetime;
	QString format;

	int type;
};

#endif

