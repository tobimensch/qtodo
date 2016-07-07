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

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qfile.h>
#include <qapplication.h>
#include <qdir.h>

#include "qtodo_list_widget.h"
#include "qtodo_list.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_list_filter.h"
#include "qtodo_widget.h"
#include "qtodo_misc.h"
#include "qtodo_settings_container.h"

QTodoListWidget::QTodoListWidget(QTodoListViewItem* _lists_item) : lists_item(_lists_item)
{
	QGridLayout* grid = new QGridLayout(this);
	
	header = new QTodoListHeaderWidget(this);
	list = new QTodoList(this);
	filter = new QTodoListFilter(this);

	grid->addWidget(header,0,0);
	grid->addWidget(list,1,0);
	grid->addWidget(filter,2,0);

	m_modified = false;
}

void QTodoListWidget::addTodo()
{
	list->addTodo();
}

QTodoList * QTodoListWidget::getList()
{
	return list;
}

void QTodoListWidget::setFile(const QString& _file)
{
	file = _file;
}

bool QTodoListWidget::load()
{
	QFile qfile(file);
	if(!qfile.exists())
		return false;

	QStringList lines;
	if(qfile.open(IO_ReadOnly))
	{
		QTextStream stream( &qfile );
		while (!stream.atEnd())
			lines += stream.readLine(); // line of text excluding '\n'
		qfile.close();
	}
	else
		return false;
	QTodoParser parser;
	parser.setTodoList(lines);
	QTodoTopNode* top_node = parser.parse();
	list->load(top_node);
	header->load(top_node);

	return true;
}

void QTodoListWidget::save()
{
	if(!m_modified)
		return;
	if(QSC::get()->backups)
		createBackup();

	QFile qfile( file );
	if ( qfile.open(IO_WriteOnly) )
	{
		QTextStream stream( &qfile );
		
		header->save(stream);
		list->save(stream);

		qfile.close();
	}

	m_modified = false;
}

void QTodoListWidget::createBackup()
{
	QDir dir;
	dir.rename(file,file+'~',true);
}

QString QTodoListWidget::getFile()
{
	return file;
}

QTodoListWidget::~ QTodoListWidget( )
{
	save();
}

void QTodoListWidget::addSection()
{
	list->addSection();
}

QTodoListHeaderWidget * QTodoListWidget::getHeader()
{
	return header;
}

QTodoListViewItem * QTodoListWidget::getListViewItem()
{
	return lists_item;
}

QTodoListFilter* QTodoListWidget::getListFilter()
{
	return filter;
}

void QTodoListWidget::configure()
{
	header->propertiesDlg();
}

void QTodoListWidget::setModified()
{
	m_modified = true;
	emit modified();
}

bool QTodoListWidget::isModified()
{
	return m_modified;
}





