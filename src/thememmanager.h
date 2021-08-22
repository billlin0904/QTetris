#pragma once

#include <QPixmap>
#include <QPen>
#include <QFont>
#include <QApplication>

class ThemeManager {
public:
	static QPixmap box(int angle) {
		if (angle == 0) {
			return QPixmap(":/QTetris/box.png");
		}
		else if (angle == 90) {
			return QPixmap(":/QTetris/box_90.png");
		}
		else if (angle == 180) {
			return QPixmap(":/QTetris/box_180.png");
		}
		return QPixmap(":/QTetris/box_270.png");
	}

    static QString smallBackgroundImage() {
        return ":/QTetris/background/small_background";
    }

	static QString backgroundImage() {
		return ":/QTetris/background/background.jpg";
	}

	static QString clearSound() {
        return qApp->applicationDirPath() + "//sound//clear.wav";
	}

	static QString moveSound() {
        return qApp->applicationDirPath() + "//sound//move.wav";
	}

	static QString fallSound() {
        return qApp->applicationDirPath() + "//sound//fall.wav";
	}

	static QString gameOverSound() {
        return qApp->applicationDirPath() + "//sound//gameover.wav";
	}

	static QString rotateSound() {
        return qApp->applicationDirPath() + "//sound//rotate.wav";
	}

	static QPen linePen() {
		return QPen(Qt::white);
	}

	static QBrush background() {
		return Qt::black;
	}

	static QBrush gameBackground() {
		return Qt::black;
	}

	static QFont font() {
		return QFont("Times", 50, QFont::Bold);
	}
};
