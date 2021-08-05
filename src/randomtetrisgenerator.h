#pragma once

#include <QColor>

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

struct TetrisShape {
	BoxShapes shape{ BoxShapes::RandomShape };
	QColor color;
};

class RandomTetrisGenerator {
public:
	static RandomTetrisGenerator& get();
	
	TetrisShape makeTetrisShape() const;

	QColor getShapeColor(BoxShapes shape) const;

private:
	RandomTetrisGenerator() = default;
};