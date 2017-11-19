#ifndef QUPDATERPLUGIN_H
#define QUPDATERPLUGIN_H

#include "../../libs/baselib/qupdateplugin.h"

class QAbstractUpdater;

class QNetUpdaterPlugin: public QUpdatePlugin
{
	Q_OBJECT
public:
	virtual QAbstractUpdater *createUpdater();
	virtual QString           name();
};

#endif // QUPDATERPLUGIN_H
