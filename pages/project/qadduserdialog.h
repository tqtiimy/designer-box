#ifndef QADDUSERDIALOG_H
#define QADDUSERDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddUserDialog;
}

class QNameEdit;

class QAddUserDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddUserDialog(QWidget *parent = 0);
	~QAddUserDialog();

	QString getName();
	QString getPassword();
	int     getLevel();
private slots:
	void on_okBtn_clicked();

	void on_cancelBtn_clicked();

private:
	Ui::QAddUserDialog *ui;
	QNameEdit *m_userName;

	QString     m_name;
	QString     m_password;
	int         m_level;
};

#endif // QADDUSERDIALOG_H
