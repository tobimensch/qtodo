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

#include <qlayout.h>
#include <qlabel.h>
#include <qimage.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qhbox.h>
#include <qtimer.h>

#include "qtodo_parser.h"
#include "qtodo_list.h"
#include "qtodo_datepicker.h"
#include "qtodo_task_edit.h"
#include "qtodo_check_label.h"
#include "qtodo_priority_widget.h"
#include "qtodo_status_widget.h"
#include "qtodo_settings_container.h"
#include "qtodo_clipboard.h"
#include "qtodo_item.h"
#include "qtodo_item_widget.h"
#include "qtodo_xpm.h"
#include "qtodo_font.h"
#include "qtodo_widget.h"
#include "qtodo_list_filter.h"
#include "qtodo_datetime_label.h"
#include "qtodo_push_button.h"
#include "qtodo_undo_manager.h"
#include "qtodo_deadline_dialog.h"
#include "qtodo_misc.h"

QTodoItem* QTodoItem::extended_hbox_below = 0;
QPtrList<QTodoItem> QTodoItem::instances;

enum
{
	NO_DEADLINE,
	HAS_DEADLINE
};

QTodoLineEdit::QTodoLineEdit(QWidget* parent) : QLineEdit(parent)
{
}

void QTodoLineEdit::keyPressEvent(QKeyEvent* event)
{
	if(event->state() == Qt::AltButton)
	{
		QFrame::keyPressEvent(event);
		return;
	}
	
	QLineEdit::keyPressEvent(event);
}


QTodoItem::QTodoItem(QWidget* parent) : QHBox(parent)
{
	init();
}

QTodoItem::QTodoItem(const QTodoItem& _item) : QHBox(0) //dynamic_cast<QWidget*>(_item.parent()))
{
	init();
	setDepth(_item.depth);
	setPriority(_item.getPriority());
	setTask(_item.getTask());
	setCreated(_item.getCreated());
	setDone(_item.getDone());
	setDeadline(_item.getDeadlineDateTime());
	setStatus(_item.getStatus());
	setAgent(_item.getAgent());
}

