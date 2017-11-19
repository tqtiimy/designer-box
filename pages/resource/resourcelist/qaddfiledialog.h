#ifndef QADDFILEDIALOG_H
#define QADDFILEDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddFileDialog;
}

class QNameEdit;

class QAddFileDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddFileDialog(QWidget *parent = 0);
	~QAddFileDialog();

	QString get_name();
	QString get_group();
	QByteArray get_data();
	QString get_exp();
private slots:
	void on_okBtn_clicked();

	void on_cancelBtn_clicked();

	void on_group_combo_currentIndexChanged(const QString &arg1);

	void on_fileBtn_clicked();

private:
	Ui::QAddFileDialog *ui;
protected:
	QNameEdit       *m_name_edit;
	QString         m_name;
	QString         m_group;
	QByteArray      m_data;
	QString         m_exp;
};

#endif // QADDFILEDIALOG_H
