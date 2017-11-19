#include "qeditorwidget.h"

#include "qstyledbarwidget.h"

#include "../../libs/shared/pluginloader.h"
#include "../../libs/core/qabstracteditorplugin.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/core/qabstractresourceeditor.h"
#include "../../libs/core/qfilemanager.h"

#include <QStackedLayout>

QEditorWidget::QEditorWidget(QWidget *parent) :
	QWidget(parent),
	m_styled_bar(new StyledBar),
	m_current_file(NULL)
{
	QMap<QString, QAbstractPlugin *> list = PluginLoader::get_plugin_by_type(RESOURCE_EDIOTR_PLUGIN_TYPE);

	m_stacked_layout = new QStackedLayout;
	m_stacked_layout->setMargin(0);
	m_stacked_layout->setSpacing(0);

	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);
	v->addWidget(m_styled_bar);
	v->addLayout(m_stacked_layout);

	this->setLayout(v);

	QMapIterator<QString, QAbstractPlugin *> it(list);
	QAbstractResourceEditor *wid;

	while (it.hasNext())
	{
		it.next();
		QAbstractEditorPlugin *plugin = (QAbstractEditorPlugin *)it.value();
		QStringList l = plugin->supports();
		foreach(const QString & s, l)
		{
			wid = plugin->create(s);

			if (wid != NULL)
			{
				if (m_widgets.keys().contains(wid->show_name()))
				{
					delete wid;
				}
				else
				{
					m_widgets.insert(wid->show_name(), wid);
					m_stacked_layout->insertWidget(-1, wid);
				}
			}
		}
	}

	m_styled_widget = new QStyledBarWidget(m_widgets.keys());
	foreach(QAbstractResourceEditor * e, m_widgets.values())
	{
		m_styled_widget->add_tool_bar(e->tool_bar());
	}

	v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);
	v->addWidget(m_styled_widget);
	m_styled_bar->setLayout(v);

	connect(m_styled_widget, SIGNAL(editor_select(QString)), this, SLOT(show_editor(QString)));

	if (m_widgets.size() > 0)
	{
		m_styled_widget->set_current_editor(m_widgets.keys().first());
	}
}

void QEditorWidget::show_editor(const QString &editor)
{
	QAbstractResourceEditor *e = m_widgets.value(editor);

	if (e != NULL)
	{
		m_stacked_layout->setCurrentWidget(e);
		m_styled_widget->show_tool_bar(e->tool_bar());
	}
}

void QEditorWidget::select_file(tagFileInfo *file)
{
	if (file == m_current_file)
	{
		return;
	}

	m_current_file = file;

	QString select;


	QMapIterator<QString, QAbstractResourceEditor *>      it(m_widgets);

	while (it.hasNext())
	{
		it.next();
		it.value()->set_file(m_current_file);

		if (select == "" && file != NULL && it.value()->support(file->m_exp))
		{
			select = it.key();
		}
	}

	if (select == "")
	{
		if (m_widgets.size() > 0)
		{
			m_styled_widget->set_current_editor(m_widgets.keys().first());
		}
	}
	else
	{
		m_styled_widget->set_current_editor(select);
	}

	if (m_current_file != NULL)
	{
		m_styled_widget->set_file_name(file->m_file_name);
	}
}
