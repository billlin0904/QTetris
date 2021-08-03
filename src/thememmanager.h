#pragma once

#include <QPixmap>
#include <QFont>

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

	static QString clearSound() {
		return ":/QTetris/clear.wav";
	}

	static QString fallSound() {
		return ":/QTetris/fall.wav";
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
