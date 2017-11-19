#ifndef QDRIVERWIDGET_H
#define QDRIVERWIDGET_H

#include "../../libs/core/qabstractpagewidget.h"

#include <QStackedLayout>
#include <QMap>

class QDriverTabView;
class QAbstractDriver;
class QDriverView;

class QDriverWidget : public QAbstractPageWidget
{
	Q_OBJECT
public:
	QDriverWidget(QWidget *parent = 0);

	void    set_undo_stack(QUndoStack *stack);
protected slots:
	void project_opened();
	void project_closed();
	void add_driver();
	void remove_driver();

	void    insert_driver_slot(QAbstractDriver *driver, int index);
	void    remove_driver_slot(QAbstractDriver *driver);

	void    widget_active();

	void    start_remove();
	void    end_remove();
protected:
	QDriverTabView      *m_tab_bar;
	QStackedLayout          *m_widget_stack;

	QMap<QAbstractDriver *, QDriverView *>   m_driver_to_widget;
};
#endif // QDRIVERWIDGET_H
