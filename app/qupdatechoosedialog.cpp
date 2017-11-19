#include "qupdatechoosedialog.h"
#include "ui_qupdatechoosedialog.h"

#include "qupdatedialog.h"

#include "qdevicetreewidget.h"

#include "../libs/core/qabstractupdate.h"


QUpdateChooseDialog::QUpdateChooseDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QUpdateChooseDialog),
	m_deviceWidget(new QDeviceTreeWidget(this))
{
	ui->setupUi(this);
	ui->verticalLayout->insertWidget(0, m_deviceWidget, 1);
	connect(m_deviceWidget, SIGNAL(doubleClick()), this, SLOT(on_updateBtn_clicked()));
}

QUpdateChooseDialog::~QUpdateChooseDialog()
{
	delete m_deviceWidget;
	delete ui;
}

void QUpdateChooseDialog::on_updateBtn_clicked()
{
	QAbstractUpdate *update = m_deviceWidget->currentUpdate();
	QObject *obj = m_deviceWidget->currentDevice();

	if (update != NULL && obj != NULL)
	{
		QUpdateDialog dlg(this);
		dlg.setInfo(update, obj);
		dlg.exec();
	}
}


void QUpdateChooseDialog::on_clearBtn_clicked()
{
	m_deviceWidget->clearOffline();
}
