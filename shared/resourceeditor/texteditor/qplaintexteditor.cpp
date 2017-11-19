#include "qplaintexteditor.h"

#include "qtexteditview.h"

#include "../../../libs/core/qfilemanager.h"

#include <QHBoxLayout>

QPlainTextEditor::QPlainTextEditor(QWidget *parent) :
	QAbstractResourceEditor(parent),
	m_editor(new QTextEditView)
{
	QHBoxLayout *h = new QHBoxLayout;
	h->setMargin(0);
	h->setSpacing(0);

	h->addWidget(m_editor);

	this->setLayout(h);
}

QString QPlainTextEditor::show_name()
{
	return tr("Plain Text Editor");
}

QWidget *QPlainTextEditor::tool_bar()
{
	return NULL;
}

bool QPlainTextEditor::support(const QString &type)
{
	QStringList list;
	list << "txt" << "xml" << "cpp" << "h";
	return list.indexOf(type) >= 0;
}

void QPlainTextEditor::set_file(tagFileInfo *info)
{
	m_editor->setReadOnly(true);

	if (info != NULL)
	{
		QString buff = QString::fromLocal8Bit(info->m_file_data);
		m_editor->setPlainText(buff);
	}
	else
	{
		m_editor->setPlainText("");
	}
}
