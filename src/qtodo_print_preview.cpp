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

#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qcheckbox.h>

#include "qtodo_print_preview.h"
#include "qtodo_list_widget.h"
#include "qtodo_list.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_item.h"
#include "qtodo_parser.h"
#include "qtodo_xpm.h"
#include "qtodo_settings_container.h"
#include "qtodo_lists.h"
#include "qtodo_widget.h"
#include "qtodo_list_view_item.h"

QTodoPrintPreview::QTodoPrintPreview(QTodoListWidget* _list_widget)
{
	setCaption(tr("Print Todo-List:") + " " + _list_widget->getHeader()->getName());
	list_widget = _list_widget;
	init();
}

QTodoPrintPreview::QTodoPrintPreview()
{
	setCaption(tr("Print deadlines"));
	list_widget = 0;
	init();
	use_filter_checkbox->hide();
}

void QTodoPrintPreview::init()
{
	setIcon(QTP::print.s(16,16));
	setGeometry(QApplication::desktop()->width()/2 - 300,QApplication::desktop()->height()/2 - 300,600,600);

	QGridLayout* grid = new QGridLayout(this);
	grid->setMargin(2);
	grid->setSpacing(2);

	textbrowser = new QTextBrowser(this);
	textbrowser->setMimeSourceFactory(new QTodoMimeSourceFactory());

	connect(textbrowser,SIGNAL(sourceChanged(const QString&)),this,SLOT(avoidSourceChange(const QString&)));

	QPushButton* print_button = new QPushButton(QTP::print.pixmap(),tr("Print"),this);
	QPushButton* close_button = new QPushButton(QTP::close.pixmap(),tr("Close"),this);
	show_icons_checkbox = new QCheckBox(tr("Show icons"),this);
	use_filter_checkbox = new QCheckBox(tr("Use filter"),this);
	show_icons_checkbox->setChecked(QSC::get()->print_icons);
	use_filter_checkbox->setChecked(QSC::get()->print_with_filter);

	connect(print_button,SIGNAL(clicked()),this,SLOT(print()));
	connect(close_button,SIGNAL(clicked()),this,SLOT(close()));
	connect(show_icons_checkbox,SIGNAL(clicked()),this,SLOT(updateView()));
	connect(use_filter_checkbox,SIGNAL(clicked()),this,SLOT(updateView()));

	QFontMetrics font_metrics(font());
	max_prio_width = 0;
	for(int i = 0; i < PRIO_NUM; ++i)
	{
		int t;
		if((t = font_metrics.width(QTodo::priorityToString(i))) > max_prio_width)
			max_prio_width = t;
	}

	max_stat_width = 0;
	for(int i = 0; i < STAT_NUM; ++i)
	{
		int t;
		if((t = font_metrics.width(QTodo::statusToString(i))) > max_stat_width)
			max_stat_width = t;
	}

	updateView();

	grid->addWidget(print_button,0,0);
	grid->addWidget(show_icons_checkbox,0,1);
	grid->addWidget(use_filter_checkbox,0,2);
	grid->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding),0,3);
	grid->addWidget(close_button,0,4);
	grid->addMultiCellWidget(textbrowser,1,1,0,4);
}

QTodoPrintPreview::~QTodoPrintPreview()
{
	QSC::get()->print_icons = show_icons_checkbox->isChecked();
	QSC::get()->print_with_filter = use_filter_checkbox->isChecked();
	QSC::get()->save();
}

