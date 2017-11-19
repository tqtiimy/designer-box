#include "qformlistwidget.h"


#include "qdesignerformhost.h"

#include "../../../libs/shared/host/qabstracthost.h"
#include "../../../libs/core/qsoftcore.h"
#include "../../../libs/shared/qprojectcore.h"
#include "../../../libs/shared/qpagemanager.h"

#include <QVBoxLayout>
#include <QVariant>
#include <QApplication>
#include <QClipboard>

QFormListWidget::QFormListWidget(QWidget *parent):
	QWidget(parent),
	m_current_form(NULL)
{
	QVBoxLayout *l = new QVBoxLayout;
	l->setMargin(0);
	l->setSpacing(0);
	this->setLayout(l);

	this->setFocusPolicy(Qt::StrongFocus);


	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;
	ac = core->get_action("FormManager.Copy");

	if (ac != NULL)
	{
		ac->setShortcut(QKeySequence::Copy);
		m_form_action_list.append(ac);
	}

	ac = core->get_action("FormManager.Cut");

	if (ac != NULL)
	{
		ac->setShortcut(QKeySequence::Cut);
		m_form_action_list.append(ac);
	}

	ac = core->get_action("FormManager.Paste");

	if (ac != NULL)
	{
		ac->setShortcut(QKeySequence::Paste);
		m_form_action_list.append(ac);
	}

	ac = core->get_action("FormManager.Delete");

	if (ac != NULL)
	{
		m_form_action_list.append(ac);
		ac->setShortcut(QKeySequence::Delete);
	}

	ac = core->get_action("FormManager.Select.All");

	if (ac != NULL)
	{
		m_form_action_list.append(ac);
		ac->setShortcut(QKeySequence::SelectAll);
	}

	QPageManager *manager = QSoftCore::get_core()->get_project_core()->get_page_manager();
	connect(manager, SIGNAL(insert_page_signal(QAbstractHost *)), this, SLOT(insert_page_slot(QAbstractHost *)));
	connect(manager, SIGNAL(remove_page_signal(QAbstractHost *)), this, SLOT(remove_page_slot(QAbstractHost *)));
}

QFormListWidget::~QFormListWidget()
{
	clear();
}

void QFormListWidget::insert_form(QAbstractHost *host, int index)
{
	if (host == NULL || m_host_to_form.keys().contains(host) || !host->get_object()->isWidgetType())
	{
		return;
	}

	if (index < 0 || index > m_forms.size())
	{
		index = m_forms.size();
	}

	QDesignerFormHost *form = new QDesignerFormHost(host, this);
	connect(form, SIGNAL(select(QAbstractHost *)), this, SIGNAL(select(QAbstractHost *)));
	form->set_select_widget(host);
	form->set_undo_stack(m_undo_stack);
	m_forms.insert(index, form);
	m_host_to_form.insert(host, form);

	set_select(host);
	select(host);
}

void QFormListWidget::remove_form(QAbstractHost *host)
{
	if (host == NULL || !m_host_to_form.keys().contains(host))
	{
		return;
	}

	if (m_current_form != NULL && m_current_form->get_root_host() == host)
	{
		if (m_forms.size() < 2)
		{
			m_current_form = NULL;
			select(NULL);
		}
		else
		{
			if (m_forms.first() == m_current_form)
			{
				set_select(m_forms.at(1)->get_root_host());
				select(m_forms.at(1)->get_root_host());
			}
			else
			{
				set_select(m_forms.at(0)->get_root_host());
				select(m_forms.at(0)->get_root_host());
			}
		}
	}

	QDesignerFormHost *form = m_host_to_form.value(host);
	m_host_to_form.remove(host);

	if (form != NULL)
	{
		m_forms.removeAll(form);
		delete form;
	}
}

void QFormListWidget::show_form(QAbstractHost *host)
{

	QDesignerFormHost *form = m_host_to_form.value(host);

	if (m_current_form == form)
	{
		return;
	}

	if (m_current_form != NULL)
	{
		m_current_form->hide();
	}

	m_current_form = form;

	if (m_current_form != NULL)
	{
		m_current_form->show();
	}
}

void QFormListWidget::clear()
{
	qDeleteAll(m_forms);
	m_forms.clear();
	m_host_to_form.clear();
	m_current_form = NULL;
}

void QFormListWidget::property_edit_slot(QAbstractProperty *pro, const QVariant &value)
{
	if (m_current_form != NULL)
	{
		m_current_form->property_edited(pro, value);
	}
}

void QFormListWidget::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QFormListWidget::same_left()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_left();
	}
}

void QFormListWidget::same_top()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_top();
	}
}


void QFormListWidget::same_right()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_right();
	}
}


void QFormListWidget::same_bottom()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_bottom();
	}
}


void QFormListWidget::same_width()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_width();
	}
}


void QFormListWidget::same_height()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_height();
	}
}


void QFormListWidget::same_geometry()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_geometry();
	}
}


void QFormListWidget::same_v_centre()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_v_centre();
	}
}


void QFormListWidget::same_h_centre()
{
	if (m_current_form != NULL)
	{
		m_current_form->same_h_centre();
	}
}

void QFormListWidget::set_select(QAbstractHost *host)
{
	QAbstractHost *par = host;

	while (par->get_parent() != NULL)
	{
		par = par->get_parent();
	}

	if (m_current_form != NULL)
	{
		if (m_current_form->get_root_host() == par)
		{
			m_current_form->set_select_widget(host);
		}
		else
		{
			show_form(par);

			if (m_current_form != NULL)
			{
				m_current_form->set_select_widget(host);
			}
		}
	}
	else
	{
		show_form(par);

		if (m_current_form != NULL)
		{
			m_current_form->set_select_widget(host);
		}
	}
}

void QFormListWidget::insert_page_slot(QAbstractHost *host)
{
	QPageManager *manager = QSoftCore::get_core()->get_project_core()->get_page_manager();

	int index = manager->get_pages().indexOf(host);
	insert_form(host, index);
}

void QFormListWidget::remove_page_slot(QAbstractHost *host)
{
	remove_form(host);
}