void QTodoItem::init()
{
	spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	branch = new QLabel(this);
	branch->setAlignment(Qt::AlignTop);
	branch->setPixmap(QTP::todo_branch.s(16,16));
	branch->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
	branch->hide();
	the_widget = new QTodoItemWidget(this);

	connect(the_widget,SIGNAL(showHBoxBelowTrigger()),this,SLOT(showHBoxBelow()));
	connect(the_widget,SIGNAL(hideHBoxBelowTrigger()),this,SLOT(hideHBoxBelow()));
	connect(the_widget,SIGNAL(contextMenuRequested(const QPoint&)),this,SLOT(actionsMenu(const QPoint&)));
	connect(the_widget,SIGNAL(selectToggled()),this,SLOT(handleSelectToggled()));
	connect(the_widget,SIGNAL(selectDoubleToggled()),this,SLOT(handleSelectDoubleToggled()));

	depth = 0;

	QGridLayout* grid = new QGridLayout(the_widget);
	grid->setSpacing(2);
	grid->setMargin(4);
	
	priority = new QTodoPriorityWidget(the_widget);
	status = new QTodoStatusWidget(the_widget);
	done = new QTodoDatetimeLabel(the_widget);
	QF::setFont(done,QF::get()->small);

	QSpacerItem* spacer = new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum);

	deadline_combo = new QTodoComboWidget(the_widget);
	deadline_combo->insert(QTP::dead.s(16,16),tr("Deadline"),HAS_DEADLINE);
	deadline_combo->insert(QTP::not_dead.s(16,16),tr("No deadline"),NO_DEADLINE);
	deadline_combo->setDirectMode(true);
	deadline_date =  new QTodoDateInput(the_widget,QDate::currentDate());
	deadline_date->setFont(QF::get()->small);
	deadline_time = new QTodoTimeInput(the_widget,QTime());
	deadline_time->setFont(QF::get()->small);

	connect(deadline_combo,SIGNAL(changed(int)),this,SLOT(hasDeadline(int)));

	deadline_combo->setCurrent(NO_DEADLINE,false);
	hasDeadline(false);

	actions = new QTodoPushButton(the_widget);
	actions->setMaximumWidth(8);
	actions->setMaximumHeight(16);
	actions->setPixmap(QTP::rightarrow.s(8,8));
	actions->setMinimumWidth(16);
	actions->setAutoRepeat(false);

	task = new QTodoTaskEdit(the_widget);
	task->setMinimumHeight(25);

	connect(task,SIGNAL(wheelEventSignal(QWheelEvent*)),this,SLOT(subWheelEvent(QWheelEvent*)));
	connect(task,SIGNAL(downNavigated(int)),this,SLOT(navigateDown(int)));
	connect(task,SIGNAL(upNavigated(int)),this,SLOT(navigateUp(int)));
	connect(task,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(cursorPositionChangedSlot(int,int)));

	hbox_below = new QHBox(the_widget);	

	QF::setFont(new QLabel(tr("Assigned to:"),hbox_below),QF::get()->small_bold);
	agent = new QTodoLineEdit(hbox_below);
	agent->setLineWidth(1);
	created_label = new QLabel(tr("Created:"),hbox_below);
	created_label->setFont(QF::get()->small_bold);
	created = new QTodoDatetimeLabel(hbox_below);
	created->setDateTime(QDateTime::currentDateTime());
	created->setFont(QF::get()->small);
	created->setMinimumWidth(100);
	hbox_below->hide();

	connect(actions,SIGNAL(clicked()),this,SLOT(actionsMenu()));

	connect(priority,SIGNAL(changed()),this,SLOT(emitPriorityChanged()));
	connect(priority,SIGNAL(changed()),this,SLOT(emitChanged()));
	connect(status,SIGNAL(changed()),this,SLOT(emitStatusChanged()));
	connect(status,SIGNAL(changed()),this,SLOT(emitChanged()));
	connect(task,SIGNAL(changed()),this,SLOT(emitChanged()));
	connect(deadline_combo,SIGNAL(changed()),this,SLOT(emitChanged()));
	connect(deadline_combo,SIGNAL(changed()),this,SLOT(slotDeadlineComboChanged()));
	connect(deadline_date,SIGNAL(dateChanged()),this,SLOT(emitChanged()));
	connect(deadline_time,SIGNAL(timeChanged()),this,SLOT(emitChanged()));
	connect(deadline_date,SIGNAL(dateChanged()),this,SLOT(emitDeadlineChanged()));
	connect(deadline_time,SIGNAL(timeChanged()),this,SLOT(emitDeadlineChanged()));
	connect(deadline_date,SIGNAL(dateChanged()),this,SLOT(updateDeadlineColor()));
	connect(deadline_time,SIGNAL(timeChanged()),this,SLOT(updateDeadlineColor()));
	connect(deadline_date,SIGNAL(oldDateChanged(const QDate&)),this,SLOT(oldDeadlineDateChanged(const QDate&)));
	connect(deadline_time,SIGNAL(oldTimeChanged(const QTime&)),this,SLOT(oldDeadlineTimeChanged(const QTime&)));

	connect(agent,SIGNAL(textChanged(const QString&)),this,SLOT(agentTextChanged(const QString&)));

	connect(this,SIGNAL(statusChanged()),this,SLOT(statusChangedSlot()));
	connect(this,SIGNAL(deadlineChanged()),this,SLOT(deadlineChangedSlot()));
	
 	grid->addWidget(QF::setFont(new QLabel(tr("Priority:"),the_widget),QF::get()->small_bold),0,0);
	grid->addWidget(priority,0,1);
	grid->addWidget(QF::setFont(new QLabel(tr("Status:"),the_widget),QF::get()->small_bold),0,2);
	grid->addWidget(status,0,3);
	grid->addWidget(done,0,4);
	grid->addItem(spacer,0,5);
	grid->addWidget(deadline_combo,0,6);
	grid->addWidget(deadline_date,0,7);
	grid->addWidget(deadline_time,0,8);
	grid->addWidget(actions,0,9);
	grid->addMultiCellWidget(task,1,1,0,9);
	grid->addMultiCellWidget(hbox_below,2,2,0,9);

	selected = false;

	updateMenuButton();
	updateColors();
	updateBorders();
	updateDeadlineColor();
	instances.append(this);

	agent_changed = false;
}

