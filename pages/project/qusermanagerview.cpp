#include "qusermanagerview.h"

#include "qadduserdialog.h"
#include "quserandlanguagetreewidget.h"

#include "../../libs/shared/qprojectcore.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/core/qactiontoolbar.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/qnamevalidator.h"
#include "../../libs/shared/qusermanager.h"
#include "../../libs/core/undocommand/quserpropertyundocommand.h"
#include "../../libs/core/undocommand/quseraddundocommand.h"
#include "../../libs/core/undocommand/qpropertychangedundocommand.h"
#include "../../libs/shared/host/qabstracthost.h"
#include "../../libs/shared/property/qabstractproperty.h"
#include "../../libs/core/qbtnlineedit.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QAction>
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QSpinBox>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QUuid>

QWidget *QUserDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
	if (m_view->m_removing)
	{
		return NULL;
	}

	QWidget *ret = NULL;

	if (index.column() == 1)
	{
		QNameEdit *edit = new QNameEdit("(.*)", parent);
		edit->setValue(index.data().toString());
		QUserManager *manager = QSoftCore::get_core()->get_project_core()->get_user_manager();
		QList<tagUserInfo *> users = manager->get_users();
		QStringList list;
		foreach(tagUserInfo * info, users)
		{
			list.append(info->m_name);
		}
		list.removeAll(index.data().toString());
		edit->setIntermediateList(list);
		ret = edit;
	}
	else if (index.column() == 3)
	{
		QSpinBox *s = new QSpinBox(parent);
		s->setRange(0, 999999);
		s->setValue(index.data().toInt());
		ret = s;
	}
	else if (index.column() == 2)
	{
		QLineEdit *e = new QLineEdit(parent);
		ret = e;
	}
	else if (index.column() == 4)
	{
		QBtnLineEdit *e = new QBtnLineEdit(parent);
		ret = e;
	}

	if (ret != NULL)
	{
		ret->setProperty("no-ManhattanStyle", true);
	}

	return ret;
}

void QUserDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect.adjusted(0, 0, 0, 0));
}

QSize QUserDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	return size;
}

void QUserDelegate::setModelData(QWidget *editor, QAbstractItemModel *, const QModelIndex &index) const
{
	QUserPropertyUndoCommand *cmd = NULL;
	QTreeWidgetItem *item = m_view->m_userList->get_item(index);
	tagUserInfo *info = m_view->m_item_to_user.value(item);

	if (info != NULL)
	{
		QString property = "";
		QVariant old;
		QString now;

		if (index.column() == 1)
		{
			property = "userName";
			old = info->m_name;
			now = ((QLineEdit *)editor)->text();
		}
		else if (index.column() == 2)
		{
			property = "password";
			old = info->m_password;
			now = ((QLineEdit *)editor)->text();
		}
		else if (index.column() == 3)
		{
			property = "level";
			old = info->m_level;
			now = ((QSpinBox *)editor)->text();
		}
		else if (index.column() == 4)
		{
			property = "information";
			old = info->m_information;
			now = ((QBtnLineEdit *)editor)->value();
		}

		if (property != "" && now != old)
		{
			cmd = new QUserPropertyUndoCommand(property, info->m_uuid, old, now);
			m_view->m_undo_stack->push(cmd);
		}
	}
}

void QUserDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if (index.column() != 4)
	{
		QItemDelegate::setEditorData(editor, index);
	}
	else
	{
		QTreeWidgetItem *item = m_view->m_userList->get_item(index);
		tagUserInfo *info = m_view->m_item_to_user.value(item);
		((QBtnLineEdit *)editor)->setValue(info->m_information);
	}
}

void QUserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;

	QColor c = QColor(255, 255, 191);

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

	painter->drawLine(option.rect.left(), option.rect.bottom(), option.rect.right(), option.rect.bottom());
}


