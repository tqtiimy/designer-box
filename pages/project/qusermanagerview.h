#ifndef QUSERMANAGERVIEW_H
#define QUSERMANAGERVIEW_H

#include <QWidget>

#include <QTreeWidget>
#include <QMap>
#include <QItemDelegate>
#include <QUndoStack>

class StyledBar;
struct tagUserInfo;

class QUserAddLanguageTreeWidget;
class QUserManagerView;

class QUserDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	QUserDelegate(QUserManagerView *view): m_view(view) {}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
protected:
	QUserManagerView    *m_view;
};

class QUserManagerView : public QWidget
{
	Q_OBJECT
public:
	explicit QUserManagerView(QWidget *parent = 0);
	void    set_undo_stack(QUndoStack *stack);
protected:
	void    contextMenuEvent(QContextMenuEvent *);
protected slots:
	void    project_opened();
	void    project_closed();

	void    add_user();
	void    remove_user();

	void    remove_select();
	void    give_up();


	void    refresh(tagUserInfo *info);

	void    inser_user_slot(tagUserInfo *info, int index);
	void    remove_user_slot(tagUserInfo *info);
protected:
	void    update_no();
protected:
	friend class QUserDelegate;
protected:
	QUserAddLanguageTreeWidget         *m_userList;
	StyledBar           *m_styledBar;
	QMap<tagUserInfo *, QTreeWidgetItem *> m_user_to_item;
	QMap<QTreeWidgetItem *, tagUserInfo *> m_item_to_user;
	QList<QTreeWidgetItem *>             m_items;
	bool                m_removing;
	QUndoStack          *m_undo_stack;

};

#endif // QUSERMANAGERVIEW_H
