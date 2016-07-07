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

#include <qstringlist.h>
#include <qpainter.h>
#include "qcalmonthlookup.h"
#include "qcalimages.h"
//#include "qapprepository.h" NOTE: tobgle change


const unsigned int nHorOffset = 14;




QCalMonthLookup::QCalMonthLookup(QWidget* parent, const char* name, WFlags f)
  : QCalendarBase(parent, name, f)
{
  m_pTodayButton = new QPushButton(tr("Today"),this);
  m_pYearBackwardButton = new QPushButton(QIconSet(calGetPixmap("back")), "", this);
  m_pYearForwardButton = new QPushButton(QIconSet(calGetPixmap("forward")), "", this);
  //m_pTodayButton->setFlat(true);
  m_pYearBackwardButton->setFlat(true);
  m_pYearForwardButton->setFlat(true);

  // --- common metrics ---
  m_dCaptionHeight = 20; // height of the caption bar

  m_currDate       = QDate::currentDate(); //QAppRepository::instance()->currentDate(); NOTE: tobgle change
  m_lookupColor    = paletteBackgroundColor(); //QColor(211, 214, 223); NOTE: tobgle change

  m_pBackButton->setPaletteBackgroundColor(m_lookupColor);
  m_pForwButton->setPaletteBackgroundColor(m_lookupColor);
  m_pTodayButton->setPaletteBackgroundColor(m_lookupColor);
  m_pYearBackwardButton->setPaletteBackgroundColor(m_lookupColor);
  m_pYearForwardButton->setPaletteBackgroundColor(m_lookupColor);

  connect(m_pTodayButton,SIGNAL(clicked()),this,SLOT(todayClicked()));
  connect(m_pYearBackwardButton,SIGNAL(clicked()),this,SLOT(yearBackwardClicked()));
  connect(m_pYearForwardButton,SIGNAL(clicked()),this,SLOT(yearForwardClicked()));
} // !QCalMonthLookup()



QCalMonthLookup::~QCalMonthLookup()
{
} // !QCalMonthLookup()



void QCalMonthLookup::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    float x_dist = 1.0f * (this->width() - 2 * nHorOffset) / 7;
  float y_dist = 1.0f * (this->height() - 3 * m_dCaptionHeight) / 6;

  // --- draw the borders ---
  p.setBrush( m_lookupColor );
  p.drawRoundRect(0, 0,
		  width(), height(),					
		  10, 10);
  p.setBrush(Qt::NoBrush);
  p.setPen(Qt::NoPen);
