#include "qresourcewidget.h"

#include "resourcelist/qresourcefilelist.h"
#include "qeditorwidget.h"

#include "../../libs/core/minisplitter.h"

#include <QVBoxLayout>

QResourceWidget::QResourceWidget():
	QAbstractPageWidget(0),
	m_resource_view(new QResourceFileList),
	m_editor(new QEditorWidget)
{
	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);

	MiniSplitter *s = new MiniSplitter;
	m_resource_view->resize(200, 200);
	s->addWidget(m_resource_view);
	s->addWidget(m_editor);

	s->setStretchFactor(0, 0);
	s->setStretchFactor(1, 1);

	v->addWidget(s);

	this->setLayout(v);

	connect(m_resource_view, SIGNAL(select(tagFileInfo *)), m_editor, SLOT(select_file(tagFileInfo *)));
}

void QResourceWidget::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
	m_resource_view->set_undo_stack(stack);
}

