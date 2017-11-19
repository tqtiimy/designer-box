#include "qadddatagroupdialog.h"
#include "ui_qadddatagroupdialog.h"

#include "../../libs/core/qnamevalidator.h"

QAddDataGroupDialog::QAddDataGroupDialog(const QStringList &list, QWidget *parent):
	QDialog(parent),
	ui(new Ui::QAddDataGroupDialog),
	m_name_edit(new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}"))
{
	ui->setupUi(this);

	ui->horizontalLayout->addWidget(m_name_edit);

	m_name_edit->setIntermediateList(list);


	this->setProperty("no-ManhattanStyle", true);

	m_name_edit->setFocus();
}

QAddDataGroupDialog::~QAddDataGroupDialog()
{
	delete ui;
}

void QAddDataGroupDialog::on_okbtn_clicked()
{
	m_name = m_name_edit->value();

	if (m_name != "")
	{
		close();
	}
}

void QAddDataGroupDialog::on_cancelbtn_clicked()
{
	close();
}

QString QAddDataGroupDialog::get_name()
{
	return m_name;
}
