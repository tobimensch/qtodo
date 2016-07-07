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

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qapplication.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>

#include "qtodo_settings.h"
#include "qtodo_settings_container.h"
#include "qtodo_help.h"
#include "qtodo_widget.h"
#include "qtodo_task_edit.h"
#include "qtodo_item.h"
#include "qtodo_misc.h"
#include "qtodo_undo_manager.h"
#include "qtodo_lists.h"

QTodoSettings::QTodoSettings()
{
	connect(help_button,SIGNAL(clicked()),this,SLOT(showHelp()));
	connect(todo_item_head_color_button,SIGNAL(clicked()),this,SLOT(colorButtonClicked()));
	connect(todo_item_body_color_button,SIGNAL(clicked()),this,SLOT(colorButtonClicked()));

	show_trayicon->setChecked(QSC::get()->tray_icon);
	dont_quit_on_window_close_button->setChecked(QSC::get()->dont_quit_on_window_close_button);
	autosave_lists_checkbox->setChecked(QSC::get()->autosave_lists);
	autosave_minutes_spinbox->setValue(QSC::get()->autosave_minutes);
	show_main_window_at_startup_checkbox->setChecked(QSC::get()->show_main_window_at_startup);
	undo_steps_spinbox->setValue(QSC::get()->undo_steps);
	language_combo->setCurrentText(QSC::get()->locale);
	create_backups_checkbox->setChecked(QSC::get()->backups);

	language_combo->insertItem("English");
	lang_id["English"] = QLocale("en").language();
	QDir dir("./translations/");
	QStringList trans_files = dir.entryList("qtodo_*.qm");
	for(QStringList::Iterator it = trans_files.begin(); it != trans_files.end(); ++it)
	{
		QLocale locale((*it).remove("qtodo_").remove(".qm"));
		language_combo->insertItem(QLocale::languageToString(locale.language()));
		lang_id[QLocale::languageToString(locale.language())] = locale.language();
	}
	cur_locale = QSC::get()->locale;
	language_combo->setCurrentText(QLocale::languageToString(QLocale(QSC::get()->locale).language()));

	todo_item_head_color_button->setPaletteBackgroundColor(QSC::get()->todo_item_head_color);
	todo_item_body_color_button->setPaletteBackgroundColor(QSC::get()->todo_item_body_color);
	todo_item_head_color_system_checkbox->setChecked(QSC::get()->todo_item_head_color_system);
	todo_item_body_color_system_checkbox->setChecked(QSC::get()->todo_item_body_color_system);
	show_todo_details_on_mouse_over_checkbox->setChecked(QSC::get()->extend_todo_on_mouse_over);
	tab_input_checkbox->setChecked(QSC::get()->tab_input);
	borderless_task_checkbox->setChecked(QSC::get()->borderless_task);
	todo_text_point_size_spinbox->setValue(QSC::get()->todo_text_point_size);
	border_width_spinbox->setValue(QSC::get()->border_width);
	todo_menu_button_checkbox->setChecked(QSC::get()->todo_item_menu_button);

	cur_todotextpointsize = QSC::get()->todo_text_point_size;

	warn_before_deadlines_expire_checkbox->setChecked(QSC::get()->warn_before_deadlines_expire);
	deadline_dialog_warning_checkbox->setChecked(QSC::get()->warn_by_deadline_dialog);
	execute_command_warning_checkbox->setChecked(QSC::get()->warn_by_command_execution);
	trayicon_warning_checkbox->setChecked(QSC::get()->warn_by_trayicon);
	execute_command_lineedit->setText(QSC::get()->warning_execute_command);
	minutes_between_warnings->setValue(QSC::get()->minutes_between_deadline_warnings);
	minutes_warn_before_deadline->setValue(QSC::get()->minutes_to_warn_before_deadline_expire);
	time_settings_groupbox->setEnabled(QSC::get()->warn_before_deadlines_expire);
	warning_actions_groupbox->setEnabled(QSC::get()->warn_before_deadlines_expire);
	execute_command_lineedit->setEnabled(QSC::get()->warn_by_command_execution);
}

void QTodoSettings::accept()
{
	save();
	apply();

	QDialog::accept();
}

void QTodoSettings::showHelp()
{
	QTodoHelp* help = new QTodoHelp();
	QTodoMisc::windowToCurrentDisplay(help->winId()); //FIXME: add "activate(const Window&) to reduce overhead
	help->exec();
}

void QTodoSettings::colorButtonClicked()
{
	QPushButton* button = dynamic_cast<QPushButton*>(const_cast<QObject*>(sender()));
	QColor col = QColorDialog::getColor(button->paletteBackgroundColor());
	if(col.isValid())
		button->setPaletteBackgroundColor(col);
}

void QTodoSettings::save()
{
	QSC::get()->tray_icon = show_trayicon->isChecked();
	QSC::get()->dont_quit_on_window_close_button = dont_quit_on_window_close_button->isChecked();
	QSC::get()->autosave_lists = autosave_lists_checkbox->isChecked();
	QSC::get()->autosave_minutes = autosave_minutes_spinbox->value();
	QSC::get()->show_main_window_at_startup = show_main_window_at_startup_checkbox->isChecked();
	QSC::get()->undo_steps = undo_steps_spinbox->value();
	QSC::get()->locale = QLocale(lang_id[language_combo->currentText()]).name().remove(QRegExp("_.*"));
	QSC::get()->backups = create_backups_checkbox->isChecked();

	QSC::get()->todo_item_head_color = todo_item_head_color_button->paletteBackgroundColor().name();
	QSC::get()->todo_item_body_color = todo_item_body_color_button->paletteBackgroundColor().name();
	QSC::get()->todo_item_head_color_system = todo_item_head_color_system_checkbox->isChecked();
	QSC::get()->todo_item_body_color_system = todo_item_body_color_system_checkbox->isChecked();
	QSC::get()->extend_todo_on_mouse_over = show_todo_details_on_mouse_over_checkbox->isChecked();
	QSC::get()->tab_input = tab_input_checkbox->isChecked();
	QSC::get()->borderless_task = borderless_task_checkbox->isChecked();
	QSC::get()->todo_text_point_size = todo_text_point_size_spinbox->value();
	QSC::get()->border_width = border_width_spinbox->value();
	QSC::get()->todo_item_menu_button = todo_menu_button_checkbox->isChecked();

	QSC::get()->warn_before_deadlines_expire = warn_before_deadlines_expire_checkbox->isChecked();
	QSC::get()->warn_by_deadline_dialog = deadline_dialog_warning_checkbox->isChecked();
	QSC::get()->warn_by_command_execution = execute_command_warning_checkbox->isChecked();
	QSC::get()->warn_by_trayicon = trayicon_warning_checkbox->isChecked();
	QSC::get()->warning_execute_command = execute_command_lineedit->text();
	QSC::get()->minutes_between_deadline_warnings = minutes_between_warnings->value();
	QSC::get()->minutes_to_warn_before_deadline_expire = minutes_warn_before_deadline->value();

	QSC::get()->save();
}

void QTodoSettings::apply()
{
	if(cur_locale != QSC::get()->locale)
	{
		QMessageBox::information(this,tr("Language change"),tr("You need to restart QTodo for this change to take effect correctly!"));
		QTodoMisc::loadTranslator(QSC::get()->locale);
	}

	QTUM::get()->setMaxUndos(QSC::get()->undo_steps);
	QTodoWidget* todo_widget = dynamic_cast<QTodoWidget*>(qApp->mainWidget());
	todo_widget->setTrayIconEnabled(QSC::get()->tray_icon);
	todo_widget->updateTrayIcon(true);
	todo_widget->prepareAutoSave();

	todo_widget->todoLists()->preserveContentsPositions();

	QTodoItem::updateColorsGlobally();
	QTodoItem::updateBordersGlobally();
	QTodoItem::updateMenuButtonGlobally();
	QTodoTaskEdit::setTabChangesFocusGlobally(QSC::get()->tab_input);
	if(cur_todotextpointsize != QSC::get()->todo_text_point_size)
		QTodoTaskEdit::setTodoTextPointSizeGlobally(QSC::get()->todo_text_point_size);
	QTodoTaskEdit::setBorderlessGlobally(QSC::get()->borderless_task);

	todo_widget->todoLists()->restoreContentsPositions();
}



