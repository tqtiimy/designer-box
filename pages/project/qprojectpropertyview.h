#ifndef QPROJECTPROPERTYVIEW_H
#define QPROJECTPROPERTYVIEW_H

#include <QWidget>
#include <QUndoStack>

class QPropertyListView;
class StyledBar;
class QAbstractProperty;

class QProjectPropertyView : public QWidget
{
	Q_OBJECT
public:
	explicit QProjectPropertyView(QWidget *parent = 0);

	void    set_undo_stack(QUndoStack *stack);
protected slots:
	void    project_opened();
	void    project_closed();
	void    property_edit(QAbstractProperty *pro, const QVariant &v);
protected:
	QPropertyListView  *m_propertyView;
	StyledBar          *m_styledBar;
	QUndoStack          *m_undo_stack;
};

#endif // QPROJECTPROPERTYVIEW_H
