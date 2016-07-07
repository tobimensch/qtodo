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

#include <qdatetime.h>
#include <qpopupmenu.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qcursor.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobjectlist.h>
#include <qhbox.h>

#include "qtodo_list.h"
#include "qtodo_item_widget.h"
#include "qtodo_item.h"
#include "qtodo_parser.h"
#include "qtodo_section_widget.h"
#include "qtodo_list_view_item.h"
#include "qtodo_list_widget.h"
#include "qtodo_radio_dialog.h"
#include "qtodo_clipboard.h"
#include "qtodo_sort_dialog.h"
#include "qtodo_xpm.h"
#include "qtodo_undo_manager.h"
#include "qtodo_task_edit.h"

QTodoList::QTodoList(QWidget* parent) : QScrollView(parent)
{
	setResizePolicy(QScrollView::AutoOneFit);
	setDragAutoScroll(true);

	vbox = new QVBox(viewport());
	vbox->setMargin(5);
	vbox->setSpacing(5);
	vbox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
	dynamic_cast<QBoxLayout*>(vbox->layout())->setAutoAdd(false);
	addChild(vbox);

	spacer = 0;
	refreshSpacer();
}

/*void QTodoList::setVBarGeometry(QScrollBar& vbar, int x, int y, int w, int h)
{
    vbar.setGeometry( x+w-w/2, y, w/2, h );
}*/

void QTodoList::refreshSpacer()
{
	dynamic_cast<QBoxLayout*>(vbox->layout())->remove(spacer);
	delete spacer;
	spacer = new QLabel(vbox);
	spacer->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
	dynamic_cast<QBoxLayout*>(vbox->layout())->addWidget(spacer);
	dynamic_cast<QBoxLayout*>(vbox->layout())->setStretchFactor(spacer,2);
	spacer->show();
}

QTodoList::~QTodoList()
{}

void QTodoList::addTodo()
{
	QTodoItem* item = new QTodoItem(vbox);

	insertTodo(item,-1);
	item->setDepth(0);
	item->setFocusOnTask();
	item->adjustSize();
	ensureVisible(0,item->mapToParent(QPoint(0,item->height())).y(),0,0);
	fixItemSize(item);
}

void QTodoList::fixItemSize(QTodoItem* item)
{
	QSize s = item->sizeHint();
	s.setWidth(vbox->width()-vbox->margin()*2);
	item->resize(s);
}

void QTodoList::load(QTodoTopNode* top_node)
{
	loadNode(top_node->top,0);

	show();
	refreshSpacer();
}

void QTodoList::loadNode(QTodoNode* node, unsigned int depth)
{
	if(!node)
		return;

	for(QTodoNode::iterator it = node->begin(); it != node->end(); ++it)
	{
		if((*it)->section == true)
		{
			QTodoListViewItemSection* lists_item = new QTodoListViewItemSection(getTodoListWidget()->getListViewItem());
			QTodoSectionWidget* section = new QTodoSectionWidget(vbox,lists_item);
			lists_item->setSection(section);
			section->setName((*it)->task);
			section->show();
			continue;
		}

		QTodoItem* item = new QTodoItem(vbox);
		insertTodoAtBottom(item);

		item->setDepth(depth);
		item->setTask((*it)->task);
		item->setPriority((*it)->priority);
		item->setDeadline((*it)->deadline);
		item->setCreated((*it)->created);
		item->setDone((*it)->done);
		item->setStatus((*it)->status);
		item->setAgent((*it)->agent);
		item->show();
		loadNode((*it)->sub,depth+1);
	}
}

void QTodoList::save(QTextStream& stream)
{
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		if(QTodoItem* item_hbox = dynamic_cast<QTodoItem*>(it.current()))
		{
			item_hbox->save(stream);
		}
		else if(QTodoSectionWidget* section = dynamic_cast<QTodoSectionWidget*>(it.current()))
			section->save(stream);
	}
}

void QTodoList::filter(const QString& str, int prio, int status, int deadline_filter_mode, const QDate& deadline)
{
	QTodoListIterator it(this);
	for(;it.current(); ++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			if(item->filter(str,prio,status,deadline_filter_mode,deadline))
			{
				QTodoItem* parent_it = item;
				for(;QTodoItem* parent = parentTodo(parent_it);parent_it = parent)
					parent->show();
				item->show();
			}
			else
				item->hide();
		}
	}
}

QTodoSectionWidget* QTodoList::addSection()
{
	QTodoSectionWidget* section = new QTodoSectionWidget(vbox,0);
	section->show();
	return section; //FIXME
}

int QTodoList::findWidget(QWidget* widget)
{
	return dynamic_cast<QBoxLayout*>(vbox->layout())->findWidget(widget);
}

void QTodoList::addSubTodo(QTodoItem* parent)
{
	addTodo(findWidget(parent)+1,parent->getDepth()+1);
}

void QTodoList::addTodoAbove(QTodoItem* sister)
{
	addTodo(findWidget(sister),sister->getDepth());
}

void QTodoList::addTodoAtTop()
{
	addTodo(0,0);
}

