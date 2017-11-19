#include "qdesignerformhost.h"

#include "widgethost.h"
#include "qselectwidget.h"
#include "../qdesignermimedata.h"

#include "../../../libs/shared/host/qabstracthost.h"
#include "../../../libs/shared/qhostfactory.h"
#include "../../../libs/shared/property/qabstractproperty.h"
#include "../../../libs/core/undocommand/qpropertychangedundocommand.h"
#include "../../../libs/core/undocommand/qhostparentchangedundocommand.h"
#include "../../../libs/core/undocommand/qaddhostundocommand.h"
#include "../../../libs/shared/qprojectcore.h"
#include "../../../libs/core/qsoftcore.h"

#include <QLayout>
#include <QPainter>
#include <QStringList>
#include <QUuid>

QDesignerFormHost::QDesignerFormHost(QAbstractHost *host, QWidget *parent):
	QObject(parent),
	m_root_host(host),
	m_widget_host(new WidgetHost()),
	m_rubber(new QRubberBand(QRubberBand::Rectangle, (QWidget *)host->get_object())),
	m_selection(new Selection((QWidget *)host->get_object())),
	m_click(false)
{
	connect(m_root_host, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
	        this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
	connect(m_root_host, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
	        this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
	m_widget_to_host.insert(m_root_host->get_object(), m_root_host);
	install_all_event(m_root_host);

	QList<QAbstractHost *> children = m_root_host->get_children();

	while (children.size() > 0)
	{
		QAbstractHost *h = children.takeFirst();
		connect(h, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
		        this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
		connect(h, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
		        this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
		m_widget_to_host.insert(h->get_object(), h);
		children += h->get_children();

		if (h->property("accept_drop").toBool())
		{
			((QWidget *)h->get_object())->setAcceptDrops(true);
		}
	}

	m_widget_host->setFormWidget((QWidget *)m_root_host->get_object());

	QLayout *l = parent->layout();

	l->addWidget(m_widget_host);
	m_widget_host->setVisible(false);
	((QWidget *)m_root_host->get_object())->setVisible(true);
	((QWidget *)m_root_host->get_object())->setAcceptDrops(true);
	connect(host, SIGNAL(destroyed()), this, SLOT(host_destory()));
	connect(m_widget_host, SIGNAL(size_changed(QRect, QRect)), this, SLOT(form_size_changed(QRect, QRect)));
	connect(m_selection, SIGNAL(sizeChanged(QWidget *, QRect, QRect)), this, SLOT(widget_size_changed(QWidget *, QRect, QRect)));
}

QDesignerFormHost::~QDesignerFormHost()
{
	((QWidget *)m_root_host->get_object())->setParent(NULL);
	delete m_widget_host;
	m_widget_host = NULL;

	if (m_rubber != NULL)
	{
		delete m_rubber;
	}

	delete m_selection;
}

bool QDesignerFormHost::eventFilter(QObject *o, QEvent *e)
{
	QAbstractHost *h = host_from_object(o);

	if (h != NULL)
	{
		switch (e->type())
		{
		case QEvent::Paint:
			return handle_paint_event(h, (QPaintEvent *)e);

		case QEvent::MouseButtonPress:
			return handle_mouse_press_event(h, (QMouseEvent *)e);

		case QEvent::MouseButtonRelease:
			return handle_mouse_release_event(h, (QMouseEvent *)e);

		case QEvent::MouseMove:
			return handle_mouse_move_event(h, (QMouseEvent *)e);

		case QEvent::MouseButtonDblClick:
			return handle_mouse_db_clicked_event(h, (QMouseEvent *)e);

		case QEvent::DragEnter:
			return handle_drag_enter_event(h, (QDragEnterEvent *)e);

		case QEvent::DragLeave:
			return handle_drag_leave_event(h, (QDragLeaveEvent *)e);

		case QEvent::Drop:
			return handle_drop_event(h, (QDropEvent *)e);

		case QEvent::Resize:
		case QEvent::ParentChange:
		case QEvent::Move:
			if (h != m_root_host)
			{
				m_selection->updateGeometry((QWidget *)h->get_object());
			}
			else
			{
				m_widget_host->updateFormGeometry();
			}

			break;

		default:
			break;
		}
	}

	return false;
}

void QDesignerFormHost::show()
{
	m_widget_host->show();
	select_widget(NULL);
}

void QDesignerFormHost::hide()
{
	m_widget_host->hide();
}

QAbstractHost *QDesignerFormHost::host_from_object(QObject *obj)
{
	QAbstractHost *h;

	while (obj != NULL)
	{
		h = m_widget_to_host.value(obj);

		if (h != NULL)
		{
			return h;
		}

		obj = obj->parent();
	}

	return NULL;
}

bool QDesignerFormHost::handle_paint_event(QAbstractHost *host, QPaintEvent *e)
{
	QWidget *wid = (QWidget *)host->get_object();

	if (host == m_root_host)
	{
		QPainter p(wid);
		p.setBrush(Qt::transparent);

		if (!host->property("grab").toBool())
		{
			p.setPen(QPen(QColor(128, 128, 128)));


			QRect re = e->rect();

			int x = re.x() / 10;
			int y = re.y() / 10;
			int w = re.width() / 10 + 2;
			int h = re.height() / 10 + 2;

			for (int i = 0; i < w; i++)
			{
				for (int j = 0; j < h; j++)
				{
					p.drawPoint((x + i) * 10, (y + j) * 10);
				}
			}
		}
		else
		{
			p.fillRect(wid->rect(), QColor(128, 128, 128, 80));
		}

		p.setPen(QColor(80, 80, 80));
		p.drawRect(0, 0, wid->width() - 1, wid->height() - 1);
	}

	if (host->property("need_frame").toBool())
	{
		QPainter p(wid);
		p.setBrush(Qt::transparent);
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(128, 128, 128));
		pen.setWidth(1);
		p.setPen(pen);
		p.drawRect(wid->rect().adjusted(0, 0, -1, -1));
	}

	if (host->property("drag_enter").toBool())
	{
		QPainter p(wid);
		p.setBrush(Qt::transparent);
		p.fillRect(wid->rect(), QColor(128, 128, 128, 80));
	}

	return false;
}

bool QDesignerFormHost::handle_mouse_press_event(QAbstractHost *host, QMouseEvent *e)
{
	if (m_root_host == host)
	{
		m_clickPoint = e->pos();
		m_rubber->setGeometry(QRect(m_clickPoint, QSize()));
		m_rubber->show();
		m_selection->clear();
	}
	else
	{
		m_move_point = e->pos();
		QWidget *wid = (QWidget *)host->get_object();

		if (m_selection->selectedWidgets().size() != 0)
		{
			if (m_selection->isWidgetSelected(wid))
			{
				if ((e->modifiers() & Qt::ControlModifier) != 0)
				{
					if (m_selection->current() == wid)
					{
						QWidget *w = m_selection->removeWidget(wid);
						select_widget(w);
						return true;
					}
					else
					{
						select_widget(wid);
					}
				}
			}
			else
			{
				if ((e->modifiers() & Qt::ControlModifier) == 0)
				{
					m_selection->clear();
					select_widget(wid);
				}
				else
				{
					QAbstractHost *curr = m_widget_to_host.value(m_selection->current());
					QAbstractHost *s = m_widget_to_host.value(wid);
					QAbstractHost *temp;

					while (s->get_parent() != NULL)
					{
						temp = s->get_parent();

						if (temp == curr)
						{
							break;
						}

						if (temp == curr->get_parent())
						{
							select_widget((QWidget *)s->get_object());
							break;
						}

						s = temp;
					}
				}
			}

		}
		else
		{
			select_widget(wid);
		}

		m_click = true;
	}

	return true;
}

bool QDesignerFormHost::handle_mouse_release_event(QAbstractHost *host, QMouseEvent *e)
{
	if (!m_clickPoint.isNull())
	{
		m_clickPoint = QPoint();
		QRect re = m_rubber->geometry();
		m_rubber->setGeometry(QRect(0, 0, 0, 0));
		m_rubber->hide();

		foreach(QObject * obj, m_widget_to_host.keys())
		{
			if (obj == m_root_host->get_object())
			{
				continue;
			}

			QAbstractHost *h = m_widget_to_host.value(obj);

			if (h->get_parent() != m_root_host)
			{
				continue;
			}

			QWidget *wid = (QWidget *)obj;

			if (!(re & wid->geometry()).isNull())
			{
				m_selection->addWidget(wid);
			}
		}

		if (m_selection->selectedWidgets().size() > 0)
		{
			select_widget(m_selection->selectedWidgets().first());
		}
		else
		{
			select_widget(NULL);
		}

	}
	else
	{
		m_click = false;
	}

	return true;
}

bool QDesignerFormHost::handle_mouse_move_event(QAbstractHost *host, QMouseEvent *e)
{
	if (!m_clickPoint.isNull())
	{
		QRect re(QRect(m_clickPoint, e->pos()).normalized());
		m_rubber->setGeometry(re);
	}
	else
	{
		if (m_click)
		{
			if ((e->pos() - m_clickPoint).manhattanLength() < 10)
			{
				return true;
			}

			m_click = false;
			QWidgetList list = m_selection->selectedWidgets();

			if (list.size() > 0)
			{
				QList<QDesignerDnDItemInterface *>   items;

				foreach(QWidget * wid, list)
				{
					items.append(new FormDnItem(m_widget_to_host.value(wid), e->globalPos()));
					wid->hide();
					m_selection->hide(wid);
				}
				QDesignerMimeData::execDrag(items, (QWidget *)m_root_host->get_object());
				foreach(QWidget * wid, list)
				{
					wid->show();
					m_selection->show(wid);
				}
			}
		}
	}

	return true;
}

bool QDesignerFormHost::handle_mouse_db_clicked_event(QAbstractHost *host, QMouseEvent *e)
{
	if (host != m_root_host)
	{
	}

	return true;
}

bool QDesignerFormHost::handle_drag_enter_event(QAbstractHost *host, QDragEnterEvent *e)
{
	const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(e->mimeData());

	if (mimeData != NULL)
	{
		mimeData->acceptEvent(e);
	}

	if (host != m_root_host)
	{
		host->setProperty("drag_enter", true);
		((QWidget *)host->get_object())->update();
	}

	return true;
}

bool QDesignerFormHost::handle_drag_leave_event(QAbstractHost *host, QDragLeaveEvent *e)
{
	if (host != m_root_host)
	{
		host->setProperty("drag_enter", false);
		((QWidget *)host->get_object())->update();
	}

	return true;
}

bool QDesignerFormHost::handle_drop_event(QAbstractHost *host, QDropEvent *e)
{
	const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(e->mimeData());

	if (mimeData == NULL)
	{
		return true;
	}

	mimeData->acceptEvent(e);

	QList<QDesignerDnDItemInterface *>   list = mimeData->items();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QDesignerDnDItemInterface * item, list)
	{
		if (item->type() == QDesignerDnDItemInterface::CopyDrop)
		{
			QAbstractHost *h = QHostFactory::create_host(item->name());

			if (h != NULL)
			{
				h->set_uuid(QUuid::createUuid().toString());
				QRect re = h->get_property_value("geometry").toRect();
				re.moveTo(e->pos() - item->hotSpot());
				h->set_property_value("geometry", re);
				h->set_default();
				QList<QAbstractHost *> list;
				QList<int>          index;
				list.append(h);
				index.append(host->get_child_count());
				new QAddHostUndoCommand(host, list, index, AHT_ADD, cmd);
			}
		}
		else
		{
			QAbstractHost *h = item->host();
			QRect re = h->get_property_value("geometry").toRect();
			QRect new_re = re;
			new_re.moveTo(e->pos() - item->hotSpot());
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", re, new_re, cmd);

			if (h->get_parent() != host)
			{
				new QHostParentChangedUndoCommand(h, h->get_parent(),
				                                  h->get_parent()->get_children().indexOf(h),
				                                  host,
				                                  host->get_child_count(),
				                                  cmd);
			}
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

	if (host != m_root_host)
	{
		host->setProperty("drag_enter", false);
		((QWidget *)host->get_object())->update();
	}

	return true;
}

void QDesignerFormHost::host_destory()
{
	m_rubber = NULL;
}

void QDesignerFormHost::select_widget(QWidget *wid)
{
	if (wid == NULL)
	{
		m_selection->clear();
		m_widget_host->showHandle();
		emit select(m_root_host);
	}
	else
	{
		m_selection->addWidget(wid);
		m_selection->setCurrent(wid);
		m_widget_host->hideHandle();
		emit select(m_widget_to_host.value(wid));
	}
}

QAbstractHost *QDesignerFormHost::get_select_widget()
{
	QWidget *wid = m_selection->current();

	if (wid == NULL)
	{
		return m_root_host;
	}
	else
	{
		return m_widget_to_host.value(wid);
	}
}

QAbstractHost *QDesignerFormHost::get_root_host()
{
	return m_root_host;
}

void QDesignerFormHost::set_select_widget(QAbstractHost *host)
{
	select_widget(NULL);

	if (m_root_host != host)
	{
		select_widget((QWidget *)host->get_object());
	}
}

void QDesignerFormHost::property_edited(QAbstractProperty *pro, const QVariant &value)
{
	if (pro == NULL)
	{
		return;
	}

	QAbstractHost *h = m_root_host;

	if (m_selection->selectedWidgets().size() > 0)
	{
		h = m_widget_to_host.value(m_selection->current());
	}

	QString name = pro->get_property("name").toString();
	QAbstractProperty *ppro = pro;

	while (ppro->get_parent() != NULL)
	{
		ppro = ppro->get_parent();
		name = ppro->get_property("name").toString() + "." + name;
	}

	if (ppro->get_attribute(ATTR_CAN_SAME).toBool())
	{
		QList<QAbstractHost *> list;
		list.append(h);
		foreach(QWidget * wid, m_selection->selectedWidgets())
		{
			QAbstractHost *temp = m_widget_to_host.value(wid);
			list.removeAll(temp);
			QAbstractProperty *p1;
			p1 = temp->get_property(name);

			if (p1 != NULL && p1->get_property("type").toString() == pro->get_property("type").toString())
			{
				list.append(temp);
			}
		}

		if (list.size() > 0)
		{
			QPropertyChangedUndoCommand *p = new QPropertyChangedUndoCommand("", name, QVariant(), QVariant());
			foreach(QAbstractHost * host, list)
			{
				new QPropertyChangedUndoCommand(host->get_uuid(), name, host->get_property_value(name), value, p);

			}
			m_undo_stack->push(p);
		}
		else
		{
			QPropertyChangedUndoCommand *p = new QPropertyChangedUndoCommand(h->get_uuid(), name, h->get_property_value(name), value);

			m_undo_stack->push(p);
		}
	}
	else
	{
		QPropertyChangedUndoCommand *p = new QPropertyChangedUndoCommand(h->get_uuid(), name, h->get_property_value(name), value);

		m_undo_stack->push(p);
	}
}

void QDesignerFormHost::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}

void QDesignerFormHost::same_left()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(base_rect.left(), re.top());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_top()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(re.left(), base_rect.top());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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
void QDesignerFormHost::same_right()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(base_rect.left() + base_rect.width() - re.width(), re.top());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_bottom()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(re.left(), base_rect.top() + base_rect.height() - re.height());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_width()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.setWidth(base_rect.width());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_height()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.setHeight(base_rect.height());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_geometry()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.setWidth(base_rect.width());
		re.setHeight(base_rect.height());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_v_centre()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(base_rect.left() + (base_rect.width() - re.width()) / 2, re.top());

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::same_h_centre()
{
	if (m_selection->selectedWidgets().size() <= 1)
	{
		return;
	}

	QRect base_rect = m_selection->current()->geometry();

	QUndoCommand *cmd = new QUndoCommand;

	foreach(QWidget * wid, m_selection->selectedWidgets())
	{
		QAbstractHost *h = m_widget_to_host.value(wid);
		QRect re = wid->geometry();
		re.moveTo(re.left(), base_rect.top() + (base_rect.height() - re.height()) / 2);

		if (re != wid->geometry())
		{
			new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", wid->geometry(), re, cmd);
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

void QDesignerFormHost::form_size_changed(const QRect &old, const QRect &now)
{
	if (old != now)
	{
		QPropertyChangedUndoCommand *cmd = new QPropertyChangedUndoCommand(m_root_host->get_uuid(), "geometry", old, now);
		m_undo_stack->push(cmd);

	}
}

void QDesignerFormHost::widget_size_changed(QWidget *wid, const QRect &old, const QRect &now)
{
	if (old != now)
	{
		QAbstractHost *h = m_widget_to_host.value(wid);

		if (h != NULL)
		{
			QPropertyChangedUndoCommand *cmd = new QPropertyChangedUndoCommand(h->get_uuid(), "geometry", old, now);
			m_undo_stack->push(cmd);
		}

	}
}

void QDesignerFormHost::insert_host_slot(QList<QAbstractHost *> hosts, QList<int>)
{
	m_selection->clear();

	int count = hosts.size();
	QList<QAbstractHost *> c;

	for (int i = 0; i < count; i++)
	{
		QAbstractHost *h = hosts.at(i);
		install_all_event(h);
		connect(h, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
		        this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
		connect(h, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
		        this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
		m_widget_to_host.insert(h->get_object(), h);

		if (h->property("accept_drop").toBool())
		{
			((QWidget *)h->get_object())->setAcceptDrops(true);
		}

		c = h->get_children();

		while (c.size() > 0)
		{
			QAbstractHost *hh = c.takeFirst();
			connect(hh, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
			        this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
			connect(hh, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
			        this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
			m_widget_to_host.insert(hh->get_object(), hh);
			c += hh->get_children();
		}

		QWidget *wid = (QWidget *)h->get_object();
		m_selection->addWidget(wid);
		wid->show();
	}

	select_widget(m_selection->selectedWidgets().first());
}

void QDesignerFormHost::remove_host_slot(QList<QAbstractHost *> hosts)
{
	select_widget(NULL);
	foreach(QAbstractHost * h, hosts)
	{
		QList<QAbstractHost *> children = h->get_children();
		remove_all_event(h);
		disconnect(h, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
		           this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
		disconnect(h, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
		           this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
		m_widget_to_host.remove(h->get_object());

		while (children.size() > 0)
		{
			QAbstractHost *hh = children.takeFirst();
			disconnect(hh, SIGNAL(insert_children_signal(QList<QAbstractHost *>, QList<int>)),
			           this, SLOT(insert_host_slot(QList<QAbstractHost *>, QList<int>)));
			disconnect(hh, SIGNAL(remove_children_signal(QList<QAbstractHost *>)),
			           this, SLOT(remove_host_slot(QList<QAbstractHost *>)));
			m_widget_to_host.remove(hh->get_object());
			children += hh->get_children();
		}
	}
}

void QDesignerFormHost::install_all_event(QAbstractHost *host)
{
	QObjectList list;
	list.append(host->get_object());

	while (list.size() > 0)
	{
		QObject *obj = list.takeFirst();

		if (obj == m_rubber)
		{
			continue;
		}

		obj->installEventFilter(this);
		list += obj->children();
	}
}

void QDesignerFormHost::remove_all_event(QAbstractHost *host)
{
	QObjectList list;
	list.append(host->get_object());

	while (list.size() > 0)
	{
		QObject *obj = list.takeFirst();

		if (obj == m_rubber)
		{
			continue;
		}

		obj->removeEventFilter(this);
		list += obj->children();
	}
}
