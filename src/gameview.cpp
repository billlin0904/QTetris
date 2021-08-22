#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGamepad>
#include <QSound>

#include "thememmanager.h"
#include "box.h"
#include "gameview.h"

static auto constexpr kDefaultSpeed = 400;

static QPixmap toQPixmap(BoxGroup* item) {
	auto size = item->boundingRect().size().toSize();
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	foreach(auto * item, item->childItems()) {
		QStyleOptionGraphicsItem opt;
		item->paint(&painter, &opt);
	}
	return pixmap;
}

GameView::GameView(RandomTetrisGenerator *generator, QWidget* parent)
	: QGraphicsView(parent)
    , background_(ThemeManager::backgroundImage())
    , small_background_(ThemeManager::smallBackgroundImage()) {
    init(generator);
}

GameView::~GameView() = default;

void GameView::init(RandomTetrisGenerator *generator) {
    random_generator_.reset(generator == nullptr ? new Tetris7BagGenerator() : generator);

	setRenderHint(QPainter::Antialiasing);
	setCacheMode(CacheBackground);
	setWindowTitle(tr("QTetris"));

	setMinimumSize(810, 510);
	setMaximumSize(810, 510);

	auto scene = new QGraphicsScene();
	scene->setSceneRect(5, 5, 800, 500);
	scene->setBackgroundBrush(ThemeManager::background());
	setScene(scene);

	top_line_ = scene->addLine(197, 47, 403, 47, ThemeManager::linePen());
	bottom_line_ = scene->addLine(197, 453, 403, 453, ThemeManager::linePen());
	left_line_ = scene->addLine(197, 47, 197, 453, ThemeManager::linePen());
	right_line_ = scene->addLine(403, 47, 403, 453, ThemeManager::linePen());
	
    box_group_ = new BoxGroup(random_generator_.get());
	QObject::connect(box_group_, SIGNAL(newBox()), this, SLOT(clearFullRows()));
	QObject::connect(box_group_, SIGNAL(gameFinished()), this, SLOT(gameOver()));
	scene->addItem(box_group_);

	game_score_ = new QGraphicsTextItem("0");
	game_score_->setFont(ThemeManager::font());

	game_level_ = new QGraphicsTextItem("0");
	game_level_->setFont(ThemeManager::font());
	game_level_->setPos(20, 150);

    tspin_text_ = new QGraphicsTextItem();
    tspin_text_->setFont(ThemeManager::font());
    tspin_text_->setPos(230, 220);
    tspin_text_->setZValue(2);
    tspin_text_->hide();
    scene->addItem(tspin_text_);

	top_line_->hide();
	bottom_line_->hide();
	left_line_->hide();
	right_line_->hide();
	game_score_->hide();
	game_level_->hide();

	auto mask = new QWidget();
	mask->setAutoFillBackground(true);
	mask->setPalette(QPalette(QColor(0, 0, 0, 50)));
	mask->resize(900, 600);
	mask_widget_ = scene->addWidget(mask);
	mask_widget_->setPos(-50, -50);
	mask_widget_->setZValue(1);

	gamepad_ = new QGamepad(0, this);
	QObject::connect(gamepad_, &QGamepad::buttonDownChanged, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyDown);
		}
		});
	QObject::connect(gamepad_, &QGamepad::buttonR1Changed, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyFastDown);
		}
		});
	QObject::connect(gamepad_, &QGamepad::buttonRightChanged, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyRight);
		}
		});
	QObject::connect(gamepad_, &QGamepad::buttonLeftChanged, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyLeft);
		}
		});
	QObject::connect(gamepad_, &QGamepad::buttonAChanged, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyRotate);
		}
		});
	QObject::connect(gamepad_, &QGamepad::buttonBChanged, this, [=](bool pressed) {
		if (pressed) {
            keyPress(KeyEvents::KeyAntiRotate);
		}
		});
}

void GameView::keyPress(KeyEvents event) {
    box_group_->keyPress(event);
}

void GameView::initGame() {
    random_generator_->reset();

    //box_group_->createBox(QPointF(300, 70));
    //hint_box_ = toQPixmap(box_group_);

    box_group_->setFocus();

    int j = 1;
    for (auto& box_group : next_box_group_) {
        box_group.reset(new BoxGroup(random_generator_.get()));
        box_group->createBox(QPointF(460, (70 * j++)));
        scene()->addItem(box_group.get());
    }

	game_speed_ = kDefaultSpeed;
	box_group_->startTimer(kDefaultSpeed);
	scene()->setBackgroundBrush(ThemeManager::gameBackground());

	game_score_->show();
	game_level_->show();
	top_line_->show();
	bottom_line_->show();
	left_line_->show();
	right_line_->show();
	box_group_->show();

    script_action_(this);
}

void GameView::startGame() {
	mask_widget_->hide();
	initGame();
}

void GameView::clearEffect(OneBox *box) {
	auto *blur_effect = new QGraphicsBlurEffect();
	box->setGraphicsEffect(blur_effect);
	auto animation = new QPropertyAnimation(box, "scale");
	animation->setEasingCurve(QEasingCurve::OutBounce);
	animation->setDuration(250);
	animation->setStartValue(2);
	animation->setEndValue(0.25);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	QObject::connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
}

