#ifndef QDATAGROUPREMOVEDIALOG_H
#define QDATAGROUPREMOVEDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui
{
	class QDataGroupRemoveDialog;
}

class QDataGroupRemoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QDataGroupRemoveDialog(QWidget *parent = 0);
	~QDataGroupRemoveDialog();

	QStringList get_result();
private slots:
	void on_okbtn_clicked();

	void on_cancelbtn_clicked();

	void  item_clicked(QTreeWidgetItem *item);
protected:
	QIcon get_icon(bool check);

private:
	Ui::QDataGroupRemoveDialog *ui;
protected:
	QStringList     m_result;
	QMap<QTreeWidgetItem *, QString>  m_item_to_uuid;
	QIcon           m_true_icon;
	QIcon           m_false_icon;
};

#endif // QDATAGROUPREMOVEDIALOG_H
