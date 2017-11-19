#ifndef QADDDATAGROUPDIALOG_H
#define QADDDATAGROUPDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddDataGroupDialog;
}

class QNameEdit;

class QAddDataGroupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddDataGroupDialog(const QStringList &list, QWidget *parent = 0);
	~QAddDataGroupDialog();

	QString get_name();
private slots:
	void on_okbtn_clicked();

	void on_cancelbtn_clicked();

private:
	Ui::QAddDataGroupDialog *ui;
protected:
	QNameEdit   *m_name_edit;
	QString     m_name;
};

#endif // QADDDATAGROUPDIALOG_H
