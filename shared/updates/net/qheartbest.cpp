#include "qheartbest.h"

#include "basedefine.h"

#include <QUdpSocket>
#include <QHostAddress>

QHeartbest::QHeartbest(QObject *parent) :
	QObject(parent),
	m_heartbest(new QUdpSocket(this))
{
	m_heartbest->bind(HEARTBEAT_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_heartbest, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void QHeartbest::readyRead()
{
	char data[256];
	QHostAddress addr;
	int count = m_heartbest->readDatagram(data, 256, &addr);

	if (count > 0)
	{
		data[count] = 0;
		int index;
		QString dataString(data);
		index = dataString.indexOf(":");

		if (index > 0)
		{
			QString code = dataString.left(index);
			QString name = dataString.mid(index + 1);

			if (code.toInt() == HEARTBEST_CODE)
			{
				m_heartbest->writeDatagram(data, count, addr, HEARTBEAT_WRITE_PORT);
				emit heartbest(addr.toIPv4Address(), name);
			}
		}
	}
}
