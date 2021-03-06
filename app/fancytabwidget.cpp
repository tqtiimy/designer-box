#include "fancytabwidget.h"

#include "fancyactionbar.h"
#include "qstatuswidget.h"
#include "qdebugwidget.h"

#include "../libs/core/stylehelper.h"
#include "../libs/core/styledbar.h"
#include "../libs/core/qsoftcore.h"
#include "../libs/core/minisplitter.h"


#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QStyleFactory>
#include <QPainter>
#include <QSplitter>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolButton>
#include <QToolTip>
#include <QAnimationGroup>
#include <QPropertyAnimation>
#include <QStyle>
#include <QPixmapCache>
#include <QStyleOption>
#include <QStatusBar>
#include <QApplication>

const int FancyTabBar::m_rounding = 22;
const int FancyTabBar::m_textPadding = 4;

void FancyTab::fadeIn()
{
	animator.stop();
	animator.setDuration(80);
	animator.setEndValue(40);
	animator.start();
}

void FancyTab::fadeOut()
{
	animator.stop();
	animator.setDuration(160);
	animator.setEndValue(0);
	animator.start();
}

void FancyTab::setFader(float value)
{
	m_fader = value;
	tabbar->update();
}


FancyTabBar::FancyTabBar(QWidget *parent)
	: QWidget(parent)
{
	m_hoverIndex = -1;
	m_currentIndex = -1;
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	setStyle(QStyleFactory::create(QLatin1String("windows")));
	setMinimumWidth(qMax(2 * m_rounding, 40));
	setAttribute(Qt::WA_Hover, true);
	setFocusPolicy(Qt::NoFocus);
	setMouseTracking(true);
	m_triggerTimer.setSingleShot(true);

	connect(&m_triggerTimer, SIGNAL(timeout()), this, SLOT(emitCurrentIndex()));
}

FancyTabBar::~FancyTabBar()
{
	delete style();
}

QSize FancyTabBar::tabSizeHint(bool minimum) const
{
	QFont boldFont(font());
	boldFont.setPointSizeF(StyleHelper::sidebarFontSize());
	boldFont.setBold(true);
	QFontMetrics fm(boldFont);
	int spacing = 8;
	int width = 60 + spacing + 2;
	int maxLabelwidth = 0;

	for (int tab = 0 ; tab < count() ; ++tab)
	{
		int width = fm.width(tabText(tab));

		if (width > maxLabelwidth)
		{ maxLabelwidth = width; }
	}

	int iconHeight = minimum ? 0 : 32;
	return QSize(qMax(width, maxLabelwidth + 4), iconHeight + spacing + fm.height());
}

void FancyTabBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter p(this);

	for (int i = 0; i < count(); ++i)
		if (i != currentIndex())
		{ paintTab(&p, i); }

	if (currentIndex() != -1)
	{ paintTab(&p, currentIndex()); }
}

void FancyTabBar::mouseMoveEvent(QMouseEvent *e)
{
	int newHover = -1;

	for (int i = 0; i < count(); ++i)
	{
		QRect area = tabRect(i);

		if (area.contains(e->pos()))
		{
			newHover = i;
			break;
		}
	}

	if (newHover == m_hoverIndex)
	{ return; }

	if (validIndex(m_hoverIndex))
	{ m_tabs[m_hoverIndex]->fadeOut(); }

	m_hoverIndex = newHover;

	if (validIndex(m_hoverIndex))
	{
		m_tabs[m_hoverIndex]->fadeIn();
		m_hoverRect = tabRect(m_hoverIndex);
	}
}

bool FancyTabBar::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		if (validIndex(m_hoverIndex))
		{
			QString tt = tabToolTip(m_hoverIndex);

			if (!tt.isEmpty())
			{
				QToolTip::showText(static_cast<QHelpEvent *>(event)->globalPos(), tt, this);
				return true;
			}
		}
	}

	return QWidget::event(event);
}

void FancyTabBar::enterEvent(QEvent *e)
{
	Q_UNUSED(e)
	m_hoverRect = QRect();
	m_hoverIndex = -1;
}