void QTodoItem::oldDeadlineDateChanged(const QDate& old_date)
{
	QDateTime old_datetime =  QDateTime(old_date,deadline_time->getQTime());

	QTUM::get()->addSingleChange(new QTodoChangeDeadline(list(),list()->findWidget(this),old_datetime));

	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->updateTrayIcon();
	QTodoDeadlineDialog::reloadGlobally();
}

void QTodoItem::oldDeadlineTimeChanged(const QTime& old_time)
{
	QDateTime old_datetime =  QDateTime(deadline_date->getQDate(),old_time);

	QTUM::get()->addSingleChange(new QTodoChangeDeadline(list(),list()->findWidget(this),old_datetime));

	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->updateTrayIcon();
	QTodoDeadlineDialog::reloadGlobally();
}

void QTodoItem::subWheelEvent(QWheelEvent* event)
{
	list()->subWheelEvent(event);
}

void QTodoItem::navigateDown(int i)
{
	list()->navigateDown(this,i);
}

void QTodoItem::navigateUp(int i)
{
	list()->navigateUp(this,i);
}

void QTodoItem::cursorPositionChangedSlot(int, int)
{
	if(!list())
		return;

	int cursor_y = task->cursorYPosition();
	QPoint pos = mapToParent(QPoint(0,0));
	pos.setY(pos.y()+cursor_y+task->mapToParent(QPoint(0,0)).y());
	list()->ensureVisible(0,pos.y());
}

void QTodoItem::setDepth(unsigned int _depth)
{
	depth = _depth;
	spacer->setMinimumWidth(10*depth);

	if(list())
	{
		int pos_above = list()->findPosAboveTodoItem(this);
		QTodoListIterator it(list());
		it.goTo(pos_above);
		unsigned int above_depth;
		if(it.item())
			above_depth = it.item()->getDepth();
		else
			above_depth = 0;
		if(_depth)
		{
			branch->show();
			if(above_depth != _depth)
			{
				if(branch->pixmap()->isNull())
				{
					branch->setPixmap(QTP::todo_branch.s(16,16));
				}
				branch->show();
			}
			else
			{
				branch->setPixmap(QPixmap());
				branch->setMinimumWidth(16);
			}
				
		}
		else
		{
			branch->hide();
		}
	}
}

QTodoItem::~QTodoItem()
{
	if(extended_hbox_below == this)
		extended_hbox_below = 0;
	instances.remove(this);
}

int QTodoItem::getDepth() const
{
	return depth;
}

void QTodoItem::setPriority(int _priority)
{
	priority->setCurrent(_priority,false);
}

void QTodoItem::setTask(const QString& _task)
{
	task->mySetText(_task,true);
}

void QTodoItem::setCreated(const QDateTime& _created)
{
	created->setDateTime(_created);
	if(_created.isValid())
		created_label->show();
	else
		created_label->hide();
}

void QTodoItem::setDone(const QDateTime& _done)
{
	done->setDateTime(_done);
	if(_done.isValid())
	{
		deadline_date->setTextMode(true);
		deadline_date->setEditable(false);
		deadline_time->setTextMode(true);
		deadline_time->setEditable(false);
		/*if(deadline_combo->getCurrent() == NO_DEADLINE)
			deadline_combo->hide();*/ //FIXME: do we want this?
		deadline_combo->setFrozen(true);
	}
	else
	{
		deadline_date->setTextMode(false);
		deadline_date->setEditable(true);
		deadline_time->setTextMode(false);
		deadline_time->setEditable(true);
		deadline_combo->setFrozen(false);
	}
}

