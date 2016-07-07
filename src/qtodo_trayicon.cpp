/*
 * trayicon.cpp - system-independent trayicon class (adapted from Qt example)
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*
 * trayicon_x11.cpp - X11 trayicon (for use with KDE and GNOME)
 * Copyright (C) 2003  Justin Karneges
 * GNOME2 Notification Area support: Tomasz Sterna
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <qapplication.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qpopupmenu.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "qtodo_trayicon.h"
#include "qtodo_widget.h"

/*!
  \class QTodoTrayIcon qtrayicon.h
  \brief The QTodoTrayIcon class implements an entry in the system tray.
*/

/*!
  Creates a QTodoTrayIcon object. \a parent and \a name are propagated
  to the QObject constructor. The icon is initially invisible.

  \sa show
*/
QTodoTrayIcon::QTodoTrayIcon( QObject *parent, const char *name )
: QObject(parent, name), pop(0), d(0)
{
	v_isWMDock = false;
}

/*!
  Creates a QTodoTrayIcon object displaying \a icon and \a tooltip, and opening
  \a popup when clicked with the right mousebutton. \a parent and \a name are
  propagated to the QObject constructor. The icon is initially invisible.

  \sa show
*/
QTodoTrayIcon::QTodoTrayIcon( const QPixmap &icon, const QString &tooltip, QPopupMenu *popup, QObject *parent, const char *name )
: QObject(parent, name), pop(popup), pm(icon), tip(tooltip), d(0)
{
	v_isWMDock = false;
}

/*!
  Removes the icon from the system tray and frees all allocated resources.
*/
QTodoTrayIcon::~QTodoTrayIcon()
{
    sysRemove();
    delete tool_tip;
}

/*!
  Sets the context menu to \a popup. The context menu will pop up when the
  user clicks the system tray entry with the right mouse button.
*/
void QTodoTrayIcon::setPopup( QPopupMenu* popup )
{
    pop = popup;
}

/*!
  Returns the current popup menu.

  \sa setPopup
*/
QPopupMenu* QTodoTrayIcon::popup() const
{
    return pop;
}

/*!
  \property QTodoTrayIcon::icon
  \brief the system tray icon.
*/
void QTodoTrayIcon::setIcon( const QPixmap &icon )
{
    //if(!popup()) {
    //    tip = "";
    //}

    pm = icon;
    sysUpdateIcon();
}

QPixmap QTodoTrayIcon::icon() const
{
    return pm;
}

/*!
  \property QTodoTrayIcon::toolTip
  \brief the tooltip for the system tray entry

  On some systems, the tooltip's length is limited and will be truncated as necessary.
*/
void QTodoTrayIcon::setToolTip( const QString &tooltip )
{
    tip = tooltip;
    sysUpdateToolTip();
}

QString QTodoTrayIcon::toolTip() const
{
    return tip;
}

/*!
  Shows the icon in the system tray.

  \sa hide
*/
void QTodoTrayIcon::show()
{
    sysInstall();
}

/*!
  Hides the system tray entry.
*/
void QTodoTrayIcon::hide()
{
    sysRemove();
}

/*!
  \reimp
*/
bool QTodoTrayIcon::event( QEvent *e )
{
    switch ( e->type() ) {
    case QEvent::MouseMove:
	mouseMoveEvent( (QMouseEvent*)e );
	break;

    case QEvent::MouseButtonPress:
	mousePressEvent( (QMouseEvent*)e );
	break;

    case QEvent::MouseButtonRelease:
	mouseReleaseEvent( (QMouseEvent*)e );
	break;

    case QEvent::MouseButtonDblClick:
	mouseDoubleClickEvent( (QMouseEvent*)e );
	break;
    default:
	return QObject::event( e );
    }

    return true;
}

/*!
  This event handler can be reimplemented in a subclass to receive
  mouse move events for the system tray entry.

  \sa mousePressEvent(), mouseReleaseEvent(), mouseDoubleClickEvent(),  QMouseEvent
*/
void QTodoTrayIcon::mouseMoveEvent( QMouseEvent *e )
{
    e->ignore();
}

