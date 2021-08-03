#pragma once

#include <QThread>
#include <QMediaPlayer>

class SoundEffectWorker : public QObject {
	Q_OBJECT
public:
	SoundEffectWorker();

public slots:
	void onPlay(QString const &file, int duration);

private:
	QMediaPlayer player_;
};

class SoundManager : public QObject {
	Q_OBJECT
public:
	SoundManager();

	~SoundManager();

	void playClearSound();

	void playFallSound();

signals:
	void play(QString const& file, int duration);

private:
	QThread thread_;
	SoundEffectWorker worker_;
};
