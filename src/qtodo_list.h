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

#include <vector>
#include <map>
#include <list>

#include <qscrollview.h>
#include <qdatetime.h>
#include <qptrlist.h>

#include "qtodo_parser.h"
#include "qtodo_sort_dialog.h"
 
#ifndef QTODO_LIST
#define QTODO_LIST


//CONTEXTMENUID
enum
{
	CMID_ADDTODO,
	CMID_ADDTODOATBOTTOM,
	CMID_ADDTODOATTOP,
	CMID_ADDSECTION,
	CMID_COPY,
	CMID_CUT,
	CMID_PASTE,
	CMID_SELECT_ALL,
	CMID_DESELECT_ALL,
	CMID_SORT
};

//TODODELETE
enum
{
	TDDL_TODO_AND_SUB,
	TDDL_TODO_BUT_SUBUP,
	TDDL_TODO_NOT_SUB,
	TDDL_ONLY_SUB
};

//TODOCOPY
enum
{
	TDCP_TODO_AND_SUB,
	TDCP_TODO_NOT_SUB,
	TDCP_ONLY_SUB,
	TDCP_ALL
};

//TODOCUT
enum
{
	TDCT_TODO_AND_SUB,
	TDCT_TODO_BUT_SUBUP,
	TDCT_TODO_NOT_SUB,
	TDCT_ONLY_SUB,
	TDCT_ALL
};

//TODOPASTE
enum
{
	TDP_ABOVE,
	TDP_BELOW,
	TDP_SUB,
	TDP_ATTOP,
	TDP_ATBOTTOM
};

class QTodoItem;
class QTodo;
typedef std::vector<QTodo*> QTodoNode;
class QVBox;
class QVBoxLayout;
class QGridLayout;
class QPopupMenu;
class QLabel;
class QTodoTopNode;
class QTextStream;
class QTodoSectionWidget;
class QTodoListWidget;
class QTodoItem;

///TODOlist containing TODOitems
///and sections
class QTodoList : public QScrollView
{
	Q_OBJECT
	
	public:
	QTodoList(QWidget*);
	virtual ~QTodoList();
	void addTodo();
	void addTodo(int,int);
	void addTodoAtTop();
	void addSubTodo(QTodoItem*);
	void addTodoAbove(QTodoItem*);
	void addTodoBelow(QTodoItem*);
	void insertTodo(QTodoItem*,int,int);
	void insertTodo(QTodoItem*,int);
	void insertTodoAtBottom(QTodoItem*);
	void takeTodo(QTodoItem*);
	void takeTodos(std::vector<QTodoItem*>*);
	void takeAll();
	void removeTodo(QTodoItem*);
	void cut();
	void copy();
	void pasteTodo(QTodoItem*);
	void paste();
	void pasteAtTop();
	void pasteAtBottom();
	void _delete();
	void adjustDepth(int);
	std::vector<QTodoItem*> getSubTodos(QTodoItem*);
	void selectSubTodos(QTodoItem*,bool);
	bool hasSubTodos(QTodoItem*);
	bool isFirstSubTodo(QTodoItem*);
	QTodoItem* parentTodo(QTodoItem*);
	QTodoSectionWidget* addSection();
	void load(QTodoTopNode*);
	void save(QTextStream&);
	QTodoListWidget* getTodoListWidget();
	void contextMenu(const QPoint&);
	void contextMenu(QPopupMenu*);
	void contextMenuUpdate(QPopupMenu*);
	void contextMenuEnabled(QPopupMenu*);
	int count();
	int countSelected();
	void prepareList();
	void sort();
	void sort(QTodoItem*);
	int findWidget(QWidget*);
	//also below all sub-todos
	int findPosBelowTodoItem(QTodoItem*);
	//also above all sub-todos
	int findPosAboveTodoItem(QTodoItem*);
	void setModified();
	void deselectAll();
	void selectAll();
	bool isItemSelected();
	void preserveContentsYPos();
	void restoreContentsYPos();
	void jumpToFirst();
	void jumpToLast();
	void ensureItemVisible(QTodoItem*);
	
	public slots:
	void filter(const QString&,int priority = ANY,int status = ANY, int deadline_filter_mode = ANY,const QDate& = QDate());
	void subWheelEvent(QWheelEvent*);
	void navigateUp(QTodoItem*,int);
	void navigateDown(QTodoItem*,int);

	private slots:
	void contextMenuActivated(int);

	private:
	//void setVBarGeometry(QScrollBar&,int, int, int, int);
	void viewportResizeEvent(QResizeEvent*);
	void loadNode(QTodoNode*,unsigned int);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void refreshSpacer();
	void removeTodos(std::vector<QTodoItem*>);
	void adjustDepth(std::vector<QTodoItem*>, int);
	void removeWidget(QWidget*);
	void fixItemSize(QTodoItem*);

	QVBox* vbox;
	QLabel* spacer;
	int preserved_contents_y_pos;
	
	std::vector<QWidget*> content;

	friend class QTodoListIterator;
};

class QTodoListIterator
{
	public:
	QTodoListIterator(QTodoList*);
	QWidget* operator++();
	QWidget* operator--();
	QWidget* current();
	const QWidget* currentConst();
	QWidget* goTo(unsigned int);
	bool isItem();
	QTodoItem* item();
	bool isSection();
	unsigned int currentIndex();
	void take();
	void remove();
	
	unsigned int count();
	static unsigned int count(QTodoList*);

	private:
	unsigned int index;
	QTodoList* list;
};

class QTodoItemSortPtrList;

class QTodoListItemsSorter
{
	public:
	QTodoListItemsSorter(QPtrList<QWidget>*,const QTodoSortCriteriaMap*);
	QPtrList<QWidget>* get();
	
	private:
	QTodoItemSortPtrList* getSubList(QPtrListIterator<QWidget>);

	QPtrList<QWidget>* input;
	QPtrList<QWidget>* output;
};

class QTodoItemSortPtrList : public QPtrList<QTodoItem>
{
	public:
	void setSortCriterias(const QTodoSortCriteriaMap*);
	QTodoItem* top_item;

	private:
	virtual int compareItems(QPtrCollection::Item, QPtrCollection::Item);

	const QTodoSortCriteriaMap* sort_criterias;
};

#endif

