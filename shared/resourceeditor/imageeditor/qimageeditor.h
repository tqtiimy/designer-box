#ifndef QIMAGEEDITOR_H
#define QIMAGEEDITOR_H

#include "../../../libs/core/qabstractresourceeditor.h"

class QActionToolBar;
class QImageView;

class QImageEditor : public QAbstractResourceEditor
{
	Q_OBJECT
public:
	explicit QImageEditor(QWidget *parent = 0);


	virtual QWidget    *tool_bar();
	virtual QString     show_name();
	virtual bool        support(const QString &type);
	virtual void        set_file(tagFileInfo *info);
protected slots:
	void    outline();
protected:
	QActionToolBar     *m_tool_bar;
	QImageView          *m_image_view;
	QList<QAction *>     m_actions;

};

#endif // QIMAGEEDITOR_H
