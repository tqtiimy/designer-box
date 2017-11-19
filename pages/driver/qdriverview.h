#ifndef QDRIVERVIEW_H
#define QDRIVERVIEW_H

#include <QWidget>
#include <QUndoStack>

class QPropertyListView;
class QAbstractDriver;
class QDriverDataListView;
class QAbstractProperty;

class QDriverView : public QWidget
{
	Q_OBJECT
public:
	explicit QDriverView(QAbstractDriver *driver, QWidget *parent = 0);
	void    set_undo_stack(QUndoStack *stack);
public slots:
	void    property_edit_signal(QAbstractProperty *pro, const QVariant &value);
signals:
	void active();
	void start_remove();
	void end_remove();
protected:
	QPropertyListView       *m_property_list_view;
	QAbstractDriver         *m_driver;
	QDriverDataListView     *m_data_list_view;
	QUndoStack              *m_undo_stack;
};

#endif // QDRIVERVIEW_H
