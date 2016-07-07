/***************************************************************************
 * trayicon.h - system-independent trayicon class (adapted from Qt example)*
 *   Copyright (C) 2003  Justin Karneges                                   *
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

#ifndef QTODO_TRAYICON_H
#define QTODO_TRAYICON_H

#include <qobject.h>
#include <qimage.h>
#include <qtooltip.h>

class QPopupMenu;

/**
Clase que crea un icono en la bandeja de sistema

@author Luis Enrique Meneses Wong,,,
*/
class QTodoTrayIcon : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QString toolTip READ toolTip WRITE setToolTip )
	Q_PROPERTY( QPixmap icon READ icon WRITE setIcon )

public:
	QTodoTrayIcon( QObject *parent = 0, const char *name = 0 );
	QTodoTrayIcon( const QPixmap &, const QString &, QPopupMenu *popup = 0, QObject *parent = 0, const char *name = 0 );
	~QTodoTrayIcon();

	// use WindowMaker dock mode.  ignored on non-X11 platforms
	void setWMDock(bool use) { v_isWMDock = use; }
	bool isWMDock() { return v_isWMDock; }

	// Set a popup menu to handle RMB
	void setPopup( QPopupMenu * );
	QPopupMenu* popup() const;

	QPixmap icon() const;
	QString toolTip() const;

	void gotCloseEvent();

public slots:
	void setIcon( const QPixmap &icon );
	void setToolTip( const QString &tip );

	void show();
	void hide();

signals:
	void clicked( const QPoint&, int);
	void doubleClicked( const QPoint& );
	void closed();

protected:
	bool event( QEvent * );
	virtual void mouseMoveEvent( QMouseEvent *e );
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseReleaseEvent( QMouseEvent *e );
	virtual void mouseDoubleClickEvent( QMouseEvent *e );

private:
	QPopupMenu *pop;
	QPixmap pm;
	QString tip;
	bool v_isWMDock;

	// system-dependant part
public:
	class QTodoTrayIconPrivate;
private:
	QTodoTrayIconPrivate *d;
	QToolTip* tool_tip;
	void sysInstall();
	void sysRemove();
	void sysUpdateIcon();
	void sysUpdateToolTip();

	friend class QTodoTrayIconPrivate;
};

class QTodoTrayIconToolTip : public QToolTip
{
	public:
	QTodoTrayIconToolTip(QWidget*,QToolTipGroup* group = 0);
	virtual ~QTodoTrayIconToolTip();

	void maybeTip(const QPoint&);
};

#endif
