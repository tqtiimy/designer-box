#ifndef QPROJECTWIDGET_H
#define QPROJECTWIDGET_H

#include "../../libs/core/qabstractpagewidget.h"

class  MiniSplitter;
class QProjectPropertyView;
class QUserManagerView;
class QLanguageManagerView;
class QAbstractProperty;

class QProjectWidget : public QAbstractPageWidget
{
	Q_OBJECT
public:
	QProjectWidget();

	void    set_undo_stack(QUndoStack *stack);
protected:
	MiniSplitter       *m_splitter;
	QProjectPropertyView    *m_property_view;
	QUserManagerView    *m_user_view;
	QLanguageManagerView *m_language_view;
};

#endif // QPROJECTWIDGET_H
