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

#include <qtextedit.h>
#include <qptrlist.h>

#ifndef QTODO_TASK_EDIT
#define QTODO_TASK_EDIT

class QTextParagraph;
class QComboBox;
class QPopupMenu;

class QTodoTaskEdit : public QTextEdit
{
	Q_OBJECT

	public:
	QTodoTaskEdit(QWidget*);
	~QTodoTaskEdit();
	static void setTabChangesFocusGlobally(bool);
	static void setTodoTextPointSizeGlobally(int);
	static void setBorderlessGlobally(bool);
	int cursorYPosition();
	QString myText();
	QString myRichText();
	QString myPlainText();
	QStringList myLinks();
	void mySetText(const QString&,bool prevent_emit_change=false);
	void setBorderless(bool);

	public slots:
	void adaptToTextChange();
	void setPointSize(int);

	signals:
	void wheelEventSignal(QWheelEvent*);
	void focusInEventSignal(QFocusEvent*);
	void upNavigated(int);
	void downNavigated(int);
	void changed();

	private slots:
	void menuActivated(int);
	void menuAboutToHide();
	void deleteActionsMenu();
	void linkMenuActivated(int);
	void openWithMenuActivated(int);
	void openWithComboLineEditActivated();
	void emitChanged();

	private:
	QPopupMenu* createPopupMenu(const QPoint&);
	void increaseHeight();
	void decreaseHeight();
	void setStrikeOut(bool);
	bool strikeOut() const;
	void resizeEvent(QResizeEvent*);
	void keyPressEvent(QKeyEvent*);
	void contentsMousePressEvent(QMouseEvent*);
	void contentsWheelEvent(QWheelEvent*);
	void focusInEvent(QFocusEvent*);
	void setHeight(int);
	void insertRichText(const QString&);
	void setLink(const QString& = QString::null);
	void getAnchor(const QPoint&,int&,int&,int&,int&);
	QString paragraphRichText(QTextParagraph*);
	QString paragraphPlainText(QTextParagraph*);
	void paragraphLinks(QTextParagraph*,QStringList&);
	QString makeFormatChangeTags(QTextFormat*, QTextFormat*, QTextFormat*,const QString&,const QString&);
	QString makeFormatEndTags(QTextFormat*, QTextFormat*, const QString&);

	QPoint cur_anchor_pos;
	bool is_rich_text;
	bool unchanged;
	bool prevent_emit_change;
	bool todo_item_preselected;
	QPopupMenu* actions;

	static QComboBox* open_with_combo;
	static QPtrList<QTodoTaskEdit> instances;
};

#endif

