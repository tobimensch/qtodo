/*
Copyright (c) 2002 - 200x, Stephan Stapel
 All rights reserved.


Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions
are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
The names of the contributors to the component(s)
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <qdatetime.h>
#include "qcalendarbase.h"
#include "qcalimages.h"




QCalendarBase::QCalendarBase(QWidget* parent, const char* name, WFlags f)
    : QWidget(parent, name, f)
{
    m_bShowCursor          = true;
    m_bAppsRounded         = true;
    m_bBaseDraggingEnabled = true;
    m_pBackButton          = new QPushButton(QIconSet(calGetPixmap("back")), "", this);
    m_pForwButton          = new QPushButton(QIconSet(calGetPixmap("forward")), "", this);

    m_pBackButton->setFlat(true);
    m_pForwButton->setFlat(true);

    QObject::connect(m_pBackButton, SIGNAL(clicked()),
		     this, SLOT(backwardClicked()));
    QObject::connect(m_pForwButton, SIGNAL(clicked()),
		     this, SLOT(forwardClicked()));
} // !QCalendarBase()



QCalendarBase::~QCalendarBase()
{
	if(getDate() != m_initial_date)
		emit initialDateChanged(m_initial_date);
    // nothing in the base class
} // !~QCalendarBase()



QColor QCalendarBase::getBgColor() const
{
    return QColor(240, 240, 248);
} // !getBgColor()



QColor QCalendarBase::getCaptionBgColor() const
{
    return QColor(colorGroup().background());
} // !getCaptionBgColor()



QColor QCalendarBase::getAppTitleColor() const
{
    return QColor(60, 60, 60);
} // !getAppTitleColor()



QColor QCalendarBase::getCurrentDayBgColor() const
{
    return QColor(232, 232, 255);
} // !getCurrentDayBgColor()



QColor QCalendarBase::getSundayColor() const
{
    return QColor(235, 20, 20);
} // !geSundayColor()



QColor QCalendarBase::getDragColor() const
{
    return QColor(150, 150, 25);
} // !getDragColor()


void QCalendarBase::forwardClicked()
{
    onForwardClicked();
} // !forwardClicked()



void QCalendarBase::backwardClicked()
{
    onBackwardClicked();
} // !backwardClicked()



void QCalendarBase::onForwardClicked()
{
    // nothing happens in the base class
} // !onForwardClicked()



void QCalendarBase::onBackwardClicked()
{
    // nothing happens in the base class
} // !onBackwardClicked()



QDate QCalendarBase::getDate()
{
    return m_currDate;
} // !getDate()



void QCalendarBase::setDate(const QDate& newDate)
{
    m_initial_date = newDate;
    m_currDate = newDate;
    repaint();
    emit dateChanged(QDateTime(newDate));
} // !setDate()
