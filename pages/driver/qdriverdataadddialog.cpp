#include "qdriverdataadddialog.h"
#include "ui_qdriverdataadddialog.h"

#include "../../libs/core/qnamevalidator.h"

QDriverDataAddDialog::QDriverDataAddDialog(const QStringList &list, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QDriverDataAddDialog),
	m_name_edit(new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}"))
{
	ui->setupUi(this);
	ui->horizontalLayout->addWidget(m_name_edit);
	m_name_edit->setIntermediateList(list);
	setProperty("no-ManhattanStyle", true);
	m_name_edit->setFocus();
}

QDriverDataAddDialog::~QDriverDataAddDialog()
{
	delete ui;
}

void QDriverDataAddDialog::on_okbtn_clicked()
{
	m_name = m_name_edit->value();

	if (m_name != "")
	{
		close();
	}
}

void QDriverDataAddDialog::on_cancelbtn_clicked()
{
	close();
}

QString QDriverDataAddDialog::get_name()
{
	return m_name;
}
