#include "qupdateplugin.h"

#include "net/qnetupdate.h"

#include <qplugin.h>

QString QUpdatePlugin::get_plugin_name()
{
	return UPDATE_PLUGIN_BASE_NAME;
}

QAbstractUpdate *QUpdatePlugin::create(const QString &name)
{
	QAbstractUpdate *obj = NULL;

	if (name == "Update-Net")
	{
		obj = new QNetUpdate;
	}

	return obj;
}

QStringList QUpdatePlugin::supports()
{
	QStringList list;
	list << "Update-Net";
	return list;
}

Q_EXPORT_PLUGIN2(QUpdatePlugin, QUpdatePlugin)