void QTodoPrintPreview::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    printer.setFullPage(true);
    if ( printer.setup(this) ) {
	QPainter p( &printer );
	QPaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body(margin*dpix/(72*2), margin*dpiy/(72*2),
		   metrics.width()-margin*dpix/72,
		   metrics.height()-margin*dpiy/72 );
	QFont font("times", 10);
	QSimpleRichText richText( textbrowser->text(), font, textbrowser->context(), textbrowser->styleSheet(),
				  textbrowser->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    p.setClipRect( body );
	    richText.draw( &p, body.left(), body.top(), view, textbrowser->colorGroup() );
	    p.setClipping( FALSE );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
		QString date = QDateTime::currentDateTime().toString();
	    p.drawText( view.right() - p.fontMetrics().width( date ),
			view.top() - p.fontMetrics().ascent() + 5, date );
		QString page_number = tr("Page") + " " + QString::number(page);
	    p.drawText( view.right() - p.fontMetrics().width( page_number ),
			view.bottom() + p.fontMetrics().ascent() + 5, page_number );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
}

QString QTodoPrintPreview::todoListHead()
{
	QString text;
	text += "<u><h2>"+tr("Todo-List:")+" "+list_widget->getHeader()->getName()+"</h2></u>";
	if(!list_widget->getHeader()->getDescription().isEmpty())
		text += "<b>"+tr("Description:")+"</b> " + list_widget->getHeader()->getDescription();
	text += "<hr>";
	return text;
}

QString QTodoPrintPreview::deadlinesHead()
{
	QString text;
	text += "<u><h2>"+tr("Deadlines")+"</h2></u>";
	text += "<hr>";
	return text;
}

QString QTodoPrintPreview::todoItemRichText(QTodoItem* todo_item)
{
	QFontMetrics font_metrics(font());
	QString text;
	text += "<table cellspacing=0 cellpadding=0><tr>";
	if(todo_item->getDepth())
		text += "<td width=\""+QString::number(todo_item->getDepth()*5)+"%\"></td>";
	text += "<td width=\""+QString::number(100-todo_item->getDepth()*5)+"%\">";
	text += "<table cellspacing=0 cellpadding=0 width=100%><tr bgcolor='#dddddd'>";
	text += "<td width=100%>";
	text += "<nobr><b>"+tr("Priority:")+"</b> ";
	if(show_icons_checkbox->isChecked())
		text += "<img src=\"p"+QString::number(todo_item->getPriority())+"\"> ";
	text += QTodo::priorityToString(todo_item->getPriority());
	if(font_metrics.width(QTodo::priorityToString(todo_item->getPriority())) < max_prio_width)
	{
		text += "<img height=\"1\" width=\""+QString::number(max_prio_width-font_metrics.width(QTodo::priorityToString(todo_item->getPriority())))+"\" src=\"t\"> ";
	}
	text += " <b>"+tr("Status:")+"</b> ";
	if(show_icons_checkbox->isChecked())
		text += "<img src=\"s"+QString::number(todo_item->getStatus())+"\"> ";
	text += QTodo::statusToString(todo_item->getStatus());
	if(font_metrics.width(QTodo::statusToString(todo_item->getStatus())) < max_stat_width)
	{
		text += "<img height=\"1\" width=\""+QString::number(max_stat_width-font_metrics.width(QTodo::statusToString(todo_item->getStatus())))+"\" src=\"t\"> ";
	}
	if(!todo_item->getDeadlineString().isEmpty())
	{
		text += " <b>"+tr("Deadline:")+"</b> ";
		text += todo_item->getDeadlineString();
	}
	text += "</tr><tr><td width=100%>";
	text += todo_item->getTaskRichText().replace("\n","<br>");
	if(!todo_item->getAgent().isEmpty())
	{
		text += "<br><i>" + tr("Assigned to:") + " </i>" + todo_item->getAgent();
	}
	text += "</td></tr></table>";
	text += "</td></tr></table>";
	return text;
}

void QTodoPrintPreview::updateView()
{
	QString text;
	if(list_widget)
		text += todoListHead();
	else
		text += deadlinesHead();

	if(list_widget)
	{
		QTodoListIterator it(list_widget->getList());
		for(;it.current();++it)
		{
			if(QTodoItem* todo_item = dynamic_cast<QTodoItem*>(it.current()))
			{
				if(use_filter_checkbox->isChecked() && todo_item->isHidden())
					continue;
				
				text += todoItemRichText(todo_item);
			}
		}
	}
	else
	{
		std::map<QDateTime,QTodoItem*> items;
		QTodoLists* todo_lists = dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists();
		QTodoListViewItem* item = dynamic_cast<QTodoListViewItem*>(todo_lists->firstChild());
		for(;item;item = dynamic_cast<QTodoListViewItem*>(item->nextSibling()))
		{
			QTodoListIterator list_it(item->list());
			for(;list_it.current();++list_it)
			{
				if(QTodoItem* todo_item = dynamic_cast<QTodoItem*>(list_it.current()))
				{
					if(todo_item->getDeadlineString().isEmpty() || todo_item->getStatus() == STAT_DONE)
						continue;

					items[todo_item->getDeadlineDateTime()] = todo_item;
				}
			}
		}
		
		for(std::map<QDateTime,QTodoItem*>::iterator it = items.begin(); it != items.end(); ++it)
			text += todoItemRichText((*it).second);
	}
	textbrowser->setText(text);
}

QTodoMimeSourceFactory::QTodoMimeSourceFactory() : QMimeSourceFactory()
{
}

const QMimeSource* QTodoMimeSourceFactory::data(const QString& abs_name) const
{
	QPixmap pixmap;
	int id = abs_name.right(abs_name.length()-1).toInt();
	if(abs_name.startsWith("p"))
	{
		pixmap = QTodo::priorityToPixmap(id,12,12);
	}
	else if(abs_name.startsWith("s"))
	{
		pixmap = QTodo::statusToPixmap(id,12,12);
	}
	else if(abs_name.startsWith("t"))
	{
		pixmap = QTP::transparency;
	}

	QMimeSourceFactory* mf = new QMimeSourceFactory();
	mf->setPixmap(abs_name,pixmap);
	const QMimeSource* r = mf->data(abs_name);
	return r;
}

void QTodoPrintPreview::avoidSourceChange(const QString&)
{
	updateView();
}



