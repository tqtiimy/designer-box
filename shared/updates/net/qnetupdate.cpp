#include "qnetupdate.h"

#include "qlistendevice.h"
#include "qheartbest.h"
#include "qsendfile.h"

#include <QDateTime>
#include <QVariant>

QNetUpdate::QNetUpdate(QObject *parent):
	QAbstractUpdate(parent),
	m_listen(new QListenDevice(this)),
	m_heartbest(new QHeartbest(this)),
	m_timer(new QTimer(this)),
	m_sendFile(new QSendFile(this))
{
	connect(m_listen, SIGNAL(findDevice(int, QString)), this, SLOT(findDevice(int, QString)));
	connect(m_heartbest, SIGNAL(heartbest(int, QString)), this, SLOT(heartbest(int, QString)));
	m_timer->start(100);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(check()));
	connect(m_sendFile, SIGNAL(updateFailed()), this, SIGNAL(upateFailed()));
	connect(m_sendFile, SIGNAL(updateSuccess()), this, SIGNAL(updateSuccess()));
	connect(m_sendFile, SIGNAL(sendIndex(int)), this, SIGNAL(sendIndex(int)));
}

QNetUpdate::~QNetUpdate()
{
	delete m_listen;
	delete m_timer;
}

void QNetUpdate::findDevice(int addr, const QString &name)
{
	QObject *obj = m_nameToDevice.value(name);

	if (obj == NULL)
	{
		obj = new QObject;
		obj->setProperty("name", name);
		obj->setProperty("address", addr);
		obj->setProperty("time", QDateTime::currentDateTime());
		addDevice(obj);
	}
}

void QNetUpdate::check()
{
	QDateTime dt = QDateTime::currentDateTime();
	QDateTime temp;
	QList<QObject *> list;
	foreach(QObject * obj, m_devices)
	{
		temp = obj->property("time").toDateTime();

		if (temp.msecsTo(dt) > 5000)
		{
			list.append(obj);
		}
	}

	foreach(QObject * obj, list)
	{
		m_devices.removeAll(obj);
		m_nameToDevice.remove(obj->property("name").toString());
		delete obj;
	}
}

void QNetUpdate::heartbest(int addr, const QString &name)
{
	QObject *obj = m_nameToDevice.value(name);

	if (obj != NULL)
	{
		if (obj->property("address").toInt() == addr)
		{
			obj->setProperty("time", QDateTime::currentDateTime());
		}
	}
}

void QNetUpdate::update(const QString &name)
{
	QObject *device = m_nameToDevice.value(name);

	if (device != NULL)
	{
		m_sendFile->update(device);
	}
}

void QNetUpdate::addFiles(const QStringList &files, const QString &basePath)
{
	m_sendFile->addFiles(files, basePath);
}

void QNetUpdate::clearFiles()
{
	m_sendFile->clear();
}

QString QNetUpdate::name()
{
	return tr("Net");
}