/*!
  This event handler can be reimplemented in a subclass to receive
  mouse press events for the system tray entry.

  \sa mouseReleaseEvent(), mouseDoubleClickEvent(),
  mouseMoveEvent(), QMouseEvent
*/
void QTodoTrayIcon::mousePressEvent( QMouseEvent *e )
{
#ifndef Q_WS_WIN
// This is for X11, menus appear on mouse press
// I'm not sure whether Mac should be here or below.. Somebody check?
	switch ( e->button() ) {
		case RightButton:
			if ( pop ) {
				pop->popup( e->globalPos() );
				e->accept();
			}
			break;
		case LeftButton:
		case MidButton:
			emit clicked( e->globalPos(), e->button() );
			break;
		default:
			break;
	}
#endif
	e->ignore();
}

/*!
  This event handler can be reimplemented in a subclass to receive
  mouse release events for the system tray entry.

  The default implementations opens the context menu when the entry
  has been clicked with the right mouse button.

  \sa setPopup(), mousePressEvent(), mouseDoubleClickEvent(),
  mouseMoveEvent(), QMouseEvent
*/
void QTodoTrayIcon::mouseReleaseEvent( QMouseEvent *e )
{
#ifdef Q_WS_WIN
// This is for Windows, where menus appear on mouse release
	switch ( e->button() ) {
		case RightButton:
			if ( pop ) {
				// Necessary to make keyboard focus
				// and menu closing work on Windows.
				pop->setActiveWindow();
				pop->popup( e->globalPos() );
				pop->setActiveWindow();
				e->accept();
			}
			break;
		case LeftButton:
		case MidButton:
			emit clicked( e->globalPos(), e->button() );
			break;
		default:
			break;
	}
#endif
	e->ignore();
}

/*!
  This event handler can be reimplemented in a subclass to receive
  mouse double click events for the system tray entry.

  Note that the system tray entry gets a mousePressEvent() and a
  mouseReleaseEvent() before the mouseDoubleClickEvent().

  \sa mousePressEvent(), mouseReleaseEvent(),
  mouseMoveEvent(), QMouseEvent
*/
void QTodoTrayIcon::mouseDoubleClickEvent( QMouseEvent *e )
{
	if ( e->button() == LeftButton )
		emit doubleClicked( e->globalPos() );
	e->accept();
}

/*!
  \fn void QTodoTrayIcon::clicked( const QPoint &p )

  This signal is emitted when the user clicks the system tray icon
  with the left mouse button, with \a p being the global mouse position
  at that moment.
*/

/*!
  \fn void QTodoTrayIcon::doubleClicked( const QPoint &p )

  This signal is emitted when the user double clicks the system tray
  icon with the left mouse button, with \a p being the global mouse position
  at that moment.
*/

void QTodoTrayIcon::gotCloseEvent()
{
	closed();
}

//#if QT_VERSION < 0x030200
extern Time qt_x_time;
//#endif

//----------------------------------------------------------------------------
// common stuff
//----------------------------------------------------------------------------

// for Gnome2 Notification Area
static XErrorHandler old_handler = 0;
static int dock_xerror = 0;
extern "C" int dock_xerrhandler(Display* dpy, XErrorEvent* err)
{
	dock_xerror = err->error_code;
	return old_handler(dpy, err);
}

static void trap_errors()
{
	dock_xerror = 0;
	old_handler = XSetErrorHandler(dock_xerrhandler);
}

static bool untrap_errors()
{
	XSetErrorHandler(old_handler);
	return (dock_xerror == 0);
}

