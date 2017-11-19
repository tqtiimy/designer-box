#include "qdataresever.h"

#include "basedefine.h"

#include <QHostAddress>
#include <QFile>
#include <QApplication>
#include <QMessageBox>

QDataResever::QDataResever(const QString name, uint address, QObject *parent):
	QObject(parent),
	m_socket(new QUdpSocket),
	m_address(address),
	m_name(name)
{
	m_socket->bind(SEND_FILE_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

QDataResever::~QDataResever()
{
	m_socket->close();
	delete m_socket;
}

void QDataResever::readyRead()
{
	char data[4200] = {0};
	QHostAddress addr;
	int count = m_socket->readDatagram(data, 4200, &addr);

	if (addr.toIPv4Address() == m_address && count > 0)
	{
		int index = 0;

		for (int i = 0; i < count; i++)
		{
			if (data[i] == ':')
			{
				data[i] = 0;
				index = i + 1;
				break;
			}
		}

		int code = QString(data).toInt();

		if (code == 0 || index >= count)
		{
			return;
		}

		if (code == START_UPDATE_CODE)
		{
			clear();
			send(QString("%1:%2").arg(code).arg(m_name).toLocal8Bit());
		}
		else if (code == FINISH_UPDATE_CODE)
		{
			if (finish())
			{
				send(QString("%1:%2").arg(code).arg(m_name).toLocal8Bit());
				emit finishSignal();
			}
			else
			{
				clear();
				send(QString("%1:%2").arg(FAILED_SEND_CODE).arg(m_name).toLocal8Bit());
			}
		}
		else if (code == GIVE_UP_UPDATE_CODE)
		{
			clear();
			send(QString("%1:%2").arg(code).arg(m_name).toLocal8Bit());
		}
		else if (code == FILE_UPDATE_CODE)
		{
			int sum = 0;

			for (int i = index; i < count - 2; i++)
			{
				sum += (data[i] & 0xffff);
			}

			sum = sum & 0xffff;

			if (sum != ((data[count - 2] & 0xff) << 8) + (data[count - 1] & 0xff))
			{
				clear();
				send(QString("%1:%2").arg(FAILED_UPDATE_CODE).arg(m_name).toLocal8Bit());
			}
			else
			{
				QByteArray array(data + index, count - index - 2);
				m_fileBuffer += array;
				send(QString("%1:%2").arg(code).arg(m_name).toLocal8Bit());
			}
		}
	}
}

void QDataResever::clear()
{
	m_fileBuffer.clear();
}

void QDataResever::send(const QByteArray &data)
{
	m_socket->writeDatagram(data, QHostAddress(m_address), SEND_FILE_WRITE_PORT);
}

bool QDataResever::finish()
{
	if (m_fileBuffer.size() < 6)
	{
		return false;
	}

	QFile file(QApplication::applicationDirPath() + "/temp.tmp");

	if (file.open(QFile::WriteOnly))
	{
		file.resize(0);
		file.write(m_fileBuffer);
		file.close();
		clear();
		return true;
	}

	return false;
}
