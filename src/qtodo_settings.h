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

#include <map>

#include <qdialog.h>
#include <qlocale.h>
 
#ifndef QTODO_SETTINGS
#define QTODO_SETTINGS

#include "settings.h"

class QTodoSettings : public QTodoSettingsBase
{
	Q_OBJECT

	public:
	QTodoSettings();

	private slots:
	void colorButtonClicked();
	void showHelp();
	void accept();

	private:
	void save();
	void apply();

	std::map<QString,QLocale::Language> lang_id;
	QString cur_locale;
	int cur_todotextpointsize;
};

#endif

