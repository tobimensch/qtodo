#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <unistd.h>

#include <qapplication.h>
#include <qpopupmenu.h>
#include <qdir.h>

#include "qtodo_widget.h"
#include "qtodo_settings_container.h"
#include "qtodo_trayicon.h"
#include "qtodo_xpm.h"
#include "qtodo_misc.h"
#include "qtodo_lists.h"

void quit_signalhandler(int)
{
	//..necessary cleanup operations before terminating
	//save all TODOlists
	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->save();
	dynamic_cast<QTodoWidget*>(qApp->mainWidget())->todoLists()->save();
	//save all settings
	QSC::save();

	exit(0);
}

QString shellCall(const QString& call)
{
        char buf[1024];
        FILE *ptr;

	QString output;
        if ((ptr = popen(call.ascii(), "r")) != NULL)
                while (fgets(buf, 1024, ptr) != NULL)
                        output += buf;
                (void) pclose(ptr);
        return output;
}

QString binDir()
{
	return shellCall("whereis qtodo | sed -e 's/qtodo://' | sed -e 's/qtodo//';").stripWhiteSpace();
}

int main(int argc, char *argv[])
{
	signal(SIGTERM, quit_signalhandler); // register a SIGTERM handler
	signal(SIGINT, quit_signalhandler); // register a SIGINT handler
	signal(SIGQUIT, quit_signalhandler); // register a SIGQUIT handler

	QApplication myapp(argc,argv);

	//Do some magic to setup the right data directory
	QString qtodo_binDir = binDir();
	QString qtodo_data_dir;
	if(!qtodo_binDir.isEmpty())
	{
		qtodo_data_dir = qtodo_binDir + "/../share/qtodo/";
		
		QDir dir;
		if(!dir.exists(qtodo_data_dir))
			qtodo_data_dir = "";
	}
	
	if(!qtodo_data_dir.isEmpty())
		chdir(qtodo_data_dir);

	QTodoWidget::system_palette = qApp->palette();

	QSC::get(); //make sure the settings container exists

	QTodoMisc::loadTranslator(QSC::get()->locale);

	QPopupMenu* tray_menu = new QPopupMenu();

	QTodoTrayIcon* tray_icon = new QTodoTrayIcon(QTP::todo_list.s(16,16),"QTodo",tray_menu);
	tray_icon->setToolTip("<pre>QTodoDer <b>ultimative</b> Test</pre>");

	if(QSC::get()->tray_icon)
		tray_icon->show();

	//create main widget
	QTodoWidget* myqtodo_widget = new QTodoWidget();
	myqtodo_widget->setTrayIcon(tray_icon);
	myqtodo_widget->setIcon(QTP::todo_list.s(16,16));

	qApp->connect(tray_icon,SIGNAL(clicked(const QPoint&,int)),myqtodo_widget,SLOT(trayIconClicked(const QPoint&,int)));
	qApp->connect(tray_icon,SIGNAL(doubleClicked(const QPoint&)),myqtodo_widget,SLOT(trayIconDoubleClicked(const QPoint&)));
	qApp->connect(tray_menu,SIGNAL(activated(int)),myqtodo_widget,SLOT(trayMenuActivated(int)));
	qApp->connect(tray_menu,SIGNAL(aboutToShow()),myqtodo_widget,SLOT(trayMenuAboutToShow()));

	//and set it to actually being the main one
	myapp.setMainWidget( myqtodo_widget );

	//show the damn thing
	if(QSC::get()->show_main_window_at_startup)
		myqtodo_widget->show();
	else if(!QSC::get()->tray_icon)
		myqtodo_widget->showMinimized();

	//and execute the GUI loop
	int return_code = myapp.exec();

	//save all TODOlists
	myqtodo_widget->save();
	//save all settings
	QSC::save();

	//delete the main window
	delete myqtodo_widget;

	return return_code;
}