void FancyTabBar::leaveEvent(QEvent *e)
{
	Q_UNUSED(e)
	m_hoverIndex = -1;
	m_hoverRect = QRect();

	for (int i = 0 ; i < m_tabs.count() ; ++i)
	{
		m_tabs[i]->fadeOut();
	}
}

QSize FancyTabBar::sizeHint() const
{
	QSize sh = tabSizeHint();
	return QSize(sh.width(), sh.height() * m_tabs.count());
}

QSize FancyTabBar::minimumSizeHint() const
{
	QSize sh = tabSizeHint(true);
	return QSize(sh.width(), sh.height() * m_tabs.count());
}

QRect FancyTabBar::tabRect(int index) const
{
	QSize sh = tabSizeHint();

	if (sh.height() * m_tabs.count() > height())
	{ sh.setHeight(height() / m_tabs.count()); }

	return QRect(0, index * sh.height(), sh.width(), sh.height());

}

int FancyTabBar::findTab(const QString &label)
{
	for (int i = 0; i < m_tabs.size(); i++)
	{
		if (m_tabs.at(i)->text == label)
		{
			return i;
		}
	}

	return -1;
}

void FancyTabBar::emitCurrentIndex()
{
	emit currentChanged(m_currentIndex);
}

void FancyTabBar::mousePressEvent(QMouseEvent *e)
{
	e->accept();

	for (int index = 0; index < m_tabs.count(); ++index)
	{
		if (tabRect(index).contains(e->pos()))
		{

			if (isTabEnabled(index))
			{
				m_currentIndex = index;
				update();
				m_triggerTimer.start(0);
			}

			break;
		}
	}
}

void FancyTabBar::paintTab(QPainter *painter, int tabIndex) const
{
	if (!validIndex(tabIndex))
	{
		qWarning("invalid index");
		return;
	}

	painter->save();

	QRect rect = tabRect(tabIndex);
	bool selected = (tabIndex == m_currentIndex);
	bool enabled = isTabEnabled(tabIndex);

	if (selected)
	{
		painter->save();
		QLinearGradient grad(rect.topLeft(), rect.topRight());
		grad.setColorAt(0, QColor(255, 255, 255, 140));
		grad.setColorAt(1, QColor(255, 255, 255, 210));
		painter->fillRect(rect.adjusted(0, 0, 0, -1), grad);
		painter->restore();

		painter->setPen(QColor(0, 0, 0, 110));
		painter->drawLine(rect.topLeft() + QPoint(1, -1), rect.topRight() - QPoint(0, 1));
		painter->drawLine(rect.bottomLeft(), rect.bottomRight());
		painter->setPen(QColor(0, 0, 0, 40));
		painter->drawLine(rect.topLeft(), rect.bottomLeft());

		painter->setPen(QColor(255, 255, 255, 50));
		painter->drawLine(rect.topLeft() + QPoint(0, -2), rect.topRight() - QPoint(0, 2));
		painter->drawLine(rect.bottomLeft() + QPoint(0, 1), rect.bottomRight() + QPoint(0, 1));
		painter->setPen(QColor(255, 255, 255, 40));
		painter->drawLine(rect.topLeft() + QPoint(0, 0), rect.topRight());
		painter->drawLine(rect.topRight() + QPoint(0, 1), rect.bottomRight() - QPoint(0, 1));
		painter->drawLine(rect.bottomLeft() + QPoint(0, -1), rect.bottomRight() - QPoint(0, 1));
	}

	QString tabText(this->tabText(tabIndex));
	QRect tabTextRect(rect);
	const bool drawIcon = rect.height() > 36;
	QRect tabIconRect(tabTextRect);
	tabTextRect.translate(0, drawIcon ? -2 : 1);
	QFont boldFont(painter->font());
	boldFont.setPointSizeF(StyleHelper::sidebarFontSize());
	boldFont.setBold(true);
	painter->setFont(boldFont);
	painter->setPen(selected ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 110));
	const int textFlags = Qt::AlignCenter | (drawIcon ? Qt::AlignBottom : Qt::AlignVCenter) | Qt::TextWordWrap;

	if (enabled)
	{
		painter->drawText(tabTextRect, textFlags, tabText);
		painter->setPen(selected ? QColor(60, 60, 60) : StyleHelper::panelTextColor());
	}
	else
	{
		painter->setPen(selected ? StyleHelper::panelTextColor() : QColor(0, 0, 0, 120));
	}

	if (!selected && enabled)
	{
		painter->save();
		int fader = int(m_tabs[tabIndex]->fader());
		QLinearGradient grad(rect.topLeft(), rect.topRight());
		grad.setColorAt(0, Qt::transparent);
		grad.setColorAt(0.5, QColor(255, 255, 255, fader));
		grad.setColorAt(1, Qt::transparent);
		painter->fillRect(rect, grad);
		painter->setPen(QPen(grad, 1.0));
		painter->drawLine(rect.topLeft(), rect.topRight());
		painter->drawLine(rect.bottomLeft(), rect.bottomRight());
		painter->restore();
	}

	if (!enabled)
	{ painter->setOpacity(0.7); }

	if (drawIcon)
	{
		int textHeight = painter->fontMetrics().boundingRect(QRect(0, 0, width(), height()), Qt::TextWordWrap, tabText).height();
		tabIconRect.adjust(0, 4, 0, -textHeight);
		StyleHelper::drawIconWithShadow(tabIcon(tabIndex), tabIconRect, painter, enabled ? QIcon::Normal : QIcon::Disabled);
	}

	painter->translate(0, -1);
	painter->drawText(tabTextRect, textFlags, tabText);
	painter->restore();
}