/*  p.setBrush( m_lookupColor );
  p.drawRect(0, 0,
		  width(), height());//,					
		  //10, 10);
  p.setBrush(Qt::NoBrush);
  p.setPen(Qt::NoPen);*/

  // --- draw month name ---
  QFont font = p.font();	
  //font.setBold(true); //tobgle change: this caused performance problems
  p.setFont(font);
  p.setPen(Qt::NoPen);
  QRect monthRect(0, 
		  0, 
		  width(), 
		  (unsigned int)(m_dCaptionHeight));
  p.drawText(monthRect,
	     Qt::AlignCenter,
	     m_currDate.toString("MMMM yyyy"));

  p.setPen(QColor(20, 20, 20));
  p.drawLine(nHorOffset,
	     (unsigned int)(m_dCaptionHeight + 2.0f),
	     width() - nHorOffset, 
	     (unsigned int)(m_dCaptionHeight + 2.0f));
  p.setPen(Qt::NoPen);

  // --- draw week days for caption ---    
  for (unsigned int i = 0; i < 7; i++)
    {
      QRect dayRect((unsigned int)(nHorOffset + i * x_dist),
		    (unsigned int)(m_dCaptionHeight + 5.0f),
		    (unsigned int)(x_dist),
		    (unsigned int)(m_dCaptionHeight));
      p.drawText(dayRect,
		 Qt::AlignHCenter,
		 QDate::shortDayName(i + 1) );
    } // !for(i)

  font.setBold(false);
  p.setFont(font);
    
  // --- draw caption outline ---
  p.drawRect(0, 
	     0,
	     width(), 
	     (unsigned int)(m_dCaptionHeight));    
    
  // --- draw the day numbers ---
  p.setPen(Qt::NoPen);
  p.setBrush(QColor(248, 248, 254));
  p.drawRect(nHorOffset, (unsigned int)(2 * m_dCaptionHeight),
	     width() - 2 * nHorOffset,
	     (unsigned int)(height() - 3 * m_dCaptionHeight));
    
  p.setPen(QPen(QColor(20, 20, 20), 2));
  p.moveTo(nHorOffset, 
	   (unsigned int)(height() - m_dCaptionHeight));
  p.lineTo(nHorOffset,
	   (unsigned int)(2 * m_dCaptionHeight));
  p.lineTo(width() - nHorOffset, 
	   (unsigned int)(2 * m_dCaptionHeight));
  p.setPen(QPen(QColor(20, 20, 20), 1));
  p.moveTo(nHorOffset, 
	   (unsigned int)(height() - m_dCaptionHeight));
  p.lineTo(width() - nHorOffset, 
	   (unsigned int)(height() - m_dCaptionHeight));
    
  QDate tempDate;
  QDate tempDate2;
  unsigned int col = 1;
  unsigned int row = 0;
    
  font = p.font();	
  p.setFont(font);
  p.setPen(QColor(20, 20, 20));
    
  // ---- draw the previous month ----	
  tempDate = m_currDate.addMonths(-1);
  tempDate2.setYMD(m_currDate.year(), // first day of the current month
		   m_currDate.month(),
		   1);
  tempDate2 = tempDate2.addDays(-1);
    
  if (tempDate2.dayOfWeek() != 7) // the new month starts at a monday, drawing the prev month is not necessary
    {	
      p.setPen(QColor(138, 138, 138));
      for (col = 1; col <= (unsigned int)tempDate2.dayOfWeek(); col++)
	{		
	  unsigned int day = col - tempDate2.dayOfWeek() + tempDate2.day(); // day to be drawn		
	  p.drawText(QRect((unsigned int)(nHorOffset + (col - 1) * x_dist),
			   (unsigned int)(2 * m_dCaptionHeight + 0 /* row */ * y_dist),
			   (unsigned int)(x_dist),
			   (unsigned int)(y_dist)),
		     Qt::AlignCenter,
		     QString::number(day));
	    
	  // store the mapping
	  QDate mapDate(tempDate2.year(), tempDate2.month(), day);
	  m_rgDayMappings[col + 0 * 7] = mapDate;
	} // !for(i)
    }
    
  // ---- draw the current month ----
  p.setPen(QColor(20, 20, 20));
  for (unsigned int i = 1; i <= (unsigned int)m_currDate.daysInMonth(); i++)
    {
      tempDate.setYMD(m_currDate.year(),
		      m_currDate.month(),
		      i);
      col                            = tempDate.dayOfWeek();	
      m_rgDayMappings[col + row * 7] =tempDate;   // store the mapping
	
      // set up the bounding rect
      QRect dayRect = QRect((unsigned int)(nHorOffset + (col - 1) * x_dist),
			    (unsigned int)(2 * m_dCaptionHeight + row * y_dist),
			    (unsigned int)(x_dist),
			    (unsigned int)(y_dist));
	
      // draw the selected day
      if (i == (unsigned int)m_currDate.day())
	{
	  // draw the box under the caption, make
	  // the box slightly smaller than the bounding rect
	  QRect markerRect(dayRect.x() + 2, 
			   dayRect.y() + 2,
			   dayRect.width() - 4, 
			   dayRect.height() - 4);
	  p.fillRect(markerRect, 
		     colorGroup().highlight());
	    
	  // set up a new pen style
	  QFont font = p.font();		    		
	  p.setFont(font);
	  p.setPen(QColor(255, 255, 255));
	}
      else if (tempDate == QDate::currentDate()) //NOTE: QAppRepository::instance()->currentDate()) tobgle change
	{
	    // mark current day
	    p.fillRect(dayRect,
		       QCalendarBase::getCurrentDayBgColor());
	}
      else if (col == 7) // indicate sundays
	{
	  p.setPen(QCalendarBase::getSundayColor());
	}
	
      p.drawText(dayRect,
		 Qt::AlignCenter,
		 QString::number(i));
	
      if ((i == (unsigned int)m_currDate.day()) || 
	  (tempDate == QDate::currentDate() /*QAppRepository::instance()->currentDate() NOTE: tobgle change*/) ||
	  (col == 7)) // reset drawing properties
	{
	  QFont font = p.font();
	  font.setUnderline(false);
	  p.setFont(font);
	  p.setPen(QColor(20, 20, 20));
	}
	
      if (col == 7)
	row++;
    } // !for(i)	
    
  // ---- draw the next month ----
  tempDate.setYMD(tempDate.year(),
		  tempDate.month(),
		  1);
  tempDate = tempDate.addMonths(1);
  tempDate = tempDate.addDays(-1);
    
  p.setPen(QColor(138, 138, 138));
  while (row < 6)
    {
      tempDate                       = tempDate.addDays(1);
      col                            = tempDate.dayOfWeek();
      m_rgDayMappings[col + row * 7] = tempDate;
	
      QRect dayRect = QRect((unsigned int)(nHorOffset + (col - 1) * x_dist),
			    (unsigned int)(2 * m_dCaptionHeight + row /* row */ * y_dist),
			    (unsigned int)(x_dist),
			    (unsigned int)(y_dist));
      p.drawText(dayRect,
		 Qt::AlignCenter,
		 QString::number(tempDate.day()));
	
      if (col == 7)
	row++;
    }
    
  p.end();
} // !paintEvent()