void QTodoItem::setDeadline(const QDateTime& _deadline, bool prevent_emit_change)
{
	if(_deadline.isNull())
	{
		deadline_combo->setCurrent(NO_DEADLINE,false);
		hasDeadline(false);
		return;
	}
	else
	{
		deadline_combo->setCurrent(HAS_DEADLINE,false);
		deadline_date->setDate(_deadline.date(),prevent_emit_change);
		deadline_time->setTime(_deadline.time(),prevent_emit_change);
		hasDeadline(true);
		updateDeadlineColor();
	}
}

int QTodoItem::getPriority() const
{
	return priority->getCurrent();
}

QString QTodoItem::getTask() const
{
	return task->myText();
}

QString QTodoItem::getTaskRichText() const
{
	return task->myRichText();
}

QString QTodoItem::getTaskPlainText() const
{
	return task->myPlainText();
}

QStringList QTodoItem::getTaskLinks() const
{
	return task->myLinks();
}

QDateTime QTodoItem::getCreated() const
{
	return created->getDateTime();
}

QDateTime QTodoItem::getDone() const
{
	return done->getDateTime();
}

QString QTodoItem::getDoneString() const
{
	return done->getDateTimeString();
}

QString QTodoItem::getCreatedString() const
{
	return created->getDateTimeString();
}

QString QTodoItem::getCreatedLocalString() const
{
	return created->getDateTimeLocalString();
}

QString QTodoItem::getDeadlineString()const
{
	if(deadline_date->isShown())
		return deadline_date->getQDate().toString(Qt::ISODate) + " " + deadline_time->getQTime().toString();
	else
		return QString();
}

QString QTodoItem::getDeadlineLocalString()const
{
	return QTodoMisc::dateTime2LocalString(getDeadlineDateTime());
}

QDate QTodoItem::getDeadlineDate() const
{
	if(deadline_date->isShown())
		return deadline_date->getQDate();
	else
		return QDate();
}

QTime QTodoItem::getDeadlineTime() const
{
	if(deadline_time->isShown())
		return deadline_time->getQTime();
	else
		return QTime();
}

QDateTime QTodoItem::getDeadlineDateTime() const
{
	if(deadline_date->isShown())
		return QDateTime(deadline_date->getQDate(),deadline_time->getQTime());
	else
		return QDateTime();
}

void QTodoItem::addSubTodo()
{
	list()->addSubTodo(this);
}

void QTodoItem::addTodoBelow()
{
	list()->addTodoBelow(this);
}

void QTodoItem::addTodoAbove()
{
	list()->addTodoAbove(this);
}

void QTodoItem::save(QTextStream& stream)
{
	QString spacer;
	for(int i = 0; i < getDepth(); ++i)
		spacer += "\t";
	stream << spacer << "- Priority: " << QTodo::priorityToNonLocalizedString(getPriority()) << "\tStatus: " << QTodo::statusToNonLocalizedString(getStatus());
	if(!getAgent().stripWhiteSpace().isEmpty())
		stream << "\tAgent: " << getAgent();
	if(!getCreatedString().isEmpty())
		stream << "\tCreated: " << getCreatedString().replace("-","\\").replace(":","-");
	if(!getDoneString().isEmpty())
		stream << "\tDone: " << getDoneString().replace("-","\\").replace(":","-");
	if(!getDeadlineString().isEmpty())
		stream << "\tDeadline: " << getDeadlineString().replace("-","\\").replace(":","-");
	stream << endl;
	if(!getTask().stripWhiteSpace().isEmpty())
		stream << spacer << getTask().replace("-","\"-\"").replace("\n","\n"+spacer) << endl;
}

bool QTodoItem::contains(const QString& string)
{
	if(
	getTaskPlainText().contains(string,false) ||
	getAgent().contains(string,false))
		return true;
	else
		return false; 
}

void QTodoItem::setStatus(int _status, bool b)
{
	status->setCurrent(_status,b);
}

