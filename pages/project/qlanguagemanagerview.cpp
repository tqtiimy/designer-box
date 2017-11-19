#include "qlanguagemanagerview.h"

#include "qaddlanguagedialog.h"
#include "quserandlanguagetreewidget.h"

#include "../../libs/shared/qprojectcore.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/core/qactiontoolbar.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/qnamevalidator.h"
#include "../../libs/shared/qlanguagemanager.h"
#include "../../libs/shared/qlanguage.h"
#include "../../libs/core/undocommand/qlanguagepropertyundocommand.h"
#include "../../libs/core/undocommand/qlanguageaddundocommand.h"
#include "../../libs/shared/host/qabstracthost.h"
#include "../../libs/shared/property/qabstractproperty.h"
#include "../../libs/core/undocommand/qpropertychangedundocommand.h"
#include "../../libs/core/qbtnlineedit.h"


#include <QVBoxLayout>
#include <QHeaderView>
#include <QAction>
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QSpinBox>
#include <QKeyEvent>
#include <QMenu>
#include <QUuid>

QWidget *QLanguageDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
	QWidget *ret = NULL;

	if (!m_view->m_removing)
	{
		if (index.column() == 1)
		{
			QNameEdit *edit = new QNameEdit("(.*)", parent);
			edit->setValue(index.data().toString());
			QStringList list;
			list.removeAll(index.data().toString());
			edit->setIntermediateList(list);
			ret = edit;
		}
		else if (index.column() == 2)
		{
			ret = new QBtnLineEdit(parent);
		}

		if (ret != NULL)
		{
			ret->setProperty("no-ManhattanStyle", true);
		}
	}

	return ret;
}

void QLanguageDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	editor->setGeometry(option.rect.adjusted(0, 0, 0, 0));
}

QSize QLanguageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(21);
	return size;
}

void QLanguageDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLanguagePropertyUndoCommand *cmd = NULL;
	QTreeWidgetItem *item = m_view->m_languageList->get_item(index);
	QLanguage *language = m_view->m_item_to_language.value(item);

	if (language != NULL)
	{
		QString property = "";
		QVariant old;
		QString now;

		if (index.column() == 1)
		{
			property = "languageName";
			old = language->get_language_name();
			now = ((QLineEdit *)editor)->text();
		}
		else if (index.column() == 2)
		{
			property = "information";
			old = language->get_information();
			now = ((QBtnLineEdit *)editor)->value();
		}

		if (property != "" && now != old)
		{
			cmd = new QLanguagePropertyUndoCommand(property, language->get_uuid(), old, now);
			m_view->m_undo_stack->push(cmd);
		}
	}
}

void QLanguageDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if (index.column() != 2)
	{
		QItemDelegate::setEditorData(editor, index);
	}
	else
	{
		QTreeWidgetItem *item = m_view->m_languageList->get_item(index);
		QLanguage *l = m_view->m_item_to_language.value(item);
		((QBtnLineEdit *)editor)->setValue(l->get_information());
	}
}

void QLanguageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QLanguageManagerView::QLanguageManagerView(QWidget *parent) :
	QWidget(parent),
	m_languageList(new QUserAddLanguageTreeWidget),
	m_styledBar(new StyledBar),
	m_removing(false)
{

	QVBoxLayout *l = new QVBoxLayout;
	l->setMargin(0);
	l->setSpacing(0);
	l->addWidget(m_styledBar);
	l->addWidget(m_languageList);

	this->setLayout(l);

	m_languageList->setFrameStyle(QFrame::NoFrame);

	QStringList list;
	list << tr("No.") << tr("Language") << tr("Explanation");
	m_languageList->setHeaderLabels(list);
	m_languageList->setAlternatingRowColors(true);
	m_languageList->setEditTriggers(QAbstractItemView::EditKeyPressed);
	m_languageList->setRootIsDecorated(false);
	m_languageList->header()->setMovable(false);
	m_languageList->setItemDelegate(new QLanguageDelegate(this));
	m_languageList->setColumnWidth(0, 100);
	m_languageList->installEventFilter(this);

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

	ac = core->get_action("LanguageManager.Add");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(add_language()));
		aclist.append(ac);
		ac->setEnabled(false);
	}

	ac = core->get_action("LanguageManager.Del");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_language()));
		aclist.append(ac);
		ac->setEnabled(false);
	}

	ac = new QAction(this);
	ac->setSeparator(true);
	aclist.append(ac);


	toolbar->addButtonActions(aclist);

	ac = core->get_action("LanguageManager.Remove");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_select()));
	}

	ac = core->get_action("LanguageManager.Give-Up");

	if (ac != NULL)
	{
		connect(ac, SIGNAL(triggered()), this, SLOT(give_up()));
	}

	QLabel *label = new QLabel;
	label->setText(tr("Language Manager"));
	label->setStyleSheet("color:rgb(255,255,255);");
	toolbar->setLabelWidget(label);
	connect(m_languageList, SIGNAL(clicked(QModelIndex)), m_languageList, SLOT(edit(QModelIndex)));

	QPalette p = m_languageList->palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
	m_languageList->setPalette(p);

	connect(core->get_project_core(), SIGNAL(opened_signals()), this, SLOT(project_opened()));
	connect(core->get_project_core(), SIGNAL(closed_signals()), this, SLOT(project_closed()));
	connect(core->get_project_core()->get_language_manager(), SIGNAL(refresh(QLanguage *)), this, SLOT(refresh(QLanguage *)));
	connect(core->get_project_core()->get_language_manager(), SIGNAL(insert_language_signal(int, QLanguage *)),
	        this, SLOT(insert_language_slot(int, QLanguage *)));
	connect(core->get_project_core()->get_language_manager(), SIGNAL(remove_language_signal(QLanguage *)),
	        this, SLOT(remove_language_slot(QLanguage *)));
}

void QLanguageManagerView::project_opened()
{
	QLanguageManager *manager = QSoftCore::get_core()->get_project_core()->get_language_manager();

	QList<QLanguage *> list = manager->get_all_languages();
	QString str;
	foreach(QLanguage * l, list)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(m_languageList);
		item->setText(0, QString::number(list.indexOf(l) + 1));
		item->setText(1, l->get_language_name());
		str = l->get_information();
		str.replace("\r\n", " ");
		str.replace("\n", " ");
		item->setText(2, str);
		item->setToolTip(2, l->get_information());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		m_items.append(item);
		m_language_to_item.insert(l, item);
		m_item_to_language.insert(item, l);
	}

	QSoftCore *core = QSoftCore::get_core();

	QAction *ac;

	ac = core->get_action("LanguageManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("LanguageManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void QLanguageManagerView::project_closed()
{
	m_language_to_item.clear();
	m_item_to_language.clear();
	m_items.clear();
	m_languageList->clear();

	QSoftCore *core = QSoftCore::get_core();

	QAction *ac;

	ac = core->get_action("LanguageManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("LanguageManager.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}
}

void QLanguageManagerView::add_language()
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{
		QWidget *wid = qApp->focusWidget();

		if (wid != NULL)
		{
			wid->clearFocus();
		}

		QAddLanguageDialog dlg(this);
		dlg.exec();

		if (dlg.get_language() != "")
		{
			QLanguage *l = new QLanguage;
			l->set_language_name(dlg.get_language());
			l->set_file_name(dlg.get_language() + ".xml");
			l->set_uuid(QUuid::createUuid().toString());

			QUndoCommand *cmd = new QUndoCommand;
			QLanguageAddUndoCommand *p = new QLanguageAddUndoCommand(NULL, -1, LAT_EMPTY, cmd);
			new QLanguageAddUndoCommand(l, -1, LAT_ADD, p);

			if (QSoftCore::get_core()->get_project_core()->get_language_manager()->get_all_languages().size() == 0)
			{
				new QPropertyChangedUndoCommand(QSoftCore::get_core()->get_project_core()->get_project_host()->get_uuid(),
				                                "start_language", "", l->get_language_name(), cmd);
			}

			m_undo_stack->push(cmd);
		}
	}
}

