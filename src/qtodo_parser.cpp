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

#include <climits>

#include <qregexp.h>
#include <qobject.h>
#include <qpixmap.h>
 
#include "qtodo_parser.h"
#include "qtodo_xpm.h"

QTodoParser::QTodoParser()
{
	header = true; //the beginning is supposed to be a header
}

bool QTodoParser::isDelimiter()
{
	return ((*it).find(QRegExp("^[ |\t]*-")) != -1);
}

bool QTodoParser::isSection()
{
	if(isDelimiter())
		return false;
	
	++it;
	if(isDecoration())
	{
		--it;
		return true;
	}
	--it;
	/*if((*it).find(QRegExp(":[ |\t]*$")) != -1)
		return true;*/

	return false;
}

unsigned int QTodoParser::delimiterDepth()
{
	if(((*it).find(QRegExp("^[ |\t]*-"))) == 0) //FIXME: doubled code from isDelimiter()
	{
		cur_depth = ((*it).find("-"));
		return cur_depth;
	}
	return UINT_MAX-1;
}

void QTodoParser::getTodoInformation(QTodo* todo, const QString& line)
{
	QStringList infos = QStringList::split(':',line);
	QStringList keys;
	QStringList values;
	bool first = true;
	for(QStringList::Iterator it = infos.begin(); it != infos.end(); it++)
	{
		(*it) = (*it).stripWhiteSpace();

		if(first)
		{
			keys += (*it);
			first = false;
			continue;
		}

		QStringList tmp = QStringList::split(QRegExp("\\s"),(*it));
		QString next_key;
		if(tmp.fromLast() != tmp.end())
		{
			next_key = (*tmp.fromLast());
			tmp.erase(tmp.fromLast());
		}
		values += (tmp.join(" "));
		keys += next_key;

		QStringList::Iterator sit = it;
		sit++;
		if(sit == infos.end() && !next_key.isEmpty())
			(*values.fromLast()) += " " + next_key;
	}

	QStringList::Iterator key_it = keys.begin();
	for(QStringList::Iterator it = values.begin(); it != values.end(); it++, key_it++)
	{
		(*it) = (*it).stripWhiteSpace();
		if((*key_it) == "Priority")
			todo->priority = QTodo::nonLocalizedStringToPriority((*it));
		else if((*key_it) == "Created")
			todo->created = QDateTime::fromString((*it).replace("-",":").replace("\\","-"),Qt::ISODate);
		else if((*key_it) == "Done")
			todo->done = QDateTime::fromString((*it).replace("-",":").replace("\\","-"),Qt::ISODate);
		else if((*key_it) == "Deadline")
			todo->deadline = QDateTime::fromString((*it).replace("-",":").replace("\\","-"),Qt::ISODate);
		else if((*key_it) == "Status")
			todo->status = QTodo::nonLocalizedStringToStatus((*it));
		else if((*key_it) == "Agent")
			todo->agent = (*it);
	}
}

QTodo* QTodoParser::getTodo()
{
	QTodo* todo = new QTodo;
	for(int i = 0;it != raw_list.end(); ++it, ++i)
	{
		if((*it).stripWhiteSpace().isEmpty())
			continue;
		if(i > 0 && (isDelimiter() || isSection()))
		{
			--it;
			break;
		}
		if(!i) //==0 , the first line
		{
			getTodoInformation(todo,(*it).stripWhiteSpace().remove(0,1).stripWhiteSpace());
		}
		else
		{	
			todo->task += myStripWhiteSpace((*it)).replace("\"-\"","-");
			todo->task += "\n";
		}
	}
	if(todo->task.endsWith("\n"))
		todo->task.setLength(todo->task.length()-1);
	return todo;
}

void QTodoParser::setTodoList(QStringList _raw_list)
{
	raw_list = _raw_list;
}

bool QTodoParser::isDecoration()
{
	if((*it).find(QRegExp(".()*-{2,}")) != -1)
		return true;
	else
		return false;
}

bool QTodoParser::isHeader()
{
	if(header && (!isDelimiter() || isDecoration()))
	{
		return true;
	}
	else
	{
		header = false;
		return false;
	}
}

QTodoTopNode* QTodoParser::parse()
{
	QTodoTopNode* top_node = new QTodoTopNode;
	int i = 0;
	for(it = raw_list.begin(); it != raw_list.end() && isHeader(); ++it, ++i)
	{		
		if(!i) //first line
		{
			top_node->name += (*it).remove(" TODO");
		}
		else if(i > 1) //everything under the decoration
			top_node->description += (*it);
	}
	top_node->top = parse(delimiterDepth());
	return top_node;
}

