/*
 *
 * Copyright (c) 1998,1999, Mark Jackson.  
 * Copyright (c) 2005,2006, Tobias Glaesser <tobi.web@gmx.de> ... improved, modified, added timepicker
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

#include <ctype.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <qevent.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qvbox.h>
#include <qlabel.h>

#include "qtodo_datepicker.h"
#include "qtodo_settings_container.h"
#include "qcalmonthlookup.h"
#include "qtodo_xpm.h"
#include "qtodo_misc.h"
#include "qtodo_push_button.h"
#include "qtodo_clock.h"
#include "qtodo_datetimeedit.h"
#include "qtodo_datetime_label.h"

void attachWidgetToWidget(QWidget* base, QWidget* widget)
{
	widget->move(base->mapToGlobal(base->rect().bottomLeft()));
	if(widget->mapToGlobal(widget->rect().bottomRight()).x() > QApplication::desktop()->width())
		widget->move(widget->x() - widget->width() + base->width(), widget->y());
	if(widget->mapToGlobal(widget->rect().bottomLeft()).x() < 0)
		widget->move(widget->x() - widget->mapToGlobal(widget->rect().bottomLeft()).x(), widget->y());
	if(widget->mapToGlobal(widget->rect().bottomRight()).y() > QApplication::desktop()->height())
		widget->move(widget->x(), base->mapToGlobal(base->rect().topRight()).y() - widget->height());
}

QTodoDateInput::QTodoDateInput(QWidget* p, const QDate sdate)
		: QHBox(p)
{
	setLineWidth(0);
	setFrameStyle(QFrame::NoFrame);
	setFocusPolicy(QWidget::ClickFocus);

	date_text = new QTodoDatetimeLabel(this);
	date_text->setType(DateLabel);
	date_text->hide();
	date_edit=new QTodoDateEdit(this);
	date_edit->setAutoAdvance(true);

	date_edit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
	//date_edit->setFrameStyle(QFrame::Panel | QFrame::Plain);
	date_edit->setFrameStyle(QFrame::NoFrame);
	date_edit->setLineWidth(5);

	use_picker = new QTodoPushButton(this);
	use_picker->setFlat(true);
	use_picker->setMaximumWidth(16);
	use_picker->setMaximumHeight(16);
	use_picker->setPixmap(QTP::calendar.s(16,16));
	//use_picker->setPaletteBackgroundColor(QColor(QSC::get()->todo_item_head_color));

	CHECK_PTR(date_edit);

	setDate(sdate);

	prevent_emit_change = false;

	connect(use_picker,SIGNAL(clicked()),this,SLOT(slotDayLookup()));
	connect(date_edit,SIGNAL(valueChanged(const QDate&)),this,SLOT(emitDateChanged(const QDate&)));
	connect(date_edit,SIGNAL(oldValueChanged( const QDate& )),this,SLOT(emitOldDateChanged(const QDate&)));

	//date_edit->setValidator(new GofunDateValidator(this));
	//connect(date_edit,SIGNAL(returnPressed()),SLOT(slotNewText()));

}

void QTodoDateInput::emitOldDateChanged(const QDate& old_date)
{
	emit oldDateChanged(old_date);
}

void QTodoDateInput::keyPressEvent(QKeyEvent* event)
{
	if(event->state() == AltButton)
		event->ignore();
	QHBox::keyPressEvent(event);
}

void QTodoDateInput::setTextMode(bool b)
{
	if(b)
	{
		date_edit->hide();
		date_text->show();
		use_picker->hide();
	}
	else
	{
		date_text->hide();
		date_edit->show();
		use_picker->show();
	}
}

void QTodoDateInput::setEditable(bool b)
{
	use_picker->setEnabled(b);
	date_edit->setEnabled(b);
}

void QTodoDateInput::emitDateChanged(const QDate& newDate)
{
	date_text->setDateTime(newDate);
	if(!prevent_emit_change)
		emit(dateChanged());
	prevent_emit_change = false;
}

void QTodoDateInput::setDate(const QDate newDate, bool _prevent_emit_change)
{
	prevent_emit_change = _prevent_emit_change;
	date_edit->setDate(newDate);
	date_text->setDateTime(newDate);
}

const QDate QTodoDateInput::getQDate()
{
	return date_edit->date();
}

void QTodoDateInput::slotClassifyRequest(int m,int y,char* c)
{
	emit(classifyRequest(m,y,c));
}

void QTodoDateInput::slotDayLabel(int m, int d, int y, char* t)
{
	emit(dayLabel(m,d,y,t));
}

void QTodoDateInput::slotDayLookup()
{
	cal = new QCalMonthLookup(this,0,WType_Popup | WDestructiveClose);
	cal->setDate(date_edit->date());
	cal->setMaximumWidth(250);
	cal->setMaximumHeight(200);
	attachWidgetToWidget(this,cal);

	connect(cal,SIGNAL(dateChanged(const QDateTime&)),this,SLOT(dateChangedByLookup(const QDateTime&)));
	connect(cal,SIGNAL(initialDateChanged(const QDate&)),this,SLOT(emitOldDateChanged(const QDate&)));

	cal->show();
}

void QTodoDateInput::resizeEvent(QResizeEvent* e)
{
	//date_edit->setGeometry(2,2,width()-27,height()-4);
	QWidget::resizeEvent(e);
}

void QTodoDateInput::mousePressEvent(QMouseEvent* qme)
{
	int x = qme->x();
	int y = qme->y();
	if (x > width()-20 && x < width()-4 && y > 5 && y < height()-5)
	{
/*		if (cal->isVisible())
		{
			cal->hide();
		}
		else
		{
			QRect rect = geometry();
			cal->setGeometry(rect.x(),rect.y()+height(),rect.width(),rect.height());
			cal->show();
		}*/
	}
}

