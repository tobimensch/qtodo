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

#include <vector>
#include <qdatetime.h>

#ifndef QTODO_UNDO_MANAGER
#define QTODO_UNDO_MANAGER

class QTodoList;
class QTodoItem;

class QTodoChange
{
	public:
	QTodoChange(QTodoList* _list, int _pos) : list(_list), pos(_pos) {};
	virtual ~QTodoChange() {};
	virtual void undo() = 0;
	const QTodoList* getList() { return list; };

	protected:
	QTodoList* list;
	int pos;
};

class QTodoChangeInsert : public QTodoChange
{
	public:
	QTodoChangeInsert(QTodoList*,int);

	private:
	void undo();
};

class QTodoChangeDelete : public QTodoChange
{
	public:
	QTodoChangeDelete(QTodoList*, int, QTodoItem*);

	private:
	void undo();

	QTodoItem* item;
};

class QTodoChangePriority : public QTodoChange
{
	public:
	QTodoChangePriority(QTodoList*, int, int);

	private:
	void undo();

	int priority;
};

class QTodoChangeStatus : public QTodoChange
{
	public:
	QTodoChangeStatus(QTodoList*, int, int, const QDateTime&);

	private:
	void undo();

	int status;
	QDateTime done;
};

class QTodoChangeDeadline : public QTodoChange
{
	public:
	QTodoChangeDeadline(QTodoList*, int, const QDateTime&);

	private:
	void undo();

	QDateTime deadline;
};

class QTodoChangeDepth : public QTodoChange
{
	public:
	QTodoChangeDepth(QTodoList*, int, int);

	private:
	void undo();

	int depth;
	
};

class QTodoChangeSet
{
	public:
	~QTodoChangeSet();

	void addChange(QTodoChange*);
	void undo();
	void deleteChangesToList(QTodoList*);
	bool isEmpty();

	private:
	std::vector<QTodoChange*> changes;
};

class QTodoUndoManager
{
	public:
	static QTodoUndoManager* get() { _instance ? _instance : _instance = new QTodoUndoManager(); return _instance; }
	bool isUndoPossible();
	void undo();
	void addChange(QTodoChange*);
	void addSingleChange(QTodoChange*);
	void startRecording();
	void stopRecording();
	void setMaxUndos(unsigned int);
	void deleteUndosRelatedToList(QTodoList*);
	bool isRecording() { return recording; }

	private:
	QTodoUndoManager();
	std::vector<QTodoChangeSet*> change_sets;
	unsigned int max_undos;
	bool recording;

	static QTodoUndoManager* _instance;
};

typedef QTodoUndoManager QTUM;

#endif

