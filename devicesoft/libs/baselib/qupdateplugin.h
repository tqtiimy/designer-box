#ifndef QUPDATEPLUGIN_H
#define QUPDATEPLUGIN_H

#include "baselibglobal.h"

#include <QObject>

class QAbstractUpdater;

class BASELIB_EXPORT QUpdatePlugin : public QObject
{
	Q_OBJECT
public:
	virtual QAbstractUpdater *createUpdater() = 0;
	virtual QString           name() = 0;
signals:

public slots:

};

#define QUpdatePlugin_iid "QUpdatePlugin"

Q_DECLARE_INTERFACE(QUpdatePlugin, QUpdatePlugin_iid)//定义接口

#endif // QUPDATEPLUGIN_H
