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

#ifndef QTODO_CLIPBOARD
#define QTODO_CLIPBOARD

class QTodoItem;
class QTodoItem;

typedef std::vector<QTodoItem*> QTodoItemVector;

class QTodoClipboard
{
	public:
	~QTodoClipboard();
	static QTodoClipboard* get() { _instance ? _instance : _instance = new QTodoClipboard(); return _instance; }
	bool isEmpty();
	void clear();
	void add(QTodoItem*);
	void add(std::vector<QTodoItem*>);
	QTodoItemVector* getItems();
	void setCut(bool);
	void doExport();
	int maxDepth();
	int minDepth();

	private:
	QTodoClipboard();
	static QTodoClipboard* _instance;
	bool cut;

	QTodoItemVector items;
};

typedef QTodoClipboard QCB;

#endif

