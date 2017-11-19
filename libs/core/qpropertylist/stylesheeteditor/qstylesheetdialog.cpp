#include "qstylesheetdialog.h"

#include "qsheetitemlist.h"
#include "qshowwidget.h"
#include "qbaseeditorwidget.h"
#include "qaddsheetitemdialog.h"

#include "../../minisplitter.h"
#include "../../undocommand/qpropertychangedundocommand.h"

#include "../../../shared/property/stylesheetitem/qstylesheetitemfactory.h"
#include "../../../shared/property/stylesheetitem/stylesheetstruct.h"
#include "../../../shared/property/qabstractproperty.h"
#include "../../../shared/property/stylesheetitem/qabstractstylesheetitem.h"
#include "../../../shared/host/qabstracthost.h"
#include "../../../shared/xmlobject.h"
#include "../../../shared/qhostfactory.h"

#include <QVBoxLayout>

QStyleSheetDialog::QStyleSheetDialog(QAbstractProperty *property, QUndoStack *stack, QWidget *parent) :
	QDialog(parent),
	m_item_list(new QSheetItemList),
	m_show_widget(new QShowWidget),
	m_property(property),
	m_undo_stack(stack),
	m_stacked_widget(new QStackedWidget)
{
	MiniSplitter *s = new MiniSplitter;


	MiniSplitter *ss = new MiniSplitter(Qt::Vertical);
	ss->addWidget(m_stacked_widget);
	ss->addWidget(m_show_widget);
	ss->setStretchFactor(0, 0);
	ss->setStretchFactor(1, 1);

	s->addWidget(m_item_list);
	s->addWidget(ss);

	s->setStretchFactor(0, 0);
	s->setStretchFactor(1, 1);

	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);

	v->addWidget(s);

	this->setLayout(v);

	connect(m_item_list, SIGNAL(select(QAbstractStylesheetItem *)), this, SLOT(select_changed(QAbstractStylesheetItem *)));
	connect(m_show_widget, SIGNAL(clear()), this, SLOT(clear()));
	connect(m_show_widget, SIGNAL(clearall()), this, SLOT(clearall()));
	connect(m_show_widget, SIGNAL(ok()), this, SLOT(ok()));
	connect(m_show_widget, SIGNAL(cancel()), this, SLOT(close()));
	connect(m_item_list, SIGNAL(add()), this, SLOT(add()));
	connect(m_item_list, SIGNAL(remove(QAbstractStylesheetItem *)), this, SLOT(remove(QAbstractStylesheetItem *)));
	connect(m_item_list, SIGNAL(changed(QAbstractStylesheetItem *)), this, SLOT(changed(QAbstractStylesheetItem *)));

	QAbstractHost *host = m_property->get_host();

	m_host = QHostFactory::create_host(host->get_attribute(HOST_TYPE));

	QList<QAbstractProperty *> list = host->get_propertys();

	if (host->get_attribute(HOST_TYPE) == "form")
	{
		m_host->setProperty("need_frame", true);
	}
	else
	{
		m_host->setProperty("need_frame", host->property("need_frame"));
	}

	foreach(QAbstractProperty * pro, list)
	{
		if (pro->get_attribute("group") != "Events")
		{
			m_host->set_property_value(pro->get_property("name").toString(), pro->get_value());
		}
	}

	m_show_widget->set_host(m_host);

	tagStylesheetItems items = m_property->get_value().value<tagStylesheetItems>();
	QString name = m_property->get_property("name").toString();
	QAbstractStylesheetItem *maker;

	if (items.size() == 0)
	{
		tagStylesheetItem it;
		it.m_attributes.insert("title", "Normal");
		items.append(it);
	}
	else
	{
		if (items.first().m_attributes.value("title") != "Normal")
		{
			tagStylesheetItem it;
			it.m_attributes.insert("title", "Normal");
			items.insert(0, it);
		}
	}

	foreach(tagStylesheetItem item, items)
	{
		maker = QStylesheetItemFactory::createItem(name);

		if (maker != NULL)
		{
			QVariant v;
			v.setValue<tagStylesheetItem>(item);
			maker->setAttribute("title", item.m_attributes.value("title").toString());
			maker->setValue(v);
			add_item(maker);
		}
	}

	if (m_items.size() > 0)
	{
		m_item_list->set_select(m_items.first());
	}

	item_changed();

	this->resize(100, 100);
}

QStyleSheetDialog::~QStyleSheetDialog()
{
	qDeleteAll(m_items);
	delete m_host;
}

void QStyleSheetDialog::add_item(QAbstractStylesheetItem *item)
{
	m_items.append(item);
	m_item_list->add(item);

	QBaseEditorWidget *wid = create_editor_widget(m_property->get_property("name").toString());

	if (wid != NULL)
	{
		connect(wid, SIGNAL(changed()), this, SLOT(item_changed()));
		wid->init(item);
		wid->set_item(item);
		m_item_to_editor.insert(item, wid);
		m_editor_to_item.insert(wid, item);
		m_stacked_widget->insertWidget(-1, wid);
	}


}

