#include "qimageeditor.h"

#include "qimageview.h"

#include "../../../libs/core/qactiontoolbar.h"
#include "../../../libs/core/qfilemanager.h"

#include <QVBoxLayout>

QImageEditor::QImageEditor(QWidget *parent) :
	QAbstractResourceEditor(parent),
	m_tool_bar(new QActionToolBar(Qt::AlignLeft, this)),
	m_image_view(new QImageView)
{
	QAction *ac;

	ac = new QAction(this);
	ac->setSeparator(true);
	m_actions.append(ac);
	ac = new QAction(QIcon(":/images/fitinscreen.png"), tr("Fit image int the screen"), NULL);
	connect(ac, SIGNAL(triggered()), m_image_view, SLOT(fitToScreen()));
	m_actions.append(ac);
	ac = new QAction(QIcon(":/images/originalsize.png"), tr("Original size"), NULL);
	connect(ac, SIGNAL(triggered()), m_image_view, SLOT(resetToOriginalSize()));
	m_actions.append(ac);
	ac = new QAction(QIcon(":/images/zoomin.png"), tr("Zoomin"), NULL);
	connect(ac, SIGNAL(triggered()), m_image_view, SLOT(zoomIn()));
	m_actions.append(ac);
	ac = new QAction(QIcon(":/images/zoomout.png"), tr("Zoomout"), NULL);
	connect(ac, SIGNAL(triggered()), m_image_view, SLOT(zoomOut()));
	m_actions.append(ac);
	ac = new QAction(QIcon(":/images/outline.png"), tr("Out Line"), NULL);
	connect(ac, SIGNAL(triggered()), this, SLOT(outline()));
	ac->setCheckable(true);
	ac->setChecked(true);
	m_actions.append(ac);

	m_tool_bar->addButtonActions(m_actions);

	QVBoxLayout *v = new QVBoxLayout;
	v->setMargin(0);
	v->setSpacing(0);
	v->addWidget(m_image_view);
	this->setLayout(v);

	set_file(NULL);
}

QString QImageEditor::show_name()
{
	return tr("Image View");
}

QWidget *QImageEditor::tool_bar()
{
	return m_tool_bar;
}

bool QImageEditor::support(const QString &type)
{
	QStringList list;
	list << "png" << "jpg" << "bmp";
	return list.indexOf(type) >= 0;
}

void QImageEditor::set_file(tagFileInfo *info)
{
	m_image_view->resetToOriginalSize();

	if (info != NULL)
	{
		m_image_view->setImage(info->m_file_data);
	}
	else
	{
		m_image_view->setImage(QByteArray());
	}

	foreach(QAction * ac, m_actions)
	{
		ac->setEnabled(info != NULL);
	}
}

void QImageEditor::outline()
{
	QAction *ac = m_actions.last();
	//ac->setChecked(!ac->isChecked());
	m_image_view->setViewOutline(ac->isChecked());
}
