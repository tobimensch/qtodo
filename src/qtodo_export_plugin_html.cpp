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

#include "qtodo_export_plugin_html.h"
#include "qtodo_misc.h"

QTodoExportPluginRegistrar* QTodoExportPluginHTML::registrar = new QTodoExportPluginRegistrar(&QTodoExportPluginHTML::get);
QTodoExportPluginBase* QTodoExportPluginHTML::instance = 0;

QTodoExportPluginHTML::QTodoExportPluginHTML() : QTodoExportPluginBase()
{
	setEncoding(QTextStream::UnicodeUTF8);
}

void QTodoExportPluginHTML::doExport()
{
	exportHead();
	exportBody();
}

void QTodoExportPluginHTML::exportHead()
{
	addLine("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">");
	addLine("<html>");
	addLine("<head>");
	addLine("<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">");
	addLine("<title>"+name()+"</title>");
	addLine("</head>");
	addLine("<body>");
	addLine("<h1>");
	addLine(QObject::tr("Todo-List:")+" "+name());
	addLine("</h1><hr><p>");
	if(!description().isEmpty())
	{
		addLine("<b>"+QObject::tr("Description: ")+"</b>");
		addLine(description()+" ");
	}
	addLine("<b>"+QObject::tr("Standing:")+"</b>");
	addLine(QTodoMisc::dateTime2LocalString(QDateTime::currentDateTime()));
	addLine("</p>");
}

void QTodoExportPluginHTML::exportBody( )
{
	for(; listIterator().current(); ++listIterator())
	{
		if(QTodoItem* item = listIterator().item())
			exportItem(*item);
	}	

	addLine("</body></html>");
}

void QTodoExportPluginHTML::exportItem(const QTodoItem& item)
{
	addLine("<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"margin-bottom:5px\">");
	addLine("<tr>");
	addLine("<td width=\""+QString::number(30*item.getDepth())+"\"></td>");
	addLine("<td>");
	addLine("<table cellspacing=\"0\" width=\"100%\">");
	addLine("<tr bgcolor=\"#eeeeee\">");
	addLine("<td>");
	addLine("<small>");
	//if(item.getPriority() != PRIO_NA)
		addLine("<b>"+QObject::tr("Priority:")+"</b> "+QTodo::priorityToString(item.getPriority()));
	//if(item.getStatus() != STAT_NA)
		addLine("<b>"+QObject::tr("Status:")+"</b> "+QTodo::statusToString(item.getStatus()));
	addLine("&nbsp;");
	if(item.getDeadlineDateTime().isValid())
	{
		addLine("</small>");
		addLine("</td><td align=\"right\"><small>");
		//addLine("<span align=\"right\">");
		addLine("<b>");
		addLine(QObject::tr("Deadline:")+"</b> "+item.getDeadlineLocalString());
		//addLine("</span>");
	}
	addLine("</small>");
	addLine("</td>");
	addLine("</tr><tr><td>");
	addLine(item.getTaskRichText().replace("\n","<br>"));
	addLine("<br><small><i>");
	addLine(QObject::tr("Created:")+" "+item.getCreatedLocalString());
	if(!item.getAgent().isEmpty())
		addLine(" "+QObject::tr("Assigned to:")+" "+item.getAgent());
	addLine("</i></small></td></tr></table></td></tr></table>");
}

QString QTodoExportPluginHTML::documentTypeName()
{
	return "HTML";
}

QString QTodoExportPluginHTML::pluginDescription()
{
	return QObject::tr("Save to hyper-text-markup-language");
}

QString QTodoExportPluginHTML::documentTypeExtension()
{
	return "html";
}

