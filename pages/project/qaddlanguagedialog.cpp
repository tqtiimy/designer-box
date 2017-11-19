#include "qaddlanguagedialog.h"
#include "ui_qaddlanguagedialog.h"


#include "../../libs/core/qnamevalidator.h"

#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qlanguagemanager.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/shared/qlanguage.h"

QAddLanguageDialog::QAddLanguageDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QAddLanguageDialog),
	m_languageName(new QNameEdit("(.*)", this))
{
	ui->setupUi(this);
	this->setProperty("no-ManhattanStyle", true);

	ui->horizontalLayout->addWidget(m_languageName);
	QLanguageManager *manager = QSoftCore::get_core()->get_project_core()->get_language_manager();
	QList<QLanguage *> languages = manager->get_all_languages();
	QStringList list;
	foreach(QLanguage * info, languages)
	{
		list.append(info->get_language_name());
	}
	m_languageName->setIntermediateList(list);
	m_languageName->setFocus();
}

QAddLanguageDialog::~QAddLanguageDialog()
{
	delete ui;
}

void QAddLanguageDialog::on_okBtn_clicked()
{
	QString str = m_languageName->value();

	if (str != "")
	{
		m_language = str;
		close();
	}
}

void QAddLanguageDialog::on_cancelBtn_clicked()
{
	close();
}

QString QAddLanguageDialog::get_language()
{
	return m_language;
}
