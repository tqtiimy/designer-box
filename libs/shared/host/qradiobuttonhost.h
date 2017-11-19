#ifndef QRADIOBUTTONHOST_H
#define QRADIOBUTTONHOST_H

#include "qabstractbuttonhost.h"

class QRadioButtonHost: public QAbstractButtonHost
{
	Q_OBJECT
public:
	Q_INVOKABLE QRadioButtonHost(QAbstractHost *parent = 0);

	static QString get_show_name();
	static QString get_show_icon();
	static QString get_show_group();
protected:
	void    init_property();
protected:
	void    create_object();
};
#endif // QRADIOBUTTONHOST_H
