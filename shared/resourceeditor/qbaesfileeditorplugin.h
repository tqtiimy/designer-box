#ifndef QBAESFILEEDITORPLUGIN_H
#define QBAESFILEEDITORPLUGIN_H

#include "../../libs/core/qabstracteditorplugin.h"

#include <QObject>

#define EDITOR_PLUGIN_BASE_NAME "Plugin.Resource.Editor.Base"

class QBaesFileEditorPlugin : public QAbstractEditorPlugin
{
	Q_OBJECT
public:
	virtual QString     get_plugin_name();//获取页面的名称


	virtual QAbstractResourceEditor    *create(const QString &name);
	virtual QStringList supports();
};

#endif // QBAESFILEEDITORPLUGIN_H