QUserManagerView::QUserManagerView(QWidget *parent) :
	QWidget(parent),
	m_userList(new QUserAddLanguageTreeWidget),
	m_styledBar(new StyledBar),
	m_removing(false)
{

	QVBoxLayout *l = new QVBoxLayout;
	l->setMargin(0);
	l->setSpacing(0);
	l->addWidget(m_styledBar);
	l->addWidget(m_userList);

	this->setLayout(l);

	m_userList->setFrameStyle(QFrame::NoFrame);

	QStringList list;
	list << tr("No.") << tr("User Name") << tr("Password") << tr("User Level") << tr("Explanation");
	m_userList->setHeaderLabels(list);
	m_userList->setAlternatingRowColors(true);
	m_userList->setEditTriggers(QAbstractItemView::EditKeyPressed);
	m_userList->setRootIsDecorated(false);
	m_userList->header()->setMovable(false);
	m_userList->setItemDelegate(new QUserDelegate(this));
	m_userList->setColumnWidth(0, 100);
	m_userList->installEventFilter(this);

	QActionToolBar *toolbar = new QActionToolBar(Qt::AlignRight);
	l = new QVBoxLayout;
	l->setMargin(0);
	l->setSpacing(5);
	l->addWidget(toolbar);
	m_styledBar->setLayout(l);

	QSoftCore *core = QSoftCore::get_core();

	QAction *ac;
	QList<QAction *> aclist;

	ac = new QAction(this);
	ac->setSeparator(true);
	aclist.append(ac);

	ac = core->get_action("UserManager.Add");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(add_user()));
		ac->setEnabled(false);
	}

	ac = core->get_action("UserManager.Del");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_user()));
		ac->setEnabled(false);
	}

	ac = new QAction(this);
	ac->setSeparator(true);
	aclist.append(ac);

	toolbar->addButtonActions(aclist);

	ac = core->get_action("UserManager.Remove");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_select()));
	}

	ac = core->get_action("UserManager.Give-Up");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(give_up()));
	}

	QLabel *label = new QLabel;
	label->setText(tr("User Manager"));
	label->setStyleSheet("color:rgb(255,255,255);");
	toolbar->setLabelWidget(label);
	connect(m_userList, SIGNAL(clicked(QModelIndex)), m_userList, SLOT(edit(QModelIndex)));

	QPalette p = m_userList->palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
	m_userList->setPalette(p);

	connect(core->get_project_core(), SIGNAL(opened_signals()), this, SLOT(project_opened()));
	connect(core->get_project_core(), SIGNAL(closed_signals()), this, SLOT(project_closed()));
	connect(core->get_project_core()->get_user_manager(), SIGNAL(refresh(tagUserInfo *)), this, SLOT(refresh(tagUserInfo *)));
	connect(core->get_project_core()->get_user_manager(), SIGNAL(inser_user_signal(tagUserInfo *, int)),
	        this, SLOT(inser_user_slot(tagUserInfo *, int)));
	connect(core->get_project_core()->get_user_manager(), SIGNAL(remove_user_signal(tagUserInfo *)),
	        this, SLOT(remove_user_slot(tagUserInfo *)));
}

void QUserManagerView::project_opened()
{
	QUserManager *manager = QSoftCore::get_core()->get_project_core()->get_user_manager();

	QList<tagUserInfo *> list = manager->get_users();
	QString str;
	foreach(tagUserInfo * info, list)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(m_userList);
		item->setText(0, QString::number(list.indexOf(info) + 1));
		item->setText(1, info->m_name);
		item->setText(2, info->m_password);
		item->setText(3, QString::number(info->m_level));
		str = info->m_information;
		str.replace("\r\n", " ");
		str.replace("\n", " ");
		item->setText(4, str);
		item->setToolTip(4, info->m_information);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		m_items.append(item);
		m_user_to_item.insert(info, item);
		m_item_to_user.insert(item, info);
	}


	QSoftCore *core = QSoftCore::get_core();

	QAction *ac;

	ac = core->get_action("UserManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("UserManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

}

void QUserManagerView::project_closed()
{
	m_user_to_item.clear();
	m_item_to_user.clear();
	m_items.clear();
	m_userList->clear();


	QSoftCore *core = QSoftCore::get_core();

	QAction *ac;

	ac = core->get_action("UserManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("UserManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	m_undo_stack->clear();
}

void QUserManagerView::add_user()
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{
		QWidget *wid = qApp->focusWidget();

		if (wid != NULL)
		{
			wid->clearFocus();
		}

		QAddUserDialog dlg(this);
		dlg.exec();

		QString name = dlg.getName();

		if (name != "")
		{
			tagUserInfo info;
			info.m_information = "";
			info.m_name = name;
			info.m_level = dlg.getLevel();
			info.m_password = dlg.getPassword();
			info.m_uuid = QUuid::createUuid().toString();
			QUndoCommand *cmd = new QUndoCommand;
			QUserAddUndoCommand *p = new QUserAddUndoCommand(NULL, UAT_EMPTY, -1, cmd);
			new QUserAddUndoCommand(&info, UAT_ADD, -1, p);

			if (QSoftCore::get_core()->get_project_core()->get_user_manager()->get_users().size() == 0)
			{
				new QPropertyChangedUndoCommand(QSoftCore::get_core()->get_project_core()->get_project_host()->get_uuid(),
				                                "start_user", "", info.m_uuid, cmd);
			}

			m_undo_stack->push(cmd);
		}
	}
}

void QUserManagerView::remove_user()
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{
		QWidget *wid = qApp->focusWidget();

		if (wid != NULL)
		{
			wid->clearFocus();
		}

		m_removing = true;
		m_userList->set_move(true);

		QSoftCore *core = QSoftCore::get_core();
		QAction *ac;
		ac = core->get_action("UserManager.Add");

		if (ac != NULL)
		{
			ac->setEnabled(false);
		}

		ac = core->get_action("UserManager.Del");

		if (ac != NULL)
		{
			ac->setEnabled(false);
		}

		foreach(QTreeWidgetItem * item, m_items)
		{
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
			item->setCheckState(0, Qt::Unchecked);
		}
	}
}