void GameView::clearFullRows() {
	for (int y = 429; y > 50; y -= 20) {
		auto list = scene()->items(
			199,
			y,
			202,
			22, 
			Qt::ContainsItemShape,
			Qt::DescendingOrder);
		if (list.count() == 10) {
			foreach(auto * item, list) {
				clearEffect(dynamic_cast<OneBox*>(item));
			}
			rows_ << y;
		}
	}

	if (rows_.count() > 0) {
		sound_mgr_.playClearSound();
		QTimer::singleShot(400, this, SLOT(moveBox()));
	}
	else {
        spawnBox();
	}
}

void GameView::moveBox() {
	qDebug() << "moveBox";
	for (int i = rows_.count(); i > 0; --i) {
		auto row = rows_.at(i - 1);
		foreach(auto * item, 
			scene()->items(
				199, 
				49,
				202,
				row - 47,
				Qt::ContainsItemShape,
				Qt::DescendingOrder)) {
			item->moveBy(0, 20);
		}
	}
	updateScore(rows_.count());
	rows_.clear();
    spawnBox();
}

void GameView::pauseGame() {
	box_group_->stopTimer();
	mask_widget_->show();
}

void GameView::returnGame() {
	mask_widget_->hide();
	box_group_->startTimer(game_speed_);
}

void GameView::updateScore(int full_row_num) {
    if (full_row_num == 3) {
        tspin_text_->setHtml(tr("<font color=white>T-Spin</font>"));
    } else if (full_row_num == 2) {
        tspin_text_->setHtml(tr("<font color=white>Double</font>"));
    } else if (full_row_num == 1) {
        tspin_text_->setHtml(tr("<font color=white>Single</font>"));
    }

    tspin_text_->show();
    QTimer::singleShot(400, [this]() {
        tspin_text_->hide();
    });
}

void GameView::spawnBox() {
	const auto cur_shape = next_box_group_[0]->boxShape();

    const std::array<BoxShapes, kNextBoxShapeSize> next_box_shapes{
		next_box_group_[1]->boxShape(),
		next_box_group_[2]->boxShape(),
		random_generator_->makeBoxShape()
	};

	box_group_->createBox(QPointF(300, 70), cur_shape);

	int i = 1;
	int j = 0;
	for (auto& box_group : next_box_group_) {
		box_group->clearBoxGroup(true);
		box_group->createBox(QPointF(460, (70 * i++)), next_box_shapes[j]);
		++j;
	}
}

void GameView::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Down:
        keyPress(KeyEvents::KeyDown);
        sound_mgr_.playMoveSound();
        break;
    case Qt::Key_S:
        keyPress(KeyEvents::KeyFastDown);
        sound_mgr_.playMoveSound();
        break;
    case Qt::Key_Left:
        keyPress(KeyEvents::KeyLeft);
        sound_mgr_.playMoveSound();
        break;
    case Qt::Key_Right:
        keyPress(KeyEvents::KeyRight);
        sound_mgr_.playMoveSound();
        break;
    case Qt::Key_Space:
        keyPress(KeyEvents::KeyRotate);
        sound_mgr_.playRotateSound();
        break;
    case Qt::Key_Alt:
        keyPress(KeyEvents::KeyAntiRotate);
        sound_mgr_.playRotateSound();
        break;
    case Qt::Key_Escape:
        restartGame();
        break;
    }
    hint_box_ = toQPixmap(box_group_);
}

void GameView::setBoxTag(BoxTag const &tags) {
    auto x = 0;
    auto y = 0;

    for (auto &tag : tags) {
        if (tag == 'x') {
            auto* box = box_group_->createBox();
            box->setPos(QPointF(((x % 10) * -20), ((y % 20) * -20)));
        }
        //qDebug() << (x % 10) * -20 << "," << (y % 20) * -20;
        ++x;
        if (x % 10 == 0) {
            ++y;
        }
    }

    box_group_->setPos(QPointF(390, 440));
}

void GameView::drawBackground(QPainter* painter, const QRectF& view_rect) {    
    const QRectF line_rect {
        199,
        49,
        202,
        402,
	};

	const QRectF next_box_rect{
		410,
		45,
		100,
		210,
	};

    painter->drawImage(view_rect, background_);

    QPen pen;
    pen.setWidth(8);
    pen.setBrush(QBrush(QColor(12, 12, 11)));
    painter->setPen(pen);

    painter->setBrush(QBrush(QColor(12, 12, 11)));
    painter->drawRect(next_box_rect);

    painter->drawImage(QPoint(199, 49), small_background_);

    painter->setBrush(QBrush(QColor(12, 12, 11)));
    //painter->drawRect(line_rect);

    painter->setPen(QColor(53, 53, 52));
	qreal left = line_rect.left();
	qreal top = line_rect.top();

	QVarLengthArray<QLineF, 100> lines;
	for (qreal x = left; x < line_rect.right(); x += kGridSize) {
		lines.append(QLineF(x, line_rect.top(), x, line_rect.bottom()));
	}		
	for (qreal y = top; y < line_rect.bottom(); y += kGridSize) {
		lines.append(QLineF(line_rect.left(), y, line_rect.right(), y));
	}
	painter->drawLines(lines.data(), lines.size());
}

void GameView::gameOver() {
    //sound_mgr_.playGameOver();
    //QTimer::singleShot(6500, this, SLOT(restartGame()));
    restartGame();
}

void GameView::restartGame() {
	mask_widget_->hide();
	for (auto& box_group : next_box_group_) {
		box_group->clearBoxGroup(true);
	}
	box_group_->clearBoxGroup();
	box_group_->hide();

    foreach(auto * item,
             scene()->items()) {
        auto box = dynamic_cast<OneBox*>(item);
        if (box != nullptr) {
            scene()->removeItem(item);
            box->deleteLater();
        }
    }
	initGame();
}
