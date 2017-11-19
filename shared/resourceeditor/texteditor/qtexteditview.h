#ifndef QTEXTEDITVIEW_H
#define QTEXTEDITVIEW_H

#include <QPlainTextEdit>

class QTextEditView;

class QLineWidget : public QWidget
{
	Q_OBJECT
public:
	QLineWidget(QTextDocument *document, QWidget *parent = 0);
protected:
	void mousePressEvent(QMouseEvent *);
	QSize   sizeHint()const;
	void paintEvent(QPaintEvent *);
protected:
	QTextEditView  *editor()const;
protected:
	QTextDocument *m_document;

};

class QTextEditView : public QPlainTextEdit
{
	Q_OBJECT
public:
	QTextEditView(QWidget *parent = 0);

	int extraAreaWidth() const;

	void painteLineWidget(QPainter *p);
	void extraAreaMouseEvent(QMouseEvent *e);
public slots:
	void    findNext();
	void    findPrev();
	void    find(const QString text);
	void    replace(const QString &text);
	void    replaceNext(const QString &text);
	void    replaceAll(const QString &text);

signals:
	void    find();
protected:
	void    resizeEvent(QResizeEvent *e);
	void    paintEvent(QPaintEvent *e);
protected slots:
	void    zoomOut();
	void    zoomIn();
	void    changedFont();

	void    clipboardDataChanged();
	void    updateLineWidget();
	void    updateExtraAreaWidth();

	void    positionChanged();
protected:
	QLineWidget         *m_lineWidget;
	QString             m_findText;
	bool                m_needShowCursor;
};

#endif // QTEXTEDITVIEW_H