void QTodoList::addTodoBelow(QTodoItem* sister)
{
	addTodo(findPosBelowTodoItem(sister),sister->getDepth());
}

int QTodoList::findPosBelowTodoItem(QTodoItem* item)
{
	QTodoListIterator it(this);
	it.goTo(findWidget(item)+1);
	for(;it.current();++it)
	{
		if(it.isItem())
		{
			if(dynamic_cast<QTodoItem*>(it.current())->getDepth() <= item->getDepth())
				break;
		}
		else
		{
			break;
		}
	}
	return it.currentIndex();
}

int QTodoList::findPosAboveTodoItem(QTodoItem* item)
{
	QTodoListIterator it(this);
	it.goTo(findWidget(item)-1);
	for(;it.current();--it)
	{
		if(it.isItem())
		{
			if(dynamic_cast<QTodoItem*>(it.current())->getDepth() <= item->getDepth())
				break;
		}
		else
		{
			break;
		}
	}
	return it.currentIndex();
}

void QTodoList::addTodo(int index, int depth)
{
	QTodoItem* item = new QTodoItem(vbox);
	insertTodo(item,index,depth);
	item->setFocusOnTask();
	item->adjustSize();
	ensureVisible(0,item->mapToParent(QPoint(0,item->height()/2)).y(),0,item->height()/2);
	QSize s = item->sizeHint();
	s.setWidth(vbox->width()-vbox->margin()*2);
	item->resize(s);
}

void QTodoList::insertTodo(QTodoItem* item, int index, int depth)
{
	insertTodo(item,index);
	item->setDepth(depth);

	QTodoListIterator it(this);
	it.goTo(index-1);
	if(QTodoItem* _item = it.item())
	{
		if(_item->getDepth() < item->getDepth())
		{
			it.goTo(index+1);
			if((_item = it.item()) != 0 && _item->getDepth() == item->getDepth())
			{
				_item->setDepth(_item->getDepth());
			}
		}
	}
}

void QTodoList::insertTodo(QTodoItem* item, int index)
{
	if(index == -1)
		index = findWidget(spacer);

	if(item->parent() != vbox)
		item->reparent(vbox,QPoint(0,0));
	dynamic_cast<QBoxLayout*>(vbox->layout())->insertWidget(index,item);
	item->show();

	content.insert(content.begin()+index, item);

	QTUM::get()->addChange(new QTodoChangeInsert(this,findWidget(item)));
	if(QTUM::get()->isRecording())
			setModified();
	//refreshSpacer();
}

void QTodoList::removeTodo(QTodoItem* item)
{
	QTUM::get()->addChange(new QTodoChangeDelete(this,findWidget(item),new QTodoItem(*item)));
	removeWidget(item);
	delete item;
	setModified();
}

void QTodoList::removeWidget(QWidget* widget)
{
	for(std::vector<QWidget*>::iterator it = content.begin(); it != content.end(); ++it)
		if((*it) == widget)
		{
			content.erase(it);
			break;
		}
	setModified();
}

void QTodoList::insertTodoAtBottom(QTodoItem* item)
{
	insertTodo(item,-1);
}

void QTodoList::takeTodo(QTodoItem* item)
{
	item->hide();
	QTUM::get()->addChange(new QTodoChangeDelete(this,findWidget(item),new QTodoItem(*item)));
	dynamic_cast<QBoxLayout*>(vbox->layout())->remove(item);
	removeWidget(item);
	setModified();
}

void QTodoList::takeTodos(std::vector<QTodoItem*>* items)
{
	for(std::vector<QTodoItem*>::iterator it = items->begin(); it != items->end(); ++it)
		takeTodo((*it));
}

void QTodoList::takeAll()
{
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		it.take();
	}
}

QTodoListWidget* QTodoList::getTodoListWidget()
{
	return dynamic_cast<QTodoListWidget*>(parent());
}

void QTodoList::mouseMoveEvent(QMouseEvent*)
{}

QTodoListIterator::QTodoListIterator(QTodoList* _list) : index(0), list(_list)
{
	/*const QObjectList* wl = list->vbox->queryList("QWidget");
	int cur_index;
	m_count = 0;
	for(QObjectListIt it(*wl); it.current(); ++it)
	{
		if((cur_index = dynamic_cast<QBoxLayout*>(list->vbox->layout())->findWidget(dynamic_cast<QWidget*>(it.current()))) != -1)
		{
			QWidget* widget = dynamic_cast<QWidget*>(it.current());
			map[cur_index] = widget;
			++m_count;
		}
	}
	--m_count;

	if(m_count < 0) { m_count = 0; }
	delete wl;*/
}

QWidget* QTodoListIterator::operator++()
{
	++index;
	return current();
}

QWidget* QTodoListIterator::operator--()
{
	--index;
	return current();
}

QWidget* QTodoListIterator::goTo(unsigned int _index)
{
	index = _index;
	return current();
}

QWidget* QTodoListIterator::current()
{
	if(list->content.size() > index)
	{
		return list->content[index];
	}
	return 0;
}

const QWidget* QTodoListIterator::currentConst()
{
	if(list->content[index])
	{
		return list->content[index];
	}
	return 0;
}

