#ifndef QHEARTBEST_H
#define QHEARTBEST_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class QHeartbest : public QObject
{
	Q_OBJECT
public:
	explicit QHeartbest(const QString &name, uint address, QObject *parent = 0);
	~QHeartbest();
signals:
	void    offline();
protected slots:
	void    readyRead();
	void    timeout();
protected:
	QUdpSocket  *m_socket;
	uint         m_address;
	QString     m_name;
	int         m_times;
	QTimer      *m_timer;
};

#endif // QHEARTBEST_H
