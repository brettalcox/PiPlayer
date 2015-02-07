#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTableWidget>

namespace Ui {
class player;
}
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QAbstractButton)
QT_FORWARD_DECLARE_CLASS(QAbstractItemView)

class PlaylistModel;

class player : public QWidget
{
    Q_OBJECT

public:
    explicit player(QWidget *parent = 0);
    ~player();

public slots:
    void openFile();
    void remove();
    void togglePlayback();
    void seekForward();
    void seekBackward();
    void previousClicked();
    void nextClicked();
    void stopClicked();
    void volumeChanged(int volume);

signals:
    void play();
    void next();
    void previous();
    void changeMuted(bool muting);

private slots:
    void updateState(QMediaPlayer::State state);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void jump(const QModelIndex &index);
    void setPosition(int position);
    void updateInfo();
    void handleError();
    void playlistPositionChanged(int);
    void volumeClicked();

private:
    void createWidgets();

    QMediaPlayer mediaPlayer;
    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;
    QAbstractButton *volumeButton;
    QSlider *positionSlider;
    QSlider *volumeSlider;
    QLabel *positionLabel;
    QLabel *infoLabel;
    QPoint offset;

    QAbstractItemView *playlistView;
    QMediaPlaylist *playlist;

    PlaylistModel *playlistModel;
    QMediaPlayer::State playerState;

};

#endif // PLAYER_H
