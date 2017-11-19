#include "qdriverdatalistview.h"

#include "qdatachossedialog.h"
#include "qdriverdataadddialog.h"

#include "../../libs/shared/driver/qabstractdriver.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qdatamanager.h"
#include "../../libs/shared/qdata.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/qnamevalidator.h"
#include "../../libs/core/qbtnlineedit.h"

#include "../../libs/core/undocommand/qdriverdataaddundocommand.h"
#include "../../libs/core/undocommand/qdriverdatachangedundocommand.h"

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>
#include <QSpinBox>
#include <QComboBox>
#include <QMenu>
#include <QUuid>

QString DataType[DT_WRITE + 1] =
{
	"Read",
	"Write"
};

QWidget *QDriverDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
	if (m_view->m_is_remove)
	{
		return NULL;
	}

	QWidget *ret = NULL;
	tagDriverDataInfo *data = m_view->m_item_to_data.value(m_view->itemFromIndex(index));

	switch (index.column())
	{
	case LH_NAME:
		{
			QNameEdit *e = new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}", parent);
			QStringList l = m_view->m_driver->get_all_data_names();
			l.removeAll(data->m_name);
			e->setIntermediateList(l);
			e->setContextMenuPolicy(Qt::NoContextMenu);
			ret = e;
		}
		break;

	case LH_ADDRESS:
	case LH_PERIOD:
	case LH_LEVEL:
		{
			QSpinBox *s = new QSpinBox(parent);
			s->setRange(0, 9999999);
			ret = s;
		}
		break;

	case LH_SCALE:
		{
			QComboBox *b = new QComboBox(parent);
			QStringList list;
			list << "1" << "-1" << "10" << "-10" << "100" << "-100" << "1000" << "-1000" << "10000" << "-10000";
			b->addItems(list);
			ret = b;
		}
		break;

	case LH_INFORMATION:
		{
			QBtnLineEdit *e = new QBtnLineEdit(parent);
			e->setContextMenuPolicy(Qt::NoContextMenu);
			ret = e;
		}
		break;
	}

	if (ret != NULL)
	{
		ret->setProperty("no-ManhattanStyle", true);
	}

	return ret;
}

void QDriverDataDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	editor->setGeometry(option.rect.adjusted(0, 0, 0, 0));
}

QSize QDriverDataDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	size.rwidth() += 4;
	return size;
}

void QDriverDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(qApp->style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->setPen(color);

	int right = option.direction == Qt::LeftToRight ? option.rect.right() : option.rect.left();
	painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
}

void QDriverDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if (editor->inherits("QBtnLineEdit"))
	{
		QBtnLineEdit *e = (QBtnLineEdit *)editor;
		e->setValue(index.data(Qt::UserRole).toString());
	}
	else if (editor->inherits("QComboBox"))
	{
		QComboBox *b = (QComboBox *)editor;
		b->setCurrentIndex(b->findText(index.data().toString()));
	}
	else
	{
		QItemDelegate::setEditorData(editor, index);
	}
}

void QDriverDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QVariant old_value;
	QVariant new_value;
	QString  key;
	QString  driver_uuid;
	QString  data_uuid;

	tagDriverDataInfo *data = m_view->m_item_to_data.value(m_view->itemFromIndex(index));

	if (data != NULL)
	{
		data_uuid = data->m_uuid;
		driver_uuid = m_view->m_driver->get_uuid();
		int column = index.column();

		if (column == LH_NAME)
		{
			key = "name";
			old_value = data->m_name;
			QNameEdit *e = (QNameEdit *)editor;
			new_value = e->value();
		}
		else if (column == LH_ADDRESS)
		{
			key = "address";
			old_value = data->m_data_address;
			QSpinBox *s = (QSpinBox *)editor;
			new_value = s->value();
		}
		else if (column == LH_SCALE)
		{
			key = "scale";
			old_value = data->m_scale;
			QComboBox *b = (QComboBox *)editor;
			new_value = b->currentText().toInt();
		}
		else if (column == LH_PERIOD)
		{
			key = "period";
			old_value = data->m_period;
			QSpinBox *s = (QSpinBox *)editor;
			new_value = s->value();
		}
		else if (column == LH_LEVEL)
		{
			key = "level";
			old_value = data->m_level;
			QSpinBox *s = (QSpinBox *)editor;
			new_value = s->value();
		}
		else if (column == LH_INFORMATION)
		{
			key = "information";
			old_value = data->m_information;
			QBtnLineEdit *b = (QBtnLineEdit *)editor;
			new_value = b->value();
		}

		if (key != "" && old_value != new_value)
		{
			QDriverDataChangedUndoCommand *cmd = new QDriverDataChangedUndoCommand(
			    driver_uuid, data_uuid, key, old_value, new_value);
			m_view->m_undo_stack->push(cmd);
		}
	}
}

