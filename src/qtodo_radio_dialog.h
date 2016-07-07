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

#include <qdialog.h>
#include <qhbox.h>

#ifndef QTODO_RADIO_DIALOG
#define QTODO_RADIO_DIALOG

class QButtonGroup;

class QTodoRadioDialog : public QDialog
{
	Q_OBJECT

	public:
	QTodoRadioDialog(const QString&,const QString&);

	void add(const QString&, int);
	void add(const QPixmap&, const QString&, int);
	int selected();

	private slots:
	void enableOkButton(int);

	private:
	QPushButton* ok;
	QButtonGroup* button_group;
};

class QRadioButton;
class QButton;

class QTodoRadioButton : public	QHBox
{
	Q_OBJECT

	public:
	QTodoRadioButton(QWidget* parent);
	virtual ~QTodoRadioButton();

	void setText(const QString& text);
	void setPixmap(const QPixmap& pixmap);

	public slots:
	void onLabelClick();

	public:

	QPushButton* label;
	QRadioButton* radio_button;
	static QButton* radio_dialog_ok_button;
};

#endif

