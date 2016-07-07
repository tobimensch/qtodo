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

#include "qtodo_datetime_label.h"
#include "qtodo_misc.h"

QTodoDatetimeLabel::QTodoDatetimeLabel(QWidget* parent) : QLabel(parent)
{
	type = DateTimeLabel;
}

void QTodoDatetimeLabel::setFormat(const QString& _format)
{
	format = _format;
}

void QTodoDatetimeLabel::setDateTime(const QDateTime& _datetime)
{
	datetime = _datetime;
	if(format.isEmpty())
	{
		QString tmp;
		switch(type)
		{
			case DateTimeLabel:
				tmp = QTodoMisc::dateTime2LocalString(datetime);
				break;
			case DateLabel:
				tmp = datetime.date().toString(Qt::LocalDate);
				break;
			case TimeLabel:
				tmp = datetime.time().toString();
				if(!tmp.isEmpty())
					tmp.setLength(tmp.length()-3);
				break;
		}
		setText(tmp);
	}
	else
	{
		QString tmp;
		switch(type)
		{
			case DateTimeLabel:
				tmp = datetime.toString(format);
				break;
			case DateLabel:
				tmp = datetime.date().toString(format);
				break;
			case TimeLabel:
				tmp = datetime.time().toString(format);
				break;
		}
		setText(tmp);
	}
}

QDateTime QTodoDatetimeLabel::getDateTime()
{
	return datetime;
}

QDate QTodoDatetimeLabel::getDate()
{
	return datetime.date();
}

QTime QTodoDatetimeLabel::getTime()
{
	return datetime.time();
}

QString QTodoDatetimeLabel::getDateTimeString()
{
	return datetime.toString(Qt::ISODate).replace("T"," ");
}

void QTodoDatetimeLabel::setType(int _type)
{
	type = _type;
}

QString QTodoDatetimeLabel::getDateTimeLocalString()
{
	return text();
}


