#include <QMediaPlaylist>

#include "thememmanager.h"
#include "soundmanager.h"

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

void SoundManager::playClearSound() {
	play(0, QUrl(ThemeManager::clearSound()));
}

void SoundManager::playFallSound() {
	play(1, QUrl(ThemeManager::fallSound()));
}

void SoundManager::playGameOver() {
	play(2, QUrl(ThemeManager::gameOverSound()));
}

void SoundManager::playRotateSound() {
	play(3, QUrl(ThemeManager::rotateSound()));
}

void SoundManager::playMoveSound() {
	play(4, QUrl(ThemeManager::moveSound()));
}

void SoundManager::play(int index, QUrl const& file) {
	auto playlist = new QMediaPlaylist();
	playlist->addMedia(file);
	player_[index].setPlaylist(playlist);
	player_[index].play();
}