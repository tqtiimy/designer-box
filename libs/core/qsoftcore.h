#ifndef QSOFTCORE_H
#define QSOFTCORE_H

#include "corelibglobal.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QAction>
#include <QUndoGroup>
#include <QUndoStack>
#include <QUndoGroup>

class QAbstractPageWidget;
class QProjectCore;
class QDevicesManager;
class QFileManager;

class QActionArray
{
public:
	QActionArray(): m_type(""), m_action(NULL) {}
	~QActionArray()
	{
		qDeleteAll(m_children.values());

		if (m_action != NULL) { delete m_action; }
	}
	QString                     m_type;
	QMap<QString, QActionArray *> m_children;
	QAction                    *m_action;
};

class CORELIB_EXPORT QSoftCore : public QObject
{
	Q_OBJECT
public:
	static  QSoftCore  *get_core();
	static  void    release_core();

	QProjectCore    *get_project_core();

	void        add_undo_stack(QUndoStack *stack);
	void        set_activity_stack(QUndoStack *stack);

	void        start_update_plugin();

	QDevicesManager *get_devices_manager();
	QFileManager    *get_file_manager();
protected:
	explicit    QSoftCore(QObject *parent = 0);

	void    init_user_manager_action();
	void    init_language_manager_action();
	void    init_form_manager_action();
	void    init_project_action();
	void    init_resource_action();
	void    init_image_view_action();
	void    init_script_edit_action();
	void    init_data_action();
	void    init_running_action();
	void    init_device_action();
	void    init_undo_action();
	void    init_driver_action();
public:
	void    insert_action(const QString &key, QAction *action); //添加全局action
	QAction    *get_action(const QString &key);//获取全局action
	QStringList get_pages();
protected:
	QActionArray        m_actions;//全局action
	static  QSoftCore       *m_core;//单实例
	QProjectCore        *m_project_core;
	QUndoGroup          *m_undo_group;
	QDevicesManager             *m_devicesManager;
	QFileManager        *m_file_manager;
};

#endif // QSOFTCORE_H