void QTodoDateInput::dateChangedByLookup(const QDateTime& datetime)
{
	setDate(datetime.date());
}

void QTodoDateInput::setPaletteBackgroundColor(const QColor& col)
{
	QHBox::setPaletteBackgroundColor(col);
	use_picker->setPaletteBackgroundColor(col);
	QPalette my = palette();
	my.setColor(QColorGroup::Base,col);
	my.setColor(QColorGroup::Button,col);
	date_edit->setPalette(my);
}

QTodoTimeInput::QTodoTimeInput(QWidget* p, const QTime _time) : QHBox(p)
{
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

	time_text = new QTodoDatetimeLabel(this);
	time_text->setType(TimeLabel);
	time_text->setFormat(QTodoMisc::ISOTime);
	time_text->hide();
	time_edit = new QTodoTimeEdit(this);
	time_edit->setAutoAdvance(true);
	time_edit->setDisplay(QTodoTimeEdit::Hours | QTodoTimeEdit::Minutes);
	//time_edit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
	//time_edit->setMaximumWidth(50);

	use_picker = new QTodoPushButton(this);
	use_picker->setFlat(true);
	use_picker->setMaximumWidth(16); 
	use_picker->setMaximumHeight(16);
	use_picker->setPixmap(QTP::clock.s(12,12));


	connect(use_picker,SIGNAL(clicked()),this,SLOT(timePicker()));
	connect(time_edit,SIGNAL(valueChanged(const QTime&)),this,SLOT(emitTimeChanged(const QTime&)));
	connect(time_edit,SIGNAL(oldValueChanged(const QTime&)),this,SLOT(emitOldTimeChanged(const QTime&)));

	setTime(_time);
	prevent_emit_change = false;
}

void QTodoTimeInput::emitOldTimeChanged(const QTime& old_time)
{
	emit oldTimeChanged(old_time);
}

void QTodoTimeInput::setTime(const QTime _time, bool _prevent_emit_change)
{
	prevent_emit_change = _prevent_emit_change;
	time_edit->setTime(_time);
}

