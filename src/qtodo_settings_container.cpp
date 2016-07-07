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

#include <qsettings.h>
#include <qdir.h>
#include <qapplication.h>
#include <qtextcodec.h>

#include "qtodo_widget.h"
#include "qtodo_lists.h"
#include "qtodo_list_widget.h"
#include "qtodo_list.h"

#include "qtodo_settings_container.h"

QTodoSettingsContainer* QTodoSettingsContainer::_instance = NULL;

QTodoSettingsContainer::QTodoSettingsContainer()
{
	m_settings = new QSettings(QSettings::Native);
	
	m_settings->insertSearchPath( QSettings::Unix, QDir::homeDirPath() + "/.qtodo/");
	
	m_settings->beginGroup("config");
	
	m_settings->beginGroup("general");
	open_with_commands = m_settings->readListEntry("open_with_commands");
	todo_list_files = m_settings->readListEntry("todolistfiles");
	last_todo_list = m_settings->readEntry("lasttodolist");
	last_todo_list_pos = m_settings->readNumEntry("lasttodolistpos");
	dont_quit_on_window_close_button = m_settings->readBoolEntry("dontquitonwindowclosebutton",true);
	tray_icon = m_settings->readBoolEntry("trayicon",true);
	autosave_lists = m_settings->readBoolEntry("autosave_lists",true);
	autosave_minutes = m_settings->readNumEntry("autosave_minutes",5);
	show_main_window_at_startup = m_settings->readBoolEntry("show_main_window_at_startup",true);
	undo_steps = m_settings->readNumEntry("undo_steps",20);
	locale = m_settings->readEntry("locale",QTextCodec::locale());
	backups = m_settings->readBoolEntry("backups",true);
	m_settings->endGroup();

	m_settings->beginGroup("lookandfeel");
	main_width = m_settings->readNumEntry("main_width",365);
	main_height = m_settings->readNumEntry("main_height",240);
	main_x = m_settings->readNumEntry("main_x",-1);
	main_y = m_settings->readNumEntry("main_y",-1);
	save_main_geom = m_settings->readBoolEntry("save_main_geom",true);
	todo_item_head_color = m_settings->readEntry("todo_item_head_color","#ffffb5");
	todo_item_body_color = m_settings->readEntry("todo_item_body_color","#ffffdb");
	todo_item_head_color_system = m_settings->readBoolEntry("todo_item_head_color_system",false);
	todo_item_body_color_system = m_settings->readBoolEntry("todo_item_body_color_system",false);
	extend_todo_on_mouse_over = m_settings->readBoolEntry("extend_todo_on_mouse_over",false);
	tab_input = m_settings->readBoolEntry("tab_input",true);
	todo_text_point_size = m_settings->readNumEntry("todo_text_point_size",QFont().pointSize());
	borderless_task = m_settings->readBoolEntry("borderless_task",true);
	border_width = m_settings->readNumEntry("border_width",1);
	todo_item_menu_button = m_settings->readBoolEntry("todo_item_menu_button",false);
	m_settings->endGroup();

	if(main_x == -1)
		main_x = QApplication::desktop()->width()/2-main_width/2;
	if(main_y == -1)
		main_y = QApplication::desktop()->height()/2-main_height/2;

	m_settings->beginGroup("deadlines");
	warn_before_deadlines_expire = m_settings->readBoolEntry("warn_before_deadlines_expire",true);
	warn_by_deadline_dialog = m_settings->readBoolEntry("warn_by_deadline_dialog",true);
	warn_by_command_execution = m_settings->readBoolEntry("warn_by_command_execution",false);
	warn_by_trayicon = m_settings->readBoolEntry("warn_by_trayicon",false);
	warning_execute_command = m_settings->readEntry("warning_execute_command");
	minutes_between_deadline_warnings = m_settings->readNumEntry("minutes_between_deadline_warnings",5);
	minutes_to_warn_before_deadline_expire = m_settings->readNumEntry("minutes_to_warn_before_deadline_expire",30);
	m_settings->endGroup();

	m_settings->beginGroup("print");
	print_icons = m_settings->readBoolEntry("print_icons",false);
	print_with_filter = m_settings->readBoolEntry("print_with_filter",false);
	m_settings->endGroup();
}

