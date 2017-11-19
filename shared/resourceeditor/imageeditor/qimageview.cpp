#include "qimageview.h"

#include <QImageReader>
#include <QBuffer>
#include <QGraphicsItem>
#include <QMovie>
#include <qmath.h>

#include <QMessageBox>
#include <QWheelEvent>

class MovieItem: public QGraphicsPixmapItem
{
public:
	MovieItem(QMovie *movie):
		m_movie(movie)
	{
		setPixmap(m_movie->currentPixmap());
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
	{
		painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter->drawPixmap(offset(), m_movie->currentPixmap());
	}

private:
	QMovie *m_movie;
};

struct QImageViewPrivate
{
	QImageViewPrivate():
		imageItem(0),
		backgroundItem(0),
		outlineItem(0),
		movie(0),
		moviePaused(true)
	{

	}

	QGraphicsItem *imageItem;
	QGraphicsRectItem *backgroundItem;
	QGraphicsRectItem *outlineItem;
	QMovie            *movie;
	bool              moviePaused;
};

QImageView::QImageView(QWidget *parent):
	QGraphicsView(parent),
	d(new QImageViewPrivate())
{
	setScene(new QGraphicsScene(this));
	setTransformationAnchor(AnchorViewCenter);
	setDragMode(ScrollHandDrag);
	setViewportUpdateMode(FullViewportUpdate);
	setFrameShape(QFrame::NoFrame);
	setRenderHint(QPainter::SmoothPixmapTransform);



	QPixmap tilePixmap(64, 64);

	tilePixmap.fill(Qt::white);

	QPainter tilePainter(&tilePixmap);

	QColor color(220, 220, 220);
	tilePainter.fillRect(0, 0, 0x20, 0x20, color);
	tilePainter.fillRect(0x20, 0x20, 0x20, 0x20, color);
	tilePainter.end();

	setBackgroundBrush(tilePixmap);
}

QImageView::~QImageView()
{
	delete d;
}

void QImageView::drawBackground(QPainter *painter, const QRectF &)
{
	painter->save();
	painter->resetTransform();
	painter->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
	painter->restore();
}

bool QImageView::setImage(QByteArray array)
{
	QBuffer b(&array);
	b.open(QIODevice::ReadOnly);
	QByteArray format = QImageReader::imageFormat(&b);


	resetToOriginalSize();

	if (format.isEmpty())
	{
		QGraphicsScene *s = scene();
		s->clear();


		resetTransform();

		if (d->movie != 0)
		{
			delete d->movie;
			d->movie = 0;
		}

		b.close();
		d->backgroundItem = 0;
		d->outlineItem = 0;
		return false;
	}

	bool drawBackground = (d->backgroundItem ? d->backgroundItem->isVisible() : false);
	bool drawOutline = (d->outlineItem ? d->outlineItem->isVisible() : true);

	QGraphicsScene *s = scene();
	s->clear();


	resetTransform();

	if (d->movie != 0)
	{
		delete d->movie;
		d->movie = 0;
	}

	if (QMovie::supportedFormats().contains(format))
	{
		d->movie = new QMovie(&b, QByteArray(), this);
		d->movie->setCacheMode(QMovie::CacheAll);

		d->movie->start();
		d->moviePaused = false;
		d->imageItem = new MovieItem(d->movie);
	}
	else
	{
		QPixmap pixmap;
		pixmap = QPixmap::fromImage(QImage::fromData(array));
		QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
		pixmapItem->setTransformationMode(Qt::SmoothTransformation);
		d->imageItem = pixmapItem;
		emit imageSizeChanged(pixmap.size());
	}


	d->imageItem->setCacheMode(QGraphicsItem::NoCache);
	d->imageItem->setZValue(0);

	d->backgroundItem = new QGraphicsRectItem(d->imageItem->boundingRect());
	d->backgroundItem->setBrush(Qt::white);
	d->backgroundItem->setPen(Qt::NoPen);
	d->backgroundItem->setVisible(drawBackground);
	d->backgroundItem->setZValue(-1);

	d->outlineItem = new QGraphicsRectItem(d->imageItem->boundingRect());
	QPen outline(Qt::black, 1, Qt::DashLine);
	outline.setCosmetic(true);
	d->outlineItem->setPen(outline);
	d->outlineItem->setBrush(Qt::NoBrush);
	d->outlineItem->setVisible(drawOutline);
	d->outlineItem->setZValue(1);

	s->addItem(d->backgroundItem);
	s->addItem(d->imageItem);
	s->addItem(d->outlineItem);

	if (d->imageItem->boundingRect().height() == 0 && d->imageItem->boundingRect().width() == 0)
	{
		return false;
	}

	emitScaleFactor();
	return true;
}

void QImageView::emitScaleFactor()
{
	qreal factor = transform().m11();
	emit scaleFactorChanged(factor);
}

bool QImageView::isAnimated()const
{
	return d->movie;
}

bool QImageView::isPaused()const
{
	return d->moviePaused;
}

void QImageView::setPaused(bool paused)
{
	if (!d->movie)
	{
		return;
	}

	d->movie->setPaused(paused);
	d->moviePaused = paused;
}

void QImageView::setViewBackground(bool enable)
{
	if (!d->backgroundItem)
	{
		return;
	}

	d->backgroundItem->setVisible(enable);
}

void QImageView::setViewOutline(bool enable)
{
	if (!d->outlineItem)
	{
		return;
	}

	d->outlineItem->setVisible(enable);
}

void QImageView::doScale(qreal factor)
{
	qreal currentScale = transform().m11();
	qreal newScale = currentScale * factor;
	qreal actualFactor = factor;

	if (newScale > 1000)
	{
		actualFactor = 1000. / currentScale;
	}
	else if (newScale < 0.001)
	{
		actualFactor = 0.001 / currentScale;
	}

	scale(actualFactor, actualFactor);

	emitScaleFactor();
}

void QImageView::zoomIn()
{
	doScale(1.2);
}

void QImageView::zoomOut()
{
	doScale(1 / 1.2);
}

void QImageView::resetToOriginalSize()
{
	resetTransform();
	emitScaleFactor();
}

void QImageView::fitToScreen()
{
	if (!d->imageItem)
	{
		return;
	}

	fitInView(d->imageItem, Qt::KeepAspectRatio);
	emitScaleFactor();
}

void QImageView::updatePixmap(const QRect &rect)
{
	if (d->imageItem)
	{
		d->imageItem->update(rect);
	}
}

void QImageView::pixmapResized(const QSize &size)
{
	emit imageSizeChanged(size);
}

void QImageView::wheelEvent(QWheelEvent *event)
{
	qreal factor = qPow(1.2, event->delta() / 240.0);
	doScale(factor);
	event->accept();
}

void QImageView::showEvent(QShowEvent *)
{
	if (!d->movie)
	{
		return;
	}

	d->movie->setPaused(d->moviePaused);
}

void QImageView::hideEvent(QHideEvent *)
{
	if (!d->movie)
	{
		return;
	}

	d->movie->setPaused(true);
}
