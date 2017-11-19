#include "qsendfile.h"
#include "basedefine.h"

#include <QHostAddress>
#include <QVariant>
#include <QFile>
#include <QApplication>

QSendFile::QSendFile(QObject *parent) :
	QObject(parent),
	m_socket(new QUdpSocket(this)),
	m_position(0),
	m_address(0),
	m_deviceName(""),
	m_tryTimes(4),
	m_device(NULL)
{
	m_socket->bind(SEND_FILE_READ_PORT, QUdpSocket::ReuseAddressHint);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}


QSendFile::~QSendFile()
{
	delete m_socket;
}

void QSendFile::readyRead()
{

	char data[256] = {0};
	QHostAddress addr;
	int count = m_socket->readDatagram(data, 256, &addr);

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
		QString name = QString(data + index);

		if (name == m_deviceName)
		{
			if (code == START_UPDATE_CODE)
			{
				sendData();
			}
			else if (code == FILE_UPDATE_CODE)
			{
				sendData();
			}
			else if (code == FINISH_UPDATE_CODE)
			{
				clear();
				emit updateSuccess();
			}
			else if (code == GIVE_UP_UPDATE_CODE)
			{
				clear();
				emit updateFailed();
			}
			else if (code == FAILED_UPDATE_CODE)
			{
				m_sendSize = 0;
				sendData();
			}
			else if (code == FAILED_SEND_CODE)
			{
				send(GIVE_UP_UPDATE_CODE, "Stop update");
			}
		}
	}
}

void QSendFile::sendData()
{
	if (m_sendSize == 0)
	{
		m_tryTimes--;

		if (m_tryTimes == 0)
		{
			send(GIVE_UP_UPDATE_CODE, "Stop update");
			return;
		}
	}
	else
	{
		m_tryTimes = 3;
	}

	m_position += m_sendSize;
	m_sendSize = 0;
	int s = m_fileData.size();

	if (s == 0)
	{
		emit sendIndex(100);
		emit updateSuccess();
		return;
	}
	else
	{
		int index = 100 * m_position / m_fileData.size();
		emit sendIndex(index);
	}

	if (m_position >= m_fileData.size())
	{
		send(FINISH_UPDATE_CODE, "Update finish");
		return;
	}

	int size = m_fileData.size() - m_position;
	size = size > 4096 ? 4096 : size;
	QByteArray array = m_fileData.mid(m_position, size);
	int sum = 0;

	for (int i = 0; i < array.size(); i++)
	{
		sum += int(array[i]) & 0xffff;
	}

	sum = sum & 0xffff;
	array.append((sum >> 8) & 0xff);
	array.append(sum & 0xff);
	m_sendSize = size;
	send(FILE_UPDATE_CODE, array);
}

void QSendFile::send(int code, const QByteArray &data)
{
	char buffer[4200];

	QString str = QString::number(code);
	int count1 = str.toLocal8Bit().size();
	memcpy(buffer, str.toLocal8Bit(), count1);
	buffer[count1] = ':';
	int count2 = data.size();
	memcpy(buffer + count1 + 1, data.data(), count2);
	m_socket->writeDatagram(buffer, count2 + count1 + 1, QHostAddress(m_address), SEND_FILE_WRITE_PORT);
}

void QSendFile::addFiles(const QStringList &files, const QString &path)
{
	QByteArray temp;
	char       buffer[256];
	int size;
	int t;

	int type = 0;


	QString str = path;
	str.replace("\\", "/");
	int index = str.lastIndexOf("/");

	if (index > 0)
	{
		QString s = str.mid(index + 1);
		str = str.left(index);
		index = str.lastIndexOf("/");

		if (index > 0)
		{
			str = str.left(index);

			if (str == QApplication::applicationDirPath() + "/update")
			{
				if (s == "soft")
				{
					type = 1;
				}
				else if (s == "plugin")
				{
					type = 2;
				}
			}
		}
	}

	foreach(const QString & f, files)
	{
		QFile file(path + "/" + f);

		if (!file.open(QFile::ReadOnly))
		{
			qDebug(QString("Open file failed: %1").arg(path + "/" + f).toLocal8Bit());
		}
		else
		{
			temp.clear();
			size = f.toLocal8Bit().size();
			memcpy(buffer, f.toLocal8Bit(), size + 1);
			t = type;
			memcpy(buffer + size + 1, &t, 4);
			temp = file.readAll();
			t = file.size();
			memcpy(buffer + size + 5, &t, 4);
			temp = QByteArray(buffer, size + 9) + temp;
			m_fileData += temp;
		}
	}
}

void QSendFile::update(QObject *obj)
{
	m_device = obj;
	connect(m_device, SIGNAL(destroyed()), this, SLOT(deviceOffline()));
	m_deviceName = obj->property("name").toString();
	m_address = obj->property("address").toUInt();
	send(START_UPDATE_CODE, "Start update");
}

void QSendFile::clear()
{
	if (m_device != NULL)
	{
		disconnect(m_device, SIGNAL(destroyed()), this, SLOT(deviceOffline()));
		m_device = NULL;
	}

	m_position = 0;
	m_fileData.clear();
	m_sendSize = 0;
	m_tryTimes = 4;
	m_deviceName = "";
	m_address = 0;
}

void QSendFile::deviceOffline()
{
	clear();
	emit updateFailed();
}
