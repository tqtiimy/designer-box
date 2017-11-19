#ifndef QDATAWIDGET_H
#define QDATAWIDGET_H

#include "../../libs/core/qabstractpagewidget.h"

#include <QStackedLayout>
#include <QMap>

class QData;
class QDataViewTabWidget;

class QDataWidget : public QAbstractPageWidget
{
	Q_OBJECT
public:
	explicit QDataWidget(QWidget *parent = 0);

	void    set_undo_stack(QUndoStack *stack);
signals:

protected slots:
	void    project_opened();
	void    project_closed();

	void    start_remove();
	void    end_remove();

	void    add_group();
	void    remove_group();

	void    group_add_slot(QData *data, int index);
	void    group_remove_slot(QData *data);

	void    group_activity();
protected:
	QDataViewTabWidget      *m_tab_bar;
	QStackedLayout          *m_widget_stack;

	QMap<QData *, QWidget *>   m_data_to_widget;
};

#endif // QDATAWIDGET_H
