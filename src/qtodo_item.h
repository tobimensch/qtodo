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

#include <qhbox.h>
#include <qdatetime.h>
#include <qptrlist.h>
#include <qlineedit.h>

#ifndef QTODO_ITEM
#define QTODO_ITEM

class QTodoItem;
class QSpacerItem;
class QLabel;
class QCheckBox;
class QTodoDateEdit;
class QTextEdit;
class QLineEdit;
class QButton;
class QTodoList;
class QTodoItemWidget;
class QTodoTaskEdit;
class QTodoCheckLabel;
class QTodoPriorityWidget;
class QTodoStatusWidget;
class QPixmap;
class QTodoDateInput;
class QTodoTimeInput;
class QTodoComboWidget;
class QTodoDatetimeLabel;
class QHBox;
class QPopupMenu;

enum ActionsMenuIDs
{
	AMI_MORE,
	AMI_UNDOMORE,
	AMI_ADDSUBTODO,
	AMI_ADDTODOABOVE,
	AMI_ADDTODOBELOW,
	AMI_SORT,
	AMI_CUT,
	AMI_COPY,
	AMI_PASTE,
	AMI_DELETE
};

class QTodoLineEdit : public QLineEdit
{
	public:
	QTodoLineEdit(QWidget*);

	private:
	void keyPressEvent(QKeyEvent*);
};

///Base for all widgets that represent
///TODOs, sections etc. a TODOlist
class QTodoItem : public QHBox
{
	Q_OBJECT

	public:
	QTodoItem(QWidget*);
	QTodoItem(const QTodoItem&);
	~QTodoItem();
	void setDepth(unsigned int);
	int getDepth() const;

	void setFocusOnTask();
	void setCursorToBegin(int);
	void setCursorToEnd(int);
	QColor getCurrentDeadlineColor();
	void setDeadlineInputBackgroundColor(const QColor& col);
	void setPriority(int);
	void setTask(const QString&);
	void setCreated(const QDateTime&);
	void setDone(const QDateTime&);
	void setDeadline(const QDateTime&, bool prevent_emit_change=true);
	void setStatus(int, bool emit_change=false);
	void setAgent(const QString&);
	int getPriority() const;
	QString getTask() const;
	QString getTaskRichText() const;
	QString getTaskPlainText() const;
	QStringList getTaskLinks() const;
	QDateTime getCreated() const;
	QString getCreatedString() const;
	QString getCreatedLocalString() const;
	QDateTime getDone() const;
	QString getDoneString() const;
	QString getDeadlineString() const;
	QString getDeadlineLocalString() const;
	QDate getDeadlineDate() const;
	QTime getDeadlineTime() const;
	QDateTime getDeadlineDateTime() const;
	int getStatus() const;
	QString getAgent() const;
	void save(QTextStream&);
	bool filter(const QString&,int,int,int,const QDate& = QDate());
	bool contains(const QString&);
	QTodoList* list();
	static void updateColorsGlobally();
	static void updateBordersGlobally();
	static void updateMenuButtonGlobally();
	void updateColors();
	void updateBorders();
	void updateMenuButton();
	void prepare();
	QPopupMenu* actionsPopupMenu();
	void setSelected(bool);
	bool isSelected();
	void fixFocusDeadlineProblem();
	
	private slots:
	void updateDeadlineColor();
	void actionsMenuActivated(int);
	void actionsMenuAboutToShow();
	void actionsMenu();
	void actionsMenu(const QPoint&);
	void addSubTodo();
	void addTodoBelow();
	void addTodoAbove();
	void hasDeadline(int);
	void showHBoxBelow();
	void hideHBoxBelow();
	void statusChangedSlot();
	void deadlineChangedSlot();
	void agentTextChanged(const QString&);
	void emitChanged();
	void slotDeadlineComboChanged();
	void emitPriorityChanged();
	void emitStatusChanged();
	void emitDeadlineChanged();
	void handleSelectToggled();
	void handleSelectDoubleToggled();
	void subWheelEvent(QWheelEvent*);
	void navigateDown(int);
	void navigateUp(int);
	void cursorPositionChangedSlot(int,int);
	void oldDeadlineDateChanged(const QDate&);
	void oldDeadlineTimeChanged(const QTime&);

	signals:
	void changed();
	void priorityChanged();
	void statusChanged();
	void deadlineChanged();

	private:
	void init();
	void doShowHBoxBelow();
	void contextMenuEvent(QContextMenuEvent*);
	void keyPressEvent(QKeyEvent*);
	QColor headColor();
	QColor bodyColor();

	QButton* actions;
	QTodoPriorityWidget* priority;
	QTodoTaskEdit* task;
	QTodoComboWidget* deadline_combo;
	QLabel* created_label;
	QTodoDatetimeLabel* created;
	QTodoDatetimeLabel* done;
	QTodoDateInput* deadline_date;
	QTodoTimeInput* deadline_time;
	QTodoStatusWidget* status;
	QLineEdit* agent;
	QHBox* hbox_below;

	QWidget* spacer;
	QLabel* branch;
	QTodoItemWidget* the_widget;
	int depth;
	bool agent_changed;
	bool selected;

	static QTodoItem* extended_hbox_below;
	static QPtrList<QTodoItem> instances;
};

#endif

