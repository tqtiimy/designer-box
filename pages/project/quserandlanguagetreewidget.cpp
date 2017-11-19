#include "quserandlanguagetreewidget.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

QUserAddLanguageTreeWidget::QUserAddLanguageTreeWidget(QWidget *parent) :
	QTreeWidget(parent),
	m_moving(false)
{
}

void QUserAddLanguageTreeWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
	QTreeWidget::drawRow(painter, options, index);

	QStyleOptionViewItemV3 opt = options;
	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(qApp->style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	int right = options.direction == Qt::LeftToRight ? options.rect.right() : options.rect.left();
	painter->drawLine(right, options.rect.y(), right, options.rect.bottom());

	painter->drawLine(options.rect.left(), options.rect.bottom(), options.rect.right(), options.rect.bottom());

}

QTreeWidgetItem *QUserAddLanguageTreeWidget::get_item(const QModelIndex &index)
{
	return itemFromIndex(index);
}

void QUserAddLanguageTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_moving)
	{
		if (event->button() == Qt::LeftButton)
		{
			QTreeWidgetItem *item = itemAt(event->pos());

			if (item != NULL)
			{
				int index = this->indexAt(event->pos()).column();

				if (index == 0 || event->pos().x() >= 20)
				{
					item->setCheckState(0, item->checkState(0) == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
					return;
				}
			}
		}
	}

	QTreeWidget::mouseReleaseEvent(event);
}

void QUserAddLanguageTreeWidget::set_move(bool move)
{
	m_moving = move;
}
