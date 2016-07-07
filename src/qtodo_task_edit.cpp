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

#include <qfontmetrics.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qprocess.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <private/qrichtext_p.h>

#include "qtodo_task_edit.h"
#include "qtodo_settings_container.h"
#include "qtodo_xpm.h"
#include "qtodo_link_dialog.h"
#include "qtodo_misc.h"
#include "qtodo_font.h"
#include "qtodo_item.h"

QPtrList<QTodoTaskEdit> QTodoTaskEdit::instances;
QComboBox* QTodoTaskEdit::open_with_combo = 0;

QTodoTaskEdit::QTodoTaskEdit(QWidget* parent) : QTextEdit(parent)
{
	QFontMetrics font_metrics(currentFont());
	setHeight(font_metrics.lineSpacing()*3);
	setVScrollBarMode(QScrollView::AlwaysOff);
	QPalette my = palette();
	my.setColor(QColorGroup::Base,QColor(QSC::get()->todo_item_body_color));
	setPalette(my);
	setTabChangesFocus(QSC::get()->tab_input);
	setBorderless(QSC::get()->borderless_task);

	QStyleSheetItem* style_item = styleSheet()->item("p");
	style_item->setMargin(QStyleSheetItem::MarginVertical, 0);
	document()->setStyleSheet(styleSheet());

	setPointSize(QSC::get()->todo_text_point_size);

	connect(this,SIGNAL(textChanged()),this,SLOT(adaptToTextChange()));
	connect(this,SIGNAL(textChanged()),this,SLOT(emitChanged()));

	//FIXME: this is a workaround, because lines() is calculated wrong, when
	//not all sizes are calculated correctly (at the start)
	QTimer::singleShot(1,this,SLOT(adaptToTextChange()));

	is_rich_text = false;
	unchanged = true;
	prevent_emit_change = false;
	actions = 0;

	instances.append(this);
}

QTodoTaskEdit::~QTodoTaskEdit()
{
	instances.remove(this);
}

void QTodoTaskEdit::emitChanged()
{
	if(!prevent_emit_change)
		emit changed();
}

void QTodoTaskEdit::adaptToTextChange()
{
	show();

	int line_count = lines()+1;
	int height = (int)((float)QFontMetrics(currentFont()).lineSpacing()*((float)line_count-0.5));

	setHeight(height); //can we simply use contentsHeight() here? No! Performance for current algo has proven equally good and more reliable.
	//setHeight(contentsHeight());
}

void QTodoTaskEdit::increaseHeight()
{
	show();

	int line_count = lines()+2;
	int height = (int)((float)QFontMetrics(currentFont()).lineSpacing()*((float)line_count-0.5));

	setHeight(height); 
}

void QTodoTaskEdit::decreaseHeight()
{
	show();

	int line_count = lines();
	int height = (int)((float)QFontMetrics(currentFont()).lineSpacing()*((float)line_count-0.5));

	setHeight(height); 
}

void QTodoTaskEdit::setHeight(int height)
{
	setMaximumHeight(height);
	setMinimumHeight(height);
}

void QTodoTaskEdit::contentsWheelEvent(QWheelEvent* event)
{
	emit wheelEventSignal(event);
}

void QTodoTaskEdit::focusInEvent(QFocusEvent* event)
{
	emit focusInEventSignal(event);
}

void QTodoTaskEdit::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
		int p,i;
		case Qt::Key_PageUp:
		case Qt::Key_Up:
			getCursorPosition(&p,&i);
			if(p == 0 && lineOfChar(p,i) == 0)
				emit upNavigated(i);
			break;
		case Qt::Key_PageDown:
		case Qt::Key_Down:
			getCursorPosition(&p,&i);
			if(p == paragraphs()-1 && lineOfChar(p,i) == linesOfParagraph(p)-1)
				emit downNavigated(i);
			break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
			increaseHeight();
			break;
		case Qt::Key_Alt:
			QScrollView::keyPressEvent(event);
			return;
		default:
			break;
	}

	QTextEdit::keyPressEvent(event);
}

void QTodoTaskEdit::setTabChangesFocusGlobally(bool b)
{
	QPtrListIterator<QTodoTaskEdit> it(instances);
	for(;it.current();++it)
		it.current()->setTabChangesFocus(b);
}

