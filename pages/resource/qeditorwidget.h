#ifndef QEDITORWIDGET_H
#define QEDITORWIDGET_H

#include <QWidget>
#include <QMap>
#include <QStackedLayout>

class StyledBar;
class QStyledBarWidget;
class QAbstractResourceEditor;

struct tagFileInfo;

class QEditorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit QEditorWidget(QWidget *parent = 0);

signals:
public slots:
	void    select_file(tagFileInfo *file);
protected slots:
	void    show_editor(const QString &editor);
protected:
	QMap<QString, QAbstractResourceEditor *>      m_widgets;
	StyledBar                   *m_styled_bar;
	QStyledBarWidget            *m_styled_widget;
	QStackedLayout              *m_stacked_layout;
	tagFileInfo                 *m_current_file;

};

#endif // QEDITORWIDGET_H
