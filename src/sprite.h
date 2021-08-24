#pragma once

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>
#include <QPainter>

class Sprite : public QGraphicsObject {
    Q_OBJECT
public:
    Sprite();

private:
    void nextFrame();

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    QRectF boundingRect() const;
private:    
    int cur_frame_;
    QTimer timer_;
    QPixmap image_;
};