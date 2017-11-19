#ifndef QHEARTBEST_H
#define QHEARTBEST_H

#include <QObject>

class QUdpSocket;

class QHeartbest : public QObject
{
	Q_OBJECT
public:
	explicit QHeartbest(QObject *parent = 0);

signals:
	void    heartbest(int addr, const QString &name);
protected slots:
	void    readyRead();
protected:
	QUdpSocket      *m_heartbest;
};

#endif // QHEARTBEST_H
