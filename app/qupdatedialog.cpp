#include "qupdatedialog.h"
#include "ui_qupdatedialog.h"

#include "../libs/shared/qprojectcore.h"
#include "../libs/core/qsoftcore.h"
#include "../libs/core/qabstractupdate.h"
#include "../libs/shared/host/qabstracthost.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QApplication>
#include <QFileInfo>
#include <QProgressDialog>
#include <QMessageBox>

QUpdateDialog::QUpdateDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QUpdateDialog)
{
	ui->setupUi(this);

	QAbstractHost *host = QSoftCore::get_core()->get_project_core()->get_project_host();
	QString str;

	if (host != NULL)
	{
		str = host->get_property_value("projectPath").toString();
	}

	ui->pathEdit->setText(str);
}

QUpdateDialog::~QUpdateDialog()
{
	delete ui;
}

void QUpdateDialog::setInfo(QAbstractUpdate *update, QObject *obj)
{
	m_device = obj;
	m_update = update;
	connect(obj, SIGNAL(destroyed()), this, SLOT(deviceOffline()));
	connect(m_update, SIGNAL(upateFailed()), this, SLOT(updateFailed()));
	connect(m_update, SIGNAL(updateSuccess()), this, SLOT(updateSuccess()));
}

void QUpdateDialog::deviceOffline()
{
	QMessageBox::warning(this, tr("Warning"), tr("Device is offline!"));
	m_update->clear();
	close();
}

void QUpdateDialog::on_pathBtn_clicked()
{
	QString str = QFileDialog::getExistingDirectory(this, tr("Project Path"), QDir::currentPath());

	if (str != "")
	{
		ui->pathEdit->setText(str);
	}
}

void QUpdateDialog::on_cancelBtn_clicked()
{
	close();
}

void QUpdateDialog::on_okBtn_clicked()
{
	QString softPath = QApplication::applicationDirPath();
	QString osType = m_device->property("os").toString();

	if (osType == "")
	{
		osType = "windows";
	}

	m_update->clearFiles();
	int needSend = false;

	if (ui->softBtn->isChecked())
	{
		QString basePath = softPath + "/update/" + osType + "/soft";
		int l = basePath.size() + 1;
		QStringList files;
		QDir dir(basePath);
		QList<QFileInfo> list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
		QFileInfo info;

		while (list.size() > 0)
		{
			info = list.first();
			list.removeFirst();

			if (info.isFile())
			{
				files.append(info.filePath().mid(l));
			}
			else if (info.isDir())
			{
				QDir d(info.filePath());
				list += d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
			}
		}

		if (files.size() != 0)
		{
			m_update->addFiles(files, basePath);
			needSend = true;
		}
	}

	if (ui->pluginBtn->isChecked())
	{
		QString basePath = softPath + "/update/" + osType + "/plugin";
		int l = basePath.size() + 1;
		QStringList files;
		QDir dir(basePath);
		QList<QFileInfo> list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
		QFileInfo info;

		while (list.size() > 0)
		{
			info = list.first();
			list.removeFirst();

			if (info.isFile())
			{
				files.append(info.filePath().mid(l));
			}
			else if (info.isDir())
			{
				QDir d(info.filePath());
				list += d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
			}
		}

		if (files.size() != 0)
		{
			m_update->addFiles(files, basePath);
			needSend = true;
		}
	}

	if (ui->proBtn->isChecked())
	{
		QString basePath = ui->pathEdit->text();

		if (basePath == "")
		{
			m_update->clearFiles();
			return;
		}

		int l = basePath.size() + 1;
		QStringList files;
		QDir dir(basePath);
		QList<QFileInfo> list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
		QFileInfo info;

		while (list.size() > 0)
		{
			info = list.first();
			list.removeFirst();

			if (info.isFile())
			{
				files.append(info.filePath().mid(l));
			}
			else if (info.isDir())
			{
				QDir d(info.filePath());
				list += d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
			}
		}

		if (files.size() != 0)
		{
			m_update->addFiles(files, basePath);
			needSend = true;
		}
	}

	if (needSend)
	{
		m_update->update(m_device->property("name").toString());
		QProgressDialog dlg(this);
		connect(m_update, SIGNAL(sendIndex(int)), &dlg, SLOT(setValue(int)));
		connect(m_update, SIGNAL(upateFailed()), &dlg, SLOT(close()));
		connect(m_update, SIGNAL(updateSuccess()), &dlg, SLOT(close()));
		dlg.exec();
	}

	close();
}

void QUpdateDialog::updateFailed()
{
	QMessageBox::warning(this, tr("Warning"), tr("Update failed!"));
}

void QUpdateDialog::updateSuccess()
{
	QMessageBox::critical(this, tr("Update"), tr("Update Success!"));
}
