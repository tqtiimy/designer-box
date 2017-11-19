#ifndef QDATACHOSSEDIALOG_H
#define QDATACHOSSEDIALOG_H

#include <QDialog>

namespace Ui
{
	class QDataChosseDialog;
}

class QDataListView;

class QDataChosseDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QDataChosseDialog(const QString &value, QWidget *parent = 0);
	~QDataChosseDialog();
	QString get_data();
private slots:
	void on_pushButton_2_clicked();

	void on_pushButton_clicked();

	void on_checkBox_clicked();

private:
	Ui::QDataChosseDialog *ui;

	QString     m_data;
	QDataListView   *m_data_list;
};

#endif // QDATACHOSSEDIALOG_H
