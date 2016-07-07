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

#include <qdialog.h>

#ifndef QTODO_EXPORTER
#define QTODO_EXPORTER

class QTodoListWidget;
class QTodoExportPluginBase;
class QLineEdit;
class QComboBox;
class QPushButton;

class QTodoExporter : public QDialog
{
	Q_OBJECT

	public:
	QTodoExporter(QTodoListWidget*);

	private slots:
	void accept();
	void browseDirectory();
	void documentTypeChanged(const QString&);
	
	private:
	QTodoExportPluginBase* getExportPlugin();
	void removeExtensionFromFilename();

	QPushButton* export_button;
	QTodoListWidget* list;
	QLineEdit* filename;
	QLineEdit* directory;
	QComboBox* document_combo;
};

#endif

