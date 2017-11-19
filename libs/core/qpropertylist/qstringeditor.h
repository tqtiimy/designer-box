#ifndef QSTRINGEDITOR_H
#define QSTRINGEDITOR_H

#include "qbuttoncommoneditor.h"

class QStringEditor : public QButtonCommonEditor
{
	Q_OBJECT
public:
	Q_INVOKABLE QStringEditor(QAbstractProperty *property, QUndoStack *stack, QWidget *parent = 0);

protected:
	void btn_clicked();

protected:
	QUndoStack     *m_undo_stack;

};

#endif // QSTRINGEDITOR_H
