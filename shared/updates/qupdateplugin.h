#ifndef QUPDATEPLUGIN_H
#define QUPDATEPLUGIN_H

#include "../../libs/shared/qabstractupdateplugin.h"

#include <QObject>

#define UPDATE_PLUGIN_BASE_NAME "Plugin.Update.Base"

class QUpdatePlugin : public QAbstractUpdatePlugin
{
	Q_OBJECT
public:
	virtual QString     get_plugin_name();//获取页面的名称


	virtual QAbstractUpdate    *create(const QString &name);
	virtual QStringList supports();
};

#endif // QUPDATEPLUGIN_H
