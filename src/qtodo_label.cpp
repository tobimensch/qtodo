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

#include "qtodo_label.h"

QTodoLabel::QTodoLabel(QWidget* parent) : QLineEdit(parent)
{
	setBackgroundColor(parent->palette().color(QPalette::Active,QColorGroup::Background));
	setReadOnly(true);
	setLineWidth(0);
	setMargin(0);
	setFocusPolicy(QWidget::NoFocus);
}

void QTodoLabel::setText(const QString& _text)
{
	real_text = _text;
	real_text.replace("\n"," ");
	QString mod_text = real_text;
	if(mod_text.isEmpty())
		mod_text = tr("N/A");

	QLineEdit::setText(mod_text);
	setCursorPosition(0);
}

QString QTodoLabel::realText()
{
	return real_text;
}

void QTodoLabel::resizeEvent(QResizeEvent* e)
{
	QLineEdit::resizeEvent(e);
}

void QTodoLabel::focusInEvent(QFocusEvent*)
{
	setCursorPosition(0);
}

void QTodoLabel::styleChange(QStyle& old)
{
	QLineEdit::styleChange(old);
	setLineWidth(0); //We need this to fix something that's IMHO a bug in Qt
}

