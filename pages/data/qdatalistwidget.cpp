#include "qdatalistwidget.h"

#include "qadddatadialog.h"

#include "../../libs/shared/qdata.h"
#include "../../libs/shared/qdatamanager.h"
#include "../../libs/core/qnamevalidator.h"
#include "../../libs/core/qbtnlineedit.h"
#include "../../libs/core/undocommand/qdatapropertychanged.h"
#include "../../libs/core/undocommand/qdataaddundocommand.h"

#include <QUuid>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMetaProperty>
#include <QContextMenuEvent>
#include <QMenu>

QWidget *QDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
	if (m_view->m_is_remove)
	{
		return NULL;
	}

	QWidget *ret = NULL;
	QTreeWidgetItem *item = m_view->itemFromIndex(index);
	tagDataInfo    *data = m_view->m_item_to_data.value(item);

	switch (index.column())
	{
	case DT_NAME:
		{
			QNameEdit *e = new QNameEdit("[_a-zA-Z][_a-zA-Z0-9]{,64}", parent);
			QStringList l = m_view->m_data->get_all_data_names();
			l.removeAll(data->m_name);
			e->setIntermediateList(l);
			e->setContextMenuPolicy(Qt::NoContextMenu);
			ret = e;
		}
		break;

	case DT_VALUE:
		{
			if (data->m_type == "String")
			{
				QBtnLineEdit *e = new QBtnLineEdit(parent);
				e->setContextMenuPolicy(Qt::NoContextMenu);
				ret = e;
			}
			else if (data->m_type == "Number")
			{
				QSpinBox *s = new QSpinBox(parent);
				s->setRange(-9999999, 9999999);
				s->setContextMenuPolicy(Qt::NoContextMenu);
				ret = s;
			}
			else if (data->m_type == "Float")
			{
				QDoubleSpinBox *s = new QDoubleSpinBox(parent);
				s->setRange(-9999999, 9999999);
				s->setContextMenuPolicy(Qt::NoContextMenu);
				s->setDecimals(6);
				ret = s;
			}

		}
		break;

	case DT_PERIOD:
		{
			QSpinBox *s = new QSpinBox(parent);
			s->setRange(0, 999999);
			s->setContextMenuPolicy(Qt::NoContextMenu);
			ret = s;
		}
		break;

	case DT_INFORMATION:
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

void QDataDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	editor->setGeometry(option.rect.adjusted(0, 0, 0, 0));
}

QSize QDataDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	size.rwidth() += 4;
	return size;
}

void QDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

void QDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QTreeWidgetItem *item = m_view->itemFromIndex(index);
	tagDataInfo    *data = m_view->m_item_to_data.value(item);

	if (editor->inherits("QBtnLineEdit"))
	{
		QBtnLineEdit *e = (QBtnLineEdit *)editor;

		if (index.column() == DT_VALUE)
		{
			e->setValue(data->m_value.toString());
		}
		else if (index.column() == DT_INFORMATION)
		{
			e->setValue(data->m_information);
		}
	}
	else
	{
		QItemDelegate::setEditorData(editor, index);
	}
}

void QDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QTreeWidgetItem *item = m_view->itemFromIndex(index);
	tagDataInfo    *data = m_view->m_item_to_data.value(item);

	QString property_name;
	QVariant new_value;

	if (editor->inherits("QBtnLineEdit"))
	{
		QBtnLineEdit *e = (QBtnLineEdit *)editor;
		new_value = e->value();
	}
	else
	{
		new_value = editor->metaObject()->userProperty().read(editor);
	}

	QVariant old_value;

	if (index.column() == DT_NAME)
	{
		property_name = "name";
		old_value = data->m_name;
	}
	else if (index.column() == DT_VALUE)
	{
		property_name = "value";
		old_value = data->m_value;
	}
	else if (index.column() == DT_PERIOD)
	{
		property_name = "save";
		old_value = data->m_save_time;
	}
	else if (index.column() == DT_INFORMATION)
	{
		property_name = "information";
		old_value = data->m_information;
	}

	if (property_name != NULL && new_value != old_value)
	{
		QDataPropertyChanged *cmd = new QDataPropertyChanged(property_name, data->m_uuid,
		        old_value, new_value);
		m_view->m_undo_stack->push(cmd);
	}

}

