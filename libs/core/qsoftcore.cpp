#include "qsoftcore.h"

#include "qabstractpagewidget.h"
#include "qdevicesmanager.h"
#include "qfilemanager.h"

#include "../shared/qprojectcore.h"
#include "../shared/qabstractupdateplugin.h"
#include "../shared/pluginloader.h"

#include <QIcon>
#include <QApplication>
#include <QDir>

QSoftCore *QSoftCore::m_core = NULL;

QSoftCore::QSoftCore(QObject *parent) :
	QObject(parent),
	m_project_core(new QProjectCore),
	m_undo_group(new QUndoGroup(this)),
	m_devicesManager(new QDevicesManager(this)),
	m_file_manager(new QFileManager)
{
	QDir dir;
	dir.mkpath(QApplication::applicationDirPath() + "/resources");
	m_file_manager->load();

	init_user_manager_action();
	init_language_manager_action();
	init_form_manager_action();
	init_project_action();
	init_resource_action();
	init_image_view_action();
	init_script_edit_action();
	init_data_action();
	init_running_action();
	init_device_action();
	init_undo_action();
	init_driver_action();
}

void QSoftCore::insert_action(const QString &key, QAction *action)
{
	QStringList l = key.split(".");
	l.removeAll("");

	if (l.size() == 0)
	{
		return;
	}


	QActionArray *group = &m_actions;
	QActionArray *temp;

	foreach(const QString & str, l)
	{
		temp = group->m_children.value(str);

		if (temp == NULL)
		{
			temp = new QActionArray;
			group->m_children.insert(str, temp);
		}

		group = temp;
	}

	if (group->m_action != NULL)
	{
		delete group->m_action;
	}

	group->m_action = action;
}

QAction *QSoftCore::get_action(const QString &key)
{
	QStringList l = key.split(".");
	l.removeAll("");

	if (l.size() == 0)
	{
		return NULL;
	}

	QActionArray *group = &m_actions;
	QActionArray *temp;

	foreach(const QString & str, l)
	{
		temp = group->m_children.value(str);

		if (temp == NULL)
		{
			return NULL;
		}

		group = temp;
	}
	return group->m_action;
}

QSoftCore *QSoftCore::get_core()
{
	if (m_core == NULL)
	{
		m_core = new QSoftCore;
	}

	return m_core;
}

void QSoftCore::release_core()
{
	if (m_core != NULL)
	{
		delete m_core;
		m_core = NULL;
	}
}

void QSoftCore::init_user_manager_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/plus.png"), tr("Add User"), NULL);
	insert_action("UserManager.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove User"), NULL);
	insert_action("UserManager.Del", ac);
	ac = new QAction(QIcon(""), tr("Remove users"), NULL);
	insert_action("UserManager.Remove", ac);
	ac = new QAction(QIcon(""), tr("Give up"), NULL);
	insert_action("UserManager.Give-Up", ac);
}

void QSoftCore::init_language_manager_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/plus.png"), tr("Add Language"), NULL);
	insert_action("LanguageManager.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove Language"), NULL);
	insert_action("LanguageManager.Del", ac);
	ac = new QAction(QIcon(""), tr("Remove users"), NULL);
	insert_action("LanguageManager.Remove", ac);
	ac = new QAction(QIcon(""), tr("Give up"), NULL);
	insert_action("LanguageManager.Give-Up", ac);
}

void QSoftCore::init_form_manager_action()
{
	QAction *ac;
	ac = new QAction(QIcon(":/images/plus.png"), tr("Add Form"), NULL);
	insert_action("FormManager.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove Form"), NULL);
	insert_action("FormManager.Del", ac);
	ac = new QAction(QIcon(":/images/left.png"), tr("Left"), NULL);
	insert_action("FormManager.Same.Left", ac);
	ac = new QAction(QIcon(":/images/v-center.png"), tr("V-Center"), NULL);
	insert_action("FormManager.Same.V-Center", ac);
	ac = new QAction(QIcon(":/images/right.png"), tr("Right"), NULL);
	insert_action("FormManager.Same.Right", ac);
	ac = new QAction(QIcon(":/images/top.png"), tr("Top"), NULL);
	insert_action("FormManager.Same.Top", ac);
	ac = new QAction(QIcon(":/images/h-center.png"), tr("H-Center"), NULL);
	insert_action("FormManager.Same.H-Center", ac);
	ac = new QAction(QIcon(":/images/bottom.png"), tr("Bottom"), NULL);
	insert_action("FormManager.Same.Bottom", ac);
	ac = new QAction(QIcon(":/images/same-width.png"), tr("Same Width"), NULL);
	insert_action("FormManager.Same.Width", ac);
	ac = new QAction(QIcon(":/images/same-height.png"), tr("Same Height"), NULL);
	insert_action("FormManager.Same.Height", ac);
	ac = new QAction(QIcon(":/images/same-rect.png"), tr("Same Gemotry"), NULL);
	insert_action("FormManager.Same.Gemotry", ac);
	ac = new QAction(QIcon(":/images/editcopy.png"), tr("Copy"), NULL);
	insert_action("FormManager.Copy", ac);
	ac = new QAction(QIcon(":/images/editcut.png"), tr("Cut"), NULL);
	insert_action("FormManager.Cut", ac);
	ac = new QAction(QIcon(":/images/editpaste.png"), tr("Paste"), NULL);
	insert_action("FormManager.Paste", ac);
	ac = new QAction(QIcon(), tr("Delete"), NULL);
	insert_action("FormManager.Delete", ac);
	ac = new QAction(QIcon(), tr("Select All"), NULL);
	insert_action("FormManager.Select.All", ac);
}

