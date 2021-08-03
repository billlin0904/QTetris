#include <QTime>
#include <QtWidgets/QApplication>

#include "gameview.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GameView view;
    view.show();
    view.startGame();
    return a.exec();
}
