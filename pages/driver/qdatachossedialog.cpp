#include "qdatachossedialog.h"
#include "ui_qdatachossedialog.h"

#include "../../libs/core/qdatalistview.h"

QDataChosseDialog::QDataChosseDialog(const QString &value, QWidget *parent):
	QDialog(parent),
	ui(new Ui::QDataChosseDialog),
	m_data_list(new QDataListView)
{
	ui->setupUi(this);
	setProperty("no-ManhattanStyle", true);

	ui->verticalLayout->insertWidget(1, m_data_list);

	if (value.startsWith("{"))
	{
		ui->checkBox->setChecked(true);
		m_data_list->set_select(value);
		ui->spinBox->setVisible(false);
	}
	else
	{
		m_data_list->setVisible(false);
		ui->checkBox->setChecked(false);
		ui->spinBox->setValue(value.toInt());
		ui->spinBox->setFocus();
	}
}

QDataChosseDialog::~QDataChosseDialog()
{
	delete ui;
}

void QDataChosseDialog::on_pushButton_2_clicked()
{
	if (ui->checkBox->isChecked())
	{
		m_data = m_data_list->get_select_data();
	}
	else
	{
		m_data = QString::number(ui->spinBox->value());
	}

	close();
}

void QDataChosseDialog::on_pushButton_clicked()
{
	close();
}

QString QDataChosseDialog::get_data()
{
	return m_data;
}

void QDataChosseDialog::on_checkBox_clicked()
{
	m_data_list->setVisible(ui->checkBox->isChecked());
	ui->spinBox->setVisible(!ui->checkBox->isChecked());

	if (!ui->checkBox->isChecked())
	{
		ui->spinBox->setFocus();
	}

	resize(100, 100);
}
