#ifndef QDRIVERDATALISTVIEW_H
#define QDRIVERDATALISTVIEW_H

#include <QTreeWidget>
#include <QItemDelegate>
#include <QUndoStack>

enum enListHeard
{
    LH_NAME,
    LH_ADDRESS,
    LH_DATA,
    LH_SCALE,
    LH_PERIOD,
    LH_LEVEL,
    LH_TYPE,
    LH_INFORMATION,
    LH_BOTTOM
};

class QAbstractDriver;
struct tagDriverDataInfo;
struct tagDataInfo;

class QDriverDataListView;

class QDriverDataDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	QDriverDataDelegate(QDriverDataListView *view): m_view(view) {}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void    setEditorData(QWidget *editor, const QModelIndex &index) const;
protected:
	QDriverDataListView *m_view;
};


class QDriverDataListView : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QDriverDataListView(QAbstractDriver *driver, QWidget *parent = 0);
	void    set_undo_stack(QUndoStack *stack);
protected:
	void    init();

	tagDataInfo   *get_data(const QString &uuid);

	void          contextMenuEvent(QContextMenuEvent *);
	void    mousePressEvent(QMouseEvent *event);
protected slots:
	void    double_clicked_item(QTreeWidgetItem *item, int col);

	void    insert_data(tagDriverDataInfo *data, int index);
	void    remove_data(tagDriverDataInfo *data);
	void    data_refresh(tagDriverDataInfo *data);

	void    add();
	void    remove();
	void    delete_select();
	void    give_up();

	void    check_all();
	void    uncheck_all();
signals:
	void    start_remove();
	void    end_remove();
	void    active();
protected:
	friend class QDriverDataDelegate;
public slots:
protected:
	QAbstractDriver                                 *m_driver;
	QMap<QTreeWidgetItem *, tagDriverDataInfo *>       m_item_to_data;
	QMap<tagDriverDataInfo *, QTreeWidgetItem *>       m_data_to_item;
	QUndoStack                                      *m_undo_stack;
	bool                                            m_is_remove;
};

#endif // QDRIVERDATALISTVIEW_H
