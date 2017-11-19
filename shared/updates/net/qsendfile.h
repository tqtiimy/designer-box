#ifndef QSENDFILE_H
#define QSENDFILE_H

#include <QObject>

#include <QUdpSocket>
#include <QTimeLine>
#include <QStringList>

class QSendFile : public QObject
{
	Q_OBJECT
public:
	explicit QSendFile(QObject *parent);
	~QSendFile();

	void    addFiles(const QStringList &files, const QString &path);
	void    update(QObject *obj);
	void    clear();
protected slots:
	void    readyRead();
	void    deviceOffline();
protected:
	void    send(int code, const QByteArray &data);
	void    sendData();
signals:
	void    updateFailed();
	void    updateSuccess();
	void    sendIndex(int index);
protected:
	QUdpSocket      *m_socket;

	int             m_position;
	QByteArray      m_fileData;
	uint            m_address;
	QString         m_deviceName;
	int             m_sendSize;
	int             m_tryTimes;
	QObject         *m_device;
};

#endif // QSENDFILE_H
