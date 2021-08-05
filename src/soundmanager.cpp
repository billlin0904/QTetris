#include <QMediaPlaylist>

#include "thememmanager.h"
#include "soundmanager.h"

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

void SoundManager::playClearSound() {
	play(0, QUrl::fromLocalFile(ThemeManager::clearSound()));
}

void SoundManager::playFallSound() {
    play(1, QUrl::fromLocalFile(ThemeManager::fallSound()));
}

void SoundManager::playGameOver() {
	play(2, QUrl::fromLocalFile(ThemeManager::gameOverSound()));
}

void SoundManager::playRotateSound() {
	play(3, QUrl::fromLocalFile(ThemeManager::rotateSound()));
}

void SoundManager::playMoveSound() {
	play(4, QUrl::fromLocalFile(ThemeManager::moveSound()));
}

void SoundManager::play(int index, QUrl const& file) {
	player_[index].setSource(file);
	player_[index].play();
}
