#include "qdebugwidget.h"

#include <QStringList>
#include <QHeaderView>
#include <QDateTime>
#include <QApplication>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>

QDebugWidget    *g_debugWidget = NULL;

static void DebugMsg(QtMsgType t, const char *buffer)
{
	if (g_debugWidget != NULL)
	{
		g_debugWidget->appendMsg(t, buffer);
	}
}

QSize QDebugDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	return size;
}

void QDebugDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;

	QColor c = QColor(255, 255, 191);

	if (opt.features & QStyleOptionViewItemV2::Alternate)
	{
		c = c.lighter(112);
	}

	painter->fillRect(option.rect, c);
	opt.state &= ~ QStyle::State_HasFocus;

	if (g_debugWidget->m_itemToType.value(g_debugWidget->itemFromIndex(index)) == QtDebugMsg)
	{
		opt.palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
	}
	else
	{
		opt.palette.setColor(QPalette::HighlightedText, QColor(255, 0, 0));
	}

	QItemDelegate::paint(painter, opt, index);
	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(qApp->style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	int right = option.direction == Qt::LeftToRight ? option.rect.right() : option.rect.left();
	painter->drawLine(right, option.rect.y(), right, option.rect.bottom());

	painter->drawLine(option.rect.left(), option.rect.bottom(), option.rect.right(), option.rect.bottom());
}

QDebugWidget::QDebugWidget(QWidget *parent) :
	QTreeWidget(parent)
{
	//qInstallMsgHandler(DebugMsg);
	setProperty("panelwidget", true);
	setItemDelegate(new QDebugDelegate);
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setRootIsDecorated(false);
	header()->setMovable(false);
	setFrameStyle(QFrame::NoFrame);

	setHeaderLabels(QStringList() << "Time" << "Message");
	setColumnWidth(0, 150);

}

void QDebugWidget::appendMsg(QtMsgType t, const QString &text)
{
	QTreeWidgetItem *item;

	while (m_items.size() > 1000)
	{
		m_itemToType.remove(m_items.first());
		delete m_items.first();
		m_items.removeFirst();
	}

	item = new QTreeWidgetItem(this);
	item->setText(1, text);
	m_items.append(item);
	m_itemToType.insert(item, t);
	item->setText(0, QDateTime::currentDateTime().toString(Qt::LocaleDate));
	this->setCurrentItem(item);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	if (t == QtDebugMsg)
	{
		item->setTextColor(0, QColor(0, 0, 0));
		item->setTextColor(1, QColor(0, 0, 0));
	}
	else
	{
		item->setTextColor(0, QColor(255, 0, 0));
		item->setTextColor(1, QColor(255, 0, 0));
	}
}

void QDebugWidget::delMsg()
{
	QList<QTreeWidgetItem *> items = this->selectedItems();
	foreach(QTreeWidgetItem * item, items)
	{
		m_items.removeAll(item);
		delete item;
	}
}

void QDebugWidget::clearMsg()
{
	this->clear();
	m_items.clear();
	m_itemToType.clear();
}

void QDebugWidget::contextMenuEvent(QContextMenuEvent *e)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Remove this"), this, SLOT(delMsg()));
	menu->addAction(tr("Clear"), this, SLOT(clearMsg()));
	menu->exec(e->globalPos());
}


