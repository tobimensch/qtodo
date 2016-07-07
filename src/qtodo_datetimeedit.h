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
** $Id: qt/qdatetimeedit.h   3.3.5   edited Aug 31 12:17 $
**
** Definition of date and time edit classes
**
** Created : 001103
**
** Copyright (C) 2005 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qwidget.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qframe.h>

#ifndef QTODO_DATETIMEEDIT
#define QTODO_DATETIMEEDIT

class QTodoDateTimeEditBase : public QFrame
{
	Q_OBJECT
public:
	QTodoDateTimeEditBase( QWidget* parent=0, const char* name=0 )
			: QFrame( parent, name ) {}

	virtual bool setFocusSection( int sec ) = 0;
	virtual QString sectionFormattedText( int sec ) = 0;
	virtual void addNumber( int sec, int num ) = 0;
	virtual void removeLastNumber( int sec ) = 0;

public slots:
	virtual void stepUp() = 0;
	virtual void stepDown() = 0;

private slots:
	virtual void slotFocusInSignal() = 0;
	virtual void slotFocusOutSignal() = 0;

private:

	QTodoDateTimeEditBase( const QTodoDateTimeEditBase & );
	QTodoDateTimeEditBase &operator=( const QTodoDateTimeEditBase & );
};

class QTodoDateEditPrivate;

class QTodoDateEdit : public QTodoDateTimeEditBase
{
	Q_OBJECT
	Q_ENUMS( Order )
	Q_PROPERTY( Order order READ order WRITE setOrder )
	Q_PROPERTY( QDate date READ date WRITE setDate )
	Q_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
	Q_PROPERTY( QDate maxValue READ maxValue WRITE setMaxValue )
	Q_PROPERTY( QDate minValue READ minValue WRITE setMinValue )

public:
	QTodoDateEdit( QWidget* parent=0,  const char* name=0 );
	QTodoDateEdit( const QDate& date, QWidget* parent=0,  const char* name=0 );
	~QTodoDateEdit();

	enum Order { DMY, MDY, YMD, YDM };

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

public slots:
	virtual void setDate( const QDate& date );

private slots:
	void slotFocusInSignal();
	void slotFocusOutSignal();

public:
	QDate date() const;
	virtual void setOrder( Order order );
	Order order() const;
	virtual void setAutoAdvance( bool advance );
	bool autoAdvance() const;

	virtual void setMinValue( const QDate& d ) { setRange( d, maxValue() ); }
	QDate minValue() const;
	virtual void setMaxValue( const QDate& d ) { setRange( minValue(), d ); }
	QDate maxValue() const;
	virtual void setRange( const QDate& min, const QDate& max );
	QString separator() const;
	virtual void setSeparator( const QString& s );
	bool setFocusSection( int s );

	// Make removeFirstNumber() virtual in QTodoDateTimeEditBase in 4.0
	void removeFirstNumber( int sec );
	void setFocusInDate(const QDate& date) { focus_in_date = date; }

signals:
	void valueChanged( const QDate& date );
	void oldValueChanged( const QDate& date );

protected:
	bool event( QEvent *e );
	void timerEvent( QTimerEvent * );
	void resizeEvent( QResizeEvent * );
	void stepUp();
	void stepDown();
	QString sectionFormattedText( int sec );
	void addNumber( int sec, int num );

	void removeLastNumber( int sec );

	virtual void setYear( int year );
	virtual void setMonth( int month );
	virtual void setDay( int day );
	virtual void fix();
	virtual bool outOfRange( int y, int m, int d ) const;

protected slots:
	void updateButtons();

private:
	void init();
	int sectionOffsetEnd( int sec ) const;
	int sectionLength( int sec ) const;
	QString sectionText( int sec ) const;
	QTodoDateEditPrivate* d;

	QTodoDateEdit( const QTodoDateEdit & );
	QTodoDateEdit &operator=( const QTodoDateEdit & );

	QDate focus_in_date;

	int m_spin_width;
};

class QTodoTimeEditPrivate;

class QTodoTimeEdit : public QTodoDateTimeEditBase
{
	Q_OBJECT
	Q_SETS( Display )
	Q_PROPERTY( QTime time READ time WRITE setTime )
	Q_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
	Q_PROPERTY( QTime maxValue READ maxValue WRITE setMaxValue )
	Q_PROPERTY( QTime minValue READ minValue WRITE setMinValue )
	Q_PROPERTY( Display display READ display WRITE setDisplay )

public:
	enum Display {
	    Hours	= 0x01,
	    Minutes	= 0x02,
	    Seconds	= 0x04,
	    /*Reserved = 0x08,*/
	    AMPM	= 0x10
	};

