/***************************************************************************
 *   Copyright (C) 2006 by Tobias Glaesser                                 *
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

/****************************************************************************
** $Id: qt/aclock.h   3.3.5   edited Aug 31 12:17 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qwidget.h>
#include <qdatetime.h>

#ifndef QTODO_CLOCK_H
#define QTODO_CLOCK_H

class QTimer;

class QTodoClock : public QWidget              // analog clock widget
{
	Q_OBJECT
	public:
	QTodoClock( QWidget *parent=0, const char *name=0 );
	void setAutoMask(bool b);
	QTime getTime() const;

	signals:
	void timeChanged(const QTime&);

	protected:
	void updateMask();
	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void handleMouseEvent(QMouseEvent*);
	void drawClock(QPainter*);

	private slots:
	void timeout();

	public slots:
	void setTime( const QTime & t );

	private:
	QPoint clickPos;
	QTime time;
	QTimer *internalTimer;
};


#endif // ACLOCK_H

