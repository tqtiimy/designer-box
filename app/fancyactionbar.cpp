#include "fancyactionbar.h"

#include "../libs/core/stylehelper.h"

#include <QPropertyAnimation>

#include <QPainter>
#include <QEvent>
#include <QAction>

FancyToolButton::FancyToolButton(QWidget *parent)
	: QToolButton(parent),
	  m_fader(0)
{
	setAttribute(Qt::WA_Hover, true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

float FancyToolButton::fader()
{
	return m_fader;
}

void FancyToolButton::setFader(float value)
{
	m_fader = value;
	update();
}

void FancyToolButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);


	if (m_fader > 0 && isEnabled() && !isDown() && !isChecked())
	{
		//绘制动画过程
		painter.save();
		int fader = int(40 * m_fader);
		QLinearGradient grad(rect().topLeft(), rect().topRight());
		grad.setColorAt(0, Qt::transparent);
		grad.setColorAt(0.5, QColor(255, 255, 255, fader));
		grad.setColorAt(1, Qt::transparent);
		painter.fillRect(rect(), grad);
		painter.setPen(QPen(grad, 1.0));
		painter.drawLine(rect().topLeft(), rect().topRight());
		painter.drawLine(rect().bottomLeft(), rect().bottomRight());
		painter.restore();
	}
	else if (isDown() || isChecked())
	{
		//绘制按下或选中状态
		painter.save();
		QLinearGradient grad(rect().topLeft(), rect().topRight());
		grad.setColorAt(0, Qt::transparent);
		grad.setColorAt(0.5, QColor(0, 0, 0, 50));
		grad.setColorAt(1, Qt::transparent);
		painter.fillRect(rect(), grad);
		painter.setPen(QPen(grad, 1.0));
		painter.drawLine(rect().topLeft(), rect().topRight());
		painter.drawLine(rect().topLeft(), rect().topRight());
		painter.drawLine(rect().topLeft() + QPoint(0, 1), rect().topRight() + QPoint(0, 1));
		painter.drawLine(rect().bottomLeft(), rect().bottomRight());
		painter.drawLine(rect().bottomLeft(), rect().bottomRight());
		painter.drawLine(rect().topLeft() - QPoint(0, 1), rect().topRight() - QPoint(0, 1));
		painter.restore();
	}

	QRect iconRect(0, 0, 32, 32); //绘制图标
	iconRect.moveCenter(rect().center());
	StyleHelper::drawIconWithShadow(icon(), iconRect, &painter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
}

bool FancyToolButton::event(QEvent *e)
{
	switch (e->type())
	{
	case QEvent::Enter:
		{
			//鼠标进入
			QPropertyAnimation *animation = new QPropertyAnimation(this, "fader");
			animation->setDuration(125);
			animation->setEndValue(1.0);
			animation->start(QAbstractAnimation::DeleteWhenStopped);
		}
		break;

	case QEvent::Leave:
		{
			//鼠标离开
			QPropertyAnimation *animation = new QPropertyAnimation(this, "fader");
			animation->setDuration(125);
			animation->setEndValue(0.0);
			animation->start(QAbstractAnimation::DeleteWhenStopped);
		}
		break;

	default:
		return QToolButton::event(e);
	}

	return false;
}

QSize FancyToolButton::sizeHint() const
{
	QSizeF buttonSize = iconSize().expandedTo(QSize(64, 38));
	return buttonSize.toSize();
}

QSize FancyToolButton::minimumSizeHint() const
{
	return QSize(8, 8);
}

void FancyToolButton::actionChanged()
{
	if (QAction *action = defaultAction())
	{ setVisible(action->isVisible()); }
}




FancyActionBar::FancyActionBar(QWidget *parent)
	: QWidget(parent)
{
	setObjectName(QLatin1String("actionbar"));
	m_actionsLayout = new QVBoxLayout;
	QVBoxLayout *spacerLayout = new QVBoxLayout;
	spacerLayout->addLayout(m_actionsLayout);
	int sbh = 8;
	spacerLayout->addSpacing(sbh);
	spacerLayout->setMargin(0);
	spacerLayout->setSpacing(0);
	setLayout(spacerLayout);
	setContentsMargins(0, 2, 0, 0);
}

void FancyActionBar::paintEvent(QPaintEvent *event)
{
	//绘制工具条
	QPainter painter(this);
	Q_UNUSED(event)
	QColor light = StyleHelper::sidebarHighlight();
	QColor dark = StyleHelper::sidebarShadow();
	painter.setPen(dark);
	painter.drawLine(rect().topLeft(), rect().topRight());
	painter.setPen(light);
	painter.drawLine(rect().topLeft() + QPoint(1, 1), rect().topRight() + QPoint(0, 1));
}

void FancyActionBar::addProjectSelector(QAction *action)
{
	//添加按钮
	FancyToolButton *toolButton = new FancyToolButton(this);
	toolButton->setDefaultAction(action);
	connect(action, SIGNAL(changed()), toolButton, SLOT(actionChanged()));
	m_actionsLayout->insertWidget(-1, toolButton);
	m_buttons.insert(action, toolButton);

}

void FancyActionBar::setButtonVisible(QAction *action, bool visible)
{
	FancyToolButton *button = m_buttons.value(action);

	if (button != NULL)
	{
		button->setVisible(visible);
	}
}

QLayout *FancyActionBar::actionsLayout() const
{
	return m_actionsLayout;
}

QSize FancyActionBar::minimumSizeHint() const
{
	return sizeHint();
}
