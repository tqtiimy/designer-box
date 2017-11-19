#ifndef QLANGUAGEMANAGERVIEW_H
#define QLANGUAGEMANAGERVIEW_H

#include <QWidget>


#include <QTreeWidget>
#include <QItemDelegate>
#include <QUndoStack>

class StyledBar;
class QLanguageManagerView;
class QLanguage;
class QUserAddLanguageTreeWidget;


class QLanguageDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	QLanguageDelegate(QLanguageManagerView *view): m_view(view) {}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
protected:
	QLanguageManagerView    *m_view;
};

class QLanguageManagerView : public QWidget
{
	Q_OBJECT
public:
	explicit QLanguageManagerView(QWidget *parent = 0);
	void    set_undo_stack(QUndoStack *stack);
protected:
	void    contextMenuEvent(QContextMenuEvent *);

	void    update_no();
protected slots:
	void    project_opened();
	void    project_closed();

	void    add_language();
	void    remove_language();

	void    remove_select();
	void    give_up();

	void    refresh(QLanguage *l);

	void    insert_language_slot(int index, QLanguage *l);
	void    remove_language_slot(QLanguage *l);

protected:
	friend class QLanguageDelegate;
protected:
	QUserAddLanguageTreeWidget         *m_languageList;
	StyledBar           *m_styledBar;
	QMap<QLanguage *, QTreeWidgetItem *>      m_language_to_item;
	QMap<QTreeWidgetItem *, QLanguage *>   m_item_to_language;
	QList<QTreeWidgetItem *>             m_items;
	bool                m_removing;
	QUndoStack          *m_undo_stack;
};

#endif // QLANGUAGEMANAGERVIEW_H