QDataListWidget::QDataListWidget(QData *data, QUndoStack *stack, QWidget *parent):
	QTreeWidget(parent),
	m_data(data),
	m_undo_stack(stack),
	m_is_remove(false)
{
	QStringList list;
	list << tr("Name") << tr("Value") << tr("Type") << tr("Hold") << tr("Save Period") << tr("Information");
	setHeaderLabels(list);
	setAlternatingRowColors(true);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setRootIsDecorated(false);
	header()->setMovable(false);
	setFrameStyle(QFrame::NoFrame);
	setItemDelegate(new QDataDelegate(this));

	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(double_clicked(QModelIndex)));
	connect(m_data, SIGNAL(refresh(tagDataInfo *)), this, SLOT(refresh_item(tagDataInfo *)));
	connect(m_data, SIGNAL(data_insert(tagDataInfo *, int)), this, SLOT(data_insert_slot(tagDataInfo *, int)));
	connect(m_data, SIGNAL(data_remove(tagDataInfo *)), this, SLOT(data_remove_slot(tagDataInfo *)));
	init();

	QPalette p = palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
	setPalette(p);
}

void QDataListWidget::init()
{
	foreach(tagDataInfo * info, m_data->get_datas())
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(this);

		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

		item->setText(DT_NAME, info->m_name);
		QString temp;
		temp = info->m_value.toString();
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_VALUE, temp);
		item->setToolTip(DT_VALUE, info->m_value.toString());
		item->setText(DT_HOLD, info->m_hold ? "true" : "false");
		item->setText(DT_TYPE, info->m_type);
		item->setText(DT_PERIOD, QString::number(info->m_save_time));
		temp = info->m_information;
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_INFORMATION, temp);
		item->setToolTip(DT_INFORMATION, info->m_information);

		m_item_to_data.insert(item, info);
		m_data_to_item.insert(info, item);
	}
}

void QDataListWidget::double_clicked(const QModelIndex &index)
{
	if (m_is_remove || m_data->get_name() == "System")
	{
		return;
	}

	QTreeWidgetItem *item = itemFromIndex(index);
	tagDataInfo    *data = m_item_to_data.value(item);

	if (index.column() == DT_HOLD)
	{
		QString property_name = "hold";
		QVariant new_value = !data->m_hold;
		QVariant old_value = data->m_hold;
		QDataPropertyChanged *cmd = new QDataPropertyChanged(property_name, data->m_uuid,
		        old_value, new_value);
		m_undo_stack->push(cmd);
	}
	else if (index.column() == DT_VALUE && data->m_type == "Bool")
	{
		QString property_name = "value";
		QVariant new_value = !data->m_value.toBool();
		QVariant old_value = data->m_value;
		QDataPropertyChanged *cmd = new QDataPropertyChanged(property_name, data->m_uuid,
		        old_value, new_value);
		m_undo_stack->push(cmd);
	}
	else
	{
		edit(index);
	}
}

void QDataListWidget::refresh_item(tagDataInfo *data)
{
	QTreeWidgetItem *item = m_data_to_item.value(data);

	if (item != NULL)
	{
		item->setText(DT_NAME, data->m_name);
		QString temp;
		temp = data->m_value.toString();
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_VALUE, temp);
		item->setToolTip(DT_VALUE, data->m_value.toString());
		item->setText(DT_HOLD, data->m_hold ? "true" : "false");
		item->setText(DT_PERIOD, QString::number(data->m_save_time));
		temp = data->m_information;
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_INFORMATION, temp);
		item->setToolTip(DT_INFORMATION, data->m_information);
	}
}

void QDataListWidget::contextMenuEvent(QContextMenuEvent *e)
{
	if (m_data->get_name() != "System")
	{
		QMenu menu;

		if (m_is_remove)
		{
			menu.addAction(tr("Delete Data"), this, SLOT(delete_select()));
			menu.addAction(tr("Give Up"), this, SLOT(give_up()));
			menu.addSeparator();
			menu.addAction(tr("Check All"), this, SLOT(check_all()));
			menu.addAction(tr("Uncheck All"), this, SLOT(uncheck_all()));
		}
		else
		{
			menu.addAction(tr("Add Data"), this, SLOT(add_data()));
			menu.addAction(tr("Remove Data"), this, SLOT(remove_data()));
		}

		menu.exec(e->globalPos());
	}
}

