#ifndef QNETUPDATER_H
#define QNETUPDATER_H

#include "../../libs/baselib/qabstractupdater.h"

#include <QObject>
#include <QUdpSocket>

class QFindServer;
class QHeartbest;
class QDataResever;

class QNetUpdater : public QAbstractUpdater
{
	Q_OBJECT
public:
	explicit QNetUpdater(QObject *parent = 0);
	~QNetUpdater();
	void    start();
	void    stop();

	QString type();
	void    setName(const QString &name);
signals:
protected slots:
	void    searchResult(uint address);
	void    offline();
	void    readyRead();
protected:
	QFindServer     *m_searcher;
	QHeartbest      *m_heartbest;
	QDataResever    *m_dataResever;
	uint             m_serverAddress;
	QUdpSocket      *m_socket;
};

#endif // QNETUPDATER_H
