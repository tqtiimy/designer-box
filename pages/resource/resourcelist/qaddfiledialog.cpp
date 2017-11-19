#include "qaddfiledialog.h"
#include "ui_qaddfiledialog.h"

#include "../../../libs/core/qsoftcore.h"
#include "../../../libs/core/qfilemanager.h"
#include "../../../libs/core/qnamevalidator.h"

#include <QFileDialog>

QAddFileDialog::QAddFileDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QAddFileDialog),
	m_name_edit(new QNameEdit("(.*)"))
{
	ui->setupUi(this);
	ui->horizontalLayout->addWidget(m_name_edit);
	this->setProperty("no-ManhattanStyle", true);

	QList<tagFileGroupInfo *> groups = QSoftCore::get_core()->get_file_manager()->get_all_group();

	foreach(tagFileGroupInfo * g, groups)
	{
		ui->group_combo->addItem(g->m_group_name);
	}
	ui->group_combo->setCurrentIndex(0);
}

QAddFileDialog::~QAddFileDialog()
{
	delete ui;
}

void QAddFileDialog::on_okBtn_clicked()
{
	m_name = m_name_edit->value();

	if (m_name != "")
	{
		m_group = ui->group_combo->currentText();

		QString path = ui->file_path->text();

		if (path == "")
		{
			return;
		}

		QFile f(path);

		if (!f.open(QFile::ReadOnly))
		{
			return;
		}

		m_exp = path.mid(path.lastIndexOf(".") + 1);
		m_data = f.readAll();
		f.close();

		close();
	}
}

void QAddFileDialog::on_cancelBtn_clicked()
{
	close();
}

void QAddFileDialog::on_group_combo_currentIndexChanged(const QString &arg1)
{
	QList<tagFileGroupInfo *> groups = QSoftCore::get_core()->get_file_manager()->get_all_group();

	foreach(tagFileGroupInfo * g, groups)
	{
		if (g->m_group_name == arg1)
		{
			QStringList list;
			foreach(tagFileInfo * info, g->m_files)
			{
				list.append(info->m_file_name);
			}
			m_name_edit->setIntermediateList(list);
			m_name_edit->markIntermediateState();
		}
	}
}

QString QAddFileDialog::get_name()
{
	return m_name;
}

QString QAddFileDialog::get_group()
{
	return m_group;
}

QByteArray QAddFileDialog::get_data()
{
	return m_data;
}

void QAddFileDialog::on_fileBtn_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Add File"), QDir::currentPath(), tr("All File(*.*)"));

	if (file != "")
	{
		ui->file_path->setText(file);

		if (m_name_edit->value() == "")
		{
			int index = file.lastIndexOf("/");
			file = file.mid(index + 1);
			index = file.lastIndexOf(".");
			file = file.left(index);
			m_name_edit->setText(file);
		}
	}
}

QString QAddFileDialog::get_exp()
{
	return m_exp;
}
