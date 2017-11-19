#include "qnetupdaterplugin.h"

#include "qnetupdater.h"

#include <qplugin.h>
#include <QMessageBox>

QAbstractUpdater *QNetUpdaterPlugin::createUpdater()
{
	return new QNetUpdater;
}

QString QNetUpdaterPlugin::name()
{
	return "Net Updater";
}

Q_EXPORT_PLUGIN2(QNetUpdaterPlugin, QNetUpdaterPlugin)
