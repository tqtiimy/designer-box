#ifndef QDEVICEAPPLICATION_H
#define QDEVICEAPPLICATION_H

#include <QApplication>

class QDeviceApplication : public QApplication
{
	Q_OBJECT
public:
	explicit QDeviceApplication(int &argc, char **argv);

signals:

public slots:

};

#endif // QDEVICEAPPLICATION_H
