#ifndef QPROJECTLOADER_H
#define QPROJECTLOADER_H

#include <QObject>

class QRunningManager;
class QUpdateManager;
class QUpdateWidget;

class QProjectLoader: public QObject
{
	Q_OBJECT
public:
	QProjectLoader();
	~QProjectLoader();

	void    run();
protected slots:
	void    updateFinish();
protected:
	void    init();
protected:
	QRunningManager             *m_manager;
	QString                     m_projectPath;
	QUpdateManager              *m_updaterManager;
	QUpdateWidget               *m_updateWidget;
	QString                     m_uuid;
};

#endif // QPROJECTLOADER_H