void QTodoTimeInput::timePicker()
{
	QTodoTimePicker* time_picker = new QTodoTimePicker(this,getQTime());

	connect(time_picker,SIGNAL(timeChanged(const QTime)),this,SLOT(setTime(const QTime)));
	connect(time_picker,SIGNAL(initialTimeChanged(const QTime&)),this,SLOT(emitOldTimeChanged(const QTime& )));

	time_picker->adjustSize(); //the widget won't tell the right size without this call
	attachWidgetToWidget(this,time_picker);
	time_picker->popup();
}

const QTime QTodoTimeInput::getQTime()
{
	return time_edit->time();
}

QTodoTimePicker::QTodoTimePicker(QWidget* p, const QTime time) : QFrame(p,0,WType_Popup | WDestructiveClose)
{
	initial_time = time;

	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	setFrameStyle(Box | Plain);
	setLineWidth(0);
	setPaletteBackgroundColor(QColor(211, 214, 223));

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(5);

	/*bg_hours = new QButtonGroup();
	bg_hours->setExclusive(true);
	bg_mins = new QButtonGroup();
	bg_mins->setExclusive(true);

	QVBox* hours12 = new QVBox(this);
	for(int i = 0; i < 12; i++)
	{
		QPushButton* pb = new QPushButton(QString().sprintf("%02i",i),hours12);
		pb->setMaximumWidth(30);
		pb->setToggleButton(true);
		pb->setPaletteBackgroundColor(QColor(255,255,255));
		bg_hours->insert(pb,i);

		connect(pb,SIGNAL(clicked()),this,SLOT(processChange()));
	}

	QVBox* hours24 = new QVBox(this);
	for(int i = 12; i < 24; i++)
	{
		QPushButton* pb = new QPushButton(QString::number(i),hours24);
		pb->setMaximumWidth(30);
		pb->setToggleButton(true);
		pb->setPaletteBackgroundColor(QColor(255,255,255));
		bg_hours->insert(pb,i);

		connect(pb,SIGNAL(clicked()),this,SLOT(processChange()));
	}

	QVBox* mins = new QVBox(this);
	for(int i = 0; i < 60; i+=5)
	{
		QPushButton* pb = new QPushButton(QString().sprintf("%02i",i),mins);
		pb->setMaximumWidth(30);
		pb->setToggleButton(true);
		pb->setPaletteBackgroundColor(QColor(255,255,255));
		bg_mins->insert(pb,i);

		connect(pb,SIGNAL(clicked()),this,SLOT(processChange()));
	}
	*/

	clock = new QTodoClock(this);
	clock->resize(150,150);
	clock->setMinimumHeight(150);
	clock->setPaletteBackgroundColor(Qt::white);

	connect(clock,SIGNAL(timeChanged(const QTime&)),this,SLOT(emitTimeChanged(const QTime&)));

	QButtonGroup* am_pm_group = new QButtonGroup();
	am_pm_group->setExclusive(true);

	am_button = new QTodoPushButton(tr("am"),this);
	am_button->setToggleButton(true);
	connect(am_button,SIGNAL(clicked()),this,SLOT(toAM()));

	pm_button = new QTodoPushButton(tr("pm"),this);
	pm_button->setToggleButton(true);
	connect(pm_button,SIGNAL(clicked()),this,SLOT(toPM()));

	am_pm_group->insert(am_button,0);
	am_pm_group->insert(pm_button,1);

	QPushButton* now_button = new QTodoPushButton(tr("Now"),this);
	connect(now_button,SIGNAL(clicked()),this,SLOT(toNow()));

	setTime(time);

	/*grid->addMultiCellWidget(new QLabel(tr("Hour"),this),0,0,0,1);
	grid->addWidget(new QLabel(tr(":"),this),0,2);
	grid->addWidget(new QLabel(tr("Min"),this),0,3);
	grid->addWidget(hours12,1,0);
	grid->addWidget(hours24,1,1);*/
	/*QWidget* spacer = new QWidget(this);
	spacer->setMaximumWidth(2);
	spacer->setPalette(QColor(0,0,0));
	grid->addWidget(spacer,1,2);
	grid->addWidget(mins,1,3);*/
	//grid->addMultiCellWidget(clock,2,2,0,3);
	grid->addWidget(am_button,0,0);
	grid->addWidget(pm_button,0,1);
	grid->addMultiCellWidget(clock,1,1,0,1);
	grid->addMultiCellWidget(now_button,2,2,0,1);
}