QTodoNode* QTodoParser::parse(unsigned int depth)
{
	QTodoNode* todo_list = new QTodoNode;
	for(; it != raw_list.end(); ++it)
	{			
		if(isSection())
		{
			todo_list->push_back(getSection());
			continue;
		}
		if(isDelimiter() && delimiterDepth() == depth)
		{
			todo_list->push_back(getTodo());
		}
		else if(isDelimiter() && delimiterDepth() < depth)
		{
			--it; return todo_list;
		}
		else if(isDelimiter() && delimiterDepth() > depth)
		{
			(*todo_list->rbegin())->sub = parse(delimiterDepth());
		}
		if(it == raw_list.end())
			break;
	}
	return todo_list;
}

QTodo::QTodo()
{
	sub = 0;
	section = false;
}

QTodoTopNode::QTodoTopNode()
{
	top = 0;
}

QTodo* QTodoParser::getSection()
{
	QTodo* section = new QTodo;
	section->section = true;
	for(int i = 0;it != raw_list.end(); ++it, ++i)
	{
		if(i == 0)
		{
			section->task = (*it);
			if(section->task[section->task.length()-1] == ':')
				section->task.setLength(section->task.length()-1);
			continue;
		}

		if(!isDecoration())
			continue;
		else
			break;
	}
	return section;
}

QString QTodo::priorityToString(int prio)
{
	switch(prio)
	{
		case PRIO_NA:
			return QObject::tr("N/A");
		case PRIO_LOW:
			return QObject::tr("Low");
		case PRIO_NORMAL:
			return QObject::tr("Normal");
		case PRIO_HIGH:
			return QObject::tr("High");
		default:
			return QObject::tr("N/A");
	}
}

QString QTodo::priorityToNonLocalizedString(int prio)
{
	switch(prio)
	{
		case PRIO_NA:
			return "N/A";
		case PRIO_LOW:
			return "Low";
		case PRIO_NORMAL:
			return "Normal";
		case PRIO_HIGH:
			return "High";
		default:
			return "N/A";
	}
}

QString QTodo::statusToString(int stat)
{
	switch(stat)
	{
		case STAT_NA:
			return QObject::tr("N/A");
		case STAT_PLANNING:
			return QObject::tr("Planning");
		case STAT_IN_PROGRESS:
			return QObject::tr("In progress");
		case STAT_DONE:
			return QObject::tr("Done");
		default:
			return QObject::tr("N/A");
	}
}

QString QTodo::statusToNonLocalizedString(int stat)
{
	switch(stat)
	{
		case STAT_NA:
			return "N/A";
		case STAT_PLANNING:
			return "Planning";
		case STAT_IN_PROGRESS:
			return "In progress";
		case STAT_DONE:
			return "Done";
		default:
			return "N/A";
	}
}

Priority QTodo::stringToPriority(const QString& _string)
{
	for(int i = 0; i < PRIO_NUM; ++i)
	{
		if(priorityToString(i) == _string)
			return (Priority)i;
	}

	return PRIO_NA;
}

Priority QTodo::nonLocalizedStringToPriority(const QString& _string)
{
	for(int i = 0; i < PRIO_NUM; ++i)
	{
		if(priorityToNonLocalizedString(i) == _string)
			return (Priority)i;
	}

	return PRIO_NA;
}

Status QTodo::stringToStatus(const QString& _string)
{
	for(int i = 0; i < STAT_NUM; ++i)
	{
		if(statusToString(i) == _string)
			return (Status)i;
	}

	return STAT_NA;
}

Status QTodo::nonLocalizedStringToStatus(const QString& _string)
{
	for(int i = 0; i < STAT_NUM; ++i)
	{
		if(statusToNonLocalizedString(i) == _string)
			return (Status)i;
	}

	return STAT_NA;
}

QPixmap QTodo::priorityToPixmap(int prio,int width, int height)
{
	switch(prio)
	{
		case PRIO_NA:
			return QTP::not_available.s(width,height);
		case PRIO_LOW:
			return QTP::low.s(width,height);
		case PRIO_NORMAL:
			return QTP::normal.s(width,height);
		case PRIO_HIGH:
			return QTP::high.s(width,height);
		default:
			return QTP::not_available.s(width,height);
	}
}

QPixmap QTodo::statusToPixmap(int stat,int width, int height)
{
	switch(stat)
	{
		case STAT_NA:
			return QTP::not_available.s(width,height);
		case STAT_PLANNING:
			return QTP::planning.s(width,height);
		case STAT_IN_PROGRESS:
			return QTP::in_progress.s(width,height);
		case STAT_DONE:
			return QTP::done.s(width,height);
		default:
			return QTP::not_available.s(width,height);
	}
}

QString QTodoParser::myStripWhiteSpace(const QString& in)
{
	QString out = in;
	out.remove(0,cur_depth);
	for(int i = out.length(); i > 0; --i)
	{
		if(out[i-1].isSpace())
			out.setLength(i-1);
		else
			return out;
	}
	return out;
}