void QCalMonthLookup::mousePressEvent(QMouseEvent* thisEvent)
{
  float x_dist = 1.0f * (this->width() - 2 * nHorOffset) / 7;
  float y_dist = 1.0f * (this->height() - 3 * m_dCaptionHeight) / 6;

  // sake of consistence, col starts at 1, row at 0, see paintEvent for details
  unsigned int col = (unsigned int)(1.0f + 1.0f * (thisEvent->pos().x() - nHorOffset) / x_dist);
  unsigned int row = (unsigned int)(1.0f * (thisEvent->pos().y() - 2.0f * m_dCaptionHeight) / y_dist);

  //NOTE: tobgle addition
  if(thisEvent->pos().x() < 0 || thisEvent->pos().x() > width() ||
     thisEvent->pos().y() < 0 || thisEvent->pos().y() > height())
  {
	close();
	return;
  }

  //NOTE: tobgle addition
  if(row > 5 || col < 1 || col > 7)
  {
	return;
  }

  // store a new date
  m_currDate = m_rgDayMappings[col + row * 7];	
  repaint();

  // send notification
  emit dateChanged( QDateTime(m_currDate) );
} // !mousePressEvent()

void QCalMonthLookup::mouseDoubleClickEvent(QMouseEvent*)
{
   close();
}

void QCalMonthLookup::resizeEvent(QResizeEvent*)
{
  float y_dist = 1.0f * (this->height() - 3 * m_dCaptionHeight) / 6;
  // modify the navigation buttons
  unsigned int nBtWidth = 24;
  if (m_pBackButton)
    {
      m_pBackButton->setGeometry(width() / 2 - 70 - nBtWidth, 2,
				 nBtWidth, 20);
    }
  if (m_pForwButton)
    {
      m_pForwButton->setGeometry(width() / 2 + 70, 2,
				 nBtWidth, 20);
    }
  if (m_pYearBackwardButton)
    {
      m_pYearBackwardButton->setGeometry(width() / 2 - 70 - 2*nBtWidth, 2,
				 nBtWidth, 20);
    }
  if (m_pYearForwardButton)
    {
      m_pYearForwardButton->setGeometry(width() / 2 + 70 + nBtWidth, 2,
				 nBtWidth, 20);
    }
  if (m_pTodayButton)
    {
	int row = 6;
      m_pTodayButton->setGeometry(width() / 2 - 35,(int)(2 * m_dCaptionHeight + row * y_dist + 1),
				 70, 18);
    }
} // !resizeEvent()



void QCalMonthLookup::onBackwardClicked()
{
  m_currDate = m_currDate.addMonths(-1);
  repaint();

  emit dateChanged(m_currDate);
} // !onBackwardClicked()



void QCalMonthLookup::onForwardClicked()
{
  m_currDate = m_currDate.addMonths(1);
  repaint();

  emit dateChanged(m_currDate);
} // !onForwardClicked()

void QCalMonthLookup::yearBackwardClicked( )
{
  m_currDate = m_currDate.addMonths(-12);
  repaint();

  emit dateChanged(m_currDate);
}

void QCalMonthLookup::yearForwardClicked( )
{
  m_currDate = m_currDate.addMonths(12);
  repaint();

  emit dateChanged(m_currDate);
}

void QCalMonthLookup::todayClicked( )
{
  m_currDate = QDate::currentDate();
  repaint();

  emit dateChanged(m_currDate);
} 

