#include "qwidgetview.h"

#include "../../../shared/host/qabstracthost.h"
#include "../../../shared/qprojectcore.h"
#include "../../../shared/pluginloader.h"
#include "../../qsoftcore.h"
#include "../../../shared/qpagemanager.h"
#include "../../../shared/qhostfactory.h"

#include <QItemDelegate>
#include <QPainter>
#include <QHeaderView>
#include <QIcon>
#include <QApplication>
#include <QVBoxLayout>

class QObjectDelegate: public QItemDelegate
{
public :
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

void QObjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;
	QColor c;
	c = QColor(255, 191, 239);

	if (opt.features & QStyleOptionViewItemV2::Alternate)
	{
		c = c.lighter(112);
	}

	painter->fillRect(option.rect, c);
	opt.state &= ~ QStyle::State_HasFocus;
	QItemDelegate::paint(painter, opt, index);
	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	if (index.column() == 0)
	{
		int right = option.direction == Qt::LeftToRight ? option.rect.right() : option.rect.left();
		painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
	}
}

QSize QObjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize sz = QItemDelegate::sizeHint(option, index);
	sz.setHeight(20);
	return sz;
}

QWidgetView::QWidgetView(QAbstractHost *host, QWidget *parent) :
	QTreeWidget(parent),
	m_property_host(host)
{
	setFrameStyle(QFrame::NoFrame);
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setItemDelegate(new QObjectDelegate);
	header()->hide();

	connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(clickedItem(QTreeWidgetItem *, int)));


	QPalette p = this->palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
	this->setPalette(p);

	initWidgetList();
}

void QWidgetView::initWidgetList()
{
	QProjectCore *core = QSoftCore::get_core()->get_project_core();
	QAbstractHost *global_host = core->get_project_host();

	QList<QAbstractHost *>   list = core->get_page_manager()->get_pages();

	QTreeWidgetItem *item = new QTreeWidgetItem(this);
	item->setText(0, tr("global"));
	m_items.insert(global_host, item);

	while (list.size() > 0)
	{
		QAbstractHost *host = list.takeFirst();
		QTreeWidgetItem *par = NULL;

		if (host->get_parent() != NULL)
		{
			par = m_items.value(host->get_parent());
		}

		if (par == NULL)
		{
			item = new QTreeWidgetItem(this);
		}
		else
		{
			item = new QTreeWidgetItem(par);
		}

		item->setText(0, host->get_property_value("objectName").toString());
		item->setIcon(0, get_host_icon(host));
		m_items.insert(host, item);
		list += host->get_children();
	}

	this->expandAll();
}

void QWidgetView::clickedItem(QTreeWidgetItem *item, int)
{
	QMapIterator<QAbstractHost *, QTreeWidgetItem *>   it(m_items);

	while (it.hasNext())
	{
		it.next();

		if (it.value() == item)
		{
			emit select(it.key());
			return;
		}
	}
}

void QWidgetView::setSelect(QAbstractHost *host)
{
	QTreeWidgetItem *item = m_items.value(host);

	if (item != NULL && !item->isSelected())
	{
		this->clearSelection();
		setCurrentItem(item);
		emit select(host);
	}
}

void QWidgetView::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = options;
	QColor c;
	c = QColor(255, 191, 239);
	painter->fillRect(options.rect, c);
	opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
	opt.state &= ~ QStyle::State_HasFocus;
	QTreeWidget::drawRow(painter, opt, index);
	opt.palette.setCurrentColorGroup(QPalette::Active);

	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	if (index.column() == 0)
	{
		int right = options.direction == Qt::LeftToRight ? options.rect.right() : options.rect.left();
		painter->drawLine(right, options.rect.y(), right, options.rect.bottom());
	}
}

void QWidgetView::setSelected(QTreeWidgetItem *item)
{
	QMapIterator<QAbstractHost *, QTreeWidgetItem *> it(m_items);

	while (it.hasNext())
	{
		it.next();

		if (it.value() == item)
		{
			setSelect(it.key());
			return;
		}
	}
}

QAbstractHost *QWidgetView::currentHost()
{
	QMapIterator<QAbstractHost *, QTreeWidgetItem *>   it(m_items);

	while (it.hasNext())
	{
		it.next();

		if (it.value()->isSelected())
		{
			return it.key();
		}
	}

	return NULL;
}

QIcon QWidgetView::get_host_icon(QAbstractHost *host)
{
	tagHostInfo *info = QHostFactory::get_host_info(host->get_attribute(HOST_TYPE));

	if (info != NULL)
	{
		return QIcon(info->get_show_icon());
	}
	else
	{
		return QIcon(":/images/widget.png");
	}
}
