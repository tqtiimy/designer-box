#include "qabstractupdater.h"


QAbstractUpdater::QAbstractUpdater(QObject *parent) :
	QObject(parent),
	m_state(STATE_OFFLINE)
{
}

QAbstractUpdater::~QAbstractUpdater()
{
	stop();
}

void QAbstractUpdater::start()
{
}

void QAbstractUpdater::stop()
{

}

void QAbstractUpdater::setName(const QString &name)
{
	m_name = name;
}
