#ifndef QDATARESEVER_H
#define QDATARESEVER_H

#include <QObject>
#include <QUdpSocket>

class QDataResever : public QObject
{
	Q_OBJECT
public:
	explicit QDataResever(const QString name, uint address, QObject *parent = 0);
	~QDataResever();
signals:
	void    finishSignal();
protected:
	void    clear();
	bool    finish();
	void    send(const QByteArray &data);
protected slots:
	void    readyRead();
protected:
	QUdpSocket  *m_socket;
	uint         m_address;
	QString      m_name;
	QByteArray   m_fileBuffer;
};

#endif // QDATARESEVER_H
