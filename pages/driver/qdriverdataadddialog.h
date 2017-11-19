#ifndef QDRIVERDATAADDDIALOG_H
#define QDRIVERDATAADDDIALOG_H

#include <QDialog>

namespace Ui
{
	class QDriverDataAddDialog;
}

class QNameEdit;

class QDriverDataAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QDriverDataAddDialog(const QStringList &list, QWidget *parent = 0);
	~QDriverDataAddDialog();

	QString get_name();
private slots:
	void on_okbtn_clicked();

	void on_cancelbtn_clicked();

private:
	Ui::QDriverDataAddDialog *ui;
protected:
	QString     m_name;
	QNameEdit   *m_name_edit;
};

#endif // QDRIVERDATAADDDIALOG_H
