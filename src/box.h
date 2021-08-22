#pragma once

#include <exception>

#include <QGraphicsObject>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QPixmap>

#include "randomtetrisgenerator.h"
#include "keyevents.h"

class Exception : public std::exception {
public:
    explicit Exception(const char *what = "")
        : what_(what) {
    }

    virtual ~Exception() override = default;

    const char * what() const noexcept override {
        return what_;
    }
private:
    const char *what_;
};

class OneBox : public QGraphicsObject {
public:
    explicit OneBox(const QColor& color = Qt::red);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QPainterPath shape() const override;

    void rotate(bool anti);

private:
    int angle_{0};
    QColor color_;
};

class BoxGroup : public QObject, public QGraphicsItemGroup {
    Q_OBJECT
public:
    explicit BoxGroup(RandomTetrisGenerator *generator);

    QRectF boundingRect() const override;

    void clearBoxGroup(bool destroy_box = false);

    OneBox * createBox();

    void createBox(const QPointF& point, BoxShapes shape, QColor color);

    void createBox(const QPointF& point, BoxShapes shape = RandomShape);

    bool isColliding() const;

    BoxShapes boxShape() const;

    QColor color() const;

signals:
    void newBox();

    void gameFinished();

public slots:
    void moveOneStep();

    void startTimer(int interval);

    void stopTimer();

    void keyPress(KeyEvents event);

private:
    void rotateBox(int32_t angle, bool is_anti);

    BoxShapes shape_;
    QTransform old_transform_;
    QTimer timer_;
    QColor color_;
    RandomTetrisGenerator* generator_;
};
