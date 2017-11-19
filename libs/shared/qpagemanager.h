#ifndef QPAGEMANAGER_H
#define QPAGEMANAGER_H

#include "sharedlibglobal.h"

#include <QObject>
#include <QList>
#include <QMap>

class QAbstractHost;

class SHAREDLIB_EXPORT QPageManager: public QObject
{
	Q_OBJECT
public:
	QPageManager();

	void    load(const QString &project_path);

	void    save(const QString &project_path);

	void    clear();

	QList<QAbstractHost *>   get_pages();
	QAbstractHost          *get_page(int index);
	QAbstractHost          *get_page(const QString &uuid);
	void                    insert_page(QAbstractHost *host, int index);
	void                    remove_page(QAbstractHost *host);
	QList<QAbstractHost *>   get_pages_by_title(const QString &title);
protected slots:
	void    host_name_changed_slot(const QVariant &old_value, const QVariant &new_value);
signals:
	void    insert_page_signal(QAbstractHost *host);
	void    remove_page_signal(QAbstractHost *host);
	void    host_name_changed(QAbstractHost *host);
protected:
	void    load_page(const QString &fileName);
protected:
	QList<QAbstractHost *>       m_page_list;
	QMap<QString, QAbstractHost *> m_uuid_to_page;
	QString                 m_project_path;
};

#endif // QPAGEMANAGER_H
