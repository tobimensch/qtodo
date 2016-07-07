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

#ifndef QTODO_EXPORT_PLUGIN_HTML
#define QTODO_EXPORT_PLUGIN_HTML

class QTodoExportPluginHTML : QTodoExportPluginBase
{
	public:
	void doExport();
	QString documentTypeName();
	QString documentTypeExtension();
	QString pluginDescription();
	static QTodoExportPluginBase* get() { if(!instance) { return instance = new QTodoExportPluginHTML(); } else { return instance; } };

	static QTodoExportPluginRegistrar* registrar;
	private:
	void exportHead();
	void exportBody();
	void exportItem(const QTodoItem&);

	QTodoExportPluginHTML();

	static QTodoExportPluginBase* instance;
};

#endif
