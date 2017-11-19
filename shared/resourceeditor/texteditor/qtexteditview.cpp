#include "qtexteditview.h"

#include <QAction>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QLinearGradient>
#include <QScrollBar>
#include <QRegExp>
#include <QPainterPath>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QList>
#include <QMenu>
#include <QFontDialog>
#include <QMouseEvent>
#include <QRegExp>
#include <QTextBlock>

QLineWidget::QLineWidget(QTextDocument *document, QWidget *parent):
	QWidget(parent),
	m_document(document)
{
}

QTextEditView *QLineWidget::editor()const
{
	return qobject_cast<QTextEditView *>(parent());
}

void QLineWidget::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QRect re = this->rect().adjusted(0, 0, 1, 0);
	p.fillRect(re, QBrush(QColor(240, 240, 240)));

	editor()->painteLineWidget(&p);
}

QSize QLineWidget::sizeHint()const
{
	return QSize(editor()->extraAreaWidth(), 0);
}

void QLineWidget::mousePressEvent(QMouseEvent *e)
{
	editor()->extraAreaMouseEvent(e);
}


QTextEditView::QTextEditView(QWidget *parent):
	QPlainTextEdit(parent),
	m_lineWidget(new QLineWidget(document(), this)),
	m_needShowCursor(false)
{
	this->setWordWrapMode(QTextOption::NoWrap);
	this->setFrameStyle(QFrame::NoFrame);
	QFont f = this->font();
	f.setPointSize(12);
	f.setWeight(20);
	this->setFont(f);


	connect(this, SIGNAL(textChanged()), this, SLOT(updateLineWidget()));
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateLineWidget()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(positionChanged()));

	QPalette p = palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Normal, QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Normal, QPalette::HighlightedText));
	setPalette(p);
	setProperty("no-ManhattanStyle", true);
}

void QTextEditView::zoomOut()
{
	QFont f = font();
	f.setPointSize(f.pointSize() - 1);
	setFont(f);
	updateLineWidget();
}

void QTextEditView::zoomIn()
{
	QFont f = font();
	f.setPointSize(f.pointSize() + 1);
	setFont(f);
	updateLineWidget();
}

void QTextEditView::clipboardDataChanged()
{
}

int QTextEditView::extraAreaWidth()const
{
	int space = 0;
	const QFontMetrics fm(fontMetrics());
	int digits = 1;
	int max = qMax(1, document()->blockCount() + 1);

	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	space += fm.width(QLatin1Char('9')) * digits;
	int markWidth = fm.lineSpacing();
	space += markWidth;
	return space;
}

void QTextEditView::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);
	QRect re = contentsRect();
	m_lineWidget->setGeometry(QRect(re.left(), re.top(), extraAreaWidth(), re.height()));
	updateExtraAreaWidth();
}

void QTextEditView::updateExtraAreaWidth()
{
	setViewportMargins(extraAreaWidth(), 0, 0, 0);
}