void FancyTabBar::setCurrentIndex(int index)
{
	if (isTabEnabled(index))
	{
		m_currentIndex = index;
		update();
		emit currentChanged(m_currentIndex);
	}
}

void FancyTabBar::setTabEnabled(int index, bool enable)
{
	Q_ASSERT(index < m_tabs.size());
	Q_ASSERT(index >= 0);

	if (index < m_tabs.size() && index >= 0)
	{
		m_tabs[index]->enabled = enable;
		update(tabRect(index));
	}
}

bool FancyTabBar::isTabEnabled(int index) const
{
	Q_ASSERT(index < m_tabs.size());
	Q_ASSERT(index >= 0);

	if (index < m_tabs.size() && index >= 0)
	{ return m_tabs[index]->enabled; }

	return false;
}

class FancyColorButton : public QWidget
{
public:
	FancyColorButton(QWidget *parent)
		: m_parent(parent)
	{
		setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	}

	void mousePressEvent(QMouseEvent *ev)
	{
		if (ev->modifiers() & Qt::ShiftModifier)
		{
			QColor color = QColorDialog::getColor(StyleHelper::baseColor(), m_parent);

			if (color.isValid())
			{ StyleHelper::setBaseColor(color); }
		}
	}
private:
	QWidget *m_parent;
};

FancyTabWidget::FancyTabWidget(QWidget *parent)
	: QWidget(parent)
{
	m_tabBar = new FancyTabBar(this);

	m_selectionWidget = new QWidget(this);
	QVBoxLayout *selectionLayout = new QVBoxLayout;
	selectionLayout->setSpacing(0);
	selectionLayout->setMargin(0);

	StyledBar *bar = new StyledBar;
	QHBoxLayout *layout = new QHBoxLayout(bar);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(new FancyColorButton(this));
	selectionLayout->addWidget(bar);

	selectionLayout->addWidget(m_tabBar);

	m_actionBar = new FancyActionBar(this);
	selectionLayout->addWidget(m_actionBar);


	m_selectionWidget->setLayout(selectionLayout);
	m_selectionWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);


	m_modesStack = new QStackedLayout;
	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->setMargin(0);
	vlayout->setSpacing(0);
	m_statusbar = new QStatusBar(this);
	m_statusbar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	m_statusbar->setSizeGripEnabled(false);
	m_statusWidget = new QStatusWidget;
	m_statusbar->insertWidget(0, m_statusWidget, 1);

	m_outputView = new QStackedWidget;
	m_outputView->setVisible(false);
	m_outputView->addWidget(g_debugWidget);



	m_splitter = new MiniSplitter(Qt::Vertical);
	QWidget *wid = new QWidget;
	wid->setLayout(m_modesStack);
	m_splitter->addWidget(wid);
	m_splitter->addWidget(m_outputView);
	m_splitter->setStretchFactor(0, 1);
	m_splitter->setStretchFactor(1, 0);
	m_splitter->setSizes(QList<int>() << 100 << 100);
	vlayout->addWidget(m_splitter);
	vlayout->addWidget(m_statusbar);


	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);
	mainLayout->addWidget(m_selectionWidget);
	mainLayout->addLayout(vlayout);
	setLayout(mainLayout);

	connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(showWidget(int)));

	QSoftCore *core = QSoftCore::get_core();
	QAction *ac;

	ac = core->get_action("Debug.Run");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	ac = core->get_action("Debug.Stop");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	ac = core->get_action("Project.Open");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	ac = core->get_action("Project.Save");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	ac = core->get_action("Project.New");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	ac = core->get_action("Project.Close");

	if (ac != NULL)
	{
		m_actionBar->addProjectSelector(ac);
	}

	connect(m_statusWidget, SIGNAL(clicked(int, bool)), this, SLOT(statusClicked(int, bool)));
}

