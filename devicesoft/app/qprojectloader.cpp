#include "qprojectloader.h"

#include "qupdatewidget.h"

#include "../../libs/running/qrunningmanager.h"
#include "../libs/baselib/qupdatemanager.h"
#include "../libs/baselib/qabstractupdater.h"

#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QUuid>
#include <QProcess>

QProjectLoader::QProjectLoader():
	m_manager(new QRunningManager),
	m_updaterManager(new QUpdateManager(this))
{
	init();
	m_updateWidget = new QUpdateWidget;
	m_updateWidget->setVisible(false);

	connect(m_updaterManager, SIGNAL(updateFinish()), this, SLOT(updateFinish()));
}

QProjectLoader::~QProjectLoader()
{
	if (m_manager != NULL)
	{
		delete m_manager;
		m_manager = NULL;
	}

	if (m_updateWidget != NULL)
	{
		delete m_updateWidget;
		m_updateWidget = NULL;
	}

	delete m_updaterManager;
}

void QProjectLoader::init()
{

	QSettings s(qApp->applicationDirPath() + "/config.ini", QSettings::IniFormat, this);
	m_projectPath = s.value("project/path").toString();

	if (m_projectPath == "")
	{
		m_projectPath = qApp->applicationDirPath() + "/project";
	}

	m_uuid = s.value("project/uuid").toString();

	if (m_uuid == "")
	{
		m_uuid = QUuid::createUuid().toString();
		s.setValue("project/uuid", m_uuid);
	}
}

void QProjectLoader::run()
{
	m_updaterManager->loadUpdatePlugin();
	m_updaterManager->updaterStart(m_uuid);

	if (m_manager != NULL)
	{
		if (m_manager->load(m_projectPath))
		{
			m_manager->start();
			return;
		}
	}

	delete m_manager;
	m_manager = NULL;
	m_updateWidget->setVisible(true);
}

void QProjectLoader::updateFinish()
{
	QProcess::startDetached(QApplication::applicationDirPath() + "/softUpdate.exe");

	if (m_manager != NULL)
	{
		delete m_manager;
		m_manager = NULL;
	}

	if (m_updateWidget != NULL)
	{
		delete m_updateWidget;
		m_updateWidget = NULL;
	}
}
