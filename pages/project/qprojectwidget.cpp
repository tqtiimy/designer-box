#include "qprojectwidget.h"

#include "qprojectpropertyview.h"
#include "qusermanagerview.h"
#include "qlanguagemanagerview.h"

#include "../../libs/core/minisplitter.h"


#include <QVBoxLayout>

QProjectWidget::QProjectWidget():
	QAbstractPageWidget(0),
	m_splitter(new MiniSplitter),
	m_property_view(new QProjectPropertyView(this)),
	m_user_view(new QUserManagerView(this)),
	m_language_view(new QLanguageManagerView(this))
{
	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);

	m_splitter->addWidget(m_property_view);


	MiniSplitter *s = new MiniSplitter(Qt::Vertical);

	s->addWidget(m_user_view);
	s->addWidget(m_language_view);

	m_splitter->addWidget(s);

	m_splitter->setStretchFactor(0, 0);
	m_splitter->setStretchFactor(1, 1);

	v->addWidget(m_splitter);


	setLayout(v);
}

void QProjectWidget::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
	m_user_view->set_undo_stack(m_undo_stack);
	m_language_view->set_undo_stack(m_undo_stack);
	m_property_view->set_undo_stack(m_undo_stack);
}
