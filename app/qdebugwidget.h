#ifndef QDEBUGWIDGET_H
#define QDEBUGWIDGET_H

#include <QTreeWidget>
#include <QList>
#include <QTreeWidgetItem>
#include <QItemDelegate>

class QDebugDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class QDebugWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QDebugWidget(QWidget *parent = 0);

	void    appendMsg(QtMsgType t, const QString &text);
signals:

protected:
	friend  class QDebugDelegate;
protected:
	void    contextMenuEvent(QContextMenuEvent *);
public slots:
	void    delMsg();
	void    clearMsg();
protected:
	QMap<QTreeWidgetItem *, QtMsgType>        m_itemToType;
	QList<QTreeWidgetItem *>                 m_items;
};

extern QDebugWidget     *g_debugWidget;

#endif // QDEBUGWIDGET_H
