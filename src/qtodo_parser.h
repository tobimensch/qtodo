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
#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>
 
#ifndef QTODO_PARSER
#define QTODO_PARSER

struct QTodo;
typedef std::vector<QTodo*> QTodoNode;

enum
{
	ANY = -2
};

enum Priority
{
	PRIO_NA,
	PRIO_LOW,
	PRIO_NORMAL,
	PRIO_HIGH,
	PRIO_NUM,
	PRIO_NOT_NA,
	PRIO_NOT_LOW,
	PRIO_NOT_NORMAL,
	PRIO_NOT_HIGH
};

enum  Status
{
	STAT_NA,
	STAT_PLANNING,
	STAT_IN_PROGRESS,
	STAT_DONE,
	STAT_NUM,
	STAT_NOT_NA,
	STAT_NOT_PLANNING,
	STAT_NOT_IN_PROGRESS,
	STAT_NOT_DONE
};

///The top node of a TODOlist
///It contains the properties for the whole list
///and the first node (TODOitem).
struct QTodoTopNode
{
	QTodoTopNode();
	QString name;
	QString description;
		
	QTodoNode* top;
};

///TODOnode, contains TODOdata
///and a link to subnodes, if
///existing
struct QTodo
{
	QTodo();

	Priority priority;
	Status status;
	QString task;
	QDateTime created;
	QDateTime done;
	QDateTime deadline;
	QString agent;
	
	QTodoNode* sub;
	
	bool section;

	static QString priorityToString(int);
	static QString priorityToNonLocalizedString(int);
	static QString statusToString(int);
	static QString statusToNonLocalizedString(int);
	static Priority stringToPriority(const QString&);
	static Priority nonLocalizedStringToPriority(const QString&);
	static Status stringToStatus(const QString&);
	static Status nonLocalizedStringToStatus(const QString&);
	static QPixmap priorityToPixmap(int,int width=16, int height=16);
	static QPixmap statusToPixmap(int,int width=16, int height=16);
};

///Parses TODOlist files and returns
///the data via the top node.
class QTodoParser
{
public:
	QTodoParser();
	void setTodoList(QStringList);
	QTodoTopNode* parse();
	
private:
	QTodoNode* parse(unsigned int);
	QString myStripWhiteSpace(const QString&);
	bool isHeader();
	bool isDelimiter();
	bool isDecoration();
	bool isSection();
	unsigned int delimiterDepth();
	QTodo* getTodo();
	QTodo* getSection();
	void getTodoInformation(QTodo*,const QString&);
	
	QTodo* parseTodo(QString&);
	
	QStringList raw_list;
	QStringList::Iterator it;
	bool header;
	unsigned int cur_depth;
};

#endif