unsigned int QTodoListIterator::count()
{
	return list->content.size();
}

unsigned int QTodoListIterator::count(QTodoList* list)
{
	QTodoListIterator it(list);
	return it.count();
}

void QTodoList::subWheelEvent(QWheelEvent* event)
{
	scrollBy(0,-event->delta());
}

void QTodoList::contextMenuEvent(QContextMenuEvent* event)
{
	QWidget* child_at = childAt(event->pos());
	if(child_at != vbox && child_at != spacer)
		return;

	contextMenu(event->globalPos());
}

void QTodoList::contextMenu(const QPoint& pos)
{
	QPopupMenu* context_menu = new QPopupMenu(this);
	contextMenu(context_menu);
	contextMenuUpdate(context_menu);
	context_menu->exec(pos);
	delete context_menu;
}

void QTodoList::contextMenu(QPopupMenu* menu)
{
	menu->insertItem(QTP::add_top.s(16,16),tr("Add at top"),CMID_ADDTODOATTOP);
	menu->setAccel(ALT+Key_Prior,CMID_ADDTODOATTOP);
	menu->insertItem(QTP::add_bottom.s(16,16),tr("Add at bottom"),CMID_ADDTODOATBOTTOM);
	menu->setAccel(ALT+Key_Next,CMID_ADDTODOATBOTTOM);
	menu->insertSeparator();
	menu->insertItem(QTP::sort.s(16,16),tr("Sort"),CMID_SORT);
	menu->setAccel(CTRL+Key_O,CMID_SORT);
	menu->insertSeparator();
	menu->insertItem(QTP::editcut.s(16,16),tr("Cut"),CMID_CUT);
	menu->setAccel(ALT+Key_7,CMID_CUT);
	menu->insertItem(QTP::editcopy.s(16,16),tr("Copy"),CMID_COPY);
	menu->setAccel(ALT+Key_8,CMID_COPY);
	menu->insertItem(QTP::editpaste.s(16,16),tr("Paste"),CMID_PASTE);
	menu->setAccel(ALT+Key_9,CMID_PASTE);
	menu->insertSeparator();
	menu->insertItem(QTP::select_all.s(16,16),tr("Select all"),CMID_SELECT_ALL);
	menu->setAccel(CTRL+Key_A,CMID_SELECT_ALL);
	menu->insertItem(QTP::deselect_all.s(16,16),tr("Deselect all"),CMID_DESELECT_ALL);
	menu->setAccel(CTRL+SHIFT+Key_A,CMID_DESELECT_ALL);

	connect(menu,SIGNAL(activated(int)),this,SLOT(contextMenuActivated(int)));
}

void QTodoList::contextMenuUpdate(QPopupMenu* menu)
{
	int cnt = count();
	bool b;
	b = cnt > 0;
	menu->setItemEnabled(CMID_ADDTODOATBOTTOM,b);

	b = isItemSelected();
	menu->setItemEnabled(CMID_COPY,b);
	menu->setItemEnabled(CMID_CUT,b);

	b = !QCB::get()->isEmpty();
	menu->setItemEnabled(CMID_PASTE,b);

	b = cnt > 1;
	menu->setItemEnabled(CMID_SORT,b);
}

/*void QTodoList::disableContextMenuAccels()
{
	menu->setItemEnabled(CMID_ADDTODOATBOTTOM,false);
	menu->setItemEnabled(CMID_COPY,false);
	menu->setItemEnabled(CMID_CUT,true);
	menu->setItemEnabled(CMID_PASTE,true);
	menu->setItemEnabled(CMID_SORT,true);
}*/

void QTodoList::contextMenuActivated(int id)
{
	switch(id)
	{
	case CMID_ADDTODO:
		QTUM::get()->startRecording();
		addTodo();
		QTUM::get()->stopRecording();
		break;
	case CMID_ADDTODOATTOP:
		QTUM::get()->startRecording();
		addTodoAtTop();
		QTUM::get()->stopRecording();
		break;
	case CMID_ADDTODOATBOTTOM:
		QTUM::get()->startRecording();
		addTodo();
		QTUM::get()->stopRecording();
		break;
	case CMID_ADDSECTION:
		QTUM::get()->startRecording();
		addSection();
		QTUM::get()->stopRecording();
		break;
	case CMID_SORT:
		sort();
		break;
	case CMID_COPY:
		copy();
		break;
	case CMID_CUT:
		cut();
		break;
	case CMID_PASTE:
		paste();
		break;
	case CMID_SELECT_ALL:
		selectAll();
		break;
	case CMID_DESELECT_ALL:
		deselectAll();
		break;
	}
}

void QTodoList::sort()
{
	QTodoSortDialog sort_dialog;

	deselectAll();

	if(sort_dialog.exec() == QDialog::Accepted)
	{
		preserveContentsYPos();
		const QTodoSortCriteriaMap* criterias = sort_dialog.getCriterias();

		QPtrList<QWidget> list_widgets;
		list_widgets.append(0);
		QTodoListIterator it(this);
		for(;it.current();++it)
			list_widgets.append(it.current());

		QTodoListItemsSorter sorter(&list_widgets,criterias);
		QPtrList<QWidget>* sorted = sorter.get();

		QTUM::get()->startRecording();
		takeAll();
		for(unsigned int i = 0; i < sorted->count(); ++i)
		{
			if(QTodoItem* item = dynamic_cast<QTodoItem*>(sorted->at(i)))
			{
				insertTodo(item,i);
				item->setDepth(item->getDepth());
			}
		}
		QTUM::get()->stopRecording();
		restoreContentsYPos();
	}
}

