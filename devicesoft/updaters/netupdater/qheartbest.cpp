#include "qheartbest.h"

#include "basedefine.h"

#include <QHostAddress>
#include <QTime>
#include <QMessageBox>

QHeartbest::QHeartbest(const QString &name, uint address, QObject *parent):
	QObject(parent),
	m_socket(new QUdpSocket),
	m_address(address),
	m_name(name),
	m_times(5),
	m_timer(new QTimer)
{
	m_socket->bind(HEARTBEAT_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	m_timer->start(1000);
}


QHeartbest::~QHeartbest()
{
	m_socket->close();
	delete m_socket;
	delete m_timer;
}

void QHeartbest::readyRead()
{
	char data[256];
	QHostAddress    address;
	int count = m_socket->readDatagram(data, 256, &address);

	if (address.toIPv4Address() == m_address && count > 0)
	{
		data[count] = 0;
		QString str = data;
		int s = str.indexOf(":");

		if (s > 0)
		{
			QString code = str.left(s);
			QString value = str.mid(s + 1);

			if (code.toInt() == HEARTBEST_CODE && value == m_name)
			{
				m_times = 5;
			}
		}
	}
}

void QHeartbest::timeout()
{
	if (m_times <= 0)
	{
		m_timer->stop();
		emit offline();
		return;
	}

	m_times--;
	QString str = QString::number(HEARTBEST_CODE) + ":" + m_name;
	m_socket->writeDatagram(str.toLocal8Bit(), QHostAddress(m_address), HEARTBEAT_WRITE_PORT);
}