void QTodoTaskEdit::setTodoTextPointSizeGlobally(int s)
{
	QPtrListIterator<QTodoTaskEdit> it(instances);
	for(;it.current();++it)
		it.current()->setPointSize(s);
}

void QTodoTaskEdit::setBorderlessGlobally(bool b)
{
	QPtrListIterator<QTodoTaskEdit> it(instances);
	for(;it.current();++it)
		it.current()->setBorderless(b);
}


void QTodoTaskEdit::resizeEvent(QResizeEvent* event)
{
	//QTodoMisc::debug("task edit resize event",this);
	QTextEdit::resizeEvent(event);
	adaptToTextChange();
}

void QTodoTaskEdit::setPointSize(int s)
{
	QString _text = myRichText();

	QFont font;
	font.setPointSize(s);
	setFont(font);

	QTextEdit::setPointSize(s);

	if(_text.isEmpty())
		return;
	if(is_rich_text)
		mySetText(_text);
	else
		mySetText(text());
}

int QTodoTaskEdit::cursorYPosition()
{
	int para,index;
	getCursorPosition(&para,&index);
	
	int lines = 0;
	int lines_of_paragraph;
	int line_of_char;
	for(int para_it = 0; para_it < para; ++para_it)
	{
		if(para_it == para)
		{
			line_of_char = lineOfChar(para_it,index);
			lines += line_of_char;
		}
		else
		{
			lines_of_paragraph = linesOfParagraph(para_it);
			lines += lines_of_paragraph;
		}
	}

	int y_pos = (int)((float)QFontMetrics(currentFont()).lineSpacing()*((float)lines));
	return y_pos;
}

enum
{
	TEMID_TODO_MENU = 9, //we start at 9 to make sure, we don't conflict with system id's
	TEMID_BOLD,
	TEMID_ITALIC,
	TEMID_UNDERLINED,
	TEMID_STRIKE,
	TEMID_SET_LINK
};

QPopupMenu* QTodoTaskEdit::createPopupMenu(const QPoint& pos)
{
	QPopupMenu* menu = QTextEdit::createPopupMenu(pos);

	todo_item_preselected = dynamic_cast<QTodoItem*>(parent()->parent())->isSelected();
	actions = dynamic_cast<QTodoItem*>(parent()->parent())->actionsPopupMenu();

	menu->insertItem(QTP::rightarrow.s(16,16),tr("Todo-Menu"),actions,TEMID_TODO_MENU,0);
	menu->insertSeparator(1);
	menu->insertItem(QTP::text_bold.s(16,16),tr("Bold"),TEMID_BOLD,2);
	menu->setItemChecked(TEMID_BOLD,bold());
	menu->insertItem(QTP::text_italic.s(16,16),tr("Italic"),TEMID_ITALIC,3);
	menu->setItemChecked(TEMID_ITALIC,italic());
	menu->insertItem(QTP::text_under.s(16,16),tr("Underlined"),TEMID_UNDERLINED,4);
	menu->setItemChecked(TEMID_UNDERLINED,underline());
	menu->insertItem(QTP::text_strike.s(16,16),tr("Strike out"),TEMID_STRIKE,5);
	menu->setItemChecked(TEMID_STRIKE,strikeOut());
	menu->insertSeparator(6);
	menu->insertItem(QTP::hyperlink.s(16,16),tr("Set link"),TEMID_SET_LINK,7);
	menu->setItemEnabled(TEMID_SET_LINK,!selectedText().isEmpty());
	menu->insertSeparator(8);

	connect(menu,SIGNAL(activated(int)),this,SLOT(menuActivated(int)));
	connect(menu,SIGNAL(aboutToHide()),this,SLOT(menuAboutToHide()));
	return menu;
}

void QTodoTaskEdit::menuAboutToHide()
{
	QTimer::singleShot(1,this,SLOT(deleteActionsMenu()));
}

void QTodoTaskEdit::deleteActionsMenu()
{
	if(!todo_item_preselected)
		dynamic_cast<QTodoItem*>(parent()->parent())->setSelected(false);
	delete actions;
}

void QTodoTaskEdit::menuActivated(int id)
{
	switch(id)
	{
		case TEMID_BOLD:
			setBold(!bold());
			break;
		case TEMID_ITALIC:
			setItalic(!italic());
			break;
		case TEMID_UNDERLINED:
			setUnderline(!underline());
			break;
		case TEMID_STRIKE:
			setStrikeOut(!strikeOut());
			break;
		case TEMID_SET_LINK:
			setLink();
			break;
		default:
			break;
	}
}