void QTodoList::sort(QTodoItem* item)
{
	QTodoSortDialog sort_dialog;

	deselectAll();
	selectSubTodos(item,true);

	if(sort_dialog.exec() == QDialog::Accepted)
	{
		preserveContentsYPos();
		const QTodoSortCriteriaMap* criterias = sort_dialog.getCriterias();

		QPtrList<QWidget> list_widgets;

		std::vector<QTodoItem*> sub_items = getSubTodos(item);

		list_widgets.append(0);
		for(std::vector<QTodoItem*>::iterator it = sub_items.begin(); it != sub_items.end(); ++it)
		{
			list_widgets.append((*it));
		}

		QTodoListItemsSorter sorter(&list_widgets,criterias);
		QPtrList<QWidget>* sorted = sorter.get();

		QTUM::get()->startRecording();
		takeTodos(&sub_items);
		int pos = findWidget(item) + 1;
		for(unsigned int i = 0; i < sorted->count(); ++i)
		{
			if(QTodoItem* item = dynamic_cast<QTodoItem*>(sorted->at(i)))
			{
				insertTodo(item,pos+i);
				item->setDepth(item->getDepth());
			}
		}
		QTUM::get()->stopRecording();
		restoreContentsYPos();
	}
}

int QTodoList::count()
{
	return QTodoListIterator::count(this);
}

int QTodoList::countSelected()
{
	int i = 0;
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		if(QTodoItem* item = it.item())
			if(item->isSelected())
				++i;
	}
	return i;
}

std::vector<QTodoItem*> QTodoList::getSubTodos(QTodoItem* item)
{
	std::vector<QTodoItem*> vector;
	int item_pos = findWidget(item);
	int item_depth = item->getDepth();
	QTodoListIterator it(this);
	for(it.goTo(item_pos+1); it.current(); ++it)
	{
		if(dynamic_cast<QTodoItem*>(it.current()) && dynamic_cast<QTodoItem*>(it.current())->getDepth() > item_depth)
		{
			vector.push_back(dynamic_cast<QTodoItem*>(it.current()));
		}
		else
		{
			break;
		}
	}
	return vector;
}

bool QTodoList::hasSubTodos(QTodoItem* item)
{
	int item_pos = findWidget(item);
	int item_depth = item->getDepth();
	QTodoListIterator it(this);
	for(it.goTo(item_pos+1); it.current(); ++it)
	{
		if(dynamic_cast<QTodoItem*>(it.current()) && dynamic_cast<QTodoItem*>(it.current())->getDepth() > item_depth)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool QTodoList::isFirstSubTodo(QTodoItem* item)
{
	QTodoListIterator it(this);
	it.goTo(findWidget(item));
	--it;
	if(QTodoItem* current = dynamic_cast<QTodoItem*>(it.current()))
	{
		if(current->getDepth() < item->getDepth())
			return true;
		else
			return false;
	}
	else
	{
		return true;
	}
}

/*void QTodoList::deleteTodo(QTodoItem* todo)
{
	QTodoRadioDialog radio_dialog(tr("Delete todo"),tr("You are about to delete a todo"));
	radio_dialog.setIcon(QTP::editdelete.s(16,16));
 
	if(hasSubTodos(todo))
	{
		radio_dialog.add(tr("Remove todo and sub-todos"),TDDL_TODO_AND_SUB);
		radio_dialog.add(tr("Remove todo and move sub-todos up"),TDDL_TODO_BUT_SUBUP);
		if(!isFirstSubTodo(todo))
			radio_dialog.add(tr("Remove todo, but don't touch sub-todos"),TDDL_TODO_NOT_SUB);
		radio_dialog.add(tr("Remove only sub-todos"),TDDL_ONLY_SUB);
	}
	else
	{
		radio_dialog.add(tr("Remove todo"),TDDL_TODO_NOT_SUB);
	}
	if(radio_dialog.exec() == QDialog::Accepted)
	{
		preserveContentsYPos();
		std::vector<QTodoItem*> sub_todos;
		QTUM::get()->startRecording();
		switch(radio_dialog.selected())
		{
			case TDDL_TODO_AND_SUB:
				sub_todos = getSubTodos(todo);
				removeTodos(sub_todos);
				removeTodo(todo);
				break;
			case TDDL_TODO_BUT_SUBUP:
				sub_todos = getSubTodos(todo);
				adjustDepth(sub_todos,-1);
				removeTodo(todo);
				break;
			case TDDL_TODO_NOT_SUB:
				removeTodo(todo);
				break;
			case TDDL_ONLY_SUB:
				sub_todos = getSubTodos(todo);
				removeTodos(sub_todos);
				break;
		}
		QTUM::get()->stopRecording();
		restoreContentsYPos();
	}
}*/

void QTodoList::removeTodos(std::vector<QTodoItem*> todos)
{
	for(std::vector<QTodoItem*>::iterator it = todos.begin(); it != todos.end(); ++it)
	{
		removeTodo((*it));
	}
}

void QTodoList::adjustDepth(std::vector<QTodoItem*> todos, int i)
{
	for(std::vector<QTodoItem*>::iterator it = todos.begin(); it != todos.end(); ++it)
	{
		QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget((*it)),(*it)->getDepth()));
		(*it)->setDepth((*it)->getDepth() + i);
	}
}

