#include "qfindserver.h"

#include "basedefine.h"

#include <QHostAddress>
#include <QTime>
#include <QMessageBox>

QFindServer::QFindServer(const QString &name):
	QObject(NULL),
	m_socket(new QUdpSocket),
	m_name(name),
	m_timeline(new QTimeLine(1000)),
	m_timeout(0)
{
	m_socket->bind(SEARCH_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_timeline, SIGNAL(finished()), this, SLOT(timeout()));
}

QFindServer::~QFindServer()
{
	m_socket->close();
	delete m_timeline;
	delete m_socket;
}

void QFindServer::readyRead()
{
	char data[256];
	QHostAddress addr;
	int count = m_socket->readDatagram(data, 256, &addr);

	if (m_timeout == 0)
	{
		return;
	}

	if (count > 0)
	{
		data[count] = 0;
		QString str = data;
		int index = str.indexOf(":");
		QString code = str.left(index);
		QString value = str.mid(index + 1);

		if (code.toInt() == SERVER_CODE && value == "I am server!")
		{
			m_timeout = 0;
			m_timeline->stop();
			emit result(addr.toIPv4Address());
		}
	}
}

void QFindServer::searchServer()
{
	m_timeout = 3;
	QString str;
	str = QString::number(CLIENT_CODE) + ":" + m_name;

	if (m_timeline->state() == QTimeLine::Running)
	{
		m_timeline->stop();
	}

	m_timeline->setCurrentTime(0);
	m_timeline->start();
	m_socket->writeDatagram(str.toLocal8Bit(), QHostAddress::Broadcast, SEARCH_WRITE_PORT);
}

void QFindServer::timeout()
{
	m_timeout--;

	if (m_timeout == 0)
	{
		emit result(0);
		return;
	}

	QString str;
	str = QString::number(CLIENT_CODE) + ":" + m_name;

	if (m_timeline->state() == QTimeLine::Running)
	{
		m_timeline->stop();
	}

	m_timeline->setCurrentTime(0);
	m_timeline->start();
	m_socket->writeDatagram(str.toLocal8Bit(), QHostAddress::Broadcast, SEARCH_WRITE_PORT);
}
