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

#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qfile.h>
#include <qdir.h>
#include <qfiledialog.h>

#include "qtodo_exporter.h"
#include "qtodo_export_plugin_base.h"
#include "qtodo_xpm.h"
#include "qtodo_list_widget.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_push_button.h"
#include "qtodo_misc.h"

QTodoExporter::QTodoExporter(QTodoListWidget* _list) : list(_list)
{
	setCaption("Exporter");
	setIcon(QTP::export_list);

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(3);
	grid->setSpacing(2);

	document_combo = new QComboBox(this);
	document_combo->insertItem(tr("Please select a document type"));

	connect(document_combo,SIGNAL(activated(const QString&)),this,SLOT(documentTypeChanged(const QString&)));

	QTodoExportPlugins* plugins = QTodoExportPlugins::get();
	QTodoExportPluginBase* plugin = plugins->first();
	for(; plugin; plugin = plugins->next())
		document_combo->insertItem(plugin->documentTypeName());

	filename = new QLineEdit(this);
	filename->setText(list->getHeader()->getName().lower().simplifyWhiteSpace());
	directory = new QLineEdit(this);
	directory->setText(QDir::homeDirPath());

	QTodoPushButton* browse_directory = new QTodoPushButton(this);
	browse_directory->setPixmap(QTP::folder.s(16,16));

	connect(browse_directory,SIGNAL(clicked()),this,SLOT(browseDirectory()));

	QHBox* hbox = new QHBox(this);
	export_button = new QPushButton(tr("Export"),hbox);
	export_button->setEnabled(false);
	QPushButton* cancel_button = new QPushButton(tr("Cancel"),hbox);
	
	connect(export_button,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel_button,SIGNAL(clicked()),this,SLOT(reject()));

	QFrame* hline = new QFrame(this);
	hline->setFrameStyle(QFrame::HLine | QFrame::Raised);	

	grid->addMultiCellWidget(new QLabel(tr("You are about to export the")+" "+list->getHeader()->getName()+tr("-list"),this),0,0,0,2);
	grid->addMultiCellWidget(hline,1,1,0,2);
	grid->addWidget(new QLabel(tr("Type:"),this),2,0);
	grid->addMultiCellWidget(document_combo,2,2,1,2);
	grid->addWidget(new QLabel(tr("Filename:"),this),3,0);
	grid->addMultiCellWidget(filename,3,3,1,2);
	grid->addWidget(new QLabel(tr("Directory:"),this),4,0);
	grid->addWidget(directory,4,1);
	grid->addWidget(browse_directory,4,2);
	grid->addMultiCellWidget(hbox,5,5,0,2);
}

void QTodoExporter::accept()
{
	QTodoExportPluginBase* plugin = getExportPlugin();
	if(!plugin)
	{
		QDialog::accept();
		return;
	}

	plugin->setTodoListWidget(list);
	plugin->doExport();

	QFile file(directory->text()+"/"+filename->text());
	if(file.open(IO_WriteOnly))
	{
		QTextStream stream(&file);
		stream.setEncoding(plugin->getEncoding());
		stream <<plugin->getString();
		file.close();
	}

	QDialog::accept();
}

void QTodoExporter::browseDirectory()
{ 
	directory->setText(QTodoMisc::browseDirectory(directory->text()));
}

void QTodoExporter::documentTypeChanged(const QString&)
{
	QTodoExportPluginBase* plugin;
	if(!(plugin = getExportPlugin()))
	{
		export_button->setEnabled(false);
		removeExtensionFromFilename();
		return;
	}
	export_button->setEnabled(true);

	if(!filename->text().endsWith("."+plugin->documentTypeExtension()))
	{
		removeExtensionFromFilename();
		filename->setText(filename->text()+"."+plugin->documentTypeExtension());
	}
}

QTodoExportPluginBase* QTodoExporter::getExportPlugin()
{
	QTodoExportPlugins* plugins = QTodoExportPlugins::get();
	QTodoExportPluginBase* plugin = plugins->first();
	for(; plugin; plugin = plugins->next())
	{
		if(document_combo->currentText() == plugin->documentTypeName())
			break;
	}
	return plugin;
}

void QTodoExporter::removeExtensionFromFilename()
{
		int pos;
		if((pos = filename->text().findRev(".")) != -1)
		{
			QString tmp = filename->text();
			tmp.setLength(pos);
			filename->setText(tmp);
		}
}

