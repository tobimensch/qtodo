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

#include "qtodo_list.h"
#include "qtodo_item.h"

#ifndef QTODO_EXPORT_PLUGIN_BASE
#define QTODO_EXPORT_PLUGIN_BASE

class QTodoListWidget;
class QTodoExportPluginBase;

typedef QTodoExportPluginBase* (*fptr)();

struct QTodoExportPluginRegistrar
{
	QTodoExportPluginRegistrar(fptr);
};

class QTodoExportPlugins
{
	public:
	static QTodoExportPlugins* get() { if(instance) { return instance; } else { instance = new QTodoExportPlugins(); return instance; } ; };
	QTodoExportPluginBase* first();
	QTodoExportPluginBase* next();

	private:
	QTodoExportPlugins() {};
	
	unsigned int current;

	static QTodoExportPlugins* instance;
	std::vector<fptr> plugin_get_functions;

	public:
	bool add(fptr);
};

class QTodoExportPluginBase
{
	public:
	virtual QString documentTypeName() = 0;
	virtual QString documentTypeExtension() = 0;
	virtual QString pluginDescription() = 0;
	void setTodoListWidget(QTodoListWidget* _list_widget);

	virtual void doExport() = 0;
	const QString& getString();
	QTextStream::Encoding getEncoding() { return encoding; }
	void setEncoding(QTextStream::Encoding _encoding) { encoding = _encoding; }
	void save(QTextStream&);

	protected:
	QTodoExportPluginBase() { encoding = QTextStream::Locale; }
	virtual ~QTodoExportPluginBase() {};

	QTodoListIterator& listIterator();
	virtual void exportHead() = 0;
	virtual void exportBody() = 0;
	virtual void exportItem(const QTodoItem&) = 0;
	void addLine(const QString&);
	void add(const QString&);
	QString name();
	QString description();

	QString result;

	private:
	QTodoListWidget* list_widget;
	QTodoListIterator* list_iterator;
	QTextStream::Encoding encoding;
};

#endif

