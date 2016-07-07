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
#include <qlabel.h>
#include <qlineedit.h>
#include <qhbox.h>

#include "qtodo_link_dialog.h"
#include "qtodo_push_button.h"
#include "qtodo_xpm.h"
#include "qtodo_misc.h"

QTodoLinkDialog::QTodoLinkDialog()
{
	setCaption("Link");
	setIcon(QTP::hyperlink);

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(3);
	grid->setSpacing(2);

	QHBox* hbox = new QHBox(this);

	QPushButton* ok_button = new QPushButton(tr("Ok"),hbox);
	QPushButton* cancel_button = new QPushButton(tr("Cancel"),hbox);

	connect(ok_button,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel_button,SIGNAL(clicked()),this,SLOT(reject()));
	
	text = new QLineEdit(this);
	url = new QLineEdit(this);
	QPushButton* get_file = new QTodoPushButton(this);
	get_file->setPixmap(QTP::filefind.s(16,16));
	QPushButton* get_directory = new QTodoPushButton(this);
	get_directory->setPixmap(QTP::folder.s(16,16));

	connect(get_directory,SIGNAL(clicked()),this,SLOT(browseForDirectory()));
	connect(get_file,SIGNAL(clicked()),this,SLOT(browseForFile()));

	QFrame* hline = new QFrame(this);
	hline->setFrameStyle(QFrame::HLine | QFrame::Raised);	
	grid->addMultiCellWidget(new QLabel(tr("You can set a link to a file or any other resource here"),this),0,0,0,3);
	grid->addMultiCellWidget(hline,1,1,0,3);
	grid->addWidget(new QLabel(tr("Text:"),this),2,0);
	grid->addMultiCellWidget(text,2,2,1,3);
	grid->addWidget(new QLabel(tr("URL:"),this),3,0);
	grid->addWidget(url,3,1);
	grid->addWidget(get_file,3,2);
	grid->addWidget(get_directory,3,3);
	grid->addMultiCellWidget(hbox,4,4,0,3);
}

void QTodoLinkDialog::setText(const QString& _str)
{
	text->setText(_str);
}

void QTodoLinkDialog::setURL(const QString& _str)
{
	url->setText(_str);
}

QString QTodoLinkDialog::getText()
{
	return text->text();
}

QString QTodoLinkDialog::getURL()
{
	return url->text();
}

void QTodoLinkDialog::browseForFile()
{
	url->setText(QTodoMisc::browseFile(url->text()));
}

void QTodoLinkDialog::browseForDirectory()
{
	url->setText(QTodoMisc::browseDirectory(url->text()));
}

