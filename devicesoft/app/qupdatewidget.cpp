#include "qupdatewidget.h"
#include "ui_qupdatewidget.h"

QUpdateWidget::QUpdateWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QUpdateWidget)
{
	ui->setupUi(this);
}

QUpdateWidget::~QUpdateWidget()
{
	delete ui;
}
