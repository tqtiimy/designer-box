#ifndef QDEVICEDATAWINDOW_H
#define QDEVICEDATAWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

namespace Ui
{
	class QDeviceDataWindow;
}

class QProjectInformationTreeWidget;

class QDeviceDataWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit QDeviceDataWindow(QWidget *parent = 0);
	~QDeviceDataWindow();
protected:
	void    init_memu_bar();
private:
	Ui::QDeviceDataWindow *ui;
	QMenuBar        *m_menu_bar;
	QStatusBar      *m_status_bar;
	QProjectInformationTreeWidget       *m_tree_widget;
};

#endif // QDEVICEDATAWINDOW_H