	QTodoTimeEdit( QWidget* parent=0,  const char* name=0 );
	QTodoTimeEdit( const QTime& time, QWidget* parent=0,  const char* name=0 );
	~QTodoTimeEdit();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

public slots:
	virtual void setTime( const QTime& time );

public:
	QTime time() const;
	virtual void setAutoAdvance( bool advance );
	bool autoAdvance() const;

	virtual void setMinValue( const QTime& d ) { setRange( d, maxValue() ); }
	QTime minValue() const;
	virtual void setMaxValue( const QTime& d ) { setRange( minValue(), d ); }
	QTime maxValue() const;
	virtual void setRange( const QTime& min, const QTime& max );
	QString separator() const;
	virtual void setSeparator( const QString& s );

	uint display() const;
	void setDisplay( uint disp );

	// Make removeFirstNumber() virtual in QTodoDateTimeEditBase in 4.0
	void removeFirstNumber( int sec );
	void setFocusInTime(const QTime& time) { focus_in_time = time; }

signals:
	void valueChanged( const QTime& time );
	void oldValueChanged( const QTime& time );

protected:
	bool event( QEvent *e );
	void timerEvent( QTimerEvent *e );
	void resizeEvent( QResizeEvent * );
	void stepUp();
	void stepDown();
	QString sectionFormattedText( int sec );
	void addNumber( int sec, int num );
	void removeLastNumber( int sec );
	bool setFocusSection( int s );

	virtual bool outOfRange( int h, int m, int s ) const;
	virtual void setHour( int h );
	virtual void setMinute( int m );
	virtual void setSecond( int s );

protected slots:
	void updateButtons();

private slots:
	void slotFocusInSignal();
	void slotFocusOutSignal();

private:
	void init();
	QString sectionText( int sec );
	QTodoTimeEditPrivate* d;

	QTodoTimeEdit( const QTodoTimeEdit & );
	QTodoTimeEdit &operator=( const QTodoTimeEdit & );

	QTime focus_in_time;

	int m_spin_width;
};

class QTodoNumberSection;
class QTodoDateTimeEditorPrivate;

class QTodoDateTimeEditor : public QWidget
{
    Q_OBJECT
public:
    QTodoDateTimeEditor( QTodoDateTimeEditBase * widget, QWidget *parent,
		       const char * name=0 );
    ~QTodoDateTimeEditor();

    void setControlWidget( QTodoDateTimeEditBase * widget );
    QTodoDateTimeEditBase * controlWidget() const;

    void setSeparator( const QString& s );
    QString separator() const;

    int  focusSection() const;
    bool setFocusSection( int s );
    void appendSection( const QTodoNumberSection& sec );
    void clearSections();
    void setSectionSelection( int sec, int selstart, int selend );
    bool eventFilter( QObject *o, QEvent *e );
    int  sectionAt( const QPoint &p );
    int mapSection( int sec );

signals:
	void focusInSignal();
	void focusOutSignal();

protected:
    void init();
    bool event( QEvent *e );
    void resizeEvent( QResizeEvent * );
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent *e );
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);

private:
    QTodoDateTimeEditBase* cw;
    QTodoDateTimeEditorPrivate* d;
};

class QTodoDateTimeEditPrivate;

class QTodoDateTimeEdit : public QWidget
{
	Q_OBJECT
	Q_PROPERTY( QDateTime dateTime READ dateTime WRITE setDateTime )

public:
	QTodoDateTimeEdit( QWidget* parent=0, const char* name=0 );
	QTodoDateTimeEdit( const QDateTime& datetime, QWidget* parent=0,
	               const char* name=0 );
	~QTodoDateTimeEdit();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

public slots:
	virtual void setDateTime( const QDateTime & dt );

public:
	QDateTime dateTime() const;

	QTodoDateEdit* dateEdit() { return de; }
	QTodoTimeEdit* timeEdit() { return te; }

	virtual void setAutoAdvance( bool advance );
	bool autoAdvance() const;

signals:
	void valueChanged( const QDateTime& datetime );

protected:
	// ### make init() private in Qt 4.0
	void init();
	void resizeEvent( QResizeEvent * );

protected slots:
	// ### make these two functions private in Qt 4.0,
	//     and merge them into one with no parameter
	void newValue( const QDate& d );
	void newValue( const QTime& t );

private:
	QTodoDateEdit* de;
	QTodoTimeEdit* te;
	QTodoDateTimeEditPrivate* d;

	QTodoDateTimeEdit( const QTodoDateTimeEdit & );
	QTodoDateTimeEdit &operator=( const QTodoDateTimeEdit & );
};

#endif


