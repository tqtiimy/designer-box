#include "qresourcefilelist.h"

#include "qaddfiledialog.h"
#include "qaddfilegroupdialog.h"

#include "../../../libs/core/qfilelistview.h"
#include "../../../libs/core/styledbar.h"
#include "../../../libs/core/qfilemanager.h"
#include "../../../libs/core/qsoftcore.h"
#include "../../../libs/core/undocommand/qfilegroupaddundocommand.h"
#include "../../../libs/core/undocommand/qfileaddundocommand.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QUuid>

QResourceFileList::QResourceFileList(QWidget *parent) :
	QWidget(parent),
	m_file_list_view(new QFileListView),
	m_styled_bar(new StyledBar)
{
	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);

	v->addWidget(m_styled_bar);
	v->addWidget(m_file_list_view);

	this->setLayout(v);

	QLabel *l = new QLabel;

	l->setText(tr("Resource List"));
	l->setStyleSheet("Color:rgb(255,255,255);");

	v = new QVBoxLayout;
	v->setMargin(4);
	v->addWidget(l);
	m_styled_bar->setLayout(v);

	m_file_list_view->installEventFilter(this);

	connect(m_file_list_view, SIGNAL(remove_group_signal(tagFileGroupInfo *)), this, SLOT(remove_group(tagFileGroupInfo *)));
	connect(m_file_list_view, SIGNAL(remove_file_signal(tagFileInfo *)), this, SLOT(remove_file(tagFileInfo *)));
	connect(m_file_list_view, SIGNAL(select_file_signal(tagFileInfo *)), this, SIGNAL(select(tagFileInfo *)));
}

bool QResourceFileList::eventFilter(QObject *o, QEvent *e)
{
	if (o == m_file_list_view && e->type() == QEvent::ContextMenu)
	{
		QContextMenuEvent *event = (QContextMenuEvent *)e;
		QMenu menu;

		menu.addAction(tr("Add Group"), this, SLOT(add_group()));
		menu.addAction(tr("Add File"), this, SLOT(add_file()));

		menu.exec(event->globalPos());
		return true;
	}

	return QWidget::eventFilter(o, e);
}

void QResourceFileList::add_group()
{
	QStringList list;

	QList<tagFileGroupInfo *> groups = QSoftCore::get_core()->get_file_manager()->get_all_group();

	foreach(tagFileGroupInfo * group, groups)
	{
		list.append(group->m_group_name);
	}

	QAddFileGroupDialog dlg(list, this);
	dlg.exec();
	QString name = dlg.get_name();

	if (name != NULL)
	{
		tagFileGroupInfo group;
		group.m_group_name = name;
		group.m_uuid = QUuid::createUuid().toString();

		QFileGroupAddUndoCommand *cmd = new QFileGroupAddUndoCommand(group, list.size(), FGAT_ADD);
		m_undo_stack->push(cmd);
	}
}

void QResourceFileList::remove_group(tagFileGroupInfo *group)
{
	int index = QSoftCore::get_core()->get_file_manager()->get_all_group().indexOf(group);
	QFileGroupAddUndoCommand *cmd = new QFileGroupAddUndoCommand(*group, index, FGAT_REMOVE);
	m_undo_stack->push(cmd);
}

void QResourceFileList::add_file()
{
	QAddFileDialog dlg(this);
	dlg.exec();

	QString name = dlg.get_name();

	if (name != "")
	{
		QString group = dlg.get_group();
		QByteArray data = dlg.get_data();

		QList<tagFileGroupInfo *> groups = QSoftCore::get_core()->get_file_manager()->get_all_group();
		foreach(tagFileGroupInfo * g, groups)
		{
			if (g->m_group_name == group)
			{
				tagFileInfo info;
				info.m_file_data = data;
				info.m_file_name = name;
				info.m_group_uuid = g->m_uuid;
				info.m_exp = dlg.get_exp();
				info.m_type = FILE_USER;
				info.m_uuid = QUuid::createUuid().toString();
				QFileAddUndoCommand *cmd = new QFileAddUndoCommand(info, g->m_files.size(), FAT_ADD);
				m_undo_stack->push(cmd);
				break;
			}
		}
	}
}

void QResourceFileList::remove_file(tagFileInfo *file)
{
	tagFileGroupInfo *g = QSoftCore::get_core()->get_file_manager()->get_group(file->m_group_uuid);
	QFileAddUndoCommand *cmd = new QFileAddUndoCommand(*file, g->m_files.indexOf(file), FAT_REMOVE);
	m_undo_stack->push(cmd);
}

void QResourceFileList::set_undo_stack(QUndoStack *stack)
{
	m_undo_stack = stack;
}
