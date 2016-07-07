/*
 *
 * Copyright (c) 1998,1999, Mark Jackson.  
 * Copyright (c) 2005,2006 Tobias Glaesser <tobi.web@gmx.de> ... modified, improved, added timepicker
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as included in
 * the file COPYING in the main directory for this package.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Modified on May 5, 2000 by R. Marc Lewis <marc@blarg.net> to work 
 * with the QDate class.
 *
 */

#include <qlineedit.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qwidget.h>
#include <qdatetime.h>
#include <qhbox.h>

#ifndef QTODO_DATEPICKER
#define QTODO_DATEPICKER

class QTodoDateInput;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QTodoDateEdit;
class QTodoTimeEdit;
class QTodoPushButton;
class QTodoDatetimeLabel;
class QButtonGroup;
class QLabel;
class QCalMonthLookup;

class QTodoDateInput : public QHBox
{
	Q_OBJECT

	public:
	QTodoDateInput(QWidget*, const QDate);
	const QDate getQDate();
	//GofunDateValidator* validator(void) {return dv;}
	void setDate(const QDate, bool prevent_emit_change = false);
	void setPaletteBackgroundColor(const QColor&);
	void setTextMode(bool);
	void setEditable(bool);
	void updateFocusInDate();
	
	signals:
	void classifyRequest(int,int,char*);
	void dayLabel(int, int, int, char*);
	void dateChanged();
	void oldDateChanged(const QDate&);

	public slots:
	void setFocus();

	protected slots:
	void slotDayLabel(int, int, int, char*);
	void slotClassifyRequest(int,int,char*);
	void slotDayLookup();
	void dateChangedByLookup(const QDateTime&);

	private slots:
	void emitDateChanged(const QDate&);
	void emitOldDateChanged(const QDate&);

	protected:
	void resizeEvent(QResizeEvent*);
	void mousePressEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
	
	private:
	//GofunDateValidator* dv;
	QTodoPushButton* use_picker;
	QTodoDateEdit* date_edit;
	QTodoDatetimeLabel* date_text;
	QCalMonthLookup* cal;

	bool prevent_emit_change;
};

class QTodoTimeInput : public QHBox
{
	Q_OBJECT
	public:
	QTodoTimeInput(QWidget*,const QTime);
	const QTime getQTime();
	void setPaletteBackgroundColor(const QColor&);
	void setTextMode(bool);
	void setEditable(bool);
	void updateFocusInTime();

	public slots:
	void setTime(const QTime, bool prevent_emit_change = false);

	private slots:
	void timePicker();
	void emitTimeChanged(const QTime&);
	void emitOldTimeChanged(const QTime&);

	signals:
	void timeChanged();
	void oldTimeChanged(const QTime&);

	private:
	QTodoTimeEdit* time_edit;
	QTodoDatetimeLabel* time_text;
	QTodoPushButton* use_picker;

	bool prevent_emit_change;
};

class QTodoClock;

class QTodoTimePicker : public QFrame
{
	Q_OBJECT

	public:
	QTodoTimePicker(QWidget*,const QTime);
	~QTodoTimePicker();
	void popup();

	public slots:
	void setTime(const QTime&);

	signals:
	void timeChanged(const QTime);
	void initialTimeChanged(const QTime&);

	private slots:
	//void processChange();
	void emitTimeChanged(const QTime&);
	void toAM();
	void toPM();
	void toNow();

	private:
	void paintEvent(QPaintEvent*);
	/*void keyPressEvent(QKeyEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);*/

	private:
	QTodoClock* clock;
	QTodoPushButton* am_button;
	QTodoPushButton* pm_button;
	QTime initial_time;
};

#endif


