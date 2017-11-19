#ifndef QADDLANGUAGEDIALOG_H
#define QADDLANGUAGEDIALOG_H

#include <QDialog>

namespace Ui
{
	class QAddLanguageDialog;
}

class QNameEdit;

class QAddLanguageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QAddLanguageDialog(QWidget *parent = 0);
	~QAddLanguageDialog();

	QString     get_language();
private slots:
	void on_okBtn_clicked();

	void on_cancelBtn_clicked();

private:
	Ui::QAddLanguageDialog *ui;
	QString             m_language;
	QNameEdit *m_languageName;
};

#endif // QADDLANGUAGEDIALOG_H
