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

class RandomTetrisGenerator {
public:
	static RandomTetrisGenerator& get();
	
    BoxShapes makeBoxShape() const;

	QColor getShapeColor(BoxShapes shape) const;

private:
	RandomTetrisGenerator() = default;
};