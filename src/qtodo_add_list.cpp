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

#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include "qtodo_add_list.h"
#include "qtodo_push_button.h"
#include "qtodo_misc.h"

QTodoAddList::QTodoAddList(QWidget *parent, const char *name) :QTodoAddListBase(parent, name)
{
	/* Hack: */
	substitutePushButton(new_button);
	buttonGroup1Layout->addWidget(new_button,0,0);
	connect(new_button,SIGNAL(toggled(bool)),groupBox1,SLOT(setEnabled(bool)));
	substitutePushButton(existing_button);
	buttonGroup1Layout->addWidget(existing_button,2,0);
	connect(existing_button,SIGNAL(toggled(bool)),groupBox2,SLOT(setEnabled(bool)));
	/* Hack end */

	connect(browse_file,SIGNAL(clicked()),this,SLOT(browseFile()));
	connect(browse_directory,SIGNAL(clicked()),this,SLOT(browseDirectory()));
}

void QTodoAddList::browseFile()
{
	file->setText(QTodoMisc::browseFile(file->text()));
}

void QTodoAddList::browseDirectory()
{
	directory->setText(QTodoMisc::browseDirectory(directory->text()));
}

void QTodoAddList::substitutePushButton(QPushButton*& pointer_new)
{
	/*HACK:*/
	QPushButton* old = pointer_new;
	QIconSet iconset = *(old->iconSet());
	QWidget* p = dynamic_cast<QWidget*>(old->parent());
	pointer_new = new QTodoPushButton(p,old->name());
	pointer_new->setIconSet(iconset);
	pointer_new->setText(old->text());
	pointer_new->setToggleButton(TRUE);

	delete old;
	/*HACK end*/	
}

