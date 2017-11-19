#include "qdevicetreewidget.h"

#include "../libs/core/qsoftcore.h"
#include "../libs/core/qdevicesmanager.h"
#include "../libs/core/qabstractupdate.h"
#include "../libs/core/stylehelper.h"

#include <QHeaderView>
#include <QPainter>
#include <QApplication>
#include <QStyleOptionViewItem>
#include <QMouseEvent>

QSize QDeviceDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	return size;
}

void QDeviceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QTreeWidgetItem *item = m_listView->itemFromIndex(index);
	QStyleOptionViewItemV3 opt = option;
	QColor c;

	if (item != NULL)
	{
		if (m_listView->m_updateItems.values().contains(item))
		{
			c = opt.palette.color(QPalette::Dark);
			opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
		}
		else
		{
			c = QColor(255, 230, 191);

			if (opt.features & QStyleOptionViewItemV2::Alternate)
			{
				c = c.lighter(112);
			}
		}
	}



	painter->fillRect(option.rect, c);
	opt.state &= ~ QStyle::State_HasFocus;
	QItemDelegate::paint(painter, opt, index);
	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(qApp->style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	if (m_listView->m_deviceItems.values().contains(item))
	{
		if (index.column() != m_listView->columnCount() - 1)
		{
			int right = option.direction == Qt::LeftToRight ? option.rect.right() : option.rect.left();
			painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
		}
	}
}

QDeviceTreeWidget::QDeviceTreeWidget(QWidget *parent) :
	QTreeWidget(parent)
{
	setHeaderLabels(QStringList() << tr("Status") << tr("Name"));
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setRootIsDecorated(false);
	header()->setMovable(false);
	setItemDelegate(new QDeviceDelegate(this));
	m_icon = StyleHelper::drawIndicatorIcon(this->palette(), qApp->style());
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClick()));
	init();
}


void QDeviceTreeWidget::init()
{
	QDevicesManager *manager = QSoftCore::get_core()->get_devices_manager();

	QList<QAbstractUpdate *> updates = manager->updates();

	QList<QObject *> list;

	QTreeWidgetItem *temp, *item;
	QString str;

	foreach(QAbstractUpdate * update, updates)
	{
		item = new QTreeWidgetItem(this);
		item->setText(0, update->name());
		item->setIcon(0, m_icon);
		item->setExpanded(true);
		m_updateItems.insert(update, item);
		list = update->devices();
		connect(update, SIGNAL(newDevice(QObject *)), this, SLOT(newDevice(QObject *)));
		foreach(QObject * obj, list)
		{
			temp = new QTreeWidgetItem(item);
			temp->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			str = obj->property("name").toString();
			temp->setText(1, str);
			temp->setToolTip(1, str);
			connect(obj, SIGNAL(destroyed()), this, SLOT(deviceOffline()));
			temp->setText(0, tr("Online"));
			m_deviceItems.insert(str, temp);
		}
	}
}

void QDeviceTreeWidget::deviceOffline()
{
	QTreeWidgetItem *item = m_deviceItems.value(sender()->property("name").toString());

	if (item != NULL)
	{
		item->setText(0, tr("Offline"));
		item->setBackgroundColor(0, QColor(255, 0, 0));
	}
}

void QDeviceTreeWidget::newDevice(QObject *obj)
{
	QAbstractUpdate *update = (QAbstractUpdate *)sender();
	QTreeWidgetItem *item;
	item = m_deviceItems.value(obj->property("name").toString());

	if (item != NULL)
	{
		item->setText(0, tr("Online"));
		item->setBackgroundColor(0, QColor(255, 255, 255));
		return;
	}

	item = m_updateItems.value(update);

	if (item != NULL)
	{
		QTreeWidgetItem *temp = new QTreeWidgetItem(item);
		temp->setText(0, tr("Online"));
		temp->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		temp->setText(1, obj->property("name").toString());
		temp->setToolTip(1, obj->property("name").toString());
		connect(obj, SIGNAL(destroyed()), this, SLOT(deviceOffline()));
		m_deviceItems.insert(obj->property("name").toString(), temp);
	}
}
void QDeviceTreeWidget::clearOffline()
{

	QMapIterator<QString, QTreeWidgetItem *> it(m_deviceItems);
	QStringList list;

	while (it.hasNext())
	{
		it.next();

		if (it.value()->text(0) == tr("Offline"))
		{
			list.append(it.key());
		}
	}

	foreach(const QString & name, list)
	{
		delete m_deviceItems.value(name);
		m_deviceItems.remove(name);
	}

}

QAbstractUpdate *QDeviceTreeWidget::currentUpdate()
{
	QTreeWidgetItem *item = this->currentItem();

	if (m_deviceItems.values().contains(item))
	{
		item = item->parent();
		QMapIterator<QAbstractUpdate *, QTreeWidgetItem *>    it(m_updateItems);

		while (it.hasNext())
		{
			it.next();

			if (it.value() == item)
			{
				return it.key();
			}
		}
	}

	return NULL;
}

QObject *QDeviceTreeWidget::currentDevice()
{
	QMapIterator<QString, QTreeWidgetItem *>  it(m_deviceItems);
	QTreeWidgetItem *item = this->currentItem();
	QAbstractUpdate *update = currentUpdate();

	if (update == NULL)
	{
		return NULL;
	}

	while (it.hasNext())
	{
		it.next();

		if (it.value() == item)
		{
			return update->device(it.key());
		}
	}

	return NULL;
}

void QDeviceTreeWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
	QTreeWidgetItem *item = itemFromIndex(index);
	QStyleOptionViewItemV3 opt = options;
	QColor c;

	if (item != NULL)
	{
		if (!m_updateItems.values().contains(item))
		{
			c = QColor(255, 230, 191);
			painter->fillRect(options.rect, c);
			opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
		}
		else
		{
			c = opt.palette.color(QPalette::Dark);
			painter->fillRect(options.rect, c);
			opt.palette.setColor(QPalette::AlternateBase, c);
		}
	}


	QTreeWidget::drawRow(painter, opt, index);

	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor));
	painter->setPen(color);
	painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
}


void QDeviceTreeWidget::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);

	QTreeWidgetItem *item = itemAt(event->pos());

	if (item)
	{
		if (m_updateItems.values().contains(item))
		{
			if (event->pos().x() + header()->offset() < 20)
			{
				item->setExpanded(!item->isExpanded());
			}
		}
	}
}
