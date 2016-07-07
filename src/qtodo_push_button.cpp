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

#include "qtodo_push_button.h"

QTodoPushButton::QTodoPushButton(const QIconSet& icon,const QString& text,QWidget* parent) : QPushButton(icon,text,parent)
{
}

QTodoPushButton::QTodoPushButton(const QString& text,QWidget* parent) : QPushButton(text,parent)
{
}

QTodoPushButton::QTodoPushButton(QWidget* parent, const char* name) : QPushButton(parent,name)
{
}

QTodoPushButton::QTodoPushButton(QWidget* parent) : QPushButton(parent)
{
}

void QTodoPushButton::keyPressEvent(QKeyEvent* e)
{
	switch(e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if(isToggleButton())
				toggle();
			else
				emit clicked();
			return;
		default:
			break;
	}

	QPushButton::keyPressEvent(e);
}




