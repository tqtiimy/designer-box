#include "qdatawidget.h"

#include "qdataviewtabwidget.h"
#include "qdatalistwidget.h"
#include "qadddatagroupdialog.h"
#include "qdatagroupremovedialog.h"

#include "../../libs/shared/qprojectcore.h"
#include "../../libs/shared/qdatamanager.h"
#include "../../libs/core/qsoftcore.h"
#include "../../libs/core/qactiontoolbar.h"
#include "../../libs/core/styledbar.h"
#include "../../libs/core/undocommand/qdatagroupaddundocommand.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUuid>

QDataWidget::QDataWidget(QWidget *parent) :
	QAbstractPageWidget(parent),
	m_tab_bar(new QDataViewTabWidget),
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


	ac = core->get_action("Data.Group.Add");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(add_group()));
		ac->setEnabled(false);
	}

	ac = core->get_action("Data.Group.Del");

	if (ac != NULL)
	{
		aclist.append(ac);
		connect(ac, SIGNAL(triggered()), this, SLOT(remove_group()));
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
	connect(m_tab_bar, SIGNAL(current_index_changed(int)), m_widget_stack, SLOT(setCurrentIndex(int)));

	QDataManager *manager = QSoftCore::get_core()->get_project_core()->get_data_manager();
	connect(manager, SIGNAL(insert_group_signal(QData *, int)), this, SLOT(group_add_slot(QData *, int)));
	connect(manager, SIGNAL(remove_group_signal(QData *)), this, SLOT(group_remove_slot(QData *)));
}

void QDataWidget::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QDataWidget::project_opened()
{
	QList<QData *>  datas = QSoftCore::get_core()->get_project_core()->get_data_manager()->get_all_datas();
	foreach(QData * data, datas)
	{
		m_tab_bar->insert_data(data);
		QDataListWidget *wid = new QDataListWidget(data, m_undo_stack);
		m_widget_stack->insertWidget(-1, wid);
		connect(wid, SIGNAL(start_remove()), this, SLOT(start_remove()));
		connect(wid, SIGNAL(end_remove()), this, SLOT(end_remove()));
		connect(wid, SIGNAL(activity()), this, SLOT(group_activity()));
		m_data_to_widget.insert(data, wid);
	}

	if (datas.size() > 0)
	{
		m_tab_bar->set_current_index(0);
	}

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Data.Group.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Data.Group.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void QDataWidget::project_closed()
{
	QMapIterator<QData *, QWidget *>    it(m_data_to_widget);

	while (it.hasNext())
	{
		it.next();
		m_tab_bar->remove(it.key());
		m_widget_stack->removeWidget(it.value());
		delete it.value();
	}

	m_data_to_widget.clear();
	m_tab_bar->set_stop(false);

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Data.Group.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Data.Group.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	m_undo_stack->clear();
}

void QDataWidget::start_remove()
{
	m_tab_bar->set_stop(true);

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Data.Group.Add");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}

	ac = core->get_action("Data.Group.Del");

	if (ac != NULL)
	{
		ac->setEnabled(false);
	}
}

void QDataWidget::end_remove()
{
	m_tab_bar->set_stop(false);

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Data.Group.Add");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}

	ac = core->get_action("Data.Group.Del");

	if (ac != NULL)
	{
		ac->setEnabled(true);
	}
}

void QDataWidget::add_group()
{
	QAddDataGroupDialog dlg(QSoftCore::get_core()->get_project_core()->get_data_manager()->get_all_group_name(), this);
	dlg.exec();

	QString name = dlg.get_name();

	if (name != "")
	{
		QData data;
		data.set_uuid(QUuid::createUuid().toString());
		data.set_name(name);
		QDataGroupAddUndoCommand *cmd = new QDataGroupAddUndoCommand(data,
		        QSoftCore::get_core()
		        ->get_project_core()
		        ->get_data_manager()
		        ->get_all_datas().size(),
		        DGAT_ADD);
		m_undo_stack->push(cmd);
	}
}

void QDataWidget::remove_group()
{
	QDataGroupRemoveDialog dlg(this);

	dlg.exec();

	QStringList res = dlg.get_result();

	QDataManager *manager = QSoftCore::get_core()->get_project_core()->get_data_manager();

	if (res.size() > 0)
	{
		QUndoCommand *cmd = new QUndoCommand;

		for (int i = res.size() - 1; i >= 0; i--)
		{
			QData *data = manager->get_data_group(res.at(i));

			if (data != NULL)
			{
				new QDataGroupAddUndoCommand(*data, manager->get_all_group_name().indexOf(data->get_name()),
				                             DGAT_REMOVE, cmd);
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

void QDataWidget::group_add_slot(QData *data, int index)
{
	m_tab_bar->insert_data(data, index);
	QDataListWidget *wid = new QDataListWidget(data, m_undo_stack);
	m_widget_stack->insertWidget(index, wid);
	connect(wid, SIGNAL(start_remove()), this, SLOT(start_remove()));
	connect(wid, SIGNAL(end_remove()), this, SLOT(end_remove()));
	connect(wid, SIGNAL(activity()), this, SLOT(group_activity()));
	m_data_to_widget.insert(data, wid);
	m_tab_bar->set_current_index(index);
}

void QDataWidget::group_remove_slot(QData *data)
{
	QWidget *wid = m_data_to_widget.value(data);

	m_tab_bar->remove(data);
	m_widget_stack->removeWidget(wid);

	m_widget_stack->setCurrentIndex(m_tab_bar->current_index());
	m_data_to_widget.remove(data);
	delete wid;

}

void QDataWidget::group_activity()
{
	QWidget *wid = (QWidget *)sender();

	int index = m_widget_stack->indexOf(wid);

	if (index >= 0)
	{
		m_tab_bar->set_current_index(index);
	}
}