void QTodoTimePicker::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	p.setBrush( paletteBackgroundColor() );
	p.drawRoundRect(0, 0,
		  width(), height(),					
		  10, 10);
	p.setBrush(Qt::NoBrush);
	p.setPen(Qt::NoPen);

	p.end();

	QFrame::paintEvent(event);
}

QTodoTimePicker::~QTodoTimePicker()
{
	if(initial_time != clock->getTime())
		emit initialTimeChanged(initial_time);
}

void QTodoTimePicker::toAM()
{
	if(clock->getTime().hour() > 11)
		clock->setTime(QTime(clock->getTime().hour() - 12,clock->getTime().minute()));
	emit timeChanged(clock->getTime());
}

void QTodoTimePicker::toPM()
{
	if(clock->getTime().hour() < 12)
		clock->setTime(QTime(clock->getTime().hour() + 12,clock->getTime().minute()));
	emit timeChanged(clock->getTime());
}

void QTodoTimePicker::toNow()
{
	setTime(QTime::currentTime());
	emit timeChanged(clock->getTime());
}

void QTodoTimePicker::setTime(const QTime& time)
{
	/*bg_hours->setButton(time.hour());
	bg_mins->setButton(time.minute() - (time.minute() % 5));*/
	clock->setTime(time);
	if(time.hour() > 11)
		pm_button->setOn(true);
	else
		am_button->setOn(true);
}

void QTodoTimePicker::emitTimeChanged(const QTime& time)
{
	emit timeChanged(time);
}

void QTodoTimePicker::popup()
{
	show();
}

/*void QTodoTimePicker::processChange()
{
	QString tmp;

	if(bg_hours->selected())
	{
		tmp += dynamic_cast<QPushButton*>(bg_hours->selected())->text();
	}
	else
	{
		tmp += "00";
	}
	tmp += ":";
	if(bg_mins->selected())
	{
		tmp += dynamic_cast<QPushButton*>(bg_mins->selected())->text();
	}
	else
	{
		tmp += "00";
	}

	emit timeChanged(QTime::fromString(tmp));
}*/

/*void QTodoTimePicker::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		close();
	QFrame::keyPressEvent(event);
}*/

/*void QTodoTimePicker::mouseDoubleClickEvent(QMouseEvent*)
{
	close();
}*/

void QTodoTimeInput::setPaletteBackgroundColor(const QColor& col)
{
	QHBox::setPaletteBackgroundColor(col);
	use_picker->setPaletteBackgroundColor(col);
	QPalette my = palette();
	my.setColor(QColorGroup::Base,col);
	my.setColor(QColorGroup::Button,col);
	time_edit->setPalette(my);
}

void QTodoTimeInput::emitTimeChanged(const QTime& newTime)
{
	time_text->setDateTime(QDateTime(QDate(),newTime));
	if(!prevent_emit_change)
		emit timeChanged();
	prevent_emit_change = false;
}

void QTodoTimeInput::setTextMode(bool b)
{
	if(b)
	{
		time_edit->hide();
		time_text->show();
		use_picker->hide();
	}
	else
	{
		time_text->hide();
		time_edit->show();
		use_picker->show();
	}
}

void QTodoTimeInput::setEditable(bool b)
{
	use_picker->setEnabled(b);
	time_edit->setEnabled(b);
}

void QTodoDateInput::setFocus()
{
	date_edit->setFocusSection(0);
	date_edit->setFocus();
}

void QTodoDateInput::updateFocusInDate()
{
	date_edit->setFocusInDate(date_edit->date());
}

void QTodoTimeInput::updateFocusInTime()
{
	time_edit->setFocusInTime(time_edit->time());
}








