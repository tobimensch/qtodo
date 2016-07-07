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

#include "qtodo_undo_manager.h"
#include "qtodo_item.h"
#include "qtodo_list.h"
#include "qtodo_settings_container.h"

QTodoUndoManager* QTodoUndoManager::_instance = NULL;

QTodoUndoManager::QTodoUndoManager()
{
	max_undos = QSC::get()->undo_steps;
	recording = false;
}

bool QTodoUndoManager::isUndoPossible()
{
	return !change_sets.empty();
}

void QTodoUndoManager::setMaxUndos(unsigned int i)
{
	max_undos = i;
}

void QTodoUndoManager::startRecording()
{
	recording = true;

	if(max_undos && change_sets.size() >= max_undos)
		change_sets.erase(change_sets.begin());

	change_sets.push_back(new QTodoChangeSet());
}

void QTodoUndoManager::stopRecording()
{
	if((*change_sets.rbegin())->isEmpty())
	{
		delete (*change_sets.rbegin());
		change_sets.pop_back();
	}

	recording = false;
}

void QTodoUndoManager::undo()
{
	if(!isUndoPossible())
		return;

	(*change_sets.rbegin())->undo();
	change_sets.pop_back();
}

void QTodoUndoManager::addChange(QTodoChange* change)
{
	if(!recording)
	{
		delete change;
		return;
	}

	(*change_sets.rbegin())->addChange(change);
}

void QTodoUndoManager::addSingleChange(QTodoChange* change)
{
	startRecording();
	addChange(change);
	stopRecording();
}

void QTodoChangeSet::addChange(QTodoChange* change)
{
	changes.push_back(change);
}

void QTodoChangeSet::undo()
{
	for(std::vector<QTodoChange*>::reverse_iterator it = changes.rbegin(); it != changes.rend(); ++it)
		(*it)->undo();
}

QTodoChangeInsert::QTodoChangeInsert(QTodoList* _list, int _pos) : QTodoChange(_list,_pos)
{}

void QTodoChangeInsert::undo()
{
	QTodoListIterator it(list);
	it.goTo(pos);
	if(QTodoItem* item = it.item())
		list->removeTodo(item);
}

QTodoChangeDelete::QTodoChangeDelete(QTodoList* _list, int _pos,QTodoItem* _item) : QTodoChange(_list, _pos), item(_item)
{}

void QTodoChangeDelete::undo()
{
	list->insertTodo(item,pos);
	item->setDepth(item->getDepth()); //fix
}

QTodoChangePriority::QTodoChangePriority(QTodoList* _list, int _pos,int _priority) : QTodoChange(_list, _pos), priority(_priority)
{}

void QTodoChangePriority::undo()
{
	QTodoListIterator it(list);
	it.goTo(pos);
	if(QTodoItem* item = it.item())
		item->setPriority(priority);
}

QTodoChangeStatus::QTodoChangeStatus(QTodoList* _list, int _pos,int _status, const QDateTime& _done) : QTodoChange(_list, _pos), status(_status), done(_done)
{}

void QTodoChangeStatus::undo()
{
	QTodoListIterator it(list);
	it.goTo(pos);
	if(QTodoItem* item = it.item())
	{
		item->setStatus(status);
		item->setDone(done);
	}
}

QTodoChangeDeadline::QTodoChangeDeadline(QTodoList* _list, int _pos, const QDateTime& _deadline) : QTodoChange(_list, _pos), deadline(_deadline)
{}

void QTodoChangeDeadline::undo()
{
	QTodoListIterator it(list);
	it.goTo(pos);
	if(QTodoItem* item = it.item())
	{
		item->setDeadline(deadline);
		item->fixFocusDeadlineProblem();
	}
}

void QTodoUndoManager::deleteUndosRelatedToList(QTodoList* list)
{
	for(std::vector<QTodoChangeSet*>::iterator it = change_sets.begin(); it != change_sets.end(); ++it)
	{
		(*it)->deleteChangesToList(list);
		if((*it)->isEmpty())
		{
			delete (*it);
			it = change_sets.erase(it);
			--it;
		}	
	}
}

void QTodoChangeSet::deleteChangesToList(QTodoList* list)
{
	for(std::vector<QTodoChange*>::iterator it = changes.begin(); it != changes.end(); ++it)
	{
		if((*it)->getList() == list)
		{
			delete (*it);
			it = changes.erase(it);
			--it;
		}
	}
}

QTodoChangeSet::~QTodoChangeSet()
{
	for(std::vector<QTodoChange*>::iterator it = changes.begin(); it != changes.end(); ++it)
	{
		delete (*it); //seems to be the right place, right?
	}
}

bool QTodoChangeSet::isEmpty()
{
	return changes.empty();
}

QTodoChangeDepth::QTodoChangeDepth(QTodoList* _list, int _pos, int _depth) : QTodoChange(_list,_pos), depth(_depth)
{
}

void QTodoChangeDepth::undo()
{
	QTodoListIterator it(list);
	it.goTo(pos);
	if(it.isItem())
		dynamic_cast<QTodoItem*>(it.current())->setDepth(depth);
}






