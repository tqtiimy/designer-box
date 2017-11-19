#ifndef QDRIVERREMOVEDIALOG_H
#define QDRIVERREMOVEDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui
{
	class QDriverRemoveDialog;
}

class QDriverRemoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QDriverRemoveDialog(QWidget *parent = 0);
	~QDriverRemoveDialog();

	QStringList get_result();
protected:
	QIcon get_icon(bool check);
private slots:
	void on_okbtn_clicked();

	void on_cancelbtn_clicked();

	void item_clicked(QTreeWidgetItem *item);
private:
	Ui::QDriverRemoveDialog *ui;
protected:
	QStringList     m_result;
	QMap<QTreeWidgetItem *, QString>  m_item_to_uuid;
	QIcon           m_true_icon;
	QIcon           m_false_icon;
};

#endif // QDRIVERREMOVEDIALOG_H