void QTodoList::copy()
{
	/*QTodoRadioDialog radio_dialog(tr("Copy list"),tr("You are about to copy a todo-list"));
	radio_dialog.setIcon(QTP::editcopy.s(16,16));
	radio_dialog.add(tr("Copy all"),TDCP_ALL);

	if(radio_dialog.exec() == QDialog::Accepted)
	{*/
	preserveContentsYPos();
	QTodoListIterator it(this);
	QCB::get()->clear();
	for(;it.current();++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			if(item->isSelected())
				QCB::get()->add(item);
		}
	}
	QCB::get()->doExport();
	restoreContentsYPos();
	deselectAll();
	/*}*/
}

void QTodoList::adjustDepth(int i)
{
	QTodoListIterator it(this);
	QCB::get()->clear();
	int last_depth = -1;
	int new_depth;
	for(;it.current();++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			if(item->isSelected())
			{
				new_depth = item->getDepth()+i;
				if(new_depth < 0)
					new_depth = 0;
				if(new_depth > last_depth + 1)
					new_depth = last_depth + 1;

				if(new_depth != item->getDepth())
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
				item->setDepth(new_depth);
			}

			last_depth = item->getDepth();
		}
	}
	deselectAll();
}


/*void QTodoList::cutTodo(QTodoItem* todo)
{
	QTodoRadioDialog radio_dialog(tr("Cut todo"),tr("You are about to cut a todo"));
	radio_dialog.setIcon(QTP::editcut.s(16,16));
	if(hasSubTodos(todo))
	{
		radio_dialog.add(tr("Cut todo and sub-todos"),TDCT_TODO_AND_SUB);
		radio_dialog.add(tr("Cut todo and move sub-todos up"),TDCT_TODO_BUT_SUBUP);
		if(!isFirstSubTodo(todo))
			radio_dialog.add(tr("Cut todo, but not sub-todos"),TDCT_TODO_NOT_SUB);
		radio_dialog.add(tr("Cut only sub-todos"),TDCT_ONLY_SUB);
	}
	else
	{
		radio_dialog.add(tr("Cut todo"),TDCT_TODO_NOT_SUB);
	}
	if(radio_dialog.exec() == QDialog::Accepted)
	{
		preserveContentsYPos();
		std::vector<QTodoItem*> sub_todos = getSubTodos(todo);
		QCB::get()->clear();
		QCB::get()->setCut(true);
		QTUM::get()->startRecording();
		switch(radio_dialog.selected())
		{
			case TDCT_TODO_AND_SUB:
				QCB::get()->add(todo);
				QCB::get()->add(sub_todos);
				takeTodo(todo);
				takeTodos(&sub_todos);
				break;
			case TDCT_TODO_BUT_SUBUP:
				QCB::get()->add(todo);
				takeTodo(todo);
				adjustDepth(sub_todos,-1);
				break;
			case TDCT_TODO_NOT_SUB:	
				QCB::get()->add(todo);
				takeTodo(todo);
				break;
			case TDCT_ONLY_SUB:
				QCB::get()->add(sub_todos);
				takeTodos(&sub_todos);
				break;
		}
		QTUM::get()->stopRecording();
		QCB::get()->doExport();
		restoreContentsYPos();
	}
}*/

void QTodoList::_delete()
{
	if(!isItemSelected())
		return;

	QTodoRadioDialog radio_dialog(tr("Delete todos"),tr("You are about to delete todos"));
	radio_dialog.setIcon(QTP::editdelete.s(16,16));

	radio_dialog.add(QTP::editdelete,tr("Delete todos"),0);

	if(radio_dialog.exec() != QDialog::Accepted)
		return;

	preserveContentsYPos();
	QTodoListIterator it(this);
	QTUM::get()->startRecording();
	int last_depth = -1;
	bool last_deleted = false;
	for(;it.current();++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			if(!item->isSelected())
			{
				if(item->getDepth() - 1 > last_depth)
				{
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
					item->setDepth(last_depth+1);
				}

				last_depth = item->getDepth();
				if(last_deleted)
					item->setDepth(item->getDepth());
				last_deleted = false;
				continue;
			}
			else
			{
				last_deleted = true;
			}

			it.remove();
		}
	}
	QTUM::get()->stopRecording();
	restoreContentsYPos();
}

