#include "sprite.h"

Sprite::Sprite() 
    : cur_frame_(0) {
    QObject::connect(&timer_, &QTimer::timeout, this, &Sprite::nextFrame);
    timer_.start(30);
}

QRectF Sprite::boundingRect() const {
    return QRectF(-10, -10, 20, 20);
}

void Sprite::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->drawPixmap(-10, -10, image_, cur_frame_, 0, 20, 20);
}

void Sprite::nextFrame() {
    cur_frame_ += 20;
    if (cur_frame_ >= 300) {
        cur_frame_ = 0;
    }
    update(-10, -10, 20, 20);
}