QTodoSettingsContainer::~QTodoSettingsContainer()
{
	m_settings->writeEntry("/general/open_with_commands",open_with_commands);
	m_settings->writeEntry("/general/todolistfiles",todo_list_files);

	if(dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists()->currentListWidget())
	{
		last_todo_list = dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists()->currentListWidget()->getFile();
		last_todo_list_pos = 	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists()->currentList()->contentsY();
	}
	m_settings->writeEntry("/general/lasttodolist",last_todo_list);	
	m_settings->writeEntry("/general/lasttodolistpos",last_todo_list_pos);
	m_settings->writeEntry("/general/dontquitonwindowclosebutton",dont_quit_on_window_close_button);
	m_settings->writeEntry("/general/trayicon",tray_icon);
	m_settings->writeEntry("/general/autosave_lists",autosave_lists);
	m_settings->writeEntry("/general/autosave_minutes",autosave_minutes);
	m_settings->writeEntry("/general/show_main_window_at_startup",show_main_window_at_startup);
	m_settings->writeEntry("/general/undo_steps",undo_steps);
	m_settings->writeEntry("/general/locale",locale);
	m_settings->writeEntry("/general/backups",backups);

	if(save_main_geom)
	{
		main_width = qApp->mainWidget()->width();
		main_height = qApp->mainWidget()->height();
		main_x = qApp->mainWidget()->x();
		main_y = qApp->mainWidget()->y();
	}
	else
	{
		main_width = -1;
		main_height = -1;
		main_x = -1;
		main_y = -1;
	}
	
	m_settings->writeEntry("/lookandfeel/main_width",main_width);
	m_settings->writeEntry("/lookandfeel/main_height",main_height);
	m_settings->writeEntry("/lookandfeel/main_x",main_x);
	m_settings->writeEntry("/lookandfeel/main_y",main_y);
	m_settings->writeEntry("/lookandfeel/save_main_geom",save_main_geom);
	m_settings->writeEntry("/lookandfeel/todo_item_head_color",todo_item_head_color);
	m_settings->writeEntry("/lookandfeel/todo_item_body_color",todo_item_body_color);
	m_settings->writeEntry("/lookandfeel/todo_item_head_color_system",todo_item_head_color_system);
	m_settings->writeEntry("/lookandfeel/todo_item_body_color_system",todo_item_body_color_system);
	m_settings->writeEntry("/lookandfeel/extend_todo_on_mouse_over",extend_todo_on_mouse_over);
	m_settings->writeEntry("/lookandfeel/tab_input",tab_input);
	m_settings->writeEntry("/lookandfeel/todo_text_point_size",todo_text_point_size);
	m_settings->writeEntry("/lookandfeel/borderless_task",borderless_task);
	m_settings->writeEntry("/lookandfeel/border_width",border_width);
	m_settings->writeEntry("/lookandfeel/todo_item_menu_button",todo_item_menu_button);

	m_settings->writeEntry("/deadlines/warn_before_deadlines_expire",warn_before_deadlines_expire);
	m_settings->writeEntry("/deadlines/warn_by_deadline_dialog",warn_by_deadline_dialog);
	m_settings->writeEntry("/deadlines/warn_by_command_execution",warn_by_command_execution);
	m_settings->writeEntry("/deadlines/warn_by_trayicon",warn_by_trayicon);
	m_settings->writeEntry("/deadlines/warning_execute_command",warning_execute_command);
	m_settings->writeEntry("/deadlines/minutes_between_deadline_warnings",minutes_between_deadline_warnings);
	m_settings->writeEntry("/deadlines/minutes_to_warn_before_deadline_expire",minutes_to_warn_before_deadline_expire);

	m_settings->writeEntry("/print/print_icons",print_icons);
	m_settings->writeEntry("/print/print_with_filter",print_with_filter);

	m_settings->endGroup();
	delete m_settings;
}