void QTodoList::cut()
{
	preserveContentsYPos();
	QTodoListIterator it(this);
	QCB::get()->clear();
	QCB::get()->setCut(true);
	QTUM::get()->startRecording();
	int last_depth = -1;
	int above = -1;
	QTodoItem* previous = 0;
	bool last_cut = false;
	for(;it.current();++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			if(!item->isSelected())
			{
				if(above == -1)
					above = it.currentIndex() - 1;
				if(item->getDepth() - 1 > last_depth)
				{
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
					item->setDepth(last_depth+1);
				}

				if(last_cut)
					item->setDepth(item->getDepth());

				last_depth = item->getDepth();
				last_cut = false;
				continue;
			}
			else
			{
				last_cut = true;
			}

			if(!previous)
			{
				--it;
				previous = it.item();
				++it;
			}
			QCB::get()->add(item);
			it.take();
		}
	}
	if(previous)
		previous->setFocusOnTask();
	else
		jumpToFirst();
	QTUM::get()->stopRecording();
	QCB::get()->doExport();
	restoreContentsYPos();
}

void QTodoList::pasteTodo(QTodoItem* todo)
{
	if(QCB::get()->isEmpty())
			return;

	deselectAll();
	todo->setSelected(true);

	QTodoRadioDialog radio_dialog(tr("Paste todos"),tr("You are about to paste todos"));
	radio_dialog.setIcon(QTP::editpaste.s(16,16));

	radio_dialog.add(QTP::add_top,tr("Paste at top"),TDP_ATTOP);
	radio_dialog.add(QTP::add_above,tr("Paste above"),TDP_ABOVE);
	radio_dialog.add(QTP::add_below,tr("Paste below"),TDP_BELOW);
	radio_dialog.add(QTP::add_sub,tr("Paste as sub-todos"),TDP_SUB);
	radio_dialog.add(QTP::add_bottom,tr("Paste at bottom"),TDP_ATBOTTOM);

	if(radio_dialog.exec() == QDialog::Accepted)
	{
		//preserveContentsYPos();
		int pos;
		int last_depth = todo->getDepth();
		int base_depth = todo->getDepth() - QCB::get()->minDepth();//(*QCB::get()->getItems()->begin())->getDepth();
		QTUM::get()->startRecording();
		QTodoItem* item = 0;
		switch(radio_dialog.selected())
		{
		case TDP_ATTOP:
			pasteAtTop();
			break;
		case TDP_ATBOTTOM:
			pasteAtBottom();
			break;
		case TDP_ABOVE:
			pos = findWidget(todo);
			for(QTodoItemVector::reverse_iterator it = QCB::get()->getItems()->rbegin(); it != QCB::get()->getItems()->rend(); ++it)
			{
				item = new QTodoItem(*(*it));
				insertTodo(item,pos,(*it)->getDepth() + base_depth);
				ensureItemVisible(item);
				
				if(item->getDepth() > last_depth + 1)
				{
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
					item->setDepth(last_depth + 1);
				}
		
				last_depth = item->getDepth();
			}
			fixItemSize(item);
			QCB::get()->clear();
			break;
		case TDP_BELOW:
			pos = findPosBelowTodoItem(todo);
			for(QTodoItemVector::iterator it = QCB::get()->getItems()->begin(); it != QCB::get()->getItems()->end(); ++it)
			{
				item = new QTodoItem(*(*it));
				insertTodo(item,pos,(*it)->getDepth() + base_depth);
				ensureItemVisible(item);

				if(item->getDepth() > last_depth + 1)
				{
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
					item->setDepth(last_depth + 1);
				}
		
				last_depth = item->getDepth();
				pos++;
			}
			fixItemSize(item);
			QCB::get()->clear();
			break;
		case TDP_SUB:
			pos = findWidget(todo) + 1;
			for(QTodoItemVector::iterator it = QCB::get()->getItems()->begin(); it != QCB::get()->getItems()->end(); ++it)
			{
				item = new QTodoItem(*(*it));
				insertTodo(item,pos,(*it)->getDepth() + base_depth + 1);
				ensureItemVisible(item);

				if(item->getDepth() > last_depth + 1)
				{
					QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
					item->setDepth(last_depth + 1);
				}
		
				last_depth = item->getDepth();
				pos++;
			}
			fixItemSize(item);
			QCB::get()->clear();
			break;
		}
		QTUM::get()->stopRecording();
		//restoreContentsYPos();
		deselectAll();
	}
}

void QTodoList::paste()
{
	if(QCB::get()->isEmpty())
			return;

	deselectAll();

	QTodoRadioDialog radio_dialog(tr("Paste todos"),tr("You are about to paste todos"));
	radio_dialog.setIcon(QTP::editpaste.s(16,16));

	radio_dialog.add(QTP::add_top,tr("Paste at top"),TDP_ATTOP);
	if(count() > 0)
		radio_dialog.add(QTP::add_bottom,tr("Paste at bottom"),TDP_ATBOTTOM);

	if(radio_dialog.exec() == QDialog::Accepted)
	{
		QTUM::get()->startRecording();
		switch(radio_dialog.selected())
		{
		case TDP_ATTOP:
			pasteAtTop();
			break;
		case TDP_ATBOTTOM:
			pasteAtBottom();
			break;
		}
		QTUM::get()->stopRecording();
	}
}

