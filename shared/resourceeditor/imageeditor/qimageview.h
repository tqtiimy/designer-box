#ifndef QIMAGEVIEW_H
#define QIMAGEVIEW_H

#include <QGraphicsView>

class QImageView : public QGraphicsView
{
	Q_OBJECT
public:
	QImageView(QWidget *parent = 0);

	~QImageView();

	bool setImage(QByteArray array);

	bool isAnimated()const;
	bool isPaused()const;
	void setPaused(bool paused);

public slots:
	void setViewBackground(bool enable);
	void setViewOutline(bool enable);
	void zoomIn();
	void zoomOut();
	void resetToOriginalSize();
	void fitToScreen();
protected slots:
	void doScale(qreal factor);
	void updatePixmap(const QRect &rect);
	void pixmapResized(const QSize &size);
signals:
	void scaleFactorChanged(qreal factor);
	void imageSizeChanged(const QSize &size);
private:
	void emitScaleFactor();
protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
	void hideEvent(QHideEvent *);
	void showEvent(QShowEvent *event);
	void wheelEvent(QWheelEvent *event);
protected:
	struct QImageViewPrivate *d;
};

#endif // QIMAGEVIEW_H
