#ifndef QLISTENDEVICE_H
#define QLISTENDEVICE_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QTimer>

class QListenDevice : public QObject
{
	Q_OBJECT
public:
	explicit QListenDevice(QObject *parent = 0);
	~QListenDevice();
protected:
signals:
	void    findDevice(int address, const QString &name);
protected slots:
	void    readyRead();
	void    finish();
protected:
	QUdpSocket  *m_listenSocket;
	QTimer      *m_timer;
};

#endif // QLISTENDEVICE_H