void QTodoTaskEdit::setLink(const QString& url)
{
	QString selected_text = selectedText();

	QTodoLinkDialog link_dialog;
	link_dialog.setText(selected_text);
	link_dialog.setURL(url);
	if(link_dialog.exec() == QDialog::Accepted)
	{
		insertRichText("<a href='"+link_dialog.getURL()+"'>"+link_dialog.getText()+"</a>");
	}
}

void QTodoTaskEdit::insertRichText(const QString& _text)
{
	if(hasSelectedText())
		removeSelectedText();

	setTextFormat(RichText);
	setUpdatesEnabled(false);
	int p, i;
	getCursorPosition(&p,&i);
	int o_paraCount = paragraphs();
	append(_text);
	int paraCount = paragraphs();
	setSelection(paraCount-1,0,paraCount-1, paragraphLength(paraCount-1));
	cut();
	if(p == 0 && i == 0)
	{
		insertAt(" ",0,0);
		setCursorPosition(p,i+1);
	}
	else
	{
		setCursorPosition(p,i);
	}
	paste();
	if(p == 0 && i == 0)
	{
		setSelection(0,0,0,1);
		removeSelectedText();
	}
	if(o_paraCount < paraCount)
	{
		removeParagraph(paraCount-1);
		setCursorPosition(p,i);
	}
	setUpdatesEnabled(true);
	updateContents();
	setTextFormat(AutoText);
}

QString QTodoTaskEdit::myRichText()	//this code is a modification from the qt-sources
{
	is_rich_text = false;

	QString s = "";
	QTextParagraph* p = document()->firstParagraph();

	while(p)
	{
		QString ps = paragraphRichText(p);
		s += ps;
		s += "\n";
		p = p->next();
	}

	if(s.endsWith("\n"))
		s.setLength(s.length()-1);
	return s;
}

QString QTodoTaskEdit::myPlainText()
{
	QString s = "";
	QTextParagraph* p = document()->firstParagraph();

	while(p)
	{
		QString ps = paragraphPlainText(p);
		s += ps;
		s += "\n";
		p = p->next();
	}

	if(s.endsWith("\n"))
		s.setLength(s.length()-1);
	return s;
}

QStringList QTodoTaskEdit::myLinks()
{
	QTextParagraph* p = document()->firstParagraph();
	QStringList list;
	while(p)
	{
		paragraphLinks(p,list);
		p = p->next();
	}
	return list;
}

void QTodoTaskEdit::paragraphLinks(QTextParagraph* para,QStringList& list)
{
	int i;
	QString lastAnchorName;
	QString last_href;
	QString text;
	for (i = 0; i < para->length()-1; ++i)
	{
		QTextStringChar *c = &(para->string()->at(i));
		if(c->anchorHref() != last_href)
		{
			if(!last_href.isEmpty())
			{
				list += text;
				list += last_href;
			}
			last_href = c->anchorHref();
			if(last_href.isEmpty())
				continue;

			text = "";
		}
		text += c->c;
	}
	if(!last_href.isEmpty())
	{
		list += text;
		list += last_href;
	}
}

QString QTodoTaskEdit::paragraphRichText(QTextParagraph* para)
{
	QString s;
	QTextStringChar *formatChar = 0;
	QString spaces;
	int i;
	QString lastAnchorName;
	for (i = 0; i < para->length()-1; ++i)
	{
	QTextStringChar *c = &(para->string()->at(i));
	if (!formatChar)
	{
		s += makeFormatChangeTags(c->format(), para->formatCollection()->defaultFormat(),
						    0, QString::null, c->anchorHref() );
		formatChar = c;
	}
	else if((formatChar->format()->key() != c->format()->key()) ||
		  (c->anchorHref() != formatChar->anchorHref()))
	{
		s += makeFormatChangeTags(c->format(), para->formatCollection()->defaultFormat(),
						    formatChar->format() , formatChar->anchorHref(), c->anchorHref() );
		formatChar = c;
	}
	if(c->c == '<')
		s += "&lt;";
	else if(c->c == '>')
		s += "&gt;";
	else if (c->c =='&')
		s += "&amp;";
	else if (c->c =='\"')
		s += "&quot;";

#define QChar_linesep QChar(0x2028U)
	else if (c->c == '\n' || c->c == QChar_linesep)
		s += '\n';//"<br />"; // space on purpose for compatibility with Netscape, Lynx & Co.
	else
		s += c->c;
    }

	if(formatChar)
		s += makeFormatEndTags(formatChar->format(), para->formatCollection()->defaultFormat(), formatChar->anchorHref() );

	return s;
}

