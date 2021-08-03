#pragma once

#include <QMediaPlayer>

class SoundManager {
public:
	SoundManager();

	~SoundManager();

	void playClearSound();

	void playFallSound();

	void playGameOver();

	void playRotateSound();

	void playMoveSound();

private:
	void play(int index, QUrl const &file);

	QMediaPlayer player_[5];
};