void QTextEditView::painteLineWidget(QPainter *p)
{
	QRect re = rect();
	const QFontMetrics fm(fontMetrics());
	int extraAreaWidth = m_lineWidget->width();
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	qreal top = blockBoundingGeometry(block).translated(contentOffset()).top() + 2;
	qreal bottom = top + blockBoundingRect(block).height();

	while (block.isValid() && top < re.bottom())
	{
		if (block.isVisible() && bottom >= re.top())
		{
			int lineNumber = blockNumber + 1;

			if (blockNumber == textCursor().block().blockNumber())
			{
				p->setPen(QColor(40, 40, 40));
			}
			else
			{
				p->setPen(QColor(120, 120, 120));
			}

			QString number = QString::number(lineNumber);
			p->drawText(re.x(), (int)top, re.x() + extraAreaWidth - 4, fm.height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + blockBoundingRect(block).height();
		++blockNumber;
	}
}

void QTextEditView::extraAreaMouseEvent(QMouseEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress)
	{
		if (e->button() == Qt::LeftButton)
		{
			QTextCursor cursor = cursorForPosition(QPoint(10, e->pos().y()));
			cursor.setPosition(cursor.block().position());
			cursor.select(QTextCursor::LineUnderCursor);
			setTextCursor(cursor);
			m_lineWidget->update();
		}
	}
}

void QTextEditView::updateLineWidget()
{
	updateExtraAreaWidth();
	m_lineWidget->update();
	update();
}

void QTextEditView::find(const QString text)
{
	if (text == m_findText)
	{
		return;
	}

	m_findText = text;

	if (m_findText != "")
	{
		QTextCursor t = document()->find(m_findText);

		if (!t.isNull())
		{
			setTextCursor(t);
		}
	}

	updateLineWidget();
}

void QTextEditView::positionChanged()
{
	viewport()->update();
	updateLineWidget();
}

void QTextEditView::paintEvent(QPaintEvent *e)
{
	int r = 3;
	int dd = 2 * r;

	if (m_needShowCursor)
	{
		QTextCursor text = textCursor();

		text.select(QTextCursor::WordUnderCursor);
		QString str = text.selectedText();
		text.clearSelection();
		QRegExp reg("[_a-zA-Z][_a-zA-Z0-9]*");

		if (reg.indexIn(str) == 0 && reg.matchedLength() == str.size())
		{
			QPainter p(viewport());
			int pos = 0;

			while (1)
			{
				text = document()->find(str, pos, QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords);

				if (text.isNull())
				{
					break;
				}

				text.clearSelection();
				text.movePosition(QTextCursor::PreviousWord);
				text.movePosition(QTextCursor::StartOfWord);
				QRect rr = cursorRect(text);
				text.movePosition(QTextCursor::EndOfWord);
				QRect re = cursorRect(text);
				pos = text.position();
				re = rr.united(re);
				QPen pen = QPen(QColor(0, 0, 0, 80), 1);
				pen.setJoinStyle(Qt::RoundJoin);
				p.setPen(pen);
				QLinearGradient d(re.topLeft(), re.bottomLeft());
				d.setColorAt(0, QColor(0, 0, 0, 10));
				d.setColorAt(1, QColor(0, 0, 0, 30));
				p.setBrush(d);
				QPainterPath pa;
				pa.moveTo(re.left() + r, re.top());
				pa.arcTo(re.left(), re.top(), dd, dd, 90, 90);
				pa.lineTo(re.left(), re.bottom() - r);
				pa.arcTo(re.left(), re.bottom() - dd, dd, dd, 180, 90);
				pa.lineTo(re.right() - r, re.bottom());
				pa.arcTo(re.right() - dd, re.bottom() - dd, dd, dd, 270, 90);
				pa.lineTo(re.right(), re.top() + r);
				pa.arcTo(re.right() - dd, re.top(), dd, dd, 0, 90);
				pa.closeSubpath();
				p.drawPath(pa);
			}
		}

	}

	QString ss = m_findText;
	ss.remove(" ");

	if (ss.size() > 0 && m_findText.size() > 1)
	{
		QPainter p(viewport());
		int pos = 0;
		QTextCursor text;

		while (1)
		{
			text = document()->find(m_findText, pos);

			if (text.isNull())
			{
				break;
			}

			QTextCursor te = text;
			te.setPosition(text.selectionStart());
			QRect rr = cursorRect(te);
			te = text;
			te.setPosition(text.selectionEnd());
			QRect re = cursorRect(te);
			pos = text.position();
			re = rr.united(re);
			QPen pen = QPen(QColor(255, 239, 11).darker(150), 1);
			pen.setJoinStyle(Qt::RoundJoin);
			p.setPen(pen);
			QLinearGradient d(re.topLeft(), re.bottomLeft());
			d.setColorAt(0, QColor(255, 255, 0, 190));
			d.setColorAt(1, QColor(255, 255, 0, 130));
			p.setBrush(d);
			QPainterPath pa;
			pa.moveTo(re.left() + r, re.top());
			pa.arcTo(re.left(), re.top(), dd, dd, 90, 90);
			pa.lineTo(re.left(), re.bottom() - r);
			pa.arcTo(re.left(), re.bottom() - dd, dd, dd, 180, 90);
			pa.lineTo(re.right() - r, re.bottom());
			pa.arcTo(re.right() - dd, re.bottom() - dd, dd, dd, 270, 90);
			pa.lineTo(re.right(), re.top() + r);
			pa.arcTo(re.right() - dd, re.top(), dd, dd, 0, 90);
			pa.closeSubpath();
			p.drawPath(pa);
		}
	}

	QPlainTextEdit::paintEvent(e);
}

void QTextEditView::findNext()
{
	if (m_findText == "")
	{
		return;
	}

	QTextCursor t = document()->find(m_findText, textCursor());

	if (!t.isNull())
	{
		setTextCursor(t);
		updateLineWidget();
	}
	else
	{
		QTextCursor t = document()->find(m_findText);

		if (!t.isNull())
		{
			setTextCursor(t);
			updateLineWidget();
		}
	}
}

void QTextEditView::findPrev()
{
	if (m_findText == "")
	{
		return;
	}

	QTextCursor t = document()->find(m_findText, textCursor(), QTextDocument::FindBackward);

	if (!t.isNull())
	{
		setTextCursor(t);
		updateLineWidget();
	}
	else
	{
		QTextCursor t = document()->find(m_findText, this->toPlainText().size(), QTextDocument::FindBackward);

		if (!t.isNull())
		{
			setTextCursor(t);
			updateLineWidget();
		}
	}
}

void QTextEditView::replace(const QString &text)
{
	if (m_findText == text)
	{
		return;
	}

	if (m_findText == "")
	{
		return;
	}

	QTextCursor tt = textCursor();

	if (tt.selectedText().toLower() == m_findText.toLower())
	{
		tt.insertText(text);
		updateLineWidget();
	}
}

void QTextEditView::replaceNext(const QString &text)
{
	if (m_findText == "")
	{
		return;
	}

	replace(text);
	findNext();

}

void QTextEditView::replaceAll(const QString &text)
{
	if (m_findText == "")
	{
		return;
	}

	QTextCursor tt;

	while (1)
	{
		tt = document()->find(m_findText, tt);

		if (tt.isNull())
		{
			break;
		}

		tt.insertText(text);
	}

	updateLineWidget();
}

void QTextEditView::changedFont()
{
	QFont f = font();
	bool ok;
	f = QFontDialog::getFont(&ok, f, this, tr("Select Font"));

	if (ok)
	{
		setFont(f);
		f.setBold(false);
		f.setItalic(false);
		f.setUnderline(false);
		f.setStrikeOut(false);
		m_lineWidget->setFont(f);
		updateLineWidget();
	}
}
