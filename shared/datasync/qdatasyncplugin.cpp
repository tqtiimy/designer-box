#include "qdatasyncplugin.h"

#include <qplugin.h>

QString QDataSyncPlugin::get_plugin_name()
{
	return DATA_SYNC_PLUGIN_NAME;
}

QString QDataSyncPlugin::get_plugin_type()
{
	return DATA_SYNC_PLUGIN;
}

void QDataSyncPlugin::init_plugin()
{

}

Q_EXPORT_PLUGIN2(QDataSyncPlugin, QDataSyncPlugin)
