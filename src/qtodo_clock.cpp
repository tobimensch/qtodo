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
** $Id: qt/aclock.cpp   3.3.5   edited Aug 31 12:13 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <cmath>

#include <qtimer.h>
#include <qpainter.h>
#include <qbitmap.h>

#include "qtodo_clock.h"

//
// Constructs an analog clock widget that uses an internal QTimer.
//

QTodoClock::QTodoClock( QWidget *parent, const char *name )
		: QWidget( parent, name )
{
	time = QTime::currentTime();                // get current time
	internalTimer = new QTimer( this ); // create internal timer
	connect( internalTimer, SIGNAL(timeout()), SLOT(timeout()) );
	internalTimer->start( 5000 );               // emit signal every 5 seconds
}

void QTodoClock::handleMouseEvent(QMouseEvent* e)
{
	QPoint v;
	v = e->pos() - QPoint(width()/2,height()/2);

	QPoint b(0,-1);

	double bl = b.manhattanLength();
	double vl = sqrt(pow(v.x(),2)+pow(v.y(),2));

	double z = (double)(v.x()*b.x() + v.y()*b.y())/(bl*vl);
	double za = acos(z);

	int id;
	if(e->button() != NoButton)
		id = e->button();
	else
		id = e->state();

	int s = 0;
	switch(id)
	{
		int h;
		int m;
		case LeftButton:
			if(v.x() < 0)
				h = 12 - int(za*2.);
			else
				h = int(za*2.);
			if(time.hour() > 11)
				h += 12;
			if(h == 24 || (time.hour() < 12 && h == 12))
				--h;	
			setTime(QTime(h,time.minute(),s));
			emit timeChanged(time);
			break;
		case RightButton:
			if(v.x() < 0)
				m = 60 - int(za*10.);
			else
				m = int(za*10.);
			h = time.hour();
			if(m == 60)
			{
				m = 59;
				s = 59;
			}
			setTime(QTime(h,m,s));
			emit timeChanged(time);
			break;
		default:
			break;
	}
}

void QTodoClock::mousePressEvent(QMouseEvent* e)
{
	if(isTopLevel())
		clickPos = e->pos() + QPoint(geometry().topLeft() - frameGeometry().topLeft());

	handleMouseEvent(e);
}

void QTodoClock::mouseMoveEvent( QMouseEvent *e )
{
	if(isTopLevel())
		move( e->globalPos() - clickPos );

	handleMouseEvent(e);
}

//
// The QTimer::timeout() signal is received by this slot.
//

//
// When we set an explicit time we don't want the timeout() slot to be
// called anymore as this relies on currentTime()
//
void QTodoClock::setTime( const QTime & t )
{
	time = t;
	disconnect( internalTimer, SIGNAL(timeout()), this, SLOT(timeout()) );
	if (autoMask())
		updateMask();
	else
		update();
}


void QTodoClock::timeout()
{
	QTime old_time = time;
	time = QTime::currentTime();
	if ( old_time.minute() != time.minute()
	        || old_time.hour() != time.hour() )
	{   // minute or hour has changed
		if (autoMask())
			updateMask();
		else
			update();
	}
}


void QTodoClock::paintEvent( QPaintEvent * )
{
	if ( autoMask() )
		return;
	QPainter paint( this );
	paint.setBrush( colorGroup().foreground() );
	drawClock( &paint );
}

// If the clock is transparent, we use updateMask()
// instead of paintEvent()

void QTodoClock::updateMask()  // paint clock mask
{
	QBitmap bm( size() );
	bm.fill( color0 );                  //transparent

	QPainter paint;
	paint.begin( &bm, this );
	paint.setBrush( color1 );           // use non-transparent color
	paint.setPen( color1 );

	drawClock( &paint );

	paint.end();
	setMask( bm );
}

//
// The clock is painted using a 1000x1000 square coordinate system, in
// the a centered square, as big as possible.  The painter's pen and
// brush colors are used.
//
void QTodoClock::drawClock( QPainter *paint )
{
	paint->save();

	paint->setWindow( -500,-500, 1000,1000 );

	QRect v = paint->viewport();
	int d = QMIN( v.width(), v.height() );
	paint->setViewport( v.left() + (v.width()-d)/2,
	                    v.top() + (v.height()-d)/2, d, d );

	QPointArray pts;

	paint->save();
	paint->rotate( 30*(time.hour()%12-3) + time.minute()/2 );
	pts.setPoints( 4, -20,0,  0,-20, 300,0, 0,20 );
	paint->drawConvexPolygon( pts );
	paint->restore();

	paint->save();
	paint->rotate( (time.minute()-15)*6 );
	pts.setPoints( 4, -10,0, 0,-10, 400,0, 0,10 );
	paint->drawConvexPolygon( pts );
	paint->restore();

	for ( int i=0; i<12; i++ )
	{
		paint->drawLine( 440,0, 460,0 );
		paint->rotate( 30 );
	}

	paint->restore();
}


void QTodoClock::setAutoMask(bool b)
{
	if (b)
		setBackgroundMode( PaletteForeground );
	else
		setBackgroundMode( PaletteBackground );
	QWidget::setAutoMask(b);
}

QTime QTodoClock::getTime() const
{
	return time;
}

