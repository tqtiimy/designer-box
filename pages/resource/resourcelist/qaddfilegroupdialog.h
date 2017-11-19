#ifndef QADDFILEGROUPDIALOG_H
#define QADDFILEGROUPDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddFileGroupDialog;
}

class QNameEdit;

class QAddFileGroupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddFileGroupDialog(const QStringList &list, QWidget *parent = 0);
	~QAddFileGroupDialog();

	QString get_name();
private slots:
	void on_cancelBtn_clicked();

	void on_okBtn_clicked();

private:
	Ui::QAddFileGroupDialog *ui;
protected:
	QString     m_name;
	QNameEdit   *m_name_edit;
};

#endif // QADDFILEGROUPDIALOG_H