QString QTodoTaskEdit::paragraphPlainText(QTextParagraph* para)
{
	QString s;
	QString spaces;
	int i;
	for (i = 0; i < para->length()-1; ++i)
	{
	QTextStringChar *c = &(para->string()->at(i));

#define QChar_linesep QChar(0x2028U)
	if (c->c == '\n' || c->c == QChar_linesep)
		s += '\n';//"<br />"; // space on purpose for compatibility with Netscape, Lynx & Co.
	else
		s += c->c;
	}

	return s;
}

QString QTodoTaskEdit::makeFormatChangeTags(QTextFormat* format, QTextFormat* defaultFormat, QTextFormat *f,
			   const QString& oldAnchorHref, const QString& anchorHref  )
{
	QString tag;
	if (f)
		tag += makeFormatEndTags(f, format, oldAnchorHref);

	if (!anchorHref.isEmpty())
		tag += "<a href=\"" + anchorHref + "\">";

	if (format->font() != defaultFormat->font())
	{
		QString s;
		if ( format->font().italic())
			s += "<i>";
		if ( format->font().bold())
			s += "<b>";
		if ( format->font().underline())
			s += "<u>";
		if ( format->font().strikeOut())
			s += "<s>";
		if ( !s.isEmpty() )
			tag += s;
	}

	if(!tag.isEmpty())
		is_rich_text = true;
	return tag;
}

QString QTodoTaskEdit::makeFormatEndTags(QTextFormat* format, QTextFormat* /*defaultFormat*/, const QString& anchorHref)
{
	QString tag;

	if(format->font().strikeOut())
		tag += "</s>";
	if(format->font().underline())
		tag += "</u>";
	if(format->font().bold())
		tag += "</b>";
	if(format->font().italic())
		tag += "</i>";
	if(!anchorHref.isEmpty())
		tag += "</a>";
	if(!tag.isEmpty())
		is_rich_text = true;
	return tag;
}

void QTodoTaskEdit::mySetText(const QString& _text, bool _prevent_emit_change)
{
	prevent_emit_change = _prevent_emit_change;

	if(_text.startsWith("<!doc>") || is_rich_text)
	{
		setText(QString(_text).replace("\n","<br>"));
	}
	else
	{
		clear();	//when we don't want richtext, that was the way to go
		insert(_text);
		//setText(_text);
	}

	prevent_emit_change = false;
}

QString QTodoTaskEdit::myText()
{
	/*qDebug(text());
	if(QStyleSheet::mightBeRichText(text()))*/
	QString rich_text = myRichText();
	if(is_rich_text)
	{
		return rich_text.insert(0,"<!doc>");
	}
	else
	{
		return myPlainText();
	}
	/*else
		return text();*/
}

void QTodoTaskEdit::setStrikeOut(bool b)
{
	QFont font = currentFont();
	font.setStrikeOut(b);
	setCurrentFont(font);
}

bool QTodoTaskEdit::strikeOut() const
{
	return currentFont().strikeOut();
}

enum
{
	TELM_OPEN_WITH_COMBO,
	TELM_OPEN_WITH_GO,
	TELM_CHANGE,
	TELM_REMOVE,
};