void QLanguageManagerView::remove_language()
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{

		QWidget *wid = qApp->focusWidget();

		if (wid != NULL)
		{
			wid->clearFocus();
		}

		m_removing = true;
		m_languageList->set_move(m_removing);

		QSoftCore *core = QSoftCore::get_core();
		QAction *ac;
		ac = core->get_action("LanguageManager.Add");

		if (ac != NULL)
		{
			ac->setEnabled(false);
		}

		ac = core->get_action("LanguageManager.Del");

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

void QLanguageManagerView::contextMenuEvent(QContextMenuEvent *e)
{
	if (QSoftCore::get_core()->get_project_core()->is_opened())
	{
		if (m_removing)
		{
			QMenu menu(this);
			QList<QAction *> list;
			QAction *ac;
			QSoftCore *core = QSoftCore::get_core();
			ac = core->get_action("LanguageManager.Remove");

			if (ac != NULL)
			{
				list.append(ac);
			}

			ac = core->get_action("LanguageManager.Give-Up");

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
			ac = core->get_action("LanguageManager.Add");

			if (ac != NULL)
			{
				list.append(ac);
			}

			ac = core->get_action("LanguageManager.Del");

			if (ac != NULL)
			{
				list.append(ac);
			}

			menu.addActions(list);

			menu.exec(e->globalPos());
		}
	}
}

void QLanguageManagerView::remove_select()
{

	QList<QLanguage *> list;

	bool b = false;
	QAbstractProperty *pro = QSoftCore::get_core()->get_project_core()->get_project_host()->get_property("start_language");
	QString uuid;
	foreach(QTreeWidgetItem * item, m_items)
	{
		QLanguage *l = m_item_to_language.value(item);

		if (item->checkState(0) == Qt::Checked)
		{
			list.append(l);

			if (l->get_uuid() == pro->get_value().toString())
			{
				b = true;
			}
		}
		else
		{
			if (uuid == "")
			{
				uuid = l->get_uuid();
			}
		}
	}

	m_removing = false;
	m_languageList->set_move(m_removing);
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("LanguageManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("LanguageManager.Del");

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
			                                "start_language", pro->get_value(), uuid, cmd);
		}

		QLanguageAddUndoCommand *p = new QLanguageAddUndoCommand(NULL, -1, LAT_EMPTY, cmd);
		foreach(QLanguage * l, list)
		{
			new QLanguageAddUndoCommand(l, m_items.indexOf(m_language_to_item.value(l)) - list.indexOf(l), LAT_REMOVE, p);
		}
		m_undo_stack->push(cmd);
	}
}

void QLanguageManagerView::give_up()
{
	m_removing = false;
	m_languageList->set_move(m_removing);
	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("LanguageManager.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("LanguageManager.Del");

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

void QLanguageManagerView::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QLanguageManagerView::refresh(QLanguage *l)
{
	if (l != NULL)
	{
		QTreeWidgetItem *item = m_language_to_item.value(l);

		if (item != NULL)
		{
			item->setText(1, l->get_language_name());
			QString str = l->get_information();
			str.replace("\r\n", " ");
			str.replace("\n", " ");
			item->setText(2, str);
			item->setToolTip(2, l->get_information());
		}
	}
}

void QLanguageManagerView::insert_language_slot(int index, QLanguage *l)
{
	QTreeWidgetItem *item = new QTreeWidgetItem;
	m_languageList->insertTopLevelItem(index, item);
	item->setText(1, l->get_language_name());
	QString str = l->get_information();
	str.replace("\r\n", " ");
	str.replace("\n", " ");
	item->setText(2, str);
	item->setToolTip(2, l->get_information());
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	m_items.insert(index, item);
	m_language_to_item.insert(l, item);
	m_item_to_language.insert(item, l);
	update_no();
}

void QLanguageManagerView::remove_language_slot(QLanguage *l)
{
	QTreeWidgetItem *item = m_language_to_item.value(l);

	if (item != NULL)
	{
		m_language_to_item.remove(l);
		m_item_to_language.remove(item);
		m_items.removeAll(item);
		delete item;
		update_no();
	}
}

void QLanguageManagerView::update_no()
{
	if (m_removing)
	{
		give_up();
	}

	foreach(QTreeWidgetItem * item, m_items)
	{
		item->setText(0, QString::number(m_items.indexOf(item) + 1));
	}
}
