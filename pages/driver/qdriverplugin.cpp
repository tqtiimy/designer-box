#include "qdriverplugin.h"

#include "qdriverwidget.h"

#include <qplugin.h>
#include <QIcon>
#include <QVariant>

void QDriverPlugin::create_widget()
{
	m_widget = new QDriverWidget;
	m_widget->setProperty("panelwidget", true);
	((QDriverWidget *)m_widget)->set_undo_stack(m_undo_stack);
}

QString QDriverPlugin::get_page_name()
{
	return tr("Driver");
}

QString QDriverPlugin::get_plugin_name()
{
	return DRIVER_PLUGIN_NAME;
}

QIcon QDriverPlugin::get_page_icon()
{
	return QIcon(":/images/driver.png");
}

int QDriverPlugin::get_index()
{
	return 4;
}

bool QDriverPlugin::get_enabled()
{
	return false;
}

Q_EXPORT_PLUGIN2(QDriverPlugin, QDriverPlugin)