void QTodoList::pasteAtTop()
{
	int last_depth = -1;
	int base_depth = -QCB::get()->minDepth();
	int pos = 0;

	QTodoItem* item = 0;
	for(QTodoItemVector::iterator it = QCB::get()->getItems()->begin(); it != QCB::get()->getItems()->end(); ++it)
	{
		item = new QTodoItem(*(*it));
		insertTodo(item,pos);
		QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
		item->setDepth(item->getDepth()+base_depth);

		if(item->getDepth() > last_depth + 1)
		{
			QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
			item->setDepth(last_depth + 1);
		}

		last_depth = item->getDepth();
		pos++;
	}
	QCB::get()->clear();
	jumpToFirst();
}

void QTodoList::pasteAtBottom()
{
	int last_depth = -1;
	int base_depth = -QCB::get()->minDepth();

	QTodoItem* item = 0;
	for(QTodoItemVector::iterator it = QCB::get()->getItems()->begin(); it != QCB::get()->getItems()->end(); ++it)
	{
		item = new QTodoItem(*(*it));
		insertTodoAtBottom(item);
		QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
		item->setDepth(item->getDepth() + base_depth);
		
		if(item->getDepth() > last_depth + 1)
		{
			QTUM::get()->addChange(new QTodoChangeDepth(this,findWidget(item),item->getDepth()));
			item->setDepth(last_depth + 1);
		}

		last_depth = item->getDepth();
	}
	QCB::get()->clear();
	jumpToLast();
	fixItemSize(item);
}

bool QTodoListIterator::isItem()
{
	if(dynamic_cast<QTodoItem*>(current()))
		return true;
	else
		return false;
}

QTodoItem* QTodoListIterator::item()
{
	if(isItem())
		return (dynamic_cast<QTodoItem*>(current()));
	else
		return 0;
}

void QTodoListIterator::take()
{
	if(QTodoItem* _item = item())
	{
		list->takeTodo(_item);
		operator --();
	}
}

void QTodoListIterator::remove()
{
	if(QTodoItem* _item = item())
	{
		list->removeTodo(_item);
		operator --();
	}
}

bool QTodoListIterator::isSection()
{
	if(dynamic_cast<QTodoSectionWidget*>(current()))
		return true;
	else
		return false;
}

unsigned int QTodoListIterator::currentIndex()
{
	return index;
}

QTodoItem* QTodoList::parentTodo(QTodoItem* item)
{
	int item_pos = findWidget(item);
	int item_depth = item->getDepth();
	if(item_depth == 0)
		return 0;

	QTodoListIterator it(this);
	for(it.goTo(item_pos-1); it.current(); --it)
	{
		if(dynamic_cast<QTodoItem*>(it.current()) && dynamic_cast<QTodoItem*>(it.current())->getDepth() < item_depth)
		{
			return dynamic_cast<QTodoItem*>(it.current());
		}
	}
	return 0;
}

void QTodoList::prepareList()
{
	QTodoListIterator it(this);
	for(;it.current();++it)
	{
		if(QTodoItem* item = dynamic_cast<QTodoItem*>(it.current()))
		{
			item->prepare();
		}
	}
}

QTodoListItemsSorter::QTodoListItemsSorter(QPtrList<QWidget>* _input,const QTodoSortCriteriaMap* sort_criterias)
{	//FIXME: not yet section ready
	//1. get sub lists
	//2. sort sub lists
	//3. connect to big list
	//4. return it

	input = _input;

	std::map<int,QTodoItemSortPtrList* > lists;

	QPtrListIterator<QWidget> it(*input);
	int sub_counter = 0;

	for(;it.current() || it.atFirst();++it)
	{
		if(QTodoItemSortPtrList* sub_list = getSubList(it))
		{
			lists[sub_counter] = sub_list;
			++sub_counter;
		}
	}

	for(std::map<int,QTodoItemSortPtrList* >::iterator it = lists.begin(); it != lists.end(); ++it)
	{
		(*it).second->setSortCriterias(sort_criterias);
		(*it).second->sort();
	}

	QPtrList<QWidget>* big_list = new QPtrList<QWidget>;
	for(std::map<int,QTodoItemSortPtrList* >::iterator it = lists.begin(); it != lists.end(); ++it)
	{
		if(!(*it).second->top_item)
		{
			QPtrListIterator<QTodoItem> sit(*(*it).second);
			for(; sit.current(); ++sit)
				big_list->append(sit.current());
		}
		else
		{
			int pos = big_list->find((*it).second->top_item)+1;
			QPtrListIterator<QTodoItem> sit(*(*it).second);
			sit.toLast();
			for(; sit.current(); --sit)
				big_list->insert(pos,sit.current());
		}
	}
	output = big_list;
}


QPtrList<QWidget>* QTodoListItemsSorter::get()
{
	return output;
}

