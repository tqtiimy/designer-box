#ifndef QDATASYNCPLUGIN_H
#define QDATASYNCPLUGIN_H

#include "../../libs/shared/qabstractplugin.h"

#include <QObject>

#define DATA_SYNC_PLUGIN_NAME "Plugin.Data.Sync"
#define DATA_SYNC_PLUGIN      "Plugin.DataSync"

class QDataSyncPlugin : public QAbstractPlugin
{
	Q_OBJECT
public:
	virtual void        init_plugin();//初始化插件
	virtual QString     get_plugin_name();//获取页面的名称
	virtual QString     get_plugin_type();
};

#endif // QUPDATEPLUGIN_H
