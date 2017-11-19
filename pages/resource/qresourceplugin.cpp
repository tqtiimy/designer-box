#include "qresourceplugin.h"

#include "qresourcewidget.h"

#include <qplugin.h>
#include <QIcon>
#include <QVariant>

void QResourcePlugin::create_widget()
{
	m_widget = new QResourceWidget;
	m_widget->setProperty("panelwidget", true);
	((QResourceWidget *)m_widget)->set_undo_stack(m_undo_stack);
}

QString QResourcePlugin::get_page_name()
{
	return tr("Resource");
}

QString QResourcePlugin::get_plugin_name()
{
	return RESOURCE_PLUGIN_NAME;
}

QIcon QResourcePlugin::get_page_icon()
{
	return QIcon(":/images/resource.png");
}

int QResourcePlugin::get_index()
{
	return 5;
}

bool QResourcePlugin::get_enabled()
{
	return true;
}

Q_EXPORT_PLUGIN2(QResourcePlugin, QResourcePlugin)