QTodoItemSortPtrList* QTodoListItemsSorter::getSubList(QPtrListIterator<QWidget> it)
{
	if(!it.atFirst() && !dynamic_cast<QTodoItem*>(it.current()))
		return 0;

	QTodoItemSortPtrList* sub_list = new QTodoItemSortPtrList;

	QTodoItem* current = dynamic_cast<QTodoItem*>(it.current());

	int depth;
	if(it.atFirst())
	{
		++it;
		depth = dynamic_cast<QTodoItem*>(it.current())->getDepth();
		--it;
	}
	else
	{
		depth = current->getDepth()+1;
	}

	if(it.atFirst())
		sub_list->top_item = 0;
	else
		sub_list->top_item = current;

	++it;
	for(; it.current() && (current = dynamic_cast<QTodoItem*>(it.current())) && current->getDepth() >= depth; ++it)
	{
		if(current->getDepth() == depth)
			sub_list->append(current);
	}


	if(sub_list->count())
		return sub_list;
	else
		return 0;
}

int QTodoItemSortPtrList::compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
{
	QTodoItem* left = reinterpret_cast<QTodoItem*>(item1);
	QTodoItem* right = reinterpret_cast<QTodoItem*>(item2);

	if(!right)
		return 1;

	for(QTodoSortCriteriaMap::const_iterator it = sort_criterias->begin(); it != sort_criterias->end(); ++it)
	{
		int r;
		QTodoSortOrder order = (*it).second.second;
		switch((*it).second.first)
		{
		case SORT_PRIORITY:
			if((r = right->getPriority() - left->getPriority()) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		case SORT_STATUS:
			if((r = right->getStatus() - left->getStatus()) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		case SORT_DEADLINE:
			if((r = right->getDeadlineDateTime().toTime_t() - left->getDeadlineDateTime().toTime_t()) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		case SORT_ASSIGNED:
			if((r = right->getAgent().compare(left->getAgent())) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		case SORT_CREATED:
			if((r = right->getCreated().toTime_t() - left->getCreated().toTime_t()) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		case SORT_DONE:
			if((r = right->getDone().toTime_t() - left->getDone().toTime_t()) != 0)
				if(order == ORDER_ASCENDING)
					return -r;
				else
					return r;
			break;
		default:
			break;
		}
	}

	return 0;
}

void QTodoItemSortPtrList::setSortCriterias(const QTodoSortCriteriaMap* _sort_criterias)
{
	sort_criterias = _sort_criterias;
}

void QTodoList::navigateUp(QTodoItem* item, int i)
{
	int pos = findWidget(item);
	QTodoListIterator it(this);
	it.goTo(pos-1);
	for(;it.current(); --it)
		if(QTodoItem* current = dynamic_cast<QTodoItem*>(it.current()))
		{
			current->setCursorToEnd(i);
			ensureVisible(0,current->mapToParent(QPoint(0,0)).y(),0,0);
			return;
		}
}

void QTodoList::navigateDown(QTodoItem* item, int i)
{
	int pos = findWidget(item);
	QTodoListIterator it(this);
	it.goTo(pos+1);
	for(;it.current(); ++it)
		if(QTodoItem* current = dynamic_cast<QTodoItem*>(it.current()))
		{
			current->setCursorToBegin(i);
			ensureVisible(0,current->mapToParent(QPoint(0,current->height())).y(),0,0);
			return;
		}
}

void QTodoList::viewportResizeEvent(QResizeEvent* e)
{
	QScrollView::viewportResizeEvent(e);
}

void QTodoList::setModified()
{
	getTodoListWidget()->setModified();
}

void QTodoList::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == LeftButton)
		deselectAll();

	QScrollView::mousePressEvent(event);
}

void QTodoList::deselectAll()
{
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		if(QTodoItem* item = it.item())
			item->setSelected(false);
	}
}

void QTodoList::selectAll()
{
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		if(QTodoItem* item = it.item())
			item->setSelected(true);
	}
}

bool QTodoList::isItemSelected()
{
	QTodoListIterator it(this);
	for(; it.current(); ++it)
	{
		if(QTodoItem* item = it.item())
			if(item->isSelected())
				return true;
	}
	return false;
}

void QTodoList::preserveContentsYPos()
{
	preserved_contents_y_pos = contentsY();
}

void QTodoList::restoreContentsYPos()
{
	setContentsPos(0,preserved_contents_y_pos);
}

void QTodoList::jumpToFirst()
{
	QTodoListIterator it(this);
	if(QTodoItem* item = it.item())
	{
		item->setFocusOnTask();
		ensureVisible(0,item->mapToParent(QPoint(0,0)).y(),0,0);
	}
}

void QTodoList::jumpToLast()
{
	QTodoListIterator it(this);
	it.goTo(it.count()-1);
	if(QTodoItem* item = it.item())
		ensureItemVisible(item);
}

void QTodoList::ensureItemVisible(QTodoItem* item)
{
	item->adjustSize();
	item->setFocusOnTask();
	ensureVisible(0,item->mapToParent(QPoint(0,item->height())).y(),0,0);
}

void QTodoList::selectSubTodos(QTodoItem* item, bool b)
{
	std::vector<QTodoItem*> sub_todos = getSubTodos(item);
	for(std::vector<QTodoItem*>::iterator it = sub_todos.begin(); it != sub_todos.end(); ++it)
	{
		(*it)->setSelected(b);
	}
}







