#include <QKeyEvent>
#include <QPainter>
#include <array>
#include <random>

#include "thememmanager.h"
#include "box.h"
#include <stack>

static const std::array<QColor, 7> kColorTable{
   QColor(200, 0, 0, 100),
   QColor(255, 200, 0, 100),
   QColor(0, 0, 200, 100),
   QColor(0, 200, 0, 100),
   QColor(0, 200, 255, 100),
   QColor(200, 0, 255, 100),
   QColor(150, 100, 100, 100)
};

static BoxShapes getBoxShape() {
	// https://simon.lc/the-history-of-tetris-randomizers
	static std::vector<BoxShapes> k7Bag;

	if (k7Bag.empty()) {
		std::vector<BoxShapes> default_bag{
		BoxShapes::IShape,
		BoxShapes::JShape,
		BoxShapes::LShape,
		BoxShapes::OShape,
		BoxShapes::SShape,
		BoxShapes::TShape,
		BoxShapes::ZShape
		};
		k7Bag = default_bag;
	}

	std::random_shuffle(k7Bag.begin(), k7Bag.end());

	auto shape_id = k7Bag.back();
	k7Bag.pop_back();
	return shape_id;
}

struct RandomShape {
	BoxShapes shape{ BoxShapes::RandomShape };
	QColor color;

	static RandomShape makeRandomShape() {
		int shape_id = getBoxShape();
		return RandomShape {
			static_cast<BoxShapes>(shape_id),
			kColorTable[shape_id]
		};
	}
};

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
	painter->drawPixmap(-10, -10, 20, 20, ThemeManager::box());
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

BoxGroup::BoxGroup()
	: timer_(this) {
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
	}
}

void BoxGroup::moveOneStep() {
	moveBy(0, 20);
	if (isColliding()) {
		moveBy(0, -20);
		clearBoxGroup();
		emit newBox();
	}
}

void BoxGroup::keyPress(KeyEvents event) {
#define rotate(angle) setTransform(QTransform().rotate(angle), true)

	switch (event) {
	case KeyEvents::KeyDown:
		/*moveBy(0, 20);
		while (!isColliding()) {
			moveBy(0, 20);
		}
		moveBy(0, -20);
		clearBoxGroup();
		emit newBox();*/
		if (!isColliding()) {
			moveBy(0, 20);
		}
		else {
			moveBy(0, -20);
		}
		break;
	case KeyEvents::KeyLeft:
		moveBy(-20, 0);
		if (isColliding()) {
			moveBy(20, 0);
		}
		break;
	case KeyEvents::KeyRight:
		moveBy(20, 0);
		if (isColliding()) {
			moveBy(-20, 0);
		}
		break;
	case KeyEvents::KeyRotate:
		rotate(90);
		if (isColliding()) {
			rotate(-90);
		}
		break;
	}
}

bool BoxGroup::isColliding() const {
	auto item_list = childItems();

	QGraphicsItem* item = nullptr;
	foreach(item, item_list) {
		if (item->collidingItems().count() > 1) {
			return true;
		}
	}
	return false;
}

void BoxGroup::clearBoxGroup(bool destroy_box) {
	auto item_list = childItems();

	QGraphicsItem* item = nullptr;
	foreach(item, item_list) {
		removeFromGroup(item);
		if (destroy_box) {
			auto box = dynamic_cast<OneBox*>(item);
			box->deleteLater();
		}
	}
}

BoxShapes BoxGroup::boxShape() const {
	return shape_;
}

void BoxGroup::createBox(const QPointF& point, BoxShapes shape) {
	int shape_id;
	QColor color;

	if (shape == RandomShape) {
		const auto random_shape = RandomShape::makeRandomShape();
		shape_id = random_shape.shape;
		color = random_shape.color;
	}
	else {
		shape_id = shape;
		color = kColorTable[shape_id];
	}

	QList<OneBox*> list;
	setTransform(old_transform_);

	for (int i = 0; i < 4; ++i) {
		auto temp = new OneBox(color);
		list << temp;
		addToGroup(temp);
	}

	switch (shape_id) {
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
}

void BoxGroup::stopTimer() {
	timer_.stop();
}

void BoxGroup::startTimer(int interval) {
	timer_.start(interval);
}