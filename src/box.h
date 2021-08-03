#pragma once

#include <QGraphicsObject>
#include <QGraphicsItemGroup>
#include <QTimer>

#include "keyevents.h"

enum BoxShapes {
    IShape,
    JShape, 
    LShape,
    OShape, 
    SShape,
    TShape,
    ZShape, 
    RandomShape
};

class OneBox : public QGraphicsObject {
public:
    explicit OneBox(const QColor& color = Qt::red);

    QRectF boundingRect() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    QPainterPath shape() const;

    void rotate(bool anti);

private:
    int angle_{0};
    QColor color_;
};

class BoxGroup : public QObject, public QGraphicsItemGroup {
    Q_OBJECT
public:
    BoxGroup();

    QRectF boundingRect() const;

    void clearBoxGroup(bool destroy_box = false);

    void createBox(const QPointF& point, BoxShapes shape = RandomShape);

    bool isColliding() const;

    BoxShapes boxShape() const;

protected:
    void keyPressEvent(QKeyEvent* event);

signals:
    void newBox();

    void gameFinished();

public slots:
    void moveOneStep();

    void startTimer(int interval);

    void stopTimer();

    void keyPress(KeyEvents event);

private:
    BoxShapes shape_;
    QTransform old_transform_;
    QTimer timer_;
};