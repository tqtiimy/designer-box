#include "qnewprojectdialog.h"
#include "ui_qnewprojectdialog.h"

#include <QFileDialog>
#include <QDir>

QNewProjectDialog::QNewProjectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QNewProjectDialog)
{
	ui->setupUi(this);
}

QNewProjectDialog::~QNewProjectDialog()
{
	delete ui;
}

void QNewProjectDialog::on_pathBtn_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Project Path"), QDir::currentPath());

	if (path != "")
	{
		ui->path->setText(path);
	}
}

void QNewProjectDialog::on_okBtn_clicked()
{
	if (ui->name->text() == "" || ui->path->text() == "")
	{
		return;
	}

	close();
}

void QNewProjectDialog::on_cancelBtn_clicked()
{
	ui->name->setText("");
	close();
}

QString QNewProjectDialog::getName()
{
	return ui->name->text();
}

QString QNewProjectDialog::getPath()
{
	return ui->path->text();
}
