#pragma once

#include <array>
#include <memory>
#include <functional>

#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QImage>

#include "soundmanager.h"
#include "keyevents.h"

class OneBox;
class BoxGroup;
class RandomTetrisGenerator;

class QGamepad;
class QGraphicsBlurEffect;

using BoxTag = std::array<char, 10 * 20>;

class GameView : public QGraphicsView {
	Q_OBJECT
public:
    explicit GameView(RandomTetrisGenerator *generator = nullptr, QWidget* parent = nullptr);

    virtual ~GameView() override;

    void keyPress(KeyEvents event);

public slots:
    void startGame();

    void restartGame();
    
    void pauseGame();
    
    void returnGame();

    void clearFullRows();

    void moveBox();

    void gameOver();

    void updateScore(int full_row_num);

    void setScriptAction(std::function<void(GameView *)> action) {
        script_action_ = action;
    }

    void setBoxTag(BoxTag const &tags);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    void spawnBox();

    void clearEffect(OneBox* box);

    void init(RandomTetrisGenerator *generator);

    void initGame();

    void play(QString const &file);

    static const auto kGridSize = 20;
    static const auto kNextBoxShapeSize = 3;

    int game_speed_;
    QPixmap hint_box_;
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
    QGamepad* gamepad_;
    std::unique_ptr<RandomTetrisGenerator> random_generator_;
    std::function<void(GameView *)> script_action_;
    SoundManager sound_mgr_;
    QImage background_;
    QImage small_background_;
    std::array<std::unique_ptr<BoxGroup>, kNextBoxShapeSize> next_box_group_;
};
