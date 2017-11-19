#ifndef QABSTRACTUPDATER_H
#define QABSTRACTUPDATER_H

#include "baselibglobal.h"

#include <QObject>

enum enState
{
    STATE_ONLINE,
    STATE_LINING,
    STATE_OFFLINE
};

class BASELIB_EXPORT QAbstractUpdater : public QObject
{
	Q_OBJECT
public:
	explicit QAbstractUpdater(QObject *parent = 0);
	~QAbstractUpdater();

	virtual void    setName(const QString &name);
	virtual QString type() = 0;
	virtual void    start();
	virtual void    stop();
signals:
	void    updateFinish();
public slots:
protected:
	QString     m_name;
	enState     m_state;
};

#endif // QABSTRACTUPDATER_H
