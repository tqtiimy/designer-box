#include "qdevicedatawindow.h"
#include "ui_qdevicedatawindow.h"

#include "qprojectinformationtreewidget.h"

#include "../../libs/core/styledbar.h"
#include "../../libs/core/minisplitter.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>

QDeviceDataWindow::QDeviceDataWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QDeviceDataWindow),
	m_menu_bar(new QMenuBar(this)),
	m_status_bar(new QStatusBar),
	m_tree_widget(new QProjectInformationTreeWidget)
{
	ui->setupUi(this);
	this->setWindowTitle(tr("Device Data"));
	QDesktopWidget *desk = qApp->desktop();
	int w = desk->width();
	int h = desk->height();

	this->setMenuBar(m_menu_bar);
	this->setStatusBar(m_status_bar);
	this->setGeometry(w * 0.1, h * 0.1, w * 0.8, h * 0.8);

	this->showMaximized();

	init_memu_bar();


	QWidget *wid = new QWidget;

	StyledBar *bar = new StyledBar;

	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);
	v->addWidget(bar);
	v->addWidget(m_tree_widget);

	wid->setLayout(v);

	MiniSplitter *s = new MiniSplitter;

	s->addWidget(wid);
	setCentralWidget(s);
}

QDeviceDataWindow::~QDeviceDataWindow()
{
	delete ui;
}

void QDeviceDataWindow::init_memu_bar()
{
	QMenu *menu = m_menu_bar->addMenu(tr("File"));
	menu = m_menu_bar->addMenu(tr("Operator"));
}