QDriverDataListView::QDriverDataListView(QAbstractDriver *driver, QWidget *parent):
	QTreeWidget(parent),
	m_driver(driver),
	m_is_remove(false)
{
	this->setFrameStyle(QFrame::NoFrame);
	this->header()->setMovable(false);
	this->setItemDelegate(new QDriverDataDelegate(this));
	this->setAlternatingRowColors(true);
	this->setRootIsDecorated(false);

	QStringList list;
	list << tr("Name") << tr("Address") << tr("Data") << tr("Scale") << tr("Period") << tr("Level") << tr("Type") << tr("Inforamtion");
	setHeaderLabels(list);

	init();

	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(double_clicked_item(QTreeWidgetItem *, int)));

	connect(m_driver, SIGNAL(insert_data_signals(tagDriverDataInfo *, int)), this, SLOT(insert_data(tagDriverDataInfo *, int)));
	connect(m_driver, SIGNAL(remove_data_signals(tagDriverDataInfo *)), this, SLOT(remove_data(tagDriverDataInfo *)));
	connect(m_driver, SIGNAL(data_refresh(tagDriverDataInfo *)), this, SLOT(data_refresh(tagDriverDataInfo *)));
}

void QDriverDataListView::init()
{
	QList<tagDriverDataInfo *>   datas = m_driver->get_all_data();

	foreach(tagDriverDataInfo * data, datas)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(this);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		item->setText(LH_NAME, data->m_name);
		item->setText(LH_ADDRESS, QString::number(data->m_data_address));
		tagDataInfo *d = get_data(data->m_data_uuid);

		if (d != NULL)
		{
			item->setText(LH_DATA, d->m_data->get_name() + "." + d->m_name);
		}
		else
		{
			item->setText(LH_DATA, QString::number(data->m_temp_data));
		}

		item->setText(LH_PERIOD, QString::number(data->m_period));
		item->setText(LH_LEVEL, QString::number(data->m_level));
		item->setText(LH_TYPE, DataType[data->m_type]);
		QString str = data->m_information;
		str.replace("\r\n", " ");
		str.replace("\n", " ");
		item->setText(LH_INFORMATION, str);
		item->setData(LH_INFORMATION, Qt::UserRole, data->m_information);
		item->setText(LH_SCALE, QString::number(data->m_scale));

		for (int i = 0; i < LH_BOTTOM; i++)
		{
			item->setToolTip(i, item->text(i));
		}

		m_item_to_data.insert(item, data);
		m_data_to_item.insert(data, item);
	}
}

tagDataInfo *QDriverDataListView::get_data(const QString &uuid)
{
	QDataManager *manager = QSoftCore::get_core()->get_project_core()->get_data_manager();
	return manager->get_data(uuid);
}

void QDriverDataListView::double_clicked_item(QTreeWidgetItem *item, int col)
{
	tagDriverDataInfo *data = m_item_to_data.value(item);

	if (col != LH_TYPE && col != LH_DATA)
	{
		editItem(item, col);
	}
	else if (col == LH_DATA)
	{
		QString old_value = data->m_data_uuid;

		if (old_value == "")
		{
			old_value = QString::number(data->m_temp_data);
		}

		QDataChosseDialog dlg(old_value, this);
		dlg.exec();
		QString new_value = dlg.get_data();

		if (new_value != "")
		{

			if (old_value != new_value)
			{
				QDriverDataChangedUndoCommand *cmd = new QDriverDataChangedUndoCommand(m_driver->get_uuid(),
				        data->m_uuid,
				        "data",
				        old_value,
				        new_value);
				m_undo_stack->push(cmd);
			}
		}
	}
	else
	{
		if (data != NULL)
		{
			QVariant new_value = (data->m_type == DT_READ ? DT_WRITE : DT_READ);
			QDriverDataChangedUndoCommand *cmd = new QDriverDataChangedUndoCommand(m_driver->get_uuid(),
			        data->m_uuid,
			        "type",
			        data->m_type,
			        new_value);
			m_undo_stack->push(cmd);
		}
	}
}

void QDriverDataListView::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QDriverDataListView::data_refresh(tagDriverDataInfo *data)
{
	QTreeWidgetItem *item = m_data_to_item.value(data);

	item->setText(LH_NAME, data->m_name);
	item->setText(LH_ADDRESS, QString::number(data->m_data_address));
	tagDataInfo *d = get_data(data->m_data_uuid);

	if (d != NULL)
	{
		item->setText(LH_DATA, d->m_data->get_name() + "." + d->m_name);
	}
	else
	{
		item->setText(LH_DATA, QString::number(data->m_temp_data));
	}

	item->setText(LH_PERIOD, QString::number(data->m_period));
	item->setText(LH_LEVEL, QString::number(data->m_level));
	item->setText(LH_TYPE, DataType[data->m_type]);
	QString str = data->m_information;
	str.replace("\r\n", " ");
	str.replace("\n", " ");
	item->setText(LH_INFORMATION, str);
	item->setData(LH_INFORMATION, Qt::UserRole, data->m_information);
	item->setText(LH_SCALE, QString::number(data->m_scale));

	for (int i = 0; i < LH_BOTTOM; i++)
	{
		item->setToolTip(i, item->text(i));
	}

	emit active();
}

