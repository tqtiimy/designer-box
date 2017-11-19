#include "mainwindow.h"

#include "fancytabwidget.h"
#include "qnewprojectdialog.h"
#include "qupdatechoosedialog.h"
#include "device_data/qdevicedatawindow.h"

#include "../libs/core/manhattanstyle.h"
#include "../libs/shared/pluginloader.h"
#include "../libs/core/qabstractpage.h"
#include "../libs/core/qsoftcore.h"
#include "../libs/shared/qprojectcore.h"
#include "../libs/shared/host/qabstracthost.h"
#include "../libs/running/qrunningmanager.h"


#include <QSettings>
#include <QDesktopWidget>
#include <QFile>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMetaObject>

MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
	m_fancyTab(new FancyTabWidget(this)),
	m_menuBar(new QMenuBar(this)),
	m_running_manager(new QRunningManager(this)),
	m_device_data_window(new QDeviceDataWindow)
{

	qApp->setStyle(new ManhattanStyle(QApplication::style()->objectName()));
	this->setCentralWidget(m_fancyTab);

	connect(m_fancyTab, SIGNAL(currentChanged(int)), this, SLOT(page_changed(int)));
	QMapIterator<QString, QAbstractPlugin *> it(PluginLoader::get_plugin_by_type(PAGE_PLUGIN_NAME));

	while (it.hasNext())
	{
		it.next();
		QAbstractPage *page = (QAbstractPage *)it.value();
		int i;

		for (i = 0; i < m_pages.size(); i++)
		{
			if (page->get_index() < m_pages.at(i)->get_index())
			{
				m_pages.insert(i, page);
				break;
			}
		}

		if (i == m_pages.size())
		{
			m_pages.append(page);
		}
	}

	foreach(QAbstractPage * p, m_pages)
	{
		m_fancyTab->insertTab(m_pages.indexOf(p), p->get_widget(), p->get_page_icon(), p->get_page_name());
		m_fancyTab->setTabEnabled(m_pages.indexOf(p), p->get_enabled());
	}

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Run");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(run()));
		ac->setEnabled(false);
	}

	ac = core->get_action("Debug.Stop");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(stop()));
		ac->setEnabled(false);
		m_fancyTab->setButtonVisible(ac, false);
	}

	ac = core->get_action("Project.New");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(newProject()));
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(closedProject()));
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), core->get_project_core(), SLOT(save()));
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Open");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(openProject()));
	}

	ac = core->get_action("Device.Update");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(showDevice()));
	}

	ac = core->get_action("Device.Sync");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(showSyncChoose()));
	}

	initMenuBar();
	this->setMenuBar(m_menuBar);

	m_fancyTab->setCurrentIndex(0);
	page_changed(0);

	connect(m_running_manager, SIGNAL(start_signal()), this, SLOT(runSignal()));
	connect(m_running_manager, SIGNAL(stop_signal()), this, SLOT(stopSignal()));

	connect(QSoftCore::get_core()->get_project_core(),
	        SIGNAL(opened_signals()), this, SLOT(projectOpen()));
	connect(QSoftCore::get_core()->get_project_core(),
	        SIGNAL(closed_signals()), this, SLOT(projectClosed()));

	m_device_data_window->setVisible(false);
}

MainWindow::~MainWindow()
{
	delete m_running_manager;
	delete m_device_data_window;
}

void MainWindow::show()
{
	QSettings s(qApp->applicationDirPath() + "/config.ini", QSettings::IniFormat, this);
	QVariant v = s.value("window/geometry");

	if (v.isValid())
	{
		restoreGeometry(v.toByteArray());
	}
	else
	{
		QDesktopWidget *dw = qApp->desktop();
		setGeometry(dw->width() / 10, dw->height() / 10, dw->width() * 4 / 5, dw->height() * 4 / 5);

		saveSetting();
	}

	QMainWindow::show();
}


