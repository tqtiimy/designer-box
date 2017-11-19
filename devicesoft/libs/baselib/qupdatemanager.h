#ifndef QUPDATEMANAGER_H
#define QUPDATEMANAGER_H

#include "baselibglobal.h"

#include <QObject>
#include <QMap>

class QUpdatePlugin;
class QAbstractUpdater;

class BASELIB_EXPORT QUpdateManager : public QObject
{
	Q_OBJECT
public:
	explicit QUpdateManager(QObject *parent = 0);
	~QUpdateManager();
	void    loadUpdatePlugin();
	void    updaterStart(const QString &name);
	QStringList supports();
	QAbstractUpdater   *getUpdater(const QString &name);
signals:
	void    updateFinish();
protected:
	QMap<QString, QAbstractUpdater *>    m_updaters;
};

#endif // QUPDATEMANAGER_H
