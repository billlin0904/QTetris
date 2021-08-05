#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGamepad>
#include <QSound>

#include "thememmanager.h"
#include "box.h"
#include "gameview.h"

static auto constexpr kDefaultSpeed = 600;

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

GameView::GameView(QWidget* parent)
	: QGraphicsView(parent)
	, background_(ThemeManager::backgroundImage()) {
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

	top_line_ = scene->addLine(197, 47, 403, 47, ThemeManager::linePen());
	bottom_line_ = scene->addLine(197, 453, 403, 453, ThemeManager::linePen());
	left_line_ = scene->addLine(197, 47, 197, 453, ThemeManager::linePen());
	right_line_ = scene->addLine(403, 47, 403, 453, ThemeManager::linePen());
	
	box_group_ = new BoxGroup();
	QObject::connect(box_group_, SIGNAL(newBox()), this, SLOT(clearFullRows()));
	QObject::connect(box_group_, SIGNAL(gameFinished()), this, SLOT(gameOver()));
	scene->addItem(box_group_);

	next_box_group_ = new BoxGroup();
	scene->addItem(next_box_group_);

	//hint_box_group_ = new BoxGroup();
	//scene->addItem(hint_box_group_);

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
		box_group_->keyPress(KeyEvents::KeyDown);
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
	QObject::connect(gamepad_, &QGamepad::buttonBChanged, this, [=](bool pressed) {
		box_group_->keyPress(KeyEvents::KeyAntiRotate);
		});

	QObject::connect(box_group_, &BoxGroup::updateKeyPress, [this](KeyEvents event) {
		switch (event) {
		case KeyEvents::KeyDown:
			sound_mgr_.playFallSound();
			break;
		case KeyEvents::KeyLeft:
			sound_mgr_.playMoveSound();
			break;
		case KeyEvents::KeyRight:
			sound_mgr_.playMoveSound();
			break;
		case KeyEvents::KeyRotate:
			sound_mgr_.playRotateSound();
			break;
		case KeyEvents::KeyAntiRotate:
			sound_mgr_.playRotateSound();			
			break;
		}
		hint_box_ = toQPixmap(box_group_);
		update();
		});
}

void GameView::initGame() {
	box_group_->createBox(QPointF(300, 70));
	//hint_box_group_->createBox(QPoint(300, 432), box_group_->boxShape(), box_group_->color());
	//hint_box_group_->setOpacity(30);
	hint_box_ = toQPixmap(box_group_);

	box_group_->setFocus();

	game_speed_ = kDefaultSpeed;
	box_group_->startTimer(kDefaultSpeed);
	next_box_group_->createBox(QPoint(500, 70));
	scene()->setBackgroundBrush(ThemeManager::gameBackground());

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
			foreach(QGraphicsItem * item, list) {
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
		spawnBoxUpdateNextBox();
	}
}

void GameView::moveBox() {
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
	spawnBoxUpdateNextBox();
}

void GameView::spawnBoxUpdateNextBox() {
	box_group_->createBox(QPointF(300, 70), next_box_group_->boxShape());
	next_box_group_->clearBoxGroup(true);
	next_box_group_->createBox(QPointF(500, 70));
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

void GameView::drawBackground(QPainter* painter, const QRectF& view_rect) {
	const auto kGridSize = 20;

	QRectF const rect {
		200,
		50,
		200,
		400,
	};

    painter->drawImage(view_rect, background_);

    //painter->setBrush(QBrush(QColor(87, 206, 187)));
    //painter->drawRect(view_rect);

	painter->setBrush(QBrush(QColor(12, 12, 11)));
	painter->drawRect(rect);

	painter->setPen(QColor(53, 53, 52));
	qreal left = rect.left();
	qreal top = rect.top();

	QVarLengthArray<QLineF, 100> lines;
	for (qreal x = left; x < rect.right(); x += kGridSize) {
		lines.append(QLineF(x, rect.top(), x, rect.bottom()));
	}
		
	for (qreal y = top; y < rect.bottom(); y += kGridSize) {
		lines.append(QLineF(rect.left(), y, rect.right(), y));
	}

	painter->drawLines(lines.data(), lines.size());
}

void GameView::gameOver() {
	restartGame();
}

void GameView::restartGame() {
	mask_widget_->hide();
	next_box_group_->clearBoxGroup(true);
	box_group_->clearBoxGroup();
	box_group_->hide();

	foreach(auto * item,
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
