#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QMenuBar>

class FancyTabWidget;

class QAbstractPage;
class QRunningManager;
class QDeviceDataWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void        show();
protected:
	void        saveSetting();
	void        initMenuBar();
	void        closeEvent(QCloseEvent *);
public slots:
	void        projectOpen();
	void        projectClosed();
	void        runSignal();
	void        stopSignal();
protected slots:
	void        newProject();
	void        openProject();
	void        run();
	void        stop();
	void        showDevice();
	void        showSyncChoose();
	void        closedProject();
	void        page_changed(int index);
protected:
	FancyTabWidget              *m_fancyTab;
	QMenuBar                    *m_menuBar;
	QList<QAbstractPage *>       m_pages;
	QRunningManager             *m_running_manager;
	QDeviceDataWindow           *m_device_data_window;
};

#endif // MAINWINDOW_H