void QTodoItem::setAgent(const QString& _agent)
{
	agent->setText(_agent);
}

int QTodoItem::getStatus() const
{
	return status->getCurrent();
}

QString QTodoItem::getAgent() const
{
	return agent->text();
}

void QTodoItem::hasDeadline(int b)
{
	deadline_date->setEnabled(b);
	deadline_time->setEnabled(b);
	if(b)
	{
		deadline_date->show();
		deadline_time->show();
	}
	else
	{
		deadline_date->hide();
		deadline_time->hide();
	}
}

QTodoList* QTodoItem::list()
{
	if(parent() && parent()->parent() && parent()->parent()->parent())
		return dynamic_cast<QTodoList*>(parent()->parent()->parent());
	else
		return 0;
}

void QTodoItem::actionsMenu()
{
	actionsMenu(QPoint(-1,-1));
}

void QTodoItem::actionsMenu(const QPoint& _pos)
{
	QPoint pos;
	if(_pos.x() == -1)
		pos = actions->mapToGlobal(QPoint(0,actions->height()));
	else
		pos = _pos;


	QPopupMenu* menu = actionsPopupMenu();

	if(menu->exec(pos) != AMI_DELETE)
		delete menu;
	else
		list()->_delete();
}

QPopupMenu* QTodoItem::actionsPopupMenu()
{
	QPopupMenu* menu = new QPopupMenu(this);
	if(!QSC::get()->extend_todo_on_mouse_over)
	{
		if(hbox_below->isHidden())
		{
			menu->insertItem(QTP::show_more.s(16,16),tr("Show more"),AMI_MORE);
			menu->setAccel(ALT+Key_6,AMI_MORE);
		}
		else
		{
			menu->insertItem(QTP::show_less.s(16,16),tr("Show less"),AMI_UNDOMORE);
			menu->setAccel(ALT+Key_6,AMI_UNDOMORE);
		}
		menu->insertSeparator();
	}
	menu->insertItem(QTP::add_above.s(16,16),tr("Add above"),AMI_ADDTODOABOVE);
	menu->setAccel(ALT+Key_Comma,AMI_ADDTODOABOVE);
	menu->insertItem(QTP::add_below.s(16,16),tr("Add below"),AMI_ADDTODOBELOW);
	menu->setAccel(ALT+Key_Period,AMI_ADDTODOBELOW);
	menu->insertItem(QTP::add_sub.s(16,16),tr("Append"),AMI_ADDSUBTODO);
	menu->setAccel(ALT+Key_M,AMI_ADDSUBTODO);
	menu->insertSeparator();
	menu->insertItem(QTP::sort.s(16,16),tr("Sort"),AMI_SORT);
	menu->setAccel(ALT+Key_O,AMI_SORT);
	menu->setItemEnabled(AMI_SORT,list()->hasSubTodos(this));
	menu->insertSeparator();
	menu->insertItem(QTP::editcut.s(16,16),tr("Cut"),AMI_CUT);
	menu->setAccel(ALT+Key_7,AMI_CUT);
	menu->insertItem(QTP::editcopy.s(16,16),tr("Copy"),AMI_COPY);
	menu->setAccel(ALT+Key_8,AMI_COPY);
	menu->insertItem(QTP::editpaste.s(16,16),tr("Paste"),AMI_PASTE);
	menu->setAccel(ALT+Key_9,AMI_PASTE);
	menu->setItemEnabled(AMI_PASTE,!QCB::get()->isEmpty());
	menu->insertSeparator();
	menu->insertItem(QTP::editdelete.s(16,16),tr("Delete"),AMI_DELETE);
	menu->setAccel(ALT+Key_Minus,AMI_DELETE);

	connect(menu,SIGNAL(aboutToShow()),SLOT(actionsMenuAboutToShow()));
	connect(menu,SIGNAL(activated(int)),SLOT(actionsMenuActivated(int)));

	return menu;
}

