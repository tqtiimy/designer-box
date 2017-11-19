#include "qupdatemanager.h"

#include "qabstractupdater.h"
#include "qupdateplugin.h"

#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QPluginLoader>

QUpdateManager::QUpdateManager(QObject *parent) :
	QObject(parent)
{
}

QUpdateManager::~QUpdateManager()
{
	qDeleteAll(m_updaters.values());
	m_updaters.clear();
}

void QUpdateManager::loadUpdatePlugin()
{
	QString path = QApplication::applicationDirPath();
	path += "/device/updaters/"; //获取插件路径

	QDir dir(path);
	QFileInfoList list = dir.entryInfoList(QDir::Files);

	for (int i = 0; i < list.size(); i++)
	{
		//遍历插件
		QPluginLoader load(list.at(i).filePath());

		if (load.load())
		{
			//加载插件
			QUpdatePlugin *plugin = qobject_cast<QUpdatePlugin *>(load.instance());

			if (plugin != NULL)
			{
				qDebug(plugin->name().toLocal8Bit());
				QAbstractUpdater *updater = plugin->createUpdater();

				if (updater != NULL)
				{
					connect(updater, SIGNAL(updateFinish()), this, SIGNAL(updateFinish()));
					m_updaters.insert(plugin->name(), updater);
				}
			}
		}
	}
}

void QUpdateManager::updaterStart(const QString &name)
{
	foreach(QAbstractUpdater * updater, m_updaters.values())
	{
		updater->setName(name);
		updater->start();
	}
}

QStringList QUpdateManager::supports()
{
	return m_updaters.keys();
}

QAbstractUpdater *QUpdateManager::getUpdater(const QString &name)
{
	return m_updaters.value(name, NULL);
}
