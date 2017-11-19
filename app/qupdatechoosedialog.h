#ifndef QDEVICESDIALOG_H
#define QDEVICESDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QMap>
#include <QList>

namespace Ui
{
	class QUpdateChooseDialog;
}

class QDeviceTreeWidget;

class QUpdateChooseDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QUpdateChooseDialog(QWidget *parent = 0);
	~QUpdateChooseDialog();
private slots:
	void on_updateBtn_clicked();
	void on_clearBtn_clicked();
private:
	Ui::QUpdateChooseDialog *ui;
protected:
	QDeviceTreeWidget   *m_deviceWidget;
};

#endif // QDEVICESDIALOG_H
