#ifndef QUPDATEWIDGET_H
#define QUPDATEWIDGET_H

#include <QWidget>

namespace Ui
{
	class QUpdateWidget;
}

class QUpdateWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QUpdateWidget(QWidget *parent = 0);
	~QUpdateWidget();
signals:
	void    updateSignal();
	void    closeSignal();
private:
	Ui::QUpdateWidget *ui;
};

#endif // QUPDATEWIDGET_H
