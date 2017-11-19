#include "qprojectinformationtreewidget.h"

#include <QHeaderView>

QProjectInformationTreeWidget::QProjectInformationTreeWidget(QWidget *parent) :
	QTreeWidget(parent)
{
	this->setRootIsDecorated(false);
	header()->hide();
	this->setAlternatingRowColors(true);
	this->setFrameStyle(QFrame::NoFrame);
}

void QProjectInformationTreeWidget::set_information(const QStringList &list)
{

}
