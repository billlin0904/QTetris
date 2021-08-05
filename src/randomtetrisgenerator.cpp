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

QColor getShapeColor(BoxShapes shape) {
	return kColorTable[shape];
}

Tetris7BagGenerator::Tetris7BagGenerator()
	: engine_(std::random_device()()) {
}

BoxShapes Tetris7BagGenerator::makeBoxShape() {
	// https://simon.lc/the-history-of-tetris-randomizers

	//return BoxShapes::IShape;

	auto shape_id = BoxShapes::RandomShape;

	if (bag_.empty()) {
		std::vector<BoxShapes> default_bag{
		BoxShapes::IShape,
		BoxShapes::JShape,
		BoxShapes::LShape,
		BoxShapes::OShape,
		BoxShapes::SShape,
		BoxShapes::TShape,
		BoxShapes::ZShape
		};
		bag_ = default_bag;
	}

	std::shuffle(bag_.begin(), bag_.end(), engine_);
	shape_id = bag_.back();
	bag_.pop_back();

	return shape_id;
}
