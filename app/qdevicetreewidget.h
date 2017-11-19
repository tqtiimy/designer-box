#ifndef QDEVICETREEWIDGET_H
#define QDEVICETREEWIDGET_H

#include <QTreeWidget>
#include <QItemDelegate>


class QAbstractUpdate;
class QDeviceTreeWidget;
class QDeviceDelegate: public QItemDelegate
{
	Q_OBJECT
public :
	QDeviceDelegate(QDeviceTreeWidget *parent): m_listView(parent) {}
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
	QDeviceTreeWidget   *m_listView;
};


class QDeviceTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit QDeviceTreeWidget(QWidget *parent = 0);
public:
	void clearOffline();
	QAbstractUpdate *currentUpdate();
	QObject                  *currentDevice();
protected:
	void    init();
	void    mousePressEvent(QMouseEvent *event);
	void    drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
	friend class QDeviceDelegate;
signals:
	void    doubleClick();
protected slots:
	void deviceOffline();
	void newDevice(QObject *obj);
protected:
	QMap<QAbstractUpdate *, QTreeWidgetItem *>             m_updateItems;
	QMap<QString, QTreeWidgetItem *>                     m_deviceItems;
	QIcon                                               m_icon;
};

#endif // QDEVICETREEWIDGET_H
