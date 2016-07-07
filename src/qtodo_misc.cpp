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

/* license {{{ */
/* 

wmctrl
A command line tool to interact with an EWMH/NetWM compatible X Window Manager.

Author, current maintainer: Tomas Styblo <tripie@cpan.org>

Copyright (C) 2003

This program is free software which I release under the GNU General Public
License. You may redistribute and/or modify this program under the terms
of that license as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

To get a copy of the GNU General Puplic License,  write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/* }}} */


#include <qapplication.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qtextcodec.h>

#include <unistd.h>
#include <string.h>

#include "qtodo_misc.h"

const QString QTodoMisc::ISODate = "yyyy-MM-dd";
const QString QTodoMisc::ISOTime = "hh:mm";
const QString QTodoMisc::ISODateTime = "yyyy-MM-dd hh:mm";
int QTodoMisc::debug_count = 0;
const int QTodoMisc::MAX_PROPERTY_VALUE_LEN = 4096;
QTranslator QTodoMisc::qt_translator(0);
QTranslator QTodoMisc::qtodo_translator(0);

QString QTodoMisc::timeSpanUntilDateTimeString(const QDateTime& datetime)
{
	float secs_to_deadline = QDateTime::currentDateTime().secsTo(datetime);
	int days_to_deadline = (int)(secs_to_deadline/86400);
	int hours_to_deadline = (int)((secs_to_deadline - (days_to_deadline*86400))/3600);
	int minutes_to_deadline = (int)((secs_to_deadline - (days_to_deadline*86400) - (hours_to_deadline*3600))/60);

	days_to_deadline < 0 ? days_to_deadline = -days_to_deadline : 0;
	minutes_to_deadline < 0 ? minutes_to_deadline = -minutes_to_deadline : 0;
	hours_to_deadline < 0 ? hours_to_deadline = -hours_to_deadline : 0;

	QString time_to_deadline;
	if(secs_to_deadline < 0)
		time_to_deadline += "-";
	else
		time_to_deadline += "+";
	if(days_to_deadline != 0)
		time_to_deadline += QString::number(days_to_deadline) + "d ";
	if(hours_to_deadline != 0)
		time_to_deadline += QString::number(hours_to_deadline) + "h ";
	time_to_deadline += QString::number(minutes_to_deadline) + "m";
	return time_to_deadline;
}

QString QTodoMisc::shortText(const QString& in, uint len)
{
	if(in.length() > len)
	{
		QString out = in;
		out.setLength(len);
		out.replace(len-3,3,"...");
		return out;
	}	
	else
		return in;
}

QString QTodoMisc::dateTime2LocalString(const QDateTime& datetime)
{
	QString tmp = datetime.toString(Qt::LocalDate);
	if(!tmp.isEmpty())
		tmp.setLength(tmp.length()-3);
	return tmp;
}

void QTodoMisc::loadTranslator(const QString& locale)
{
        // translation file for Qt
        qt_translator.load(QString( "qt_" ) + locale, "./translations/" );
        qApp->installTranslator(&qt_translator);

        // translation file for application strings
        qtodo_translator.load(QString("qtodo_") + locale,"./translations/");
        qApp->installTranslator(&qtodo_translator);
}

QString QTodoMisc::browseDirectory(const QString& current)
{
	QString start = current;
	if(start.isEmpty() || !QFileInfo(start).exists())
		start = QDir::homeDirPath();
	QString _directory = QFileDialog::getExistingDirectory(start,0,0,0,qApp->translate("Misc","Select directory"));
	if(_directory.isEmpty())
		return current;

	return _directory;
}

QString QTodoMisc::browseFile(const QString& current)
{
	QString start = current;
	if(start.isEmpty() || !QFileInfo(start).exists())
		start = QDir::homeDirPath();
	QString _file = QFileDialog::getOpenFileName(start,0,0,0,qApp->translate("Misc","Browse for file"));
	if(_file.isEmpty())
		return current;
	return _file;
}

