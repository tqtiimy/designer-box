#include "qdriverview.h"

#include "qdriverdatalistview.h"

#include "../../libs/core/qpropertylist/qpropertylistview.h"
#include "../../libs/shared/property/qabstractproperty.h"
#include "../../libs/core/minisplitter.h"
#include "../../libs/core/undocommand/qpropertychangedundocommand.h"
#include "../../libs/shared/driver/qabstractdriver.h"

#include <QVBoxLayout>

QDriverView::QDriverView(QAbstractDriver *driver, QWidget *parent) :
	QWidget(parent),
	m_property_list_view(new QPropertyListView),
	m_driver(driver),
	m_data_list_view(new QDriverDataListView(driver))
{
	MiniSplitter *s = new MiniSplitter;

	s->addWidget(m_property_list_view);
	s->addWidget(m_data_list_view);

	s->setStretchFactor(0, 0);
	s->setStretchFactor(1, 1);


	QVBoxLayout *v = new QVBoxLayout;

	v->setMargin(0);
	v->setSpacing(0);

	v->addWidget(s);

	this->setLayout(v);

	m_property_list_view->setPropertys(driver->get_propertys());

	connect(m_property_list_view, SIGNAL(property_edit_signal(QAbstractProperty *, QVariant)),
	        this, SLOT(property_edit_signal(QAbstractProperty *, QVariant)));

	connect(m_property_list_view, SIGNAL(active()), this, SIGNAL(active()));
	connect(m_data_list_view, SIGNAL(active()), this, SIGNAL(active()));
	connect(m_data_list_view, SIGNAL(start_remove()), this, SIGNAL(start_remove()));
	connect(m_data_list_view, SIGNAL(end_remove()), this, SIGNAL(end_remove()));
}

void QDriverView::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
	m_data_list_view->set_undo_stack(stack);
}

void QDriverView::property_edit_signal(QAbstractProperty *pro, const QVariant &value)
{
	if (pro != NULL)
	{
		QPropertyChangedUndoCommand *cmd = new QPropertyChangedUndoCommand(pro->get_host()->get_uuid(),
		        pro->get_property("name").toString(), pro->get_value(), value);
		m_undo_stack->push(cmd);
	}
}
