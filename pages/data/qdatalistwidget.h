#ifndef QDATALISTWIDGET_H
#define QDATALISTWIDGET_H

#include <QTreeWidget>
#include <QItemDelegate>
#include <QUndoStack>

enum enDataType
{
    DT_NAME,
    DT_VALUE,
    DT_TYPE,
    DT_HOLD,
    DT_PERIOD,
    DT_INFORMATION,
    DT_BOTTOM
};

struct tagDataInfo;
class QData;
class QDataListWidget;

class QDataDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	QDataDelegate(QDataListWidget *view): m_view(view) {}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void    setEditorData(QWidget *editor, const QModelIndex &index) const;
protected:
	QDataListWidget *m_view;
};

class QDataListWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QDataListWidget(QData *data, QUndoStack *stack, QWidget *parent = 0);

protected:
	void    init();
signals:

protected slots:
	void    double_clicked(const QModelIndex &index);
	void    refresh_item(tagDataInfo *data);

	void    add_data();
	void    remove_data();

	void    delete_select();
	void    give_up();

	void    check_all();
	void    uncheck_all();

	void    data_remove_slot(tagDataInfo *data);
	void    data_insert_slot(tagDataInfo *data, int index);
signals:
	void    start_remove();
	void    end_remove();
	void    activity();
protected:
	void    contextMenuEvent(QContextMenuEvent *);
	void    mousePressEvent(QMouseEvent *event);
protected:
	friend class QDataDelegate;

protected:
	QMap<QTreeWidgetItem *, tagDataInfo *>     m_item_to_data;
	QMap<tagDataInfo *, QTreeWidgetItem *>     m_data_to_item;
	QData                                   *m_data;
	QUndoStack                              *m_undo_stack;
	bool                                    m_is_remove;
};

#endif // QDATALISTWIDGET_H
