#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QMediaPlayer>
#include <QWidget>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
class QComboBox;
QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);

private slots:
    void playClicked();
    void muteClicked();

private:
    QMediaPlayer::State playerState;
    bool playerMuted;
};

#endif // PLAYERCONTROLS_H
