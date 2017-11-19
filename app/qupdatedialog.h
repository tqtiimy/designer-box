#ifndef QUPDATEDIALOG_H
#define QUPDATEDIALOG_H

#include <QDialog>

namespace Ui
{
	class QUpdateDialog;
}

class QAbstractUpdate;

class QUpdateDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QUpdateDialog(QWidget *parent = 0);
	~QUpdateDialog();

	void    setInfo(QAbstractUpdate *update, QObject *obj);
protected slots:
	void    deviceOffline();
	void    updateSuccess();
	void    updateFailed();
private:
	Ui::QUpdateDialog *ui;
protected:
	QAbstractUpdate             *m_update;
	QObject                     *m_device;
private slots:
	void on_pathBtn_clicked();
	void on_cancelBtn_clicked();
	void on_okBtn_clicked();
};

#endif // QUPDATEDIALOG_H
