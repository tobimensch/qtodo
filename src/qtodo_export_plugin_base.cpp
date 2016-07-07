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

#include "qtodo_export_plugin_base.h"
#include "qtodo_list_widget.h"
#include "qtodo_list_header_widget.h"

QTodoExportPlugins* QTodoExportPlugins::instance = 0;

QTodoExportPluginRegistrar::QTodoExportPluginRegistrar(fptr p)
{
	QTodoExportPlugins::get()->add(p);
}

const QString& QTodoExportPluginBase::getString()
{
	return result;
}

void QTodoExportPluginBase::save(QTextStream&)
{
}

void QTodoExportPluginBase::addLine(const QString& str)
{
	result += str+"\n";
}

void QTodoExportPluginBase::add(const QString& str)
{
	result += str;
}

QString QTodoExportPluginBase::name()
{
	return list_widget->getHeader()->getName();
}

QString QTodoExportPluginBase::description()
{
	return list_widget->getHeader()->getDescription();
}

bool QTodoExportPlugins::add(fptr p)
{
	plugin_get_functions.push_back(p);
	return true;
}

QTodoExportPluginBase* QTodoExportPlugins::first()
{
	current = 0;
	if(plugin_get_functions.size() > current)
		return plugin_get_functions[0]();
	else
		return 0;
}

QTodoExportPluginBase* QTodoExportPlugins::next()
{
	++current;
	if(plugin_get_functions.size() > current)
		return plugin_get_functions[current]();
	else
		return 0;
}

void QTodoExportPluginBase::setTodoListWidget(QTodoListWidget* _list_widget)
{
	list_widget = _list_widget;
	list_iterator = new QTodoListIterator(list_widget->getList());
	result.setLength(0);
}

QTodoListIterator& QTodoExportPluginBase::listIterator()
{
	return *list_iterator;
}