void QTodoItem::actionsMenuAboutToShow()
{
	if(!selected)
	{
		list()->deselectAll();
		setSelected(true);
	}
}

void QTodoItem::actionsMenuActivated(int id)
{
	switch(id)
	{
		case AMI_MORE:
			doShowHBoxBelow();
			break;
		case AMI_UNDOMORE:
			hideHBoxBelow();
			break;
		case AMI_ADDSUBTODO:
			QTUM::get()->startRecording();
			addSubTodo();
			QTUM::get()->stopRecording();
			break;
		case AMI_ADDTODOBELOW:
			QTUM::get()->startRecording();
			addTodoBelow();
			QTUM::get()->stopRecording();
			break;
		case AMI_ADDTODOABOVE:
			QTUM::get()->startRecording();
			addTodoAbove();
			QTUM::get()->stopRecording();
			break;
		case AMI_SORT:
 			list()->sort(this);
			break;
		case AMI_CUT:
			list()->cut();
			break;
		case AMI_COPY:
			list()->copy();
			break;
		case AMI_PASTE:
			list()->pasteTodo(this);
			break;
	}
}

void QTodoItem::hideHBoxBelow()
{
	hbox_below->hide();	
}

void QTodoItem::showHBoxBelow()
{
	if(the_widget->cursorInsideWidget())
	{
		doShowHBoxBelow();
	}
}

void QTodoItem::doShowHBoxBelow()
{
	if(extended_hbox_below && extended_hbox_below != this)
		extended_hbox_below->hideHBoxBelow();
	hbox_below->show();
	extended_hbox_below = this;
}

bool QTodoItem::filter(const QString& str, int prio, int stat, int deadline_filter_mode, const QDate& dead)
{
	if(contains(str) &&
	(prio == ANY || prio == getPriority() || (prio > PRIO_NUM && getPriority() != (prio - PRIO_NUM -1))) &&
	(stat == ANY || stat == getStatus() || (stat > STAT_NUM && getStatus() != (stat - STAT_NUM -1))) &&
	(deadline_filter_mode == ANY || (!getDeadlineDate().isNull() && (
		(deadline_filter_mode == DLFM_EQ && dead == getDeadlineDate()) ||
		(deadline_filter_mode == DLFM_GREATER && getDeadlineDate() > dead) ||
		(deadline_filter_mode == DLFM_GREATER_OR_EQ && getDeadlineDate() >= dead) ||
		(deadline_filter_mode == DLFM_LESS && getDeadlineDate() < dead) ||
		(deadline_filter_mode == DLFM_LESS_OR_EQ && getDeadlineDate() <= dead)))
	))
		return true;
	else
		return false;
}

void QTodoItem::updateColorsGlobally()
{
	QPtrListIterator<QTodoItem> it(instances);
	for(;it.current();++it)
	{
		it.current()->updateColors();
	}
}

void QTodoItem::updateBordersGlobally()
{
	QPtrListIterator<QTodoItem> it(instances);
	for(;it.current();++it)
	{
		it.current()->updateBorders();
	}
}

void QTodoItem::updateMenuButtonGlobally()
{
	QPtrListIterator<QTodoItem> it(instances);
	for(;it.current();++it)
	{
		it.current()->updateMenuButton();
	}
}

void QTodoItem::updateBorders()
{
	the_widget->setLineWidth(QSC::get()->border_width);
}

void QTodoItem::updateMenuButton()
{
	if(QSC::get()->todo_item_menu_button)
		actions->show();
	else
		actions->hide();
}

QColor QTodoItem::headColor()
{
	if(selected)
		return palette().color(QPalette::Active,QColorGroup::Highlight);
	if(QSC::get()->todo_item_head_color_system)
		return QTodoWidget::system_palette.color(QPalette::Active,QColorGroup::Mid);
	else
		return QSC::get()->todo_item_head_color;
}

QColor QTodoItem::bodyColor()
{
	if(QSC::get()->todo_item_body_color_system)
		return QTodoWidget::system_palette.color(QPalette::Active,QColorGroup::Base);
	else
		return QSC::get()->todo_item_body_color;
}

