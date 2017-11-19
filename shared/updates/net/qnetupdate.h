#ifndef QNETUPDATE_H
#define QNETUPDATE_H

#include "../../../libs/core/qabstractupdate.h"

#include <QHostAddress>
#include <QTimer>

class QListenDevice;
class QHeartbest;
class QSendFile;

class QNetUpdate : public QAbstractUpdate
{
	Q_OBJECT
public:
	QNetUpdate(QObject *parent = 0);
	~QNetUpdate();

	void    addFiles(const QStringList &files, const QString &basePath);
	void    clearFiles();
	void    update(const QString &name);
	QString name();
protected slots:
	void    findDevice(int addr, const QString &name);
	void    check();
	void    heartbest(int addr, const QString &name);
protected:
	QListenDevice   *m_listen;
	QHeartbest      *m_heartbest;
	QTimer          *m_timer;
	QSendFile       *m_sendFile;
};

#endif // QNETUPDATE_H
