#ifndef QPLAINTEXTEDITOR_H
#define QPLAINTEXTEDITOR_H

#include "../../../libs/core/qabstractresourceeditor.h"

class QTextEditView;

class QPlainTextEditor : public QAbstractResourceEditor
{
	Q_OBJECT
public:
	explicit QPlainTextEditor(QWidget *parent = 0);


	virtual QWidget    *tool_bar();
	virtual QString     show_name();
	virtual bool        support(const QString &type);
	virtual void        set_file(tagFileInfo *info);
protected:
	QTextEditView       *m_editor;
};

#endif // QPLAINTEXTEDITOR_H
