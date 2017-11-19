#ifndef QADDDRIVERDIALOG_H
#define QADDDRIVERDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddDriverDialog;
}

class QNameEdit;
class QDriverListView;

class QAddDriverDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddDriverDialog(const QStringList &list, QWidget *parent = 0);
	~QAddDriverDialog();

	QString get_name();
	QString get_type();
private slots:
	void on_okbtn_clicked();

	void on_cancelbtn_clicked();

private:
	Ui::QAddDriverDialog *ui;
protected:
	QNameEdit       *m_name_edit;
	QDriverListView *m_driver_list_view;

	QString         m_name;
	QString         m_type;
};

#endif // QADDDRIVERDIALOG_H