void MainWindow::saveSetting()
{
	QFile f(qApp->applicationDirPath() + "/config.ini");

	if (!f.exists())
	{
		f.open(QIODevice::WriteOnly);
		f.resize(0);
		f.close();
	}

	QSettings s(qApp->applicationDirPath() + "/config.ini", QSettings::IniFormat, this);
	s.setValue("window/geometry", saveGeometry());
}

void MainWindow::newProject()
{
	QNewProjectDialog dlg(this);
	dlg.exec();
	QString name = dlg.getName();
	QString path = dlg.getPath();

	if (name != "" && path != "")
	{
		QSoftCore::get_core()->get_project_core()->create_new(path, name);
	}
}

void MainWindow::projectOpen()
{
	foreach(QAbstractPage * p, m_pages)
	{
		m_fancyTab->setTabEnabled(m_pages.indexOf(p), true);
	}
	this->setWindowTitle(QSoftCore::get_core()->get_project_core()->get_project_host()->get_property_value("objectName").toString() + "--Soft Configure Project");

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Run");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Debug.Stop");

	if (ac != NULL)
	{
		ac->setEnabled(true);
		m_fancyTab->setButtonVisible(ac, false);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void MainWindow::projectClosed()
{
	foreach(QAbstractPage * p, m_pages)
	{
		m_fancyTab->setTabEnabled(m_pages.indexOf(p), p->get_enabled());
	}
	this->setWindowTitle(tr("Soft Configure"));

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Run");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Debug.Stop");

	if (ac != NULL)
	{
		ac->setEnabled(false);
		m_fancyTab->setButtonVisible(ac, false);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}
}

void MainWindow::openProject()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open Project"), QDir::currentPath(), tr("Project File (*.sfb)"));

	if (file != NULL)
	{
		QSoftCore::get_core()->get_project_core()->open(file);
	}
}

void MainWindow::initMenuBar()
{
	QMenu *menu = m_menuBar->addMenu(tr("Project"));

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("Project.New");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	ac = core->get_action("Project.Open");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	menu = m_menuBar->addMenu(tr("Edit"));
	ac = core->get_action("Undo.Redo");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	ac = core->get_action("Undo.Undo");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	menu = m_menuBar->addMenu(tr("Device"));
	ac = core->get_action("Device.Update");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}

	ac = core->get_action("Device.Sync");

	if (ac != NULL)
	{
		menu->addAction(ac);
	}
}

void MainWindow::run()
{
	QString path = QSoftCore::get_core()->get_project_core()->get_project_host()->get_property_value("projectPath").toString();

	if (path != "")
	{
		QSoftCore::get_core()->get_project_core()->save();

		if (m_running_manager->load(path))
		{
			m_running_manager->start();
		}
	}
}

void MainWindow::runSignal()
{
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Run");
	m_fancyTab->setButtonVisible(ac, false);
	ac = core->get_action("Debug.Stop");
	m_fancyTab->setButtonVisible(ac, true);

	ac = core->get_action("Project.New");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Project.Open");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}
}

void MainWindow::stop()
{
	m_running_manager->stop();
}

void MainWindow::stopSignal()
{
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Stop");
	m_fancyTab->setButtonVisible(ac, false);
	ac = core->get_action("Debug.Run");
	m_fancyTab->setButtonVisible(ac, true);

	ac = core->get_action("Project.New");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Project.Open");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	stop();
	saveSetting();

	QMainWindow::closeEvent(e);
}

void MainWindow::showDevice()
{
	QUpdateChooseDialog dlg(this);
	dlg.exec();
}

void MainWindow::showSyncChoose()
{
	m_device_data_window->setVisible(true);
	m_device_data_window->raise();
}

void MainWindow::page_changed(int index)
{
	QSoftCore::get_core()->set_activity_stack(m_pages.at(index)->get_undo_stack());
}

void MainWindow::closedProject()
{
	QSoftCore::get_core()->get_project_core()->close();
}