void QDriverDataListView::contextMenuEvent(QContextMenuEvent *e)
{
	QMenu menu;

	if (!m_is_remove)
	{
		menu.addAction(tr("Add"), this, SLOT(add()));
		menu.addAction(tr("Delete"), this, SLOT(remove()));
	}
	else
	{
		menu.addAction(tr("Delete Select"), this, SLOT(delete_select()));
		menu.addAction(tr("Give Up"), this, SLOT(give_up()));
		menu.addSeparator();
		menu.addAction(tr("Check All"), this, SLOT(check_all()));
		menu.addAction(tr("Uncheck All"), this, SLOT(uncheck_all()));
	}

	menu.exec(e->globalPos());
}

void QDriverDataListView::add()
{
	QDriverDataAddDialog dlg(m_driver->get_all_data_names(), this);
	dlg.exec();
	QString name = dlg.get_name();

	if (name != "")
	{
		tagDriverDataInfo data;
		data.m_name = name;
		data.m_uuid = QUuid::createUuid().toString();
		QDriverDataAddUndoCommand *cmd = new QDriverDataAddUndoCommand(
		    m_driver->get_uuid(), data, m_driver->get_all_data().size(), DDAT_ADD);
		m_undo_stack->push(cmd);
	}
}

void QDriverDataListView::remove()
{
	QWidget *wid = qApp->focusWidget();

	if (wid != NULL)
	{
		wid->clearFocus();
	}

	foreach(QTreeWidgetItem * item, m_item_to_data.keys())
	{
		item->setCheckState(0, Qt::Unchecked);
	}
	m_is_remove = true;
	emit start_remove();
}

void QDriverDataListView::delete_select()
{
	QList<tagDriverDataInfo *> datas;

	int count = topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = topLevelItem(i);

		if (item->checkState(0) == Qt::Checked)
		{
			datas.append(m_item_to_data.value(item));
		}

		item->setData(0, Qt::CheckStateRole, QVariant());
	}

	QUndoCommand *cmd = new QUndoCommand;
	QList<tagDriverDataInfo *> list = m_driver->get_all_data();

	for (int i = datas.size() - 1; i >= 0; i--)
	{
		new QDriverDataAddUndoCommand(m_driver->get_uuid(), *datas.at(i), list.indexOf(datas.at(i)), DDAT_REMOVE, cmd);
	}

	m_undo_stack->push(cmd);
	m_is_remove = false;
	emit end_remove();
}

void QDriverDataListView::give_up()
{
	int count = topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = topLevelItem(i);
		item->setData(0, Qt::CheckStateRole, QVariant());
	}

	m_is_remove = false;
	emit end_remove();
}

void QDriverDataListView::insert_data(tagDriverDataInfo *data, int index)
{
	if (m_is_remove)
	{
		give_up();
	}

	QTreeWidgetItem *item = new QTreeWidgetItem;
	this->insertTopLevelItem(index, item);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	item->setText(LH_NAME, data->m_name);
	item->setText(LH_ADDRESS, QString::number(data->m_data_address));
	tagDataInfo *d = get_data(data->m_data_uuid);

	if (d != NULL)
	{
		item->setText(LH_DATA, d->m_data->get_name() + "." + d->m_name);
	}
	else
	{
		item->setText(LH_DATA, QString::number(data->m_temp_data));
	}

	item->setText(LH_PERIOD, QString::number(data->m_period));
	item->setText(LH_LEVEL, QString::number(data->m_level));
	item->setText(LH_TYPE, DataType[data->m_type]);
	QString str = data->m_information;
	str.replace("\r\n", " ");
	str.replace("\n", " ");
	item->setText(LH_INFORMATION, str);
	item->setData(LH_INFORMATION, Qt::UserRole, data->m_information);
	item->setText(LH_SCALE, QString::number(data->m_scale));

	for (int i = 0; i < LH_BOTTOM; i++)
	{
		item->setToolTip(i, item->text(i));
	}

	m_item_to_data.insert(item, data);
	m_data_to_item.insert(data, item);
}

void QDriverDataListView::remove_data(tagDriverDataInfo *data)
{
	if (m_is_remove)
	{
		give_up();
	}

	QTreeWidgetItem *item = m_data_to_item.value(data);
	m_data_to_item.remove(data);

	if (item != NULL)
	{
		m_item_to_data.remove(item);
		delete item;
	}
}

void QDriverDataListView::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton && m_is_remove)
	{
		QTreeWidgetItem *item = itemAt(event->pos());

		if (item != NULL)
		{
			item->setCheckState(0, item->checkState(0) == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
		}
	}
}

void QDriverDataListView::check_all()
{
	if (!m_is_remove)
	{
		return;
	}

	int count = topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = topLevelItem(i);
		item->setCheckState(0, Qt::Checked);
	}
}

void QDriverDataListView::uncheck_all()
{
	if (!m_is_remove)
	{
		return;
	}

	int count = topLevelItemCount();

	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = topLevelItem(i);
		item->setCheckState(0, Qt::Unchecked);
	}
}
