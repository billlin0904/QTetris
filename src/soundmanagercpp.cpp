#include <QMediaPlaylist>

#include "thememmanager.h"
#include "soundmanager.h"

SoundEffectWorker::SoundEffectWorker()
	: player_(this) {
	player_.setAudioRole(QAudio::NotificationRole);
}

void SoundEffectWorker::onPlay(QString const& file, int duration) {
	auto playlist = new QMediaPlaylist();
	playlist->addMedia(QUrl::fromLocalFile(file));
	player_.setPlaylist(playlist);
	player_.play();
	QThread::msleep(duration);
}

SoundManager::SoundManager() {
	worker_.moveToThread(&thread_);
	thread_.start();
	QObject::connect(this,
		&SoundManager::play,
		&worker_,
		&SoundEffectWorker::onPlay);
}

SoundManager::~SoundManager() {
	thread_.quit();
	thread_.wait();	
}

void SoundManager::playClearSound() {
	emit play(ThemeManager::clearSound(), 700);
}

void SoundManager::playFallSound() {
	emit play(ThemeManager::fallSound(), 200);
}