void QDataListWidget::add_data()
{
	QAddDataDialog dlg(m_data->get_all_data_names(), this);
	dlg.exec();

	QString name = dlg.get_name();

	if (name != "")
	{
		tagDataInfo info;
		info.m_name = name;
		info.m_uuid = QUuid::createUuid().toString();
		info.m_type = dlg.get_type();
		info.m_hold = false;
		info.m_save_time = 0;

		if (info.m_type == "Bool")
		{
			info.m_value = "false";
		}
		else if (info.m_type == "Float" || info.m_type == "Number")
		{
			info.m_value = "0";
		}
		else
		{
			info.m_value = "";
		}

		QDataAddUndoCommand *cmd = new QDataAddUndoCommand(m_data->get_uuid(), info, m_data->get_datas().size(),
		        DAT_ADD);
		m_undo_stack->push(cmd);
	}

}

void QDataListWidget::remove_data()
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

void QDataListWidget::check_all()
{
	foreach(QTreeWidgetItem * item, m_item_to_data.keys())
	{
		item->setCheckState(0, Qt::Checked);
	}
}

void QDataListWidget::uncheck_all()
{
	foreach(QTreeWidgetItem * item, m_item_to_data.keys())
	{
		item->setCheckState(0, Qt::Unchecked);
	}
}

void QDataListWidget::mousePressEvent(QMouseEvent *event)
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

void QDataListWidget::delete_select()
{
	QList<tagDataInfo *> list;
	QList<tagDataInfo *> l = m_data->get_datas();

	foreach(tagDataInfo * data, l)
	{
		QTreeWidgetItem *item = m_data_to_item.value(data);

		if (item->checkState(0) == Qt::Checked)
		{
			list.append(m_item_to_data.value(item));
		}
	}

	foreach(QTreeWidgetItem * item, m_item_to_data.keys())
	{
		item->setData(0, Qt::CheckStateRole, QVariant());
	}
	m_is_remove = false;

	if (list.size() > 0)
	{
		QUndoCommand *cmd = new QUndoCommand;

		for (int i = list.size() - 1; i >= 0; i--)
		{
			tagDataInfo *data = list.at(i);
			new QDataAddUndoCommand(m_data->get_uuid(), *data, l.indexOf(data), DAT_REMOVE, cmd);
		}

		m_undo_stack->push(cmd);
	}

	emit end_remove();
}

void QDataListWidget::give_up()
{
	foreach(QTreeWidgetItem * item, m_item_to_data.keys())
	{
		item->setData(0, Qt::CheckStateRole, QVariant());
	}
	m_is_remove = false;
	emit end_remove();
}

void QDataListWidget::data_insert_slot(tagDataInfo *data, int index)
{
	if (m_is_remove)
	{
		give_up();
	}

	QTreeWidgetItem *item = m_data_to_item.value(data);

	if (item == NULL)
	{
		item = new QTreeWidgetItem;
		this->insertTopLevelItem(index, item);
		item->setText(DT_NAME, data->m_name);
		QString temp;
		temp = data->m_value.toString();
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_VALUE, temp);
		item->setToolTip(DT_VALUE, data->m_value.toString());
		item->setText(DT_HOLD, data->m_hold ? "true" : "false");
		item->setText(DT_TYPE, data->m_type);
		item->setText(DT_PERIOD, QString::number(data->m_save_time));
		temp = data->m_information;
		temp.replace("\r\n", " ");
		temp.replace("\n", " ");
		item->setText(DT_INFORMATION, temp);
		item->setToolTip(DT_INFORMATION, data->m_information);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

		m_item_to_data.insert(item, data);
		m_data_to_item.insert(data, item);
	}

	emit activity();
}

void QDataListWidget::data_remove_slot(tagDataInfo *data)
{
	if (m_is_remove)
	{
		give_up();
	}

	QTreeWidgetItem *item = m_data_to_item.value(data);

	if (item != NULL)
	{
		m_data_to_item.remove(data);
		m_item_to_data.remove(item);
		delete item;
	}

	emit activity();
}