void QStyleSheetDialog::select_changed(QAbstractStylesheetItem *item)
{
	QBaseEditorWidget *wid = m_item_to_editor.value(item);

	if (wid == NULL)
	{
		m_stacked_widget->setCurrentIndex(-1);
		m_show_widget->set_item_sheet("");
	}
	else
	{
		m_stacked_widget->setCurrentWidget(wid);
		m_show_widget->set_item_sheet(item->makeStylesheet());
	}
}

void QStyleSheetDialog::item_changed()
{
	tagStylesheetItems items;
	QMapIterator<QBaseEditorWidget *, QAbstractStylesheetItem *>   it(m_editor_to_item);

	while (it.hasNext())
	{
		it.next();
		QVariant va;
		va.setValue<tagStylesheetItem>(it.key()->get_value());
		it.value()->setValue(va);
		items.append(it.key()->get_value());

		if (it.key() == m_stacked_widget->currentWidget())
		{
			m_show_widget->set_item_sheet(it.value()->makeStylesheet());
		}
	}

	QVariant v;
	v.setValue<tagStylesheetItems>(items);
	m_host->set_property_value(m_property->get_property("name").toString(), v);
	m_host->make_stylesheet();
}

void QStyleSheetDialog::clear()
{
	QBaseEditorWidget *wid = (QBaseEditorWidget *)m_stacked_widget->currentWidget();

	if (wid != NULL)
	{
		QAbstractStylesheetItem *item = m_editor_to_item.value(wid);

		if (item != NULL)
		{
			QString str;
			tagStylesheetItem it = item->value().value<tagStylesheetItem>();
			str = it.m_attributes.value("title").toString();
			it.m_attributes.clear();
			it.m_attributes.insert("title", str);
			QVariant v;
			v.setValue<tagStylesheetItem>(it);
			item->setValue(v);
			wid->set_item(item);
			item_changed();
		}
	}
}

void QStyleSheetDialog::clearall()
{
	QMapIterator<QBaseEditorWidget *, QAbstractStylesheetItem *>   it(m_editor_to_item);

	while (it.hasNext())
	{
		it.next();
		QString str;
		tagStylesheetItem temp = it.value()->value().value<tagStylesheetItem>();
		str = temp.m_attributes.value("title").toString();
		temp.m_attributes.clear();
		temp.m_attributes.insert("title", str);
		QVariant v;
		v.setValue<tagStylesheetItem>(temp);
		it.value()->setValue(v);
		it.key()->set_item(it.value());
	}

	item_changed();
}

void QStyleSheetDialog::ok()
{

	if (m_host->get_object()->property("styleSheet").toString() !=
	        m_property->get_host()->get_object()->property("styleSheet").toString())
	{
		QUndoCommand *cmd = new QUndoCommand;

		QVariant v = m_host->get_property_value(m_property->get_property("name").toString());
		foreach(QBaseEditorWidget * e, m_editor_to_item.keys())
		{
			e->add_resource(cmd);
		}
		new QPropertyChangedUndoCommand(m_property->get_host()->get_uuid(),
		                                m_property->get_property("name").toString(),
		                                m_property->get_value(),
		                                v, cmd);
		foreach(QBaseEditorWidget * e, m_editor_to_item.keys())
		{
			e->take_resource(cmd);
		}

		m_undo_stack->push(cmd);
	}

	close();
}

void QStyleSheetDialog::add()
{
	QAddSheetItemDialog dlg(m_property, "", this);

	dlg.exec();

	QString title = dlg.get_title();

	if (title != "")
	{
		foreach(QAbstractStylesheetItem * item, m_items)
		{
			if (item->attribute("title") == title)
			{
				m_item_list->set_select(item);
				return;
			}
		}

		QAbstractStylesheetItem *maker = QStylesheetItemFactory::createItem(m_property->get_property("name").toString());

		if (maker != NULL)
		{
			tagStylesheetItem it;
			it.m_attributes.insert("title", title);
			QVariant v;
			v.setValue<tagStylesheetItem>(it);
			maker->setAttribute("title", title);
			maker->setValue(v);
			add_item(maker);
			m_item_list->set_select(maker);
		}
	}
}

void QStyleSheetDialog::remove(QAbstractStylesheetItem *item)
{
	m_items.removeAll(item);
	QBaseEditorWidget *wid = m_item_to_editor.value(item);
	m_item_to_editor.remove(item);
	m_editor_to_item.remove(wid);
	m_stacked_widget->removeWidget(wid);
	delete wid;
	item_changed();
}

void QStyleSheetDialog::changed(QAbstractStylesheetItem *item)
{
	QAddSheetItemDialog dlg(m_property, item->attribute("title"), this);

	dlg.exec();

	QString title = dlg.get_title();

	if (title != "")
	{
		if (item->attribute("title") != title)
		{
			tagStylesheetItem it = item->value().value<tagStylesheetItem>();
			it.m_attributes.insert("title", title);
			item->setAttribute("title", title);
			QVariant v;
			v.setValue<tagStylesheetItem>(it);
			item->setValue(v);
			m_item_list->changed_item(item);
			QBaseEditorWidget *wid = m_item_to_editor.value(item);
			wid->set_item(item);
			item_changed();
		}
	}
}
