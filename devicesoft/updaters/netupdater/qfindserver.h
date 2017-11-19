#ifndef QFINDSERVER_H
#define QFINDSERVER_H

#include <QObject>

#include <QUdpSocket>
#include <QTimeLine>

class QFindServer : public QObject
{
	Q_OBJECT
public:
	explicit QFindServer(const QString &name);
	~QFindServer();
	void    searchServer();
signals:
	void    result(uint address);
public slots:
	void    readyRead();
	void    timeout();
protected:
	QUdpSocket      *m_socket;
	QString         m_name;
	QTimeLine      *m_timeline;
	int             m_timeout;
};

#endif // QFINDSERVER_H
