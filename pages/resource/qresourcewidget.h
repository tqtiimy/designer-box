#ifndef QRESOURCEWIDGET_H
#define QRESOURCEWIDGET_H

#include "../../libs/core/qabstractpagewidget.h"

class QResourceFileList;
class QEditorWidget;

class QResourceWidget : public QAbstractPageWidget
{
	Q_OBJECT
public:
	QResourceWidget();

	void    set_undo_stack(QUndoStack *stack);
protected:

	QResourceFileList   *m_resource_view;
	QEditorWidget       *m_editor;
};

#endif // QPROJECTWIDGET_H
