#include "qdataplugin.h"

#include "qdatawidget.h"

#include <qplugin.h>
#include <QIcon>
#include <QVariant>

void QDataPlugin::create_widget()
{
	m_widget = new QDataWidget;
	m_widget->setProperty("panelwidget", true);
	((QDataWidget *)m_widget)->set_undo_stack(m_undo_stack);
}

QString QDataPlugin::get_page_name()
{
	return tr("Data");
}

QString QDataPlugin::get_plugin_name()
{
	return DATA_PLUGIN_NAME;
}

QIcon QDataPlugin::get_page_icon()
{
	return QIcon(":/images/data.png");
}

int QDataPlugin::get_index()
{
	return 3;
}

bool QDataPlugin::get_enabled()
{
	return false;
}

Q_EXPORT_PLUGIN2(QDataPlugin, QDataPlugin)
