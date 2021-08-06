﻿#include <array>
#include <random>
#include <algorithm>

#include <QKeyEvent>
#include <QPainter>
#include <QDebug>

#include "randomtetrisgenerator.h"
#include "thememmanager.h"
#include "box.h"

OneBox::OneBox(const QColor& color)
	: color_(color) {
}

QRectF OneBox::boundingRect() const {
	qreal pen_width = 1;
	return QRectF(
		-10 - pen_width / 2, 
		-10 - pen_width / 2, 
		20 + pen_width, 
		20 + pen_width);
}

void OneBox::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->drawPixmap(-10, -10, 20, 20, QPixmap(ThemeManager::box(angle_)));
	painter->setBrush(color_);
	QColor pen_color = color_;
	pen_color.setAlpha(20);
	painter->setPen(pen_color);
	painter->drawRect(-10, -10, 20, 20);
}

QPainterPath OneBox::shape() const {
	QPainterPath path;
	path.addRect(-9.5, -9.5, 19, 19);
	return path;
}

void OneBox::rotate(bool anti) {
	if (!anti) {
		if (angle_ - 90 < 0) {
			angle_ = 270;
		}
		else {
			angle_ -= 90;
		}
	}
	else {
		if (angle_ + 90 == 360) {
			angle_ = 0;
		}
		else {
			angle_ += 90;
		}
	}
}

BoxGroup::BoxGroup(RandomTetrisGenerator* generator)
	: timer_(this)
	, generator_(generator) {
	setFlags(QGraphicsItem::ItemIsFocusable);
	old_transform_ = transform();
	QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(moveOneStep()));
	shape_ = RandomShape;
}

QRectF BoxGroup::boundingRect() const {
	qreal pen_width = 1;
	return QRectF(-40 - pen_width / 2, -40 - pen_width / 2, 80 + pen_width, 80 + pen_width);
}

void BoxGroup::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	case Qt::Key_Down:
		keyPress(KeyEvents::KeyDown);
		break;
	case Qt::Key_Left:
		keyPress(KeyEvents::KeyLeft);
		break;
	case Qt::Key_Right:
		keyPress(KeyEvents::KeyRight);
		break;
	case Qt::Key_Space:
		keyPress(KeyEvents::KeyRotate);
		break;
	case Qt::Key_Alt:
		keyPress(KeyEvents::KeyAntiRotate);
		break;
	}
}

void BoxGroup::moveOneStep() {
	try {
		moveBy(0, 20);
		if (isColliding()) {
			moveBy(0, -20);
			clearBoxGroup();
			emit newBox();
		}
	}
	catch (...) {
		moveBy(0, -20);
	}	
}

void BoxGroup::keyPress(KeyEvents event) {
#define rotate(angle, anti) \
	setTransform(QTransform().rotate(angle), true); \
	foreach(item, childItems()) {\
		auto box = dynamic_cast<OneBox*>(item);\
		box->rotate(anti);\
	}

	QGraphicsItem* item = nullptr;

	switch (event) {
	case KeyFastDown:
		qDebug() << "KeyDown!";
		try {
			moveBy(0, 20);
			while (!isColliding()) {
				moveBy(0, 20);
			}
			moveBy(0, -20);
			clearBoxGroup();
			emit newBox();
		}
		catch (...) {
			moveBy(0, -20);
		}
		break;
	case KeyEvents::KeyDown:
		qDebug() << "KeyDown!";
        moveOneStep();
		break;
	case KeyEvents::KeyLeft:
		try {
			moveBy(-20, 0);
			if (isColliding()) {
				moveBy(20, 0);
			}
		}
		catch (...) {
			moveBy(-20, 0);
		}		
		break;
	case KeyEvents::KeyRight:
		try {
			moveBy(20, 0);
			if (isColliding()) {
				moveBy(-20, 0);
			}
		}
		catch (...) {
			moveBy(20, 0);
		}		
		break;
	case KeyEvents::KeyRotate:
		try {
			rotate(90, false);
			if (isColliding()) {
				rotate(-90, false);
			}
		}
		catch (...) {
			rotate(-90, false);
		}
		break;
	case KeyEvents::KeyAntiRotate:
		try {
			rotate(-90, true);
			if (isColliding()) {
				rotate(90, false);
			}
		}
		catch (...) {
			rotate(90, false);
		}		
		break;
	}

	emit updateKeyPress(event);
}