void QSoftCore::init_project_action()
{
	QAction *ac;
	ac = new QAction(QIcon(":/images/fileopen.png"), tr("Open Project"), NULL);
	insert_action("Project.Open", ac);
	ac = new QAction(QIcon(":/images/filesave.png"), tr("Save Project"), NULL);
	insert_action("Project.Save", ac);
	ac = new QAction(QIcon(":/images/filenew.png"), tr("New Project"), NULL);
	insert_action("Project.New", ac);
	ac = new QAction(QIcon(":/images/close.png"), tr("Close Project"), NULL);
	insert_action("Project.Close", ac);
}

void QSoftCore::init_resource_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/plus.png"), tr("Add Resource"), NULL);
	insert_action("ResourceManager.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove Resource"), NULL);
	insert_action("ResourceManager.Del", ac);
}

void QSoftCore::init_image_view_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/fitinscreen.png"), tr("Fit image int the screen"), NULL);
	insert_action("ImageView.Fit", ac);
	ac = new QAction(QIcon(":/images/originalsize.png"), tr("Original size"), NULL);
	insert_action("ImageView.Original", ac);
	ac = new QAction(QIcon(":/images/zoomin.png"), tr("Zoomin"), NULL);
	insert_action("ImageView.Zoomin", ac);
	ac = new QAction(QIcon(":/images/zoomout.png"), tr("Zoomout"), NULL);
	insert_action("ImageView.Zoomout", ac);
}

void QSoftCore::init_script_edit_action()
{
	QAction *ac;
	ac = new QAction(QIcon(":/images/filesave_small.png"), tr("&Save"), NULL);
	insert_action("ScriptEdit.Save", ac);
	ac = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), NULL);
	insert_action("ScriptEdit.Undo", ac);
	ac = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), NULL);
	insert_action("ScriptEdit.Redo", ac);
	ac = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), NULL);
	insert_action("ScriptEdit.Cut", ac);
	ac = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), NULL);
	insert_action("ScriptEdit.Copy", ac);
	ac = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), NULL);
	insert_action("ScriptEdit.Paste", ac);
	ac = new QAction(QIcon(":/images/zoomin.png"), tr("Zoomin"), NULL);
	insert_action("ScriptEdit.Zoomin", ac);
	ac = new QAction(QIcon(":/images/zoomout.png"), tr("Zoomout"), NULL);
	insert_action("ScriptEdit.Zoomout", ac);
	ac = new QAction(QIcon(":/images/font.png"), tr("Set Font"), NULL);
	insert_action("ScriptEdit.Font", ac);
	ac = new QAction(QIcon(":/images/fitinscreen.png"), tr("Search"), NULL);
	insert_action("ScriptEdit.Search", ac);
}

void QSoftCore::init_data_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/plus.png"), tr("Add Group"), NULL);
	insert_action("Data.Group.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove Group"), NULL);
	insert_action("Data.Group.Del", ac);
}

void QSoftCore::init_driver_action()
{
	QAction *ac;

	ac = new QAction(QIcon(":/images/plus.png"), tr("Add Driver"), NULL);
	insert_action("Driver.Add", ac);
	ac = new QAction(QIcon(":/images/minus.png"), tr("Remove Driver"), NULL);
	insert_action("Driver.Del", ac);
}

void QSoftCore::init_running_action()
{
	QAction *ac;
	ac = new QAction(QIcon(":/images/run.png"), tr("Run"), NULL);
	insert_action("Debug.Run", ac);
	ac = new QAction(QIcon(":/images/stop.png"), tr("Stop"), NULL);
	insert_action("Debug.Stop", ac);
}

void QSoftCore::init_device_action()
{
	QAction *ac;
	ac = new QAction(QIcon(":/images/device.png"), tr("Update"), NULL);
	insert_action("Device.Update", ac);
	ac = new QAction(QIcon(":/images/sync.png"), tr("Sync Data"), NULL);
	insert_action("Device.Sync", ac);
}

void QSoftCore::init_undo_action()
{
	QAction *ac;
	ac = m_undo_group->createRedoAction(NULL);
	ac->setIcon(QIcon(":/images/redo.png"));
	ac->setShortcut(QKeySequence::Redo);
	insert_action("Undo.Redo", ac);
	ac = m_undo_group->createUndoAction(NULL);
	ac->setIcon(QIcon(":/images/undo.png"));
	ac->setShortcut(QKeySequence::Undo);
	insert_action("Undo.Undo", ac);
}

QProjectCore *QSoftCore::get_project_core()
{
	return m_project_core;
}

void QSoftCore::add_undo_stack(QUndoStack *stack)
{
	m_undo_group->addStack(stack);
}

void QSoftCore::set_activity_stack(QUndoStack *stack)
{
	m_undo_group->setActiveStack(stack);
}

void QSoftCore::start_update_plugin()
{
	QMap<QString, QAbstractPlugin *> plugins = PluginLoader::get_plugin_by_type(UPDATE_PLUGIN_TYPE);

	QMapIterator<QString, QAbstractPlugin *> it(plugins);

	while (it.hasNext())
	{
		it.next();
		QAbstractUpdatePlugin *pl = (QAbstractUpdatePlugin *)it.value();
		QStringList list = pl->supports();

		foreach(const QString & n, list)
		{
			QAbstractUpdate *up = pl->create(n);

			if (up != NULL)
			{
				m_devicesManager->addUpdate(up);
			}
		}
	}
}

QDevicesManager *QSoftCore::get_devices_manager()
{
	return m_devicesManager;
}

QFileManager *QSoftCore::get_file_manager()
{
	return m_file_manager;
}
