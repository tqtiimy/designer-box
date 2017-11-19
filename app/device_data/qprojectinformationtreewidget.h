#ifndef QPROJECTINFORMATIONTREEWIDGET_H
#define QPROJECTINFORMATIONTREEWIDGET_H

#include <QTreeWidget>

class QProjectInformationTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QProjectInformationTreeWidget(QWidget *parent = 0);

	void    set_information(const QStringList &list);
signals:

public slots:

};

#endif // QPROJECTINFORMATIONTREEWIDGET_H
