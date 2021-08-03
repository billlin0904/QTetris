#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGamepad>
#include <QDebug>

#include "thememmanager.h"
#include "box.h"
#include "gameview.h"

static auto constexpr kDefaultSpeed = 500;

GameView::GameView(QWidget* parent)
	: QGraphicsView(parent) {
	init();
}

void GameView::init() {
	setRenderHint(QPainter::Antialiasing);
	setCacheMode(CacheBackground);
	setWindowTitle(tr("QTetris"));

	setMinimumSize(810, 510);
	setMaximumSize(810, 510);

	auto scene = new QGraphicsScene();
	scene->setSceneRect(5, 5, 800, 500);
	scene->setBackgroundBrush(ThemeManager::background());
	setScene(scene);

	top_line_ = scene->addLine(197, 47, 403, 47);
	bottom_line_ = scene->addLine(197, 453, 403, 453);
	left_line_ = scene->addLine(197, 47, 197, 453);
	right_line_ = scene->addLine(403, 47, 403, 453);
	
	box_group_ = new BoxGroup();
	QObject::connect(box_group_, SIGNAL(newBox()), this, SLOT(clearFullRows()));
	QObject::connect(box_group_, SIGNAL(gameFinished()), this, SLOT(gameOver()));
	scene->addItem(box_group_);

	next_box_group_ = new BoxGroup();
	scene->addItem(next_box_group_);

	game_score_ = new QGraphicsTextItem("0");
	game_score_->setFont(ThemeManager::font());

	game_level_ = new QGraphicsTextItem("0");
	game_level_->setFont(ThemeManager::font());
	game_level_->setPos(20, 150);

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
	QObject::connect(gamepad_, &QGamepad::buttonDownChanged, this, [=](bool value) {
		qDebug() << "Button down";
		//if (!hold_down_btn_lock_) {
		//	hold_down_btn_lock_ = true;
			box_group_->keyPress(KeyEvents::KeyDown);			
		//}
		});
	QObject::connect(gamepad_, &QGamepad::buttonRightChanged, this, [=](bool value) {
		box_group_->keyPress(KeyEvents::KeyRight);
		});
	QObject::connect(gamepad_, &QGamepad::buttonLeftChanged, this, [=](bool value) {
		box_group_->keyPress(KeyEvents::KeyLeft);
		});
	QObject::connect(gamepad_, &QGamepad::buttonAChanged, this, [=](bool pressed) {
		box_group_->keyPress(KeyEvents::KeyRotate);
		});
}

void GameView::initGame() {
	box_group_->createBox(QPointF(300, 70));
	box_group_->setFocus();

	game_speed_ = kDefaultSpeed;
	box_group_->startTimer(kDefaultSpeed);
	next_box_group_->createBox(QPoint(500, 70));
	scene()->setBackgroundBrush(ThemeManager::initGameBackground());

	game_score_->show();
	game_level_->show();
	top_line_->show();
	bottom_line_->show();
	left_line_->show();
	right_line_->show();
	box_group_->show();
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
	animation->setStartValue(4);
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
			foreach(QGraphicsItem * item, list) {
				clearEffect(dynamic_cast<OneBox*>(item));
			}
			rows_ << y;
		}
	}

	if (rows_.count() > 0) {
		QTimer::singleShot(400, this, SLOT(moveBox()));
	}
	else {
		spawnBoxUpdateNextBox();
	}
}

void GameView::moveBox() {
	for (int i = rows_.count(); i > 0; --i) {
		auto row = rows_.at(i - 1);
		foreach(QGraphicsItem * item, 
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
	spawnBoxUpdateNextBox();
}

void GameView::spawnBoxUpdateNextBox() {
	box_group_->createBox(QPointF(300, 70), next_box_group_->boxShape());
	next_box_group_->clearBoxGroup(true);
	next_box_group_->createBox(QPointF(500, 70));
	hold_down_btn_lock_ = false;
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

}

void GameView::gameOver() {
	restartGame();
}

void GameView::keyPressEvent(QKeyEvent* event) {
	QGraphicsView::keyPressEvent(event);
}

void GameView::restartGame() {
	mask_widget_->hide();
	next_box_group_->clearBoxGroup(true);
	box_group_->clearBoxGroup();
	box_group_->hide();

	foreach(QGraphicsItem * item,
		scene()->items(
			199,
			49,
			202,
			402,
			Qt::ContainsItemBoundingRect,
			Qt::DescendingOrder)) {
		scene()->removeItem(item);
		auto box = dynamic_cast<OneBox*>(item);
		box->deleteLater();
	}
	initGame();
}
