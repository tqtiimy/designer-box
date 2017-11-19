#include "qdriverremovedialog.h"
#include "ui_qdriverremovedialog.h"

#include "../../libs/core/qsoftcore.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qdrivermanager.h"
#include "../../libs/shared/driver/qabstractdriver.h"

#include <QItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>

class QDriverDelegate: public QItemDelegate
{
public :
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

QSize QDriverDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	size.rwidth() += 4;
	return size;
}

void QDriverDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;
	QColor c = QColor(255, 230, 191);

	if (opt.features & QStyleOptionViewItemV2::Alternate)
	{
		c = c.lighter(112);
	}

	painter->fillRect(option.rect, c);
	opt.state &= ~ QStyle::State_HasFocus;
	QItemDelegate::paint(painter, opt, index);
}

QDriverRemoveDialog::QDriverRemoveDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QDriverRemoveDialog)
{
	ui->setupUi(this);

	ui->driver_list_view->setAlternatingRowColors(true);
	ui->driver_list_view->setRootIsDecorated(false);
	ui->driver_list_view->setItemDelegate(new QDriverDelegate);

	connect(ui->driver_list_view, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(item_clicked(QTreeWidgetItem *)));

	QDriverManager *manager = QSoftCore::get_core()->get_project_core()->get_driver_manager();

	m_true_icon = get_icon(true);
	m_false_icon = get_icon(false);

	foreach(QAbstractDriver * driver, manager->get_drivers())
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->driver_list_view);

		item->setText(0, driver->get_name());
		item->setData(0, Qt::DecorationRole, m_false_icon);
		item->setData(0, Qt::UserRole, false);
		m_item_to_uuid.insert(item, driver->get_uuid());
	}

}

QDriverRemoveDialog::~QDriverRemoveDialog()
{
	delete ui;
}

void QDriverRemoveDialog::on_okbtn_clicked()
{
	int count = ui->driver_list_view->topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		if (ui->driver_list_view->topLevelItem(i)->data(0, Qt::UserRole).toBool())
		{
			m_result.append(m_item_to_uuid.value(ui->driver_list_view->topLevelItem(i)));
		}
	}

	if (m_result.size() == 0)
	{
		return;
	}

	close();
}

void QDriverRemoveDialog::on_cancelbtn_clicked()
{
	close();
}

QStringList QDriverRemoveDialog::get_result()
{
	return m_result;
}

void QDriverRemoveDialog::item_clicked(QTreeWidgetItem *item)
{
	bool check = item->data(0, Qt::UserRole).toBool();
	item->setData(0, Qt::UserRole, !check);
	item->setData(0, Qt::DecorationRole, check ? m_false_icon : m_true_icon);
}

QIcon QDriverRemoveDialog::get_icon(bool check)
{
	QStyleOptionButton opt;
	opt.state |= check ? QStyle::State_On : QStyle::State_Off;

	opt.state |= QStyle::State_Enabled;

	const QStyle *style = QApplication::style();
	const int indicatiorWidth = style->pixelMetric(QStyle::PM_IndicatorWidth, &opt);
	const int indicatiorHeight = style->pixelMetric(QStyle::PM_IndicatorHeight, &opt);
	const int listViewIconSize = indicatiorWidth;
	const int pixmapWidth = indicatiorWidth;
	const int pixmapHeight = qMax(indicatiorHeight, listViewIconSize);

	opt.rect = QRect(0, 0, indicatiorWidth, indicatiorHeight);

	QPixmap pixmap = QPixmap(pixmapWidth, pixmapHeight);

	pixmap.fill(Qt::transparent);

	{
		const int xoff = (pixmapWidth > indicatiorWidth) ? (pixmapWidth - indicatiorWidth) / 2 : 0;
		const int yoff = (pixmapHeight > indicatiorHeight) ? (pixmapHeight - indicatiorHeight) / 2 : 0;
		QPainter p(&pixmap);
		p.translate(xoff, yoff);
		style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, &p);
	}
	return QIcon(pixmap);
}
