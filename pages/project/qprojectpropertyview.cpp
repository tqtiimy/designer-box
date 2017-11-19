#include "qprojectpropertyview.h"


#include "../../libs/core/qpropertylist/qpropertylistview.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/shared/property/qabstractproperty.h"
#include "../../libs/shared/host/qabstracthost.h"
#include "../../libs/core/undocommand/qpropertychangedundocommand.h"

#include <QVBoxLayout>
#include <QLabel>

QProjectPropertyView::QProjectPropertyView(QWidget *parent) :
	QWidget(parent),
	m_propertyView(new QPropertyListView(this)),
	m_styledBar(new StyledBar(this))
{

	QVBoxLayout *l = new QVBoxLayout();
	l->setMargin(0);
	l->setSpacing(0);

	l->addWidget(m_styledBar);
	l->addWidget(m_propertyView);

	this->setLayout(l);
	this->resize(200, 200);

	QLabel *label = new QLabel("Project Property");
	label->setStyleSheet("color:rgb(255,255,255);");
	l = new QVBoxLayout();
	l->setMargin(5);
	l->setSpacing(0);
	l->addWidget(label);
	m_styledBar->setLayout(l);

	connect(QSoftCore::get_core()->get_project_core(), SIGNAL(opened_signals()), this, SLOT(project_opened()));
	connect(QSoftCore::get_core()->get_project_core(), SIGNAL(closed_signals()), this, SLOT(project_closed()));
	connect(m_propertyView, SIGNAL(property_edit_signal(QAbstractProperty *, QVariant)), this, SLOT(property_edit(QAbstractProperty *, QVariant)));
}

void QProjectPropertyView::project_opened()
{
	QAbstractHost *host = QSoftCore::get_core()->get_project_core()->get_project_host();
	m_propertyView->setPropertys(host->get_propertys());
}

void QProjectPropertyView::project_closed()
{
	m_propertyView->clearAll();
}

void QProjectPropertyView::property_edit(QAbstractProperty *pro, const QVariant &v)
{
	if (pro != NULL)
	{
		QPropertyChangedUndoCommand *cmd = new QPropertyChangedUndoCommand(QSoftCore::get_core()->get_project_core()->get_project_host()->get_uuid(),
		        pro->get_property("name").toString(), pro->get_value(), v);
		m_undo_stack->push(cmd);
	}
}

void QProjectPropertyView::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
	m_propertyView->set_undo_stack(m_undo_stack);
}