static bool send_message(
	Display* dpy,	/* display */
	Window w,	/* sender (tray icon window) */
	long message,	/* message opcode */
	long data1,	/* message data 1 */
	long data2,	/* message data 2 */
	long data3	/* message data 3 */
) {
	XEvent ev;

	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = XInternAtom (dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = message;
	ev.xclient.data.l[2] = data1;
	ev.xclient.data.l[3] = data2;
	ev.xclient.data.l[4] = data3;

	trap_errors();
	XSendEvent(dpy, w, False, NoEventMask, &ev);
	XSync(dpy, False);
	return untrap_errors();
}

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

//----------------------------------------------------------------------------
// QTodoTrayIcon::QTodoTrayIconPrivate
//----------------------------------------------------------------------------

class QTodoTrayIcon::QTodoTrayIconPrivate : public QWidget
{
public:
QTodoTrayIconPrivate(QTodoTrayIcon *object, int size);
	~QTodoTrayIconPrivate() { }

	virtual void initWM(WId icon);

	virtual void setPixmap(const QPixmap &pm);

	virtual void paintEvent(QPaintEvent *);
	virtual void enterEvent(QEvent *);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void closeEvent(QCloseEvent *e);

private:
	QTodoTrayIcon *iconObject;
	QPixmap pix;
	int size;
};

QTodoTrayIcon::QTodoTrayIconPrivate::QTodoTrayIconPrivate(QTodoTrayIcon *object, int _size)
	: QWidget(0, "psidock", WRepaintNoErase)
{
	iconObject = object;
	size = _size;

	setFocusPolicy(NoFocus);
	setBackgroundMode(X11ParentRelative);

	setMinimumSize(size, size);
	setMaximumSize(size, size);
}

// This base stuff is required by both FreeDesktop specification and WindowMaker
void QTodoTrayIcon::QTodoTrayIconPrivate::initWM(WId icon)
{
	Display *dsp = x11Display();
	WId leader   = winId();

	// set the class hint
	XClassHint classhint;
	classhint.res_name  = (char*)"psidock";
	classhint.res_class = (char*)"Psi";
	XSetClassHint(dsp, leader, &classhint);

	// set the Window Manager hints
	XWMHints *hints;
	hints = XGetWMHints(dsp, leader);	// init hints
	hints->flags = WindowGroupHint | IconWindowHint | StateHint;	// set the window group hint
	hints->window_group = leader;		// set the window hint
	hints->initial_state = WithdrawnState;	// initial state
	hints->icon_window = icon;		// in WM, this should be winId() of separate widget
	hints->icon_x = 0;
	hints->icon_y = 0;
	XSetWMHints(dsp, leader, hints);	// set the window hints for WM to use.
	XFree( hints );
}

void QTodoTrayIcon::QTodoTrayIconPrivate::setPixmap(const QPixmap &pm)
{
	pix = pm;
	setIcon(pix);
	repaint();
}

void QTodoTrayIcon::QTodoTrayIconPrivate::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.drawPixmap((width() - pix.width())/2, (height() - pix.height())/2, pix);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::enterEvent(QEvent *e)
{
	// Taken from KSystemTray..
//#if QT_VERSION < 0x030200
	//if ( !qApp->focusWidget() ) {
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xfocus.display = qt_xdisplay();
		ev.xfocus.type = FocusIn;
		ev.xfocus.window = winId();
		ev.xfocus.mode = NotifyNormal;
		ev.xfocus.detail = NotifyAncestor;
		Time time = qt_x_time;
		qt_x_time = 1;
		qApp->x11ProcessEvent( &ev );
		qt_x_time = time;
	//}
//#endif
	QWidget::enterEvent(e);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::mouseMoveEvent(QMouseEvent *e)
{
	QApplication::sendEvent(iconObject, e);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::mousePressEvent(QMouseEvent *e)
{
	QApplication::sendEvent(iconObject, e);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::mouseReleaseEvent(QMouseEvent *e)
{
	QApplication::sendEvent(iconObject, e);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::mouseDoubleClickEvent(QMouseEvent *e)
{
	QApplication::sendEvent(iconObject, e);
}

void QTodoTrayIcon::QTodoTrayIconPrivate::closeEvent(QCloseEvent *e)
{
	iconObject->gotCloseEvent();
	e->accept();
}

//----------------------------------------------------------------------------
// QTodoTrayIconFreeDesktop
//----------------------------------------------------------------------------

class QTodoTrayIconFreeDesktop : public QTodoTrayIcon::QTodoTrayIconPrivate
{
public:
	QTodoTrayIconFreeDesktop(QTodoTrayIcon *object, const QPixmap &pm);
};

QTodoTrayIconFreeDesktop::QTodoTrayIconFreeDesktop(QTodoTrayIcon *object, const QPixmap &pm)
	: QTodoTrayIconPrivate(object, 22)
{
	initWM( winId() );

	// initialize NetWM
	Display *dsp = x11Display();

	// dock the widget (adapted from SIM-ICQ)
	Screen *screen = XDefaultScreenOfDisplay(dsp); // get the screen
	int screen_id = XScreenNumberOfScreen(screen); // and it's number

	char buf[32];
	snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen_id);
	Atom selection_atom = XInternAtom(dsp, buf, false);
	XGrabServer(dsp);
	Window manager_window = XGetSelectionOwner(dsp, selection_atom);
	if ( manager_window != None )
		XSelectInput(dsp, manager_window, StructureNotifyMask);
	XUngrabServer(dsp);
	XFlush(dsp);

	if ( manager_window != None )
		send_message(dsp, manager_window, SYSTEM_TRAY_REQUEST_DOCK, winId(), 0, 0);

	// some KDE mumbo-jumbo... why is it there? anybody?
	Atom kwm_dockwindow_atom = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
	Atom kde_net_system_tray_window_for_atom = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);

	long data = 0;
	XChangeProperty(dsp, winId(), kwm_dockwindow_atom, kwm_dockwindow_atom, 32, PropModeReplace, (uchar*)&data, 1);
	XChangeProperty(dsp, winId(), kde_net_system_tray_window_for_atom, XA_WINDOW, 32, PropModeReplace, (uchar*)&data, 1);

	setPixmap(pm);
}

//----------------------------------------------------------------------------
// QTodoTrayIconWindowMaker
//----------------------------------------------------------------------------

class QTodoTrayIconWharf : public QTodoTrayIcon::QTodoTrayIconPrivate
{
public:
	QTodoTrayIconWharf(QTodoTrayIcon *object, const QPixmap &pm)
		: QTodoTrayIconPrivate(object, 44)
	{
		// set the class hint
		XClassHint classhint;
		classhint.res_name  = (char*)"psidock-wharf";
		classhint.res_class = (char*)"Psi";
		XSetClassHint(x11Display(), winId(), &classhint);

		setPixmap(pm);
	}

	void setPixmap(const QPixmap &_pm)
	{
		QPixmap pm;
		QImage i = _pm.convertToImage();
		i = i.scale(i.width() * 2, i.height() * 2);
		pm.convertFromImage(i);

		QTodoTrayIconPrivate::setPixmap(pm);

		// thanks to Robert Spier for this:
		// for some reason the repaint() isn't being honored, or isn't for
		// the icon.  So force one on the widget behind the icon
		erase();
		QPaintEvent pe( rect() );
		paintEvent(&pe);
	}
};

class QTodoTrayIconWindowMaker : public QTodoTrayIcon::QTodoTrayIconPrivate
{
public:
	QTodoTrayIconWindowMaker(QTodoTrayIcon *object, const QPixmap &pm);
	~QTodoTrayIconWindowMaker();

	void setPixmap(const QPixmap &pm);

private:
	QTodoTrayIconWharf *wharf;
};

QTodoTrayIconWindowMaker::QTodoTrayIconWindowMaker(QTodoTrayIcon *object, const QPixmap &pm)
	: QTodoTrayIconPrivate(object, 32)
{
	wharf = new QTodoTrayIconWharf(object, pm);

	initWM( wharf->winId() );
}

QTodoTrayIconWindowMaker::~QTodoTrayIconWindowMaker()
{
	delete wharf;
}

void QTodoTrayIconWindowMaker::setPixmap(const QPixmap &pm)
{
	wharf->setPixmap(pm);
}

//----------------------------------------------------------------------------
// QTodoTrayIcon
//----------------------------------------------------------------------------

void QTodoTrayIcon::sysInstall()
{
	if ( d )
		return;

	if ( v_isWMDock )
		d = (QTodoTrayIconPrivate *)(new QTodoTrayIconWindowMaker(this, pm));
	else
		d = (QTodoTrayIconPrivate *)(new QTodoTrayIconFreeDesktop(this, pm));

	sysUpdateToolTip();
	d->show();

	tool_tip = new QTodoTrayIconToolTip(d);
}

void QTodoTrayIcon::sysRemove()
{
	if ( !d )
		return;

	delete d;
	d = 0;
}

void QTodoTrayIcon::sysUpdateIcon()
{
	if ( !d )
		return;

	QPixmap pix = pm;
	d->setPixmap(pix);
}

void QTodoTrayIcon::sysUpdateToolTip()
{
	if ( !d )
		return;

	if ( tip.isEmpty() )
		QToolTip::remove(d);
	else
		QToolTip::add(d, tip);
}

QTodoTrayIconToolTip::QTodoTrayIconToolTip(QWidget* parent, QToolTipGroup* group) : QToolTip(parent,group)
{
}

QTodoTrayIconToolTip::~QTodoTrayIconToolTip()
{
	remove(parentWidget());
}

void QTodoTrayIconToolTip::maybeTip(const QPoint&)
{
	tip(QRect(0,0,200,200),dynamic_cast<QTodoWidget*>(qApp->mainWidget())->trayIconToolTipText());
}