void QTodoTaskEdit::contentsMousePressEvent(QMouseEvent* event)
{
	QString anchor = anchorAt(event->pos());
	if(event->button() == LeftButton && !anchor.isEmpty())
	{

		cur_anchor_pos = event->pos();

		QPopupMenu* open_with_menu = new QPopupMenu(this);

		if(!open_with_combo)
		{
			open_with_combo = new QComboBox();
			open_with_combo->setEditable(true);
			open_with_combo->setMinimumWidth(150);
			connect(open_with_combo->lineEdit(),SIGNAL(returnPressed()),this,SLOT(openWithComboLineEditActivated()));
		}
		open_with_combo->clear();
		open_with_combo->insertStringList(QSC::get()->open_with_commands);

		QLabel* open_with_label = new QLabel(tr("Open with:"),this);
		open_with_label->setFont(QF::get()->bold);
		open_with_label->setFrameStyle(QFrame::Panel | QFrame::Plain);
		open_with_menu->insertItem(open_with_label);
		open_with_menu->insertItem(open_with_combo,TELM_OPEN_WITH_COMBO);
		open_with_menu->insertItem(QTP::play.s(16,16),tr("Go"),TELM_OPEN_WITH_GO);

		connect(open_with_menu,SIGNAL(activated(int)),this,SLOT(openWithMenuActivated(int)));

		QPopupMenu* menu = new QPopupMenu(this);

		QLabel* menu_label = new QLabel(tr("Link: "+anchor),menu);
		menu_label->setFont(QF::get()->bold);
		menu_label->setFrameStyle(QFrame::Panel | QFrame::Plain);

		menu->insertItem(menu_label);
		menu->insertItem(QTP::play.s(16,16),tr("Open with"),open_with_menu);
		menu->insertItem(QTP::edit.s(16,16),tr("Change"),TELM_CHANGE);
		menu->insertItem(QTP::eraser.s(16,16),tr("Remove"),TELM_REMOVE);

		connect(menu,SIGNAL(activated(int)),this,SLOT(linkMenuActivated(int)));

		menu->exec(QCursor::pos());
		delete menu;
		delete open_with_menu;
		open_with_combo = 0;
	}
	else
	{
		QTextEdit::contentsMousePressEvent(event);
	}
}

void QTodoTaskEdit::openWithComboLineEditActivated()
{
	openWithMenuActivated(TELM_OPEN_WITH_GO);
	dynamic_cast<QPopupMenu*>(sender()->parent()->parent())->close();
}

void QTodoTaskEdit::openWithMenuActivated(int id)
{
	QString command = open_with_combo->currentText();
	QString anchor = anchorAt(cur_anchor_pos);
	QProcess proc;
	switch(id)
	{
		case TELM_OPEN_WITH_GO:
			proc.setArguments(QStringList::split(" ",command));
			proc.addArgument(anchor);
			proc.start();
			if(proc.isRunning())
			{
				QStringList find_command = QSC::get()->open_with_commands.grep(command);
				bool new_command = true;
				for(QStringList::iterator it = find_command.begin(); it != find_command.end(); ++it)
					if((*it) == command)
						new_command = false;
				if(new_command)
				{
					QSC::get()->open_with_commands += command;
					QSC::get()->open_with_commands.sort();
				}
			}
			break;
		default:
			break;
	}
}

void QTodoTaskEdit::linkMenuActivated(int id)
{
	QString anchor = anchorAt(cur_anchor_pos);
	int para_from, index_from, para_to, index_to;
	getAnchor(cur_anchor_pos,para_from,index_from,para_to,index_to);
	/*qDebug("ParaFrom: "+QString::number(para_from));
	qDebug("IndexFrom: "+QString::number(index_from));
	qDebug("ParaTo: "+QString::number(para_to));
	qDebug("IndexTo: "+QString::number(index_to));*/
	setSelection(para_from,index_from,para_to,index_to);
	switch(id)
	{
		case TELM_CHANGE:
			setLink(anchor);
			break;
		case TELM_REMOVE:
			removeSelectedText();
			break;
	}
	removeSelection();
}

void QTodoTaskEdit::getAnchor(const QPoint& start_point,int& para_from, int& index_from, int& para_to, int& index_to)
{
	QString anchor = anchorAt(start_point);
	QTextCursor c(document());
	placeCursor(start_point,&c);

	//find start
	int i;
	index_from = -1;
	for(i = c.index(); i >= 0; --i)
	{
		if(c.paragraph()->at(i)->anchorHref() != anchor)
		{
			index_from = ++i;
			para_from = c.paragraph()->paragId();
			break;
		}
	}
	if(index_from == -1)
	{
		index_from = 0;
		para_from = c.paragraph()->paragId();
	}

	//find end
	for(i = c.index(); true; ++i)
	{
		if(c.paragraph()->at(i)->anchorHref() != anchor)
		{
			index_to = i;
			para_to = c.paragraph()->paragId();
			break;
		}
	}
}

void QTodoTaskEdit::setBorderless(bool b)
{
	if(b)
		setFrameStyle(QFrame::NoFrame);
	else
		setFrameStyle(QFrame::LineEditPanel | QFrame::Sunken);
}






