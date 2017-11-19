#include "qadddatadialog.h"
#include "ui_qadddatadialog.h"

#include "../../libs/core/qnamevalidator.h"

QAddDataDialog::QAddDataDialog(const QStringList &list, QWidget *parent):
	QDialog(parent),
	ui(new Ui::QAddDataDialog),
	m_name_edit(new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}", this))
{
	ui->setupUi(this);
	ui->horizontalLayout_2->addWidget(m_name_edit);

	m_name_edit->setIntermediateList(list);

	this->setProperty("no-ManhattanStyle", true);

	ui->type_combo->addItems(QStringList() << "String" << "Number" << "Float" << "Bool");
	ui->type_combo->setCurrentIndex(0);

	m_name_edit->setFocus();
}

QAddDataDialog::~QAddDataDialog()
{
	delete ui;
}

void QAddDataDialog::on_cancelbtn_clicked()
{
	close();
}

void QAddDataDialog::on_okbtn_clicked()
{
	m_name = m_name_edit->value();

	if (m_name == "")
	{
		return;
	}

	m_type = ui->type_combo->currentText();
	close();
}

QString QAddDataDialog::get_name()
{
	return m_name;
}

QString QAddDataDialog::get_type()
{
	return m_type;
}
