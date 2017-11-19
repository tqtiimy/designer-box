#include "qprojectplugin.h"

#include "qprojectwidget.h"


#include <qplugin.h>
#include <QIcon>
#include <QVariant>

void QProjectPlugin::create_widget()
{
	m_widget = new QProjectWidget;
	m_widget->setProperty("panelwidget", true);
	((QProjectWidget *)m_widget)->set_undo_stack(m_undo_stack);
}

QString QProjectPlugin::get_page_name()
{
	return tr("Project");
}

QString QProjectPlugin::get_plugin_name()
{
	return PROJECT_PLUGIN_NAME;
}

QIcon QProjectPlugin::get_page_icon()
{
	return QIcon(":/images/project.png");
}

int QProjectPlugin::get_index()
{
	return 1;
}

bool QProjectPlugin::get_enabled()
{
	return true;
}

Q_EXPORT_PLUGIN2(QProjectPlugin, QProjectPlugin)