void QTodoItem::updateColors()
{
	QColor head_color = headColor();
	QColor body_color = bodyColor();

	the_widget->setPaletteBackgroundColor(head_color);
	actions->setPaletteBackgroundColor(head_color);
	setDeadlineInputBackgroundColor(head_color);
	QPalette agent_palette = agent->palette();
	agent_palette.setColor(QColorGroup::Base,body_color);
	agent->setPalette(agent_palette);
	QPalette my = palette();
	my.setColor(QColorGroup::Base,body_color);
	task->setPalette(my);
}

void QTodoItem::prepare()
{
	QTimer::singleShot(1,task,SLOT(adaptToTextChange()));
}

QColor QTodoItem::getCurrentDeadlineColor()
{
	QColor state_col;
	if(QDateTime::currentDateTime() > getDeadlineDateTime())
	{ //expired
		state_col = QColor(0,0,0);
	}
	else if(QDateTime::currentDateTime().addSecs(QSC::get()->minutes_to_warn_before_deadline_expire*60) <  getDeadlineDateTime())
	{ //running
		float factor =  (float)QDateTime::currentDateTime().secsTo(getDeadlineDateTime().addSecs(-QSC::get()->minutes_to_warn_before_deadline_expire*60))/(float)(QSC::get()->minutes_to_warn_before_deadline_expire*60);
		if(factor > 1)
			factor = 1;
		state_col = QColor(0,127+((int)(factor*255))/2,0);
	}
	else
	{ //will expire soon
		float factor =  (float)QDateTime::currentDateTime().secsTo(getDeadlineDateTime())/(float)(QSC::get()->minutes_to_warn_before_deadline_expire*60);
		state_col = QColor(127+((int)((factor)*255))/2,0,0);
	}
	return state_col;
}

void QTodoItem::setDeadlineInputBackgroundColor(const QColor& col)
{
	deadline_date->setPaletteBackgroundColor(col);
	deadline_time->setPaletteBackgroundColor(col);
}

void QTodoItem::updateDeadlineColor()
{
	QColor col = getCurrentDeadlineColor();
	QImage image;
	image = QTP::dead;
	image = image.scale(16,16);
	int y = image.height() - 1;
	for(;y >= 0; --y)
	{
		int x = image.width() - 1;
		for(;x >= 0;--x)
		{
		uint* p = (uint*)image.scanLine(y)+x;
		if(QColor(*p) == Qt::white)
			*p = col.rgb();
		else if(QColor(*p) == Qt::black)
			*p = col.rgb()/2;
		}
	}
	QPixmap pixmap(image);
	
	deadline_combo->replacePixmap(pixmap,HAS_DEADLINE);
	QTimer::singleShot(1000*60,this,SLOT(updateDeadlineColor()));
}

void QTodoItem::emitChanged()
{
	list()->setModified();
	emit changed();
}

void QTodoItem::emitPriorityChanged()
{
	QTUM::get()->addSingleChange(new QTodoChangePriority(list(),list()->findWidget(this),priority->getLast()));
	emit priorityChanged();
}

void QTodoItem::emitStatusChanged()
{
	QTUM::get()->addSingleChange(new QTodoChangeStatus(list(),list()->findWidget(this),status->getLast(),getDone()));
	emit statusChanged();
}

void QTodoItem::statusChangedSlot()
{
	if(getStatus() == STAT_DONE)
	{
		if(!getDone().isValid())
			setDone(QDateTime::currentDateTime());
	}
	else
	{
		setDone(QDateTime());
	}
}

void QTodoItem::contextMenuEvent(QContextMenuEvent* event)
{
	list()->contextMenu(event->globalPos());
}

void QTodoItem::setCursorToBegin(int i)
{
	task->setFocus();
	if(task->lineOfChar(0,i) == 0)
		task->setCursorPosition(0,i);
	else
		task->setCursorPosition(0,0);
}

