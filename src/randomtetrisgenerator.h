#pragma once

#include <array>
#include <random>
#include <algorithm>

#include <QColor>
#include <vector>

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

QColor getShapeColor(BoxShapes shape);

class RandomTetrisGenerator {
public:
    virtual ~RandomTetrisGenerator() = default;

    virtual BoxShapes makeBoxShape() = 0;

protected:
    RandomTetrisGenerator() = default;
};

// https://simon.lc/the-history-of-tetris-randomizers

class NormalTetrisGenerator : public RandomTetrisGenerator {
public:
    NormalTetrisGenerator();

    BoxShapes makeBoxShape() override;

private:
    std::mt19937 engine_;
    std::uniform_int_distribution<int> dist_;
};

class Tetris7BagGenerator : public RandomTetrisGenerator {
public:
    Tetris7BagGenerator();

    BoxShapes makeBoxShape() override;

private:
    std::mt19937 engine_;
    std::vector<BoxShapes> bag_;
};