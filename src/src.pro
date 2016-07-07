# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/qtodo

INSTALLS += qtodotranslations \
            target 
target.path = /usr/local/bin 
qtodotranslations.files += ./*.qm 
qtodotranslations.path = /usr/local/share/qtodo/translations/ 
FORMS += addlist.ui \
         settings.ui \
         about.ui \
         help.ui \
         deadlines.ui \
         recovery.ui 
TRANSLATIONS += qtodo_de.ts 
DISTFILES += qt_de.qm \
             qtodo_de.qm 
IMAGES += calendar.xpm \
          configure.xpm \
          date.xpm \
          exit.xpm \
          help.xpm \
          todo_branch.xpm \
          todo_list.xpm \
          todo_section.xpm \
          yellow_minus.xpm \
          yellow_plus.xpm \
          add_above.xpm \
          add_below.xpm \
          add_bottom.xpm \
          add_sub.xpm \
          add_top.xpm \
          any.xpm \
          ascending.xpm \
          clock.xpm \
          dead.xpm \
          descending.xpm \
          done.xpm \
          editcopy.xpm \
          editcut.xpm \
          editdelete.xpm \
          editpaste.xpm \
          high.xpm \
          in_progress.xpm \
          low.xpm \
          normal.xpm \
          not_available.xpm \
          not_dead.xpm \
          not_done.xpm \
          not_high.xpm \
          not_in_progress.xpm \
          not_low.xpm \
          not_normal.xpm \
          not_not_available.xpm \
          not_planning.xpm \
          planning.xpm \
          print.xpm \
          rightarrow.xpm \
          save_all.xpm \
          show_less.xpm \
          show_more.xpm \
          sort.xpm \
          transparency.xpm \
          undo.xpm \
          archiving.xpm \
          close.xpm \
          deselect_all.xpm \
          edit.xpm \
          eraser.png \
          filefind.xpm \
          hyperlink.xpm \
          reload.xpm \
          select_all.xpm \
          text_bold.xpm \
          text_italic.xpm \
          text_strike.xpm \
          text_under.xpm \
          export_list.xpm \
          folder.xpm \
          hdd.xpm \
          eraser.xpm \
          play.xpm 
HEADERS += qtodo_widget.h \
           qtodo_list.h \
           qtodo_item_widget.h \
           qtodo_list_view_item.h \
           qtodo_settings.h \
           qtodo_parser.h \
           qtodo_list_widget.h \
           qtodo_list_header_widget.h \
           qtodo_section_widget.h \
           qtodo_lists.h \
           qtodo_item.h \
           qtodo_datepicker.h \
           qcalendarbase.h \
           qcalmonthlookup.h \
           qcalimages.h \
           navarrows.h \
           qtodo_add_list.h \
           qtodo_list_filter.h \
           qtodo_task_edit.h \
           qtodo_check_label.h \
           qtodo_help.h \
           qtodo_trayicon.h \
           qtodo_xpm.h \
           qtodo_about.h \
           qtodo_extend_widget.h \
           qtodo_priority_widget.h \
           qtodo_status_widget.h \
           qtodo_combo_widget.h \
           qtodo_misc.h \
           qtodo_settings_container.h \
           qtodo_radio_dialog.h \
           qtodo_clipboard.h \
           qtodo_deadline_dialog.h \
           qtodo_font.h \
           qtodo_print_preview.h \
           qtodo_sort_dialog.h \
           qtodo_undo_manager.h \
           qtodo_datetime_label.h \
           qtodo_push_button.h \
           qtodo_label.h \
           qtodo_exporter.h \
           qtodo_export_plugin_html.h \
           qtodo_export_plugin_base.h \
           qtodo_export_plugin_plaintext.h \
           qtodo_clock.h \
           qtodo_link_dialog.h \
           qtodo_datetimeedit.h \
           qtodo_recovery_dialog.h 
SOURCES += main.cpp \
           qtodo_widget.cpp \
           qtodo_list.cpp \
           qtodo_item_widget.cpp \
           qtodo_list_view_item.cpp \
           qtodo_settings.cpp \
           qtodo_parser.cpp \
           qtodo_list_widget.cpp \
           qtodo_list_header_widget.cpp \
           qtodo_section_widget.cpp \
           qtodo_lists.cpp \
           qtodo_item.cpp \
           qtodo_datepicker.cpp \
           qcalendarbase.cpp \
           qcalmonthlookup.cpp \
           qcalimages.cpp \
           qtodo_add_list.cpp \
           qtodo_list_filter.cpp \
           qtodo_task_edit.cpp \
           qtodo_check_label.cpp \
           qtodo_help.cpp \
           qtodo_trayicon.cpp \
           qtodo_xpm.cpp \
           qtodo_about.cpp \
           qtodo_extend_widget.cpp \
           qtodo_priority_widget.cpp \
           qtodo_status_widget.cpp \
           qtodo_combo_widget.cpp \
           qtodo_misc.cpp \
           qtodo_settings_container.cpp \
           qtodo_radio_dialog.cpp \
           qtodo_clipboard.cpp \
           qtodo_deadline_dialog.cpp \
           qtodo_font.cpp \
           qtodo_print_preview.cpp \
           qtodo_sort_dialog.cpp \
           qtodo_undo_manager.cpp \
           qtodo_datetime_label.cpp \
           qtodo_push_button.cpp \
           qtodo_label.cpp \
           qtodo_exporter.cpp \
           qtodo_export_plugin_html.cpp \
           qtodo_export_plugin_base.cpp \
           qtodo_export_plugin_plaintext.cpp \
           qtodo_clock.cpp \
           qtodo_link_dialog.cpp \
           qtodo_datetimeedit.cpp \
           qtodo_recovery_dialog.cpp 
TARGET = ../bin/qtodo
CONFIG += release \
warn_on \
qt \
thread \
stl
TEMPLATE = app
