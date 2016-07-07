/***************************************************************************
 *   Copyright (C) 2006 by Tobias Glaesser                                 *
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

#include "qtodo_export_plugin_plaintext.h"
#include "qtodo_misc.h"

QTodoExportPluginRegistrar* QTodoExportPluginPlainText::registrar = new QTodoExportPluginRegistrar(&QTodoExportPluginPlainText::get);
QTodoExportPluginBase* QTodoExportPluginPlainText::instance = 0;

QTodoExportPluginPlainText::QTodoExportPluginPlainText() : QTodoExportPluginBase()
{
}

void QTodoExportPluginPlainText::doExport()
{
	exportHead();
	exportBody();
}

void QTodoExportPluginPlainText::exportHead()
{
	addLine(QObject::tr("Todo-List:")+" "+name());
	QString hline;
	hline.fill('-',(QObject::tr("Todo-List:")+" "+name()).length());
	addLine(hline);
	addLine(description()+"\n");
}

void QTodoExportPluginPlainText::exportBody( )
{
	for(; listIterator().current(); ++listIterator())
	{
		if(QTodoItem* item = listIterator().item())
			exportItem(*item);
	}
}

void QTodoExportPluginPlainText::exportItem(const QTodoItem& item)
{
	addSpacing();
	addLine("- "+QObject::tr("Priority:")+" "+QTodo::priorityToString(item.getPriority())+"  "+QObject::tr("Status:")+" "+QTodo::statusToString(item.getStatus()));
	addSpacing();
	add("  ");
	add(QObject::tr("Created:")+" "+item.getCreatedLocalString());
	if(!item.getAgent().isEmpty())
		add("  "+QObject::tr("Assigned to:")+" "+item.getAgent());
	if(item.getDeadlineDateTime().isValid())
	{
		add("  ");
		add(QObject::tr("Deadline:") + " " + item.getDeadlineLocalString());
	}
	add("\n");
	addLine(addSpacingToText(item.getTaskPlainText()));
	QStringList links = item.getTaskLinks();
	if(!links.empty())
	{
		addSpacing();
		addLine(" " + QObject::tr("Links:"));
		for(QStringList::iterator it = links.begin(); it != links.end(); ++it)
		{
			add(addSpacingToText((*it) + " - "));
			++it;
			add((*it)+"\n");
			if(it == links.end())
				break;
		}
	}
	add("\n");
}

QString QTodoExportPluginPlainText::documentTypeName()
{
	return QObject::tr("Plain text");
}

QString QTodoExportPluginPlainText::pluginDescription()
{
	return QObject::tr("Save to plain text");
}

QString QTodoExportPluginPlainText::documentTypeExtension()
{
	return "txt";
}

void QTodoExportPluginPlainText::addSpacing()
{
	result += QString().fill(' ',listIterator().item()->getDepth()*2);
}

QString QTodoExportPluginPlainText::addSpacingToText(QString text)
{
	QString space = QString().fill(' ',listIterator().item()->getDepth()*2);
	text.insert(0,space+"   ");
	text.replace("\n","\n"+QString().fill(' ',listIterator().item()->getDepth()*2)+"   ");
	return text;
}

