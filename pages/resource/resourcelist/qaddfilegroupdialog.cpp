#include "qaddfilegroupdialog.h"
#include "ui_qaddfilegroupdialog.h"

#include "../../../libs/core/qnamevalidator.h"

QAddFileGroupDialog::QAddFileGroupDialog(const QStringList &list, QWidget *parent):
	QDialog(parent),
	ui(new Ui::QAddFileGroupDialog),
	m_name_edit(new QNameEdit("(.*)"))
{
	ui->setupUi(this);
	this->setProperty("no-ManhattanStyle", true);

	ui->horizontalLayout_2->addWidget(m_name_edit);

	m_name_edit->setIntermediateList(list);
	m_name_edit->setFocus();
}

QAddFileGroupDialog::~QAddFileGroupDialog()
{
	delete ui;
}

void QAddFileGroupDialog::on_cancelBtn_clicked()
{
	close();
}

void QAddFileGroupDialog::on_okBtn_clicked()
{
	m_name = m_name_edit->value();

	if (m_name != "")
	{
		close();
	}
}

QString QAddFileGroupDialog::get_name()
{
	return m_name;
}
