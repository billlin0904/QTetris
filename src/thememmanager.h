#pragma once

#include <QPixmap>
#include <QFont>

class ThemeManager {
public:
	static QPixmap box() {
		return QPixmap(":/images/box.gif");
	}

	static QPixmap background() {
		return QPixmap(":/images/background.png");
	}

	static QPixmap initGameBackground() {
		return QPixmap(":/images/background01.png");
	}

	static QFont font() {
		return QFont("Times", 50, QFont::Bold);
	}
};
