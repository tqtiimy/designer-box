#ifndef QUSERTREEWIDGET_H
#define QUSERTREEWIDGET_H

#include <QTreeWidget>

class QUserAddLanguageTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QUserAddLanguageTreeWidget(QWidget *parent = 0);

	QTreeWidgetItem *get_item(const QModelIndex &index);
	void        set_move(bool move);
protected:
	void    drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;

	void    mouseReleaseEvent(QMouseEvent *event);
signals:
	void    pressed_item(QTreeWidgetItem *item, int index);
public slots:
protected:
	bool    m_moving;
};

#endif // QUSERTREEWIDGET_H