void QUserManagerView::contextMenuEvent(QContextMenuEvent *e)
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{
		if (m_removing)
		{
			QMenu menu(this);
			QList<QAction *> list;
			QAction *ac;
			QSoftCore *core = QSoftCore::get_core();
			ac = core->get_action("UserManager.Remove");

			if (ac != NULL)
			{
				list.append(ac);
			}

			ac = core->get_action("UserManager.Give-Up");

			if (ac != NULL)
			{
				list.append(ac);
			}

			menu.addActions(list);

			menu.exec(e->globalPos());
		}
		else
		{
			QMenu menu(this);
			QList<QAction *> list;
			QAction *ac;
			QSoftCore *core = QSoftCore::get_core();
			ac = core->get_action("UserManager.Add");

			if (ac != NULL)
			{
				list.append(ac);
			}

			ac = core->get_action("UserManager.Del");

			if (ac != NULL)
			{
				list.append(ac);
			}

			menu.addActions(list);

			menu.exec(e->globalPos());
		}
	}
}

void QUserManagerView::remove_select()
{

	QList<tagUserInfo *> list;

	bool b = false;
	QAbstractProperty *pro = QSoftCore::get_core()->get_project_core()->get_project_host()->get_property("start_user");
	QString uuid;
	foreach(QTreeWidgetItem * item, m_items)
	{
		tagUserInfo *info = m_item_to_user.value(item);

		if (item->checkState(0) == Qt::Checked)
		{
			list.append(info);

			if (info->m_uuid == pro->get_value().toString())
			{
				b = true;
			}
		}
		else
		{
			if (uuid == "")
			{
				uuid = info->m_uuid;
			}
		}
	}

	m_removing = false;
	m_userList->set_move(false);
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("UserManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("UserManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	foreach(QTreeWidgetItem * item, m_items)
	{
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		item->setData(0, Qt::CheckStateRole, QVariant());
	}

	if (list.size() > 0)
	{
		QUndoCommand *cmd = new QUndoCommand;

		if (b)
		{
			new QPropertyChangedUndoCommand(QSoftCore::get_core()->get_project_core()->get_project_host()->get_uuid(),
			                                "start_user", pro->get_value(), uuid, cmd);
		}

		QUserAddUndoCommand *p = new QUserAddUndoCommand(NULL, UAT_EMPTY, -1, cmd);
		foreach(tagUserInfo * info, list)
		{
			new QUserAddUndoCommand(info, UAT_REMOVE, m_items.indexOf(m_user_to_item.value(info)) - list.indexOf(info), p);
		}
		m_undo_stack->push(cmd);
	}
}

void QUserManagerView::give_up()
{
	m_removing = false;
	m_userList->set_move(false);
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("UserManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("UserManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	foreach(QTreeWidgetItem * item, m_items)
	{
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		item->setData(0, Qt::CheckStateRole, QVariant());
	}
}

void QUserManagerView::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QUserManagerView::refresh(tagUserInfo *info)
{
	if (info != NULL)
	{
		QTreeWidgetItem *item = m_user_to_item.value(info);

		if (item != NULL)
		{
			item->setText(1, info->m_name);
			item->setText(2, info->m_password);
			item->setText(3, QString::number(info->m_level));
			QString str = info->m_information;
			str.replace("\r\n", " ");
			str.replace("\n", " ");
			item->setText(4, str);
			item->setToolTip(4, info->m_information);
		}
	}
	else
	{
		update_no();
	}
}

void QUserManagerView::inser_user_slot(tagUserInfo *info, int index)
{
	QTreeWidgetItem *item = m_user_to_item.value(info);

	if (item == NULL)
	{
		item = new QTreeWidgetItem;
		m_userList->insertTopLevelItem(index, item);
		item->setText(1, info->m_name);
		item->setText(2, info->m_password);
		item->setText(3, QString::number(info->m_level));
		QString str = info->m_information;
		str.replace("\r\n", " ");
		str.replace("\n", " ");
		item->setText(4, str);
		item->setToolTip(4, info->m_information);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		m_items.insert(index, item);
		m_user_to_item.insert(info, item);
		m_item_to_user.insert(item, info);
	}
}

void QUserManagerView::remove_user_slot(tagUserInfo *info)
{
	QTreeWidgetItem *item = m_user_to_item.value(info);

	if (item != NULL)
	{
		m_user_to_item.remove(info);
		m_item_to_user.remove(item);
		m_items.removeAll(item);
		delete item;
	}
}

void QUserManagerView::update_no()
{
	if (m_removing)
	{
		give_up();
	}

	for (int i = 0; i < m_items.size(); i++)
	{
		m_items.at(i)->setText(0, QString::number(i + 1));
	}
}
