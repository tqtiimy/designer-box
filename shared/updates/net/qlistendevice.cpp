#include "qlistendevice.h"

#include "basedefine.h"

#include <QDateTime>

QListenDevice::QListenDevice(QObject *parent) :
	QObject(parent),
	m_listenSocket(new QUdpSocket(this)),
	m_timer(new QTimer(this))
{
	m_listenSocket->bind(LISTEN_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_listenSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

	connect(m_timer, SIGNAL(timeout()), this, SLOT(finish()));

	m_timer->start(1000);
}

QListenDevice::~QListenDevice()
{
	delete m_listenSocket;
	m_listenSocket = NULL;
}

void QListenDevice::readyRead()
{
	QHostAddress addr;
	char data[255];
	int count = m_listenSocket->readDatagram(data, 255, &addr);
	QString dataString;

	if (count > 0)
	{
		data[count] = 0;
		dataString = data;
		int index = dataString.indexOf(":");

		if (index > 0)
		{
			QString code = dataString.left(index);
			QString name = dataString.mid(index + 1);

			if (code.toInt() == CLIENT_CODE)
			{
				emit findDevice(addr.toIPv4Address(), name);
				QString str = QString("%1:%2").arg(SERVER_CODE).arg("I am server!");
				m_listenSocket->writeDatagram(str.toLocal8Bit(), addr, LISTEN_WRITE_PORT);
			}
		}
	}
}

void QListenDevice::finish()
{
	static int time = 4;
	time--;

	if (time == 0)
	{
		m_timer->stop();
		return;
	}

	QString str = QString("%1:%2").arg(SERVER_CODE).arg("I am server!");
	m_listenSocket->writeDatagram(str.toLocal8Bit(), QHostAddress::Broadcast, SERVER_START_PORT);
}
