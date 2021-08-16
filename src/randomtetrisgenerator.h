#pragma once

#include <array>
#include <random>
#include <algorithm>
#include <deque>
#include <vector>

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

QColor getShapeColor(BoxShapes shape);

class RandomTetrisGenerator {
public:
    virtual ~RandomTetrisGenerator() = default;

    virtual BoxShapes makeBoxShape() = 0;

    virtual void reset() = 0;

protected:
    RandomTetrisGenerator() = default;
};

// https://simon.lc/the-history-of-tetris-randomizers

class NormalTetrisGenerator : public RandomTetrisGenerator {
public:
    NormalTetrisGenerator();

    BoxShapes makeBoxShape() override;

    void reset() override;
private:
    std::mt19937 engine_;
    std::uniform_int_distribution<int> dist_;
};

class TestTetrisGenerator : public RandomTetrisGenerator {
public:
    explicit TestTetrisGenerator(std::deque<BoxShapes> const & shapes);

    BoxShapes makeBoxShape() override;

    void reset() override;

private:
    std::deque<BoxShapes> const test_bag_;
    std::deque<BoxShapes> bag_;
};

class Tetris7BagGenerator : public RandomTetrisGenerator {
public:
    Tetris7BagGenerator();

    void reset() override;

    BoxShapes makeBoxShape() override;

private:
    static const std::vector<BoxShapes> kDefaultBag;
    std::mt19937 engine_;
    std::vector<BoxShapes> bag_;
};