int QTodoMisc::mainWidgetToCurrentDisplay()
{
	return windowToCurrentDisplay(qApp->mainWidget()->winId());
}

int QTodoMisc::windowToCurrentDisplay(const Window& win)
{
	Display *disp; 
	
	if (! (disp = XOpenDisplay(NULL))) {
		qDebug("Cannot open display.\n");
		return EXIT_FAILURE;
	}

	int ret;

	/* move the window to the current desktop and activate it => -r */
	if (window_to_desktop(disp, win, -1) == EXIT_SUCCESS)
	{ 
		usleep(100000); /* 100 ms - make sure the WM has enough
		time to move the window, before we activate it */
		ret = activate_window(disp, win, false);
	}
	else {
		ret = EXIT_FAILURE;
	}

	XCloseDisplay(disp);

	return ret;
}

int QTodoMisc::client_msg(Display *disp, Window win, char *msg,
        unsigned long data0, unsigned long data1, 
        unsigned long data2, unsigned long data3,
        unsigned long data4) {
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;
    
    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)) {
        return EXIT_SUCCESS;
    }
    else {
        //qDebug("Cannot send "+QString(msg)+" event.\n");
        return EXIT_FAILURE;
    }
}

int QTodoMisc::window_to_desktop (Display *disp, Window win, int desktop) {
    unsigned long *cur_desktop = NULL;
    Window root = DefaultRootWindow(disp);
   
    if (desktop == -1) {
        if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL))) {
            if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                    XA_CARDINAL, "_WIN_WORKSPACE", NULL))) {
                qDebug("Cannot get current desktop properties. "
                      "(_NET_CURRENT_DESKTOP or _WIN_WORKSPACE property)"
                      "\n");
                return EXIT_FAILURE;
            }
        }
        desktop = *cur_desktop;
    }
    delete cur_desktop;

    return client_msg(disp, win, "_NET_WM_DESKTOP", (unsigned long)desktop,
            0, 0, 0, 0);
}

int QTodoMisc::activate_window (Display *disp, Window win, bool switch_desktop) {
    unsigned long *desktop;

    /* desktop ID */
    if ((desktop = (unsigned long *)get_property(disp, win,
            XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL)
    {
        if ((desktop = (unsigned long *)get_property(disp, win,
                XA_CARDINAL, "_WIN_WORKSPACE", NULL)) == NULL)
        {
            //qDebug("Cannot find desktop ID of the window.\n");
        }
    }

    if (switch_desktop && desktop)
    {
        if (client_msg(disp, DefaultRootWindow(disp), 
                    "_NET_CURRENT_DESKTOP", 
                    *desktop, 0, 0, 0, 0) != EXIT_SUCCESS) {
            qDebug("Cannot switch desktop.\n");
        }
        delete desktop;
    }

    client_msg(disp, win, "_NET_ACTIVE_WINDOW", 
            0, 0, 0, 0, 0);
    XMapRaised(disp, win);

    return EXIT_SUCCESS;
}

char* QTodoMisc::get_property (Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size) {
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    char *ret;
    
    xa_prop_name = XInternAtom(disp, prop_name, False);
    
    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
            xa_prop_type, &xa_ret_type, &ret_format,     
            &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        qDebug("Cannot get %s property.\n", prop_name);
        return NULL;
    }
  
    if (xa_ret_type != xa_prop_type) {
        //qDebug("Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / 8) * ret_nitems;
    ret = new char[tmp_size + 1];
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }
    
    XFree(ret_prop);
    return ret;
}

void QTodoMisc::debug(const QString& text, void* p)
{
	QString pointer_str;
	//if(p)
	//	pointer_str = QString::number(static_cast<int>(p));
	//qDebug("Debug point number: "+QString::number(debug_count)+" Time: "+QTime::currentTime().toString("ss:zzz")+" Text: "+text); //+" Pointer: "+pointer_str);
	debug_count++;
}