bool BoxGroup::isColliding() const {
	auto item_list = childItems();
	if (item_list.isEmpty()) {
		throw std::exception("childItems is empty.");
	}

	foreach(auto item, item_list) {
		if (item->collidingItems().count() > 1) {
			qDebug() << "Colliding!";
			return true;
		}
	}
	return false;
}

void BoxGroup::clearBoxGroup(bool destroy_box) {
	auto item_list = childItems();

	foreach(auto item, item_list) {
		removeFromGroup(item);
		if (destroy_box) {
			auto box = dynamic_cast<OneBox*>(item);
			box->deleteLater();
		}
	}
}

QColor BoxGroup::color() const {
	return color_;
}

BoxShapes BoxGroup::boxShape() const {
	return shape_;
}

void BoxGroup::createBox(const QPointF& point, BoxShapes shape, QColor color) {
	QList<OneBox*> list;
	setTransform(old_transform_);

	for (int i = 0; i < 4; ++i) {
		auto temp = new OneBox(color);
		list << temp;
		addToGroup(temp);
	}

	switch (shape) {
	case IShape:
		shape_ = IShape;
		list.at(0)->setPos(-30, -10);
		list.at(1)->setPos(-10, -10);
		list.at(2)->setPos(10, -10);
		list.at(3)->setPos(30, -10);
		break;
	case JShape:
		shape_ = JShape;
		list.at(0)->setPos(10, -10);
		list.at(1)->setPos(10, 10);
		list.at(2)->setPos(10, 30);
		list.at(3)->setPos(-10, 30);
		break;
	case LShape:
		shape_ = LShape;
		list.at(0)->setPos(-10, -10);
		list.at(1)->setPos(-10, 10);
		list.at(2)->setPos(-10, 30);
		list.at(3)->setPos(10, 30);
		break;
	case OShape:
		shape_ = OShape;
		list.at(0)->setPos(-10, -10);
		list.at(1)->setPos(10, -10);
		list.at(2)->setPos(-10, 10);
		list.at(3)->setPos(10, 10);
		break;
	case SShape:
		shape_ = SShape;
		list.at(0)->setPos(10, -10);
		list.at(1)->setPos(30, -10);
		list.at(2)->setPos(-10, 10);
		list.at(3)->setPos(10, 10);
		break;
	case TShape:
		shape_ = TShape;
		list.at(0)->setPos(-10, -10);
		list.at(1)->setPos(10, -10);
		list.at(2)->setPos(30, -10);
		list.at(3)->setPos(10, 10);
		break;
	case ZShape:
		shape_ = ZShape;
		list.at(0)->setPos(-10, -10);
		list.at(1)->setPos(10, -10);
		list.at(2)->setPos(10, 10);
		list.at(3)->setPos(30, 10);
		break;
	}
	setPos(point);

	if (isColliding()) {
		stopTimer();
		emit gameFinished();
	}

	color_ = color;
}

void BoxGroup::createBox(const QPointF& point, BoxShapes shape) {
	BoxShapes box_shape;
	QColor color;

	if (shape == RandomShape) {
		box_shape = generator_->makeBoxShape();
	}
	else {
		box_shape = shape;
	}

	color = getShapeColor(box_shape);
	createBox(point, box_shape, color);
}

void BoxGroup::stopTimer() {
	timer_.stop();
}

void BoxGroup::startTimer(int interval) {
	timer_.start(interval);
}
