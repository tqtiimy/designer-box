#include "qadddriverdialog.h"
#include "ui_qadddriverdialog.h"

#include "../../libs/core/qnamevalidator.h"
#include "../../libs/core/qdriverlistview.h"

QAddDriverDialog::QAddDriverDialog(const QStringList &list, QWidget *parent):
	QDialog(parent),
	ui(new Ui::QAddDriverDialog),
	m_name_edit(new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}")),
	m_driver_list_view(new QDriverListView())
{
	ui->setupUi(this);
	setProperty("no-ManhattanStyle", true);

	ui->horizontalLayout->addWidget(m_name_edit);
	ui->verticalLayout->insertWidget(1, m_driver_list_view);
	m_name_edit->setIntermediateList(list);

	m_name_edit->setFocus();
}

QAddDriverDialog::~QAddDriverDialog()
{
	delete ui;
}

void QAddDriverDialog::on_okbtn_clicked()
{
	m_name = m_name_edit->value();
	m_type = m_driver_list_view->get_type();

	if (m_name != "" && m_type != "")
	{
		close();
	}
}

void QAddDriverDialog::on_cancelbtn_clicked()
{
	close();
}

QString QAddDriverDialog::get_name()
{
	return m_name;
}

QString QAddDriverDialog::get_type()
{
	return m_type;
}
