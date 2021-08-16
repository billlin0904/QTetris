#include <QTime>
#include <QtWidgets/QApplication>

#include "randomtetrisgenerator.h"
#include "box.h"
#include "gameview.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    /*
    GameView view(new TestTetrisGenerator({
        BoxShapes::OShape,
        BoxShapes::SShape,
        BoxShapes::IShape,
        BoxShapes::SShape,
        BoxShapes::LShape,
        BoxShapes::TShape,
    }));

    view.show();

    view.setScriptAction([](auto *view) {
        view->keyPress(KeyRight);
        view->keyPress(KeyRight);
        view->keyPress(KeyRight);
        view->keyPress(KeyRight);
        view->keyPress(KeyFastDown);

        view->keyPress(KeyRotate);
        view->keyPress(KeyRight);
        view->keyPress(KeyRight);
        view->keyPress(KeyFastDown);

        view->keyPress(KeyLeft);
        view->keyPress(KeyLeft);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        view->keyPress(KeyDown);
        for (int i = 0; i < 20; ++i) {
            view->keyPress(KeyRight);
        }
        view->keyPress(KeyFastDown);

        view->keyPress(KeyRotate);
        view->keyPress(KeyLeft);
        view->keyPress(KeyFastDown);

        view->keyPress(KeyLeft);
        view->keyPress(KeyLeft);
        view->keyPress(KeyLeft);
        view->keyPress(KeyLeft);
        view->keyPress(KeyFastDown);
    });
    */

    GameView view(new TestTetrisGenerator({
            BoxShapes::TShape,
            }));

    view.show();

    view.setScriptAction([](auto *view) {
        BoxTag const tags {
            'z','z','z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z','x','x',
            'x','x','x','x','x','x','x','z','z','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','z','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','z','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','z','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','x','x',
            'x','x','x','x','x','x','x','z','z','x',
            'x','x','x','x','x','x','x','z','x','x',
            };
        view->setBoxTag(tags);
    });
    view.startGame();

    return a.exec();
}
