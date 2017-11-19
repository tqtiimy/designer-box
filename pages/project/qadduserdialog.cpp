#include "qadduserdialog.h"
#include "ui_qadduserdialog.h"

#include "../../libs/core/qnamevalidator.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qusermanager.h"


QAddUserDialog::QAddUserDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QAddUserDialog),
	m_userName(new QNameEdit("(.*)", this))
{
	ui->setupUi(this);
	this->setProperty("no-ManhattanStyle", true);
	m_userName->setFocus();

	ui->horizontalLayout->addWidget(m_userName);

	QUserManager *manager = QSoftCore::get_core()->get_project_core()->get_user_manager();
	QList<tagUserInfo *> users = manager->get_users();
	QStringList list;
	foreach(tagUserInfo * info, users)
	{
		list.append(info->m_name);
	}

	m_userName->setIntermediateList(list);
	m_userName->setFocus();
}

QAddUserDialog::~QAddUserDialog()
{
	delete ui;
}

void QAddUserDialog::on_okBtn_clicked()
{
	QString name, password;
	name = m_userName->value();
	password = ui->password->text();

	if (name == "")
	{
		return;
	}

	m_name = name;
	m_password = password;
	m_level = ui->level->value();
	close();
}

void QAddUserDialog::on_cancelBtn_clicked()
{
	close();
}

QString QAddUserDialog::getName()
{
	return m_name;
}

QString QAddUserDialog::getPassword()
{
	return m_password;
}

int QAddUserDialog::getLevel()
{
	return m_level;
}