void QTodoItem::setCursorToEnd(int i)
{
	task->setFocus();
	if(task->lineOfChar(task->paragraphs()-1,i) == task->linesOfParagraph(task->paragraphs()-1)-1)
		task->setCursorPosition(task->paragraphs()-1,i);
	else
		task->setCursorPosition(task->paragraphs()-1,task->paragraphLength(task->paragraphs()-1));
}

void QTodoItem::deadlineChangedSlot()
{
}

void QTodoItem::slotDeadlineComboChanged()
{
	QDateTime old_datetime;
	if(deadline_date->isShown())
		old_datetime = QDateTime();
	else
		old_datetime =  QDateTime(deadline_date->getQDate(),deadline_time->getQTime());

	QTUM::get()->addSingleChange(new QTodoChangeDeadline(list(),list()->findWidget(this),old_datetime));

	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->updateTrayIcon();
	QTodoDeadlineDialog::reloadGlobally();

	emitDeadlineChanged();
}

void QTodoItem::emitDeadlineChanged()
{
	emit deadlineChanged();
}

void QTodoItem::agentTextChanged(const QString&)
{
	if(agent_changed)
		emitChanged();
	agent_changed = true;
}

void QTodoItem::setFocusOnTask()
{
	task->setFocus();
}

void QTodoItem::setSelected(bool b)
{
	selected = b;
	updateColors();
}

void QTodoItem::handleSelectToggled()
{
	setFocusOnTask();
	setSelected(!selected);
}

void QTodoItem::handleSelectDoubleToggled()
{
	setSelected(selected);
	list()->selectSubTodos(this,!selected);
}

bool QTodoItem::isSelected()
{
	return selected;
}

void QTodoItem::keyPressEvent(QKeyEvent* event)
{
	int state = event->state();
		event->accept();
	if(state == Qt::AltButton || (state |= Qt::AltButton & Qt::ShiftButton))
	{
		switch(event->key())
		{
			case Key_Control:
				handleSelectToggled();
				break;
			case Key_Plus:
				handleSelectDoubleToggled();
				break;
			case Key_O:
				list()->sort(this);
				break;
			case Key_1:
				priority->setFocus();
				priority->showMenu();
				break;
			case Key_2:
				status->setFocus();
				status->showMenu();
				break;
			case Key_3:
				deadline_date->setFocus();
				break;
			case Key_4:
				task->setFocus();
				break;
			case Key_5:
				doShowHBoxBelow();
				agent->setFocus();
				break;
			case Key_6:
				if(!hbox_below->isShown())
					doShowHBoxBelow();
				else
					hideHBoxBelow();
				break;
			case Key_Comma:
				QTUM::get()->startRecording();
				addTodoAbove();
				QTUM::get()->stopRecording();
				break;
			case Key_Period:
				QTUM::get()->startRecording();
				addTodoBelow();
				QTUM::get()->stopRecording();
				break;
			case Key_M:
				QTUM::get()->startRecording();
				addSubTodo();
				QTUM::get()->stopRecording();
				break;
			case Key_Greater:
				QTUM::get()->startRecording();
				setSelected(true);
				list()->selectSubTodos(this,true);
				list()->adjustDepth(-1);
				QTUM::get()->stopRecording();
				break;
			case Key_Less:
				QTUM::get()->startRecording();
				setSelected(true);
				list()->selectSubTodos(this,true);
				list()->adjustDepth(1);
				QTUM::get()->stopRecording();
				break;
		}
	}
	else
	{
		switch(event->key())
		{
			case Key_PageUp:
				list()->navigateUp(this,0);
				break;
			case Key_PageDown:
				list()->navigateDown(this,0);
				break;
			case Key_Up:
				setCursorToBegin(0);
				break;
			case Key_Down:
				setCursorToEnd(0);
				break;
		}
	}

	QFrame::keyPressEvent(event);
}

void QTodoItem::fixFocusDeadlineProblem()
{
	deadline_date->updateFocusInDate();
	deadline_time->updateFocusInTime();
}



