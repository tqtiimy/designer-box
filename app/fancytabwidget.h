#ifndef FANCYTABWIDGET_H
#define FANCYTABWIDGET_H

#include <QIcon>
#include <QWidget>

#include <QTimer>
#include <QPropertyAnimation>
#include <QTextEdit>
#include <QSplitter>
#include <QStackedWidget>

class QPainter;
class QStackedLayout;
class QStatusBar;
class QStatusBar;
class QStatusWidget;
class StyledBar;


class FancyActionBar;

class FancyTab : public QObject
{
	Q_OBJECT

	Q_PROPERTY(float fader READ fader WRITE setFader)
public:
	FancyTab(QWidget *tabbar) : enabled(true), tabbar(tabbar), m_fader(0)
	{
		animator.setPropertyName("fader");
		animator.setTargetObject(this);
	}
	float fader() { return m_fader; }
	void setFader(float value);

	void fadeIn();
	void fadeOut();

	QIcon icon;
	QString text;
	QString toolTip;
	bool enabled;

private:
	QPropertyAnimation animator;
	QWidget *tabbar;
	float m_fader;
};

class FancyTabBar : public QWidget
{
	Q_OBJECT

public:
	FancyTabBar(QWidget *parent = 0);
	~FancyTabBar();

	bool event(QEvent *event);

	void paintEvent(QPaintEvent *event);
	void paintTab(QPainter *painter, int tabIndex) const;
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void setTabEnabled(int index, bool enable);
	bool isTabEnabled(int index) const;

	void insertTab(int index, const QIcon &icon, const QString &label)
	{
		FancyTab *tab = new FancyTab(this);
		tab->icon = icon;
		tab->text = label;
		m_tabs.insert(index, tab);
	}
	void setEnabled(int index, bool enabled);
	void removeTab(int index)
	{
		FancyTab *tab = m_tabs.takeAt(index);
		delete tab;
	}
	void setCurrentIndex(int index);
	int currentIndex() const { return m_currentIndex; }

	void setTabToolTip(int index, QString toolTip) { m_tabs[index]->toolTip = toolTip; }
	QString tabToolTip(int index) const { return m_tabs.at(index)->toolTip; }

	QIcon tabIcon(int index) const { return m_tabs.at(index)->icon; }
	QString tabText(int index) const { return m_tabs.at(index)->text; }
	int count() const {return m_tabs.count(); }
	QRect tabRect(int index) const;
	int   findTab(const QString &label);
signals:
	void currentChanged(int);

public slots:
	void emitCurrentIndex();

private:
	static const int m_rounding;
	static const int m_textPadding;
	QRect m_hoverRect;
	int m_hoverIndex;
	int m_currentIndex;
	QList<FancyTab *> m_tabs;
	QTimer m_triggerTimer;
	QSize tabSizeHint(bool minimum = false) const;
};

class FancyTabWidget : public QWidget
{
	Q_OBJECT

public:
	FancyTabWidget(QWidget *parent = 0);

	void insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label);
	void removeTab(int index);
	void setBackgroundBrush(const QBrush &brush);
	void setTabToolTip(int index, const QString &toolTip);

	void paintEvent(QPaintEvent *event);
	int tagCount();
	int currentIndex() const;

	void setTabEnabled(int index, bool enable);
	bool isTabEnabled(int index) const;

	int  findTab(const QString &label);
	void setCurrentIndex(const QString &label);

	void setButtonVisible(QAction *action, bool visible);

	void setStatusButtonCheck(int index, bool checked);
signals:
	void currentAboutToShow(int index);
	void currentChanged(int index);

public slots:
	void setCurrentIndex(int index);
	void setSelectionWidgetHidden(bool hidden);

private slots:
	void showWidget(int index);
	void statusClicked(int index, bool select);
private:
	FancyTabBar *m_tabBar;
	QStackedLayout *m_modesStack;
	QStatusBar  *m_statusbar;
	QWidget *m_selectionWidget;
	FancyActionBar      *m_actionBar;
	QStatusWidget       *m_statusWidget;

	QSplitter           *m_splitter;

	QStackedWidget      *m_outputView;
	QWidget             *m_debugMsgView;
	QWidget             *m_findView;
};

#endif // FANCYTABWIDGET_H
