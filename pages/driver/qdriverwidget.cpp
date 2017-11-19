#include "qdriverwidget.h"

#include "qdrivertabview.h"
#include "qdriverview.h"
#include "qdriverremovedialog.h"
#include "qdriverdataadddialog.h"
#include "qadddriverdialog.h"

#include "../../libs/core/qactiontoolbar.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qdrivermanager.h"
#include "../../libs/shared/driver/qabstractdriver.h"
#include "../../libs/core/undocommand/qdriveraddundocommand.h"
#include "../../libs/shared/qdriverfactory.h"

QDriverWidget::QDriverWidget(QWidget *parent):
	QAbstractPageWidget(parent),
	m_tab_bar(new QDriverTabView),
	m_widget_stack(new QStackedLayout)
{
	m_widget_stack->setMargin(0);
	m_widget_stack->setSpacing(0);


	QActionToolBar *toolbar = new QActionToolBar;

	QList<QAction *> aclist;
	QAction *ac;
	QSoftCore *core = QSoftCore::get_core();

	ac = new QAction(this);
	ac->setSeparator(true);
	aclist.append(ac);


	ac = core->get_action("Driver.Add");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(add_driver()));
		ac->setEnabled(false);
	}

	ac = core->get_action("Driver.Del");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_driver()));
		ac->setEnabled(false);
	}

	ac = new QAction(this);
	ac->setSeparator(true);
	aclist.append(ac);

	toolbar->addButtonActions(aclist);

	StyledBar *stylebar = new StyledBar;
	QHBoxLayout *l = new QHBoxLayout;
	l->setMargin(0);
	l->setSpacing(0);
	l->addWidget(toolbar);
	stylebar->setLayout(l);

	QHBoxLayout *h = new QHBoxLayout;
	h->setMargin(0);
	h->setSpacing(0);
	h->insertWidget(0, m_tab_bar, 1);
	h->insertWidget(1, stylebar, 0);

	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);

	v->addLayout(h);
	v->addLayout(m_widget_stack, 1);

	this->setLayout(v);

	connect(QSoftCore::get_core()->get_project_core(), SIGNAL(opened_signals()), this, SLOT(project_opened()));
	connect(QSoftCore::get_core()->get_project_core(), SIGNAL(closed_signals()), this, SLOT(project_closed()));

	connect(QSoftCore::get_core()->get_project_core()->get_driver_manager(),
	        SIGNAL(insert_driver_signal(QAbstractDriver *, int)), this, SLOT(insert_driver_slot(QAbstractDriver *, int)));
	connect(QSoftCore::get_core()->get_project_core()->get_driver_manager(),
	        SIGNAL(remove_driver_signal(QAbstractDriver *)), this, SLOT(remove_driver_slot(QAbstractDriver *)));

	connect(m_tab_bar, SIGNAL(current_index_changed(int)), m_widget_stack, SLOT(setCurrentIndex(int)));
}

void QDriverWidget::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QDriverWidget::project_opened()
{
	QList<QAbstractDriver *>  drivers = QSoftCore::get_core()->get_project_core()
	                                    ->get_driver_manager()->get_drivers();
	foreach(QAbstractDriver * driver, drivers)
	{
		m_tab_bar->insert_data(driver);
		QDriverView *view = new QDriverView(driver);
		view->set_undo_stack(m_undo_stack);
		connect(view, SIGNAL(active()), this, SLOT(widget_active()));
		connect(view, SIGNAL(start_remove()), this, SLOT(start_remove()));
		connect(view, SIGNAL(end_remove()), this, SLOT(end_remove()));
		m_driver_to_widget.insert(driver, view);
		m_widget_stack->insertWidget(-1, view);
	}

	if (drivers.size() > 0)
	{
		m_tab_bar->set_current_index(0);
	}

	QAction *ac;
	QSoftCore *core = QSoftCore::get_core();

	ac = core->get_action("Driver.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Driver.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void QDriverWidget::project_closed()
{
	QMapIterator<QAbstractDriver *, QDriverView *> it(m_driver_to_widget);

	while (it.hasNext())
	{
		it.next();
		m_tab_bar->remove(it.key());
		m_widget_stack->removeWidget(it.value());
		delete it.value();
	}

	m_driver_to_widget.clear();

	QAction *ac;
	QSoftCore *core = QSoftCore::get_core();

	ac = core->get_action("Driver.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Driver.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	m_undo_stack->clear();
}

void QDriverWidget::add_driver()
{
	QDriverManager *manager = QSoftCore::get_core()->get_project_core()->get_driver_manager();

	QStringList list;
	foreach(QAbstractDriver * driver, manager->get_drivers())
	{
		list.append(driver->get_name());
	}

	QAddDriverDialog dlg(list, this);
	dlg.exec();

	QString name = dlg.get_name();

	if (name != "")
	{
		QString type = dlg.get_type();
		QAbstractDriver *driver = QDriverFactory::create_driver(type);

		if (driver != NULL)
		{
			QDriverManager *manager = QSoftCore::get_core()->get_project_core()->get_driver_manager();

			driver->set_name(name);
			driver->set_default();
			QDriverAddUndoCommand *cmd = new QDriverAddUndoCommand(driver, manager->get_drivers().size(),
			        DRAT_ADD);
			m_undo_stack->push(cmd);
		}
	}
}

void QDriverWidget::remove_driver()
{
	QDriverRemoveDialog dlg(this);

	dlg.exec();

	QStringList list = dlg.get_result();

	if (list.size() > 0)
	{
		QDriverManager *manager = QSoftCore::get_core()->get_project_core()->get_driver_manager();

		QUndoCommand *cmd = new QUndoCommand;

		QList<QAbstractDriver *> l = manager->get_drivers();

		for (int i = list.size() - 1; i >= 0; i--)
		{
			QAbstractDriver *driver = manager->get_driver(list.at(i));

			if (driver != NULL)
			{
				new QDriverAddUndoCommand(driver, l.indexOf(driver), DRAT_REMOVE, cmd);
			}
		}

		if (cmd->childCount() > 0)
		{
			m_undo_stack->push(cmd);
		}
		else
		{
			delete cmd;
		}
	}
}

void QDriverWidget::insert_driver_slot(QAbstractDriver *driver, int index)
{
	m_tab_bar->insert_data(driver, index);
	QDriverView *view = new QDriverView(driver);
	view->set_undo_stack(m_undo_stack);
	connect(view, SIGNAL(active()), this, SLOT(widget_active()));
	connect(view, SIGNAL(start_remove()), this, SLOT(start_remove()));
	connect(view, SIGNAL(end_remove()), this, SLOT(end_remove()));
	m_driver_to_widget.insert(driver, view);
	m_widget_stack->insertWidget(index, view);
	m_tab_bar->set_current_index(index);
}

void QDriverWidget::remove_driver_slot(QAbstractDriver *driver)
{
	m_tab_bar->remove(driver);
	QDriverView *wid = m_driver_to_widget.value(driver);
	m_widget_stack->removeWidget(wid);
	delete wid;
}

void QDriverWidget::widget_active()
{
	QDriverView *wid = (QDriverView *)sender();
	m_tab_bar->set_current_index(m_widget_stack->indexOf(wid));
}

void QDriverWidget::start_remove()
{
	m_tab_bar->set_stop(true);
	QAction *ac;
	QSoftCore *core = QSoftCore::get_core();

	ac = core->get_action("Driver.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Driver.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}
}

void QDriverWidget::end_remove()
{
	m_tab_bar->set_stop(false);

	QAction *ac;
	QSoftCore *core = QSoftCore::get_core();

	ac = core->get_action("Driver.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Driver.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}
