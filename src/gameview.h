#pragma once

#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QImage>

#include "soundmanager.h"
#include "keyevents.h"

class OneBox;
class BoxGroup;

class QGamepad;
class QGraphicsBlurEffect;

class GameView : public QGraphicsView {
	Q_OBJECT
public:
	explicit GameView(QWidget* parent = nullptr);

public slots:
    void startGame();

    void restartGame();
    
    void pauseGame();
    
    void returnGame();

    void clearFullRows();

    void moveBox();

    void gameOver();

    void updateScore(int full_row_num);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void spawnBoxUpdateNextBox();

    void clearEffect(OneBox* box);

    void init();

    void initGame();

    void play(QString const &file);

    qreal game_speed_;
    QList<int> rows_;

    QGraphicsTextItem* game_score_;
    QGraphicsTextItem* game_level_;

    QGraphicsWidget* mask_widge_;
    QGraphicsLineItem* top_line_;
    QGraphicsLineItem* bottom_line_;
    QGraphicsLineItem* left_line_;
    QGraphicsLineItem* right_line_;

    QGraphicsProxyWidget* mask_widget_;

    QGraphicsTextItem* game_welcome_text_;
    QGraphicsTextItem* game_pause_text_;
    QGraphicsTextItem* game_over_text_;

    BoxGroup* box_group_;
    BoxGroup* hint_box_group_;
    BoxGroup* next_box_group_;
    QGamepad* gamepad_;
    SoundManager sound_mgr_;
    QImage background_;
};
