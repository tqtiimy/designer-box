#ifndef QSTYLEDBARWIDGET_H
#define QSTYLEDBARWIDGET_H

#include <QWidget>

namespace Ui
{
	class QStyledBarWidget;
}

class QStyledBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QStyledBarWidget(const QStringList &editors, QWidget *parent = 0);
	~QStyledBarWidget();

	void    set_file_name(const QString &file_name);

	void    set_current_editor(const QString &editor);

	void    add_tool_bar(QWidget *wid);
	void    show_tool_bar(QWidget *wid);
signals:
	void    editor_select(const QString &editor);

private:
	Ui::QStyledBarWidget *ui;
	QWidget *m_tool_bar;
};

#endif // QSTYLEDBARWIDGET_H