void FancyTabWidget::setSelectionWidgetHidden(bool hidden)
{
	m_selectionWidget->setHidden(hidden);
}

void FancyTabWidget::insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label)
{
	m_modesStack->insertWidget(index, tab);
	m_tabBar->insertTab(index, icon, label);
	//((QFindView*)m_findView)->addPage(page);
}

void FancyTabWidget::removeTab(int index)
{
	m_modesStack->removeWidget(m_modesStack->widget(index));
	m_tabBar->removeTab(index);
}

void FancyTabWidget::setBackgroundBrush(const QBrush &brush)
{
	QPalette pal = m_tabBar->palette();
	pal.setBrush(QPalette::Mid, brush);
	m_tabBar->setPalette(pal);
}

void FancyTabWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);

	QRect rect = m_selectionWidget->rect().adjusted(0, 0, 1, 0);
	rect = style()->visualRect(layoutDirection(), geometry(), rect);
	StyleHelper::verticalGradient(&painter, rect, rect);
	painter.setPen(StyleHelper::borderColor());
	painter.drawLine(rect.topRight(), rect.bottomRight());

	QColor light = StyleHelper::sidebarHighlight();
	painter.setPen(light);
	painter.drawLine(rect.bottomLeft(), rect.bottomRight());
}

int FancyTabWidget::currentIndex() const
{
	return m_tabBar->currentIndex();
}

void FancyTabWidget::setCurrentIndex(int index)
{
	if (m_tabBar->isTabEnabled(index))
	{ m_tabBar->setCurrentIndex(index); }
}

void FancyTabWidget::showWidget(int index)
{
	emit currentAboutToShow(index);
	m_modesStack->setCurrentIndex(index);
	emit currentChanged(index);
}

void FancyTabWidget::setTabToolTip(int index, const QString &toolTip)
{
	m_tabBar->setTabToolTip(index, toolTip);
}

void FancyTabWidget::setTabEnabled(int index, bool enable)
{
	if (index == m_tabBar->currentIndex())
	{
		m_tabBar->setCurrentIndex(0);
	}

	m_tabBar->setTabEnabled(index, enable);
}

bool FancyTabWidget::isTabEnabled(int index) const
{
	return m_tabBar->isTabEnabled(index);
}

int FancyTabWidget::tagCount()
{
	return m_tabBar->count();
}

void FancyTabWidget::statusClicked(int index, bool select)
{
	m_outputView->setVisible(select);
	m_outputView->setCurrentIndex(index);
}

int FancyTabWidget::findTab(const QString &label)
{
	return m_tabBar->findTab(label);
}

void FancyTabWidget::setCurrentIndex(const QString &label)
{
	int index = findTab(label);

	if (index > 0)
	{
		setCurrentIndex(index);
	}
}

void FancyTabWidget::setButtonVisible(QAction *action, bool visible)
{
	m_actionBar->setButtonVisible(action, visible);
}

void FancyTabWidget::setStatusButtonCheck(int index, bool checked)
{
	m_statusWidget->setButtonChecked(index, checked);
}
