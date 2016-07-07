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


#ifndef _QCALMONTHLOOKUP_H_
#define _QCALMONTHLOOKUP_H_



#include <qdatetime.h>
#include "qcalendarbase.h"




/** 
 * @class   QCalMonthLookup
 * @author  Stephan Stapel, <stephan.stapel@web.de>
 * @date    01.02.03
 * @brief   Lookup widget for dates
 */
class QCalMonthLookup : public QCalendarBase
{
 Q_OBJECT

 public:
    /**
     * Standard constructor
     */
    QCalMonthLookup(QWidget* parent = 0, const char* name = 0, WFlags f = 0);

    /**
     * Standard destructor
     */
    virtual ~QCalMonthLookup();

 protected:
    /**
     * Overwritten from standard Qt widget class.
     */
    virtual void paintEvent(QPaintEvent* thisEvent);

    /**
     * Overwritten from standard Qt widget class.
     */
    virtual void mousePressEvent(QMouseEvent* thisEvent);

    /**
     * Overwritten from standard Qt widget class.
     */ //NOTE: tobgle change (added)
    virtual void mouseDoubleClickEvent(QMouseEvent* thisEvent);

    /**
     * Same as above
     */
    virtual void resizeEvent(QResizeEvent* thisEvent);

    /**
     * Overwritten from QCalendarBase. Reacts to clicking the
     * backward navigation button
     *
     * @see onForwardClicked()
     */
    virtual void onBackwardClicked();

    /**
     * Same as the prior function.
     *
     * @see onBackwardClicked()
     */
    virtual void onForwardClicked();

 private slots:
    void yearBackwardClicked();
    void yearForwardClicked();
    void todayClicked();

 private:
    QPushButton* m_pTodayButton;
    QPushButton* m_pYearForwardButton;
    QPushButton* m_pYearBackwardButton;

    /**
     * @name common metrics
     */
    /*@{*/
    float m_dCaptionHeight;
    /*@}*/

    /**
     * The background color of the lookup widget
     */
    QColor m_lookupColor;

    /**
     * Helper variable. Matches the column/ row and
     * the date value.
     * The key is computed by: col + 7 * row
     */
    QMap<unsigned int, QDate> m_rgDayMappings;
};



#endif // !_QCALMONTHLOOKUP_H_
