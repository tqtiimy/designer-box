#include "qnetupdater.h"

#include "qfindserver.h"
#include "qheartbest.h"
#include "basedefine.h"
#include "qdataresever.h"

#include <QHostInfo>
#include <QTimer>
#include <QMessageBox>
#include <QHostAddress>

QNetUpdater::QNetUpdater(QObject *parent) :
	QAbstractUpdater(parent),
	m_searcher(NULL),
	m_heartbest(NULL),
	m_dataResever(NULL),
	m_socket(new QUdpSocket)
{
	m_socket->bind(WAIT_SERVER_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

QNetUpdater::~QNetUpdater()
{
	m_socket->close();
	delete m_socket;

	if (m_searcher != NULL)
	{
		delete m_searcher;
		m_searcher = NULL;
	}

	offline();
}

void QNetUpdater::start()
{
	if (m_state != STATE_OFFLINE)
	{
		return;
	}

	m_searcher = new QFindServer(m_name);
	connect(m_searcher, SIGNAL(result(uint)), this, SLOT(searchResult(uint)));
	m_state = STATE_OFFLINE;
	m_searcher->searchServer();
}

void QNetUpdater::stop()
{
}

void QNetUpdater::searchResult(uint address)
{
	m_serverAddress = address;

	if (address != 0)
	{
		m_heartbest = new QHeartbest(m_name, m_serverAddress, this);
		connect(m_heartbest, SIGNAL(offline()), this, SLOT(offline()));
		m_dataResever = new QDataResever(m_name, m_serverAddress, this);
		connect(m_dataResever, SIGNAL(finishSignal()), this, SIGNAL(updateFinish()));
		m_state = STATE_ONLINE;
	}
	else
	{
		m_state = STATE_OFFLINE;
	}

	m_searcher->deleteLater();
	m_searcher = NULL;
}

QString QNetUpdater::type()
{
	return "Net";
}

void QNetUpdater::offline()
{
	m_state = STATE_OFFLINE;

	if (m_heartbest != NULL)
	{
		delete m_heartbest;
		m_heartbest = NULL;
	}

	if (m_dataResever != NULL)
	{
		delete m_dataResever;
		m_dataResever = NULL;
	}
}

void QNetUpdater::setName(const QString &name)
{
	QString str;
	str = QHostInfo::localHostName();
	str = str + "-" + type() + "-" + name;
	QAbstractUpdater::setName(str);
}

void QNetUpdater::readyRead()
{
	char data[256] = {0};
	int count = m_socket->readDatagram(data, 256);

	if (m_state == STATE_OFFLINE && count > 0)
	{
		data[count] = 0;
		QString str = data;
		int index = str.indexOf(":");

		if (index > 0)
		{
			if (str.left(index).toInt() == SERVER_CODE)
			{
				start();
			}
		}
	}
}
