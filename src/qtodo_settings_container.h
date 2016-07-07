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

#ifndef QTODO_SETTINGS_CONTAINER
#define QTODO_SETTINGS_CONTAINER

class QSettings;

///Contains all application settings
///and manages saving and loading of them.
class QTodoSettingsContainer
{
public:
	~QTodoSettingsContainer();
	static QTodoSettingsContainer* get() { _instance ? _instance : _instance = new QTodoSettingsContainer(); return _instance; }
	static void save() { delete _instance; _instance = 0; }
	
	QStringList open_with_commands;
	QStringList todo_list_files;
	QString locale;
	QString last_todo_list;
	QString warning_execute_command;
	int last_todo_list_pos;
	int main_width;
	int main_height;
	int main_x;
	int main_y;
	QString todo_item_head_color;
	QString todo_item_body_color;
	bool todo_item_menu_button;
	bool backups;
	bool borderless_task;
	bool save_main_geom;
	bool show_main_window_at_startup;
	bool todo_item_head_color_system;
	bool todo_item_body_color_system;
	bool dont_quit_on_window_close_button;
	bool autosave_lists;
	bool extend_todo_on_mouse_over;
	bool tray_icon;
	bool warn_before_deadlines_expire;
	bool warn_by_deadline_dialog;
	bool warn_by_command_execution;
	bool warn_by_trayicon;
	bool tab_input;
	bool print_icons;
	bool print_with_filter;
	int minutes_between_deadline_warnings;
	int minutes_to_warn_before_deadline_expire;
	int autosave_minutes;
	int todo_text_point_size;
	int undo_steps;
	int border_width;
private:
	QTodoSettingsContainer();
	
	QSettings* m_settings;
	static QTodoSettingsContainer* _instance;
};

typedef QTodoSettingsContainer QSC;

#endif

