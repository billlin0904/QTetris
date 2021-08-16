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

const std::vector<BoxShapes> Tetris7BagGenerator::kDefaultBag{
	BoxShapes::IShape,
	BoxShapes::JShape,
	BoxShapes::LShape,
	BoxShapes::OShape,
	BoxShapes::SShape,
	BoxShapes::TShape,
	BoxShapes::ZShape
};

QColor getShapeColor(BoxShapes shape) {
	return kColorTable[shape];
}

NormalTetrisGenerator::NormalTetrisGenerator()
	: engine_(std::random_device()())
	, dist_(0, 6) {
}


BoxShapes NormalTetrisGenerator::makeBoxShape() {
	return static_cast<BoxShapes>(dist_(engine_));
}

void NormalTetrisGenerator::reset() {
}

TestTetrisGenerator::TestTetrisGenerator(std::deque<BoxShapes> const & shapes)
    : test_bag_(shapes) {
}

void TestTetrisGenerator::reset() {
    bag_.clear();
}

BoxShapes TestTetrisGenerator::makeBoxShape() {
    if (bag_.empty()) {
        bag_ = test_bag_;
    }
    auto shape = bag_.front();
    bag_.pop_front();
    return shape;
}

Tetris7BagGenerator::Tetris7BagGenerator()
	: engine_(std::random_device()()) {
}

void Tetris7BagGenerator::reset() {
    bag_.clear();
}

BoxShapes Tetris7BagGenerator::makeBoxShape() {
	//return BoxShapes::OShape;

	auto shape_id = BoxShapes::RandomShape;

	if (bag_.empty()) {
		bag_ = kDefaultBag;
	}

	std::shuffle(bag_.begin(), bag_.end(), engine_);
	shape_id = bag_.back();
	bag_.pop_back();

	return shape_id;
}
