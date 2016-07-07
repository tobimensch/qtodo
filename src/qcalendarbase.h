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


#ifndef _QCALENDARBASE_H_
#define _QCALENDARBASE_H_




#include <qwidget.h>
#include <qdatetime.h>
#include <qpushbutton.h>



/**
 * @class   QCalendarBase
 * @author  Stephan Stapel, <stephan.stapel@web.de>
 * @date    03.12.02
 *
 * @brief   (abstract) base class for the calendar widgets
 *
 * The base class provides some functions for setting properties
 * that are common for all calendar widgets.
 * It also defines some signals that might (or might not) be
 * emitted by the respective implementation.
 * Furthermore, this class implements the selection mechanism for
 * the appointments in the calendar classes.
 */
class QCalendarBase : public QWidget  
{
    Q_OBJECT
 public:
    /**
     * Standard constructor
     */
    QCalendarBase(QWidget* parent = 0, const char* name = 0, WFlags f = 0);

    /**
     * Standard destructor
     */
    virtual ~QCalendarBase();

    /**
     * Returns the date that is currently being displayed by the widget
     *
     * @see setDate()
     */
    virtual QDate getDate();

    /**
     * Modifies the date that is being displayed by the widget
     *
     * @see getDate()
     */
    virtual void setDate(const QDate& newDate);

    /**
     * @name color scheme functions, used in the 
     *       calendar implementation classes
     */
    /*@{*/
    /**
     * Returns the widget's default background color,
     * i.e. the background of all day/ hour etc. fields
     */
    virtual QColor getBgColor() const;
	
    /**
     * Returns the background color of the caption area,
     * i.e. the standard background color of the current
     * style.
     */
    virtual QColor getCaptionBgColor() const;

    virtual QColor getAppTitleColor() const;

    virtual QColor getCurrentDayBgColor() const;

    virtual QColor getSundayColor() const;

    virtual QColor getDragColor() const;
    /*@}*/
	
    /**
     * @name selection functions
     */
    /*@{*/
    /**
     * Selects a particular item
     *
     * @param  thisId   id of the item that is to become
     *                  selected
     * @param  doSelect if true (default), the item gets
     *                  selected otherwise deselected
     *
     * @see deselectItem()
     */
    void selectItem(unsigned int thisId, bool doSelect = true)
	{
	    if (!doSelect)
		deselectItem(thisId);
	    m_rgSelections.push_back(thisId);
	} // !selectItem()
	
    /**
     * Deselects a particular item
     *
     * @param   thisId       id of the item that is to
     *                       be deselected
     * @param   deDeselect   if true (default), the desired
     *                       item becomes deselected, otherwise
     *                       selected
     */
    void deselectItem(unsigned int thisId, bool doDeselect = true)
	{
	    if (!doDeselect)
		selectItem(thisId);

	    QValueList<unsigned int>::iterator it = m_rgSelections.find(thisId);

	    if (it != m_rgSelections.end())
	    {
		m_rgSelections.erase(it);
	    }
	} // !deselectItem()

    /**
     * Cleans up the entire selection
     */
    inline void clearSelection()
	{
	    m_rgSelections.clear();
	} // !clearSelection()

    inline bool isSelected(unsigned int thisId) const
	{
	    return (m_rgSelections.find(thisId) != m_rgSelections.end());	 
	} // !isSelected()

    inline QValueList<unsigned int> getSelection() const
	{
	    return (QValueList<unsigned int>)(m_rgSelections);
	}
    /*@}*/

    /**
     * Returns whether the user-defined cursor is currently being
     * visible and thus moveable by the user
     *
     * @see setShowCursor()
     */ 
    inline bool showCursor() const
	{
	    return m_bShowCursor;
	} // !showCursor()

    /**
     * Makes the user-defined cursor visible or hides it.
     *
     * @see showCursor()
     */
    inline void setShowCursor(bool showIt = true)
	{
	    m_bShowCursor = showIt;
	} // !setShowCursor()


    /**
     * Returns whether the appointments are drawn rounded or not rounded
     *
     * @see setAppsRounded()
     */
    inline bool appsRounded() const
	{
	    return m_bAppsRounded;
	} // !appsRounded()

    /**
     * Makes the appointments being draw with rounded edges or 
     * not rounded edges.
     *
     * @see appsRounded()
     */
    inline void setAppsRounded(bool isRounded = true)
	{
	    m_bAppsRounded = isRounded;
	} // !setAppsRounded()

    /**
     * Returns whether the navigation controls are visible or not
     *
     * @see setNavigationVisible()
     */
    inline bool navigationVisible() const
	{
	    if (m_pBackButton)
	    {
		if (m_pBackButton->isVisible())
		{
		    // if the forward button has a different state, correct it!
		    if (m_pForwButton && !m_pForwButton->isVisible())
			m_pForwButton->show();
		    return true;
		}
		else
		{
		    // if the forward button has a different state, correct it!
		    if (m_pForwButton && m_pForwButton->isVisible())
			m_pForwButton->hide();
		    return false;
		}
	    }
	    return false;
	} // !navigationVisible()

    /**
     * Hides or shows the navigation controls
     * 
     * @see navigationVisible()
     */
    inline void setNavigationVisible(bool isVisible = true)
	{
	    if (m_pBackButton)
	    {
		if (isVisible)
		    m_pBackButton->show();
		else
		    m_pBackButton->hide();
	    }
	    if (m_pForwButton)
	    {
		if (isVisible)
		    m_pForwButton->show();
		else
		    m_pForwButton->hide();
	    }
	} // !setNavigationVisible()

    /**
     * Enables or disables 'dragging on the widget base'
     * functionality.
     *
     * @see baseDraggingEnabled()
     */
    void setBaseDraggingEnabled(bool isEnabled = true)
	{
	    m_bBaseDraggingEnabled = isEnabled;
	} // !setBaseDraggingEnabled()

    /**
     * Returns whether dragging on the widget base
     * is possible or not.
     *
     * @see setBaseDraggingEnabled()
     */
    bool baseDraggingEnabled() const
	{
	    return m_bBaseDraggingEnabled;
	} // !baseDraggingEnabled()



    /**
     * This virtual function is being called when the forward navigation
     * button is being pressed.
     * This function might be overwritten in inherited classes for
     * implementing custom functionality.
     *
     * @see onBackwardPressed()
     */
    virtual void onForwardClicked();

    /**
     * Same as onForwardClicked() but for forward navigation
     *
     * @see onForwardClicked()
     */
    virtual void onBackwardClicked();

 private slots:
    /**      
     * @name private slots that are called whenever one of the navigation buttons
     *       are pressed
     */
     /*@{*/
     /**
      * Received when the forward navigation button is being pressed
      */
     void forwardClicked();

     /**
      * Received when the backward navigation button is being pressed
      */
     void backwardClicked();
     /*@}*/

 signals:

	void initialDateChanged(const QDate&);

    /**
     * This signal is being emitted when the cursor is moved to another date   
     */
    void dateChanged(const QDateTime& newDate);

    /**
     * This signal is being emitted when an appointment was clicked onto one
     * time
     *
     * @see appointmentDoubleClicked()
     */
    void appointmentClicked(unsigned int appointmentId);

    /**
     * This signal is being emitted when an appointment is being doubleclicked
     * onto
     *
     * @see appointmentClicked()
     */
    void appointmentDoubleClicked(unsigned int appointmentId);    

    /**
     * Emitted when the user double clicks on the white space within a
     * calendar.
     *
     * @see appointmentDoubleClicked()
     */
    void calendarDoubleClicked(const QDateTime& clickDate);

    /**
     * Emitted when the user clicked on the base of the widget
     * (the background) and starts dragging around in order to
     * create a date range
     *
     * @see endBaseDragging()
     */
    void beginBaseDragging(const QDateTime& downDate);

    /**
     * Similar to beginBaseDragging() but emitted when the user
     * releases the mouse button after dragging around for
     * creating a date range 
     *
     * @see beginBaseDragging()
     */
    void endBaseDragging(const QDateTime& downDate, const QDateTime& upDate);

    /**
     * Emitted whenever the user presses the right mouse button on the calendar.
     * In case the mouse button was pressed on an existing appointment,
     * the parameter appId will also be filled.
     *
     * @param   pos     screen position (client space) where the mouse button
     *                  was pressed
     * @param   appId   in case the mouse was pressed over an existing appointment,
     *                  this parameter will be valid, otherwise it will be equal UINT_MAX
     */
    void calendarContextMenu(const QPoint& pos, unsigned int appId);

 protected:
    /** 
     * @name navigation buttons
     */
    /*@{*/
    QPushButton* m_pBackButton;
    QPushButton* m_pForwButton;
    /*@}*/

    /**
     * List of the ids of the currently selected appointments
     */
    QValueList<unsigned int> m_rgSelections;

    QDate m_currDate;

 private:

    /**
     * Are the apps being drawn rounded or not rounded?
     *
     * @see appsRounded()
     * @see setAppsRounded()
     */	 
    bool m_bAppsRounded;

    /**
     * Boolean flag that indicates whether the user-moveable
     * cursor is being displayed or not
     */
    bool m_bShowCursor;

    /**
     * Flag that indicates whether dragging on the widget base
     * is possible
     */
    bool m_bBaseDraggingEnabled;

    QDate m_initial_date;
};

#endif // !_QCALENDARBASE_H_
