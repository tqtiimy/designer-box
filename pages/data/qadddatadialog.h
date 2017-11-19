#ifndef QADDDATADIALOG_H
#define QADDDATADIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddDataDialog;
}

class QNameEdit;

class QAddDataDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddDataDialog(const QStringList &list, QWidget *parent = 0);
	~QAddDataDialog();

	QString get_name();
	QString get_type();

private slots:
	void on_cancelbtn_clicked();

	void on_okbtn_clicked();

private:
	Ui::QAddDataDialog *ui;
protected:
	QNameEdit       *m_name_edit;
	QString         m_name;
	QString         m_type;
};

#endif // QADDDATADIALOG_H
