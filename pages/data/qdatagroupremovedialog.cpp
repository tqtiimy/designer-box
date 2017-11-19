#include "qdatagroupremovedialog.h"
#include "ui_qdatagroupremovedialog.h"

#include "../../libs/core/qsoftcore.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qdatamanager.h"
#include "../../libs/shared/qdata.h"

#include <QItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>

class QGroupDelegate: public QItemDelegate
{
public :
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

QSize QGroupDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	size.rwidth() += 4;
	return size;
}

void QGroupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QDataGroupRemoveDialog::QDataGroupRemoveDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QDataGroupRemoveDialog)
{
	ui->setupUi(this);

	ui->group_list_view->setAlternatingRowColors(true);
	ui->group_list_view->setRootIsDecorated(false);
	ui->group_list_view->setItemDelegate(new QGroupDelegate);

	connect(ui->group_list_view, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(item_clicked(QTreeWidgetItem *)));

	QDataManager *manager = QSoftCore::get_core()->get_project_core()->get_data_manager();

	m_true_icon = get_icon(true);
	m_false_icon = get_icon(false);

	foreach(QData * data, manager->get_all_datas())
	{
		if (data->get_name() != "System")
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(ui->group_list_view);

			item->setText(0, data->get_name());
			item->setData(0, Qt::DecorationRole, m_false_icon);
			item->setData(0, Qt::UserRole, false);
			m_item_to_uuid.insert(item, data->get_uuid());
		}
	}

}

QDataGroupRemoveDialog::~QDataGroupRemoveDialog()
{
	delete ui;
}

void QDataGroupRemoveDialog::on_okbtn_clicked()
{
	int count = ui->group_list_view->topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		if (ui->group_list_view->topLevelItem(i)->data(0, Qt::UserRole).toBool())
		{
			m_result.append(m_item_to_uuid.value(ui->group_list_view->topLevelItem(i)));
		}
	}

	if (m_result.size() == 0)
	{
		return;
	}

	close();
}

void QDataGroupRemoveDialog::on_cancelbtn_clicked()
{
	close();
}

QStringList QDataGroupRemoveDialog::get_result()
{
	return m_result;
}

void QDataGroupRemoveDialog::item_clicked(QTreeWidgetItem *item)
{
	bool check = item->data(0, Qt::UserRole).toBool();
	item->setData(0, Qt::UserRole, !check);
	item->setData(0, Qt::DecorationRole, check ? m_false_icon : m_true_icon);
}

QIcon QDataGroupRemoveDialog::get_icon(bool check)
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
