#include "qstyledbarwidget.h"
#include "ui_qstyledbarwidget.h"

#include <QVBoxLayout>

QStyledBarWidget::QStyledBarWidget(const QStringList &editors, QWidget *parent):
	QWidget(parent),
	ui(new Ui::QStyledBarWidget),
	m_tool_bar(NULL)
{
	ui->setupUi(this);
	ui->editor->addItems(editors);
	ui->editor->setCurrentIndex(-1);
	connect(ui->editor, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(editor_select(QString)));
}

QStyledBarWidget::~QStyledBarWidget()
{
	delete ui;
}

void QStyledBarWidget::set_file_name(const QString &file_name)
{
	ui->filename->setText(tr("File Name:") + file_name);
}

void QStyledBarWidget::set_current_editor(const QString &editor)
{
	ui->editor->setCurrentIndex(ui->editor->findText(editor));
}

void QStyledBarWidget::add_tool_bar(QWidget *wid)
{
	QLayout *v = ui->tool_bar_widget->layout();

	if (v == NULL)
	{
		v = new QVBoxLayout;
		v->setMargin(0);
		v->setSpacing(0);
		ui->tool_bar_widget->setLayout(v);
	}

	if (wid != NULL)
	{
		wid->setVisible(false);
		v->addWidget(wid);
	}
}

void QStyledBarWidget::show_tool_bar(QWidget *wid)
{
	if (m_tool_bar != NULL)
	{
		m_tool_bar->setVisible(false);
	}

	m_tool_bar = wid;

	if (m_tool_bar != NULL)
	{
		m_tool_bar->setVisible(true);
	}
}
