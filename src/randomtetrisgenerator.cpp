#include <array>
#include <random>
#include <algorithm>

#include <QColor>

#include "box.h"
#include "randomtetrisgenerator.h"

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
	static BoxShapes last_shape_id = BoxShapes::RandomShape;
	static auto engines = std::mt19937(std::random_device()());

	//return BoxShapes::IShape;

	auto shape_id = BoxShapes::RandomShape;

	// Avoid random a same shape!
	while (true) {
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

		std::shuffle(k7Bag.begin(), k7Bag.end(), engines);
		shape_id = k7Bag.back();

		if (shape_id != last_shape_id) {
			k7Bag.pop_back();
			break;
		}
	}

	last_shape_id = shape_id;
	return shape_id;
}

BoxShapes RandomTetrisGenerator::makeBoxShape() const {
	return getBoxShape();
}

QColor RandomTetrisGenerator::getShapeColor(BoxShapes shape) const {
	return kColorTable[shape];
}

RandomTetrisGenerator& RandomTetrisGenerator::get() {
	static RandomTetrisGenerator gen;
	return gen;
}