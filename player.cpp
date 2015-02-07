#include "player.h"
#include "playlist.h"

#include <QtWidgets>
#include <QMediaService>
#include <QMediaMetaData>
#include <QMediaPlaylist>
#include <QDir>
#include <QAbstractItemView>
#include <QTableWidget>

player::player(QWidget *parent) : QWidget(parent)
{
    createWidgets();
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updatePosition(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateDuration(qint64)));
    connect(&mediaPlayer, SIGNAL(metaDataAvailableChanged(bool)), this, SLOT(updateInfo()));
    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(updateState(QMediaPlayer::State)));
    connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));

}

player::~player()
{
    //delete ui;
}

void player::openFile()
{
    QString directory = QFileDialog::getExistingDirectory(this,tr("Select dir for files to import"));
    //QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 //"/home",
                                                 //QFileDialog::ShowDirsOnly
                                                 //| QFileDialog::DontResolveSymlinks);
    if(directory.isEmpty())
        return;
    QDir dir(directory);
    QStringList files = dir.entryList(QStringList() << "*",QDir::Files);
    QList<QMediaContent> content;
    for(const QString& f:files)
    {
        content.push_back(QUrl::fromLocalFile(dir.path() + "/" + f));
    }
    playlist->addMedia(content);
    playButton->setEnabled(true);

}

void player::remove()
{
    playlist->clear();
}

void player::togglePlayback()
{
    if (mediaPlayer.state() == QMediaPlayer::PlayingState)
        mediaPlayer.pause();
    else
        mediaPlayer.play();
}

void player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        playlist->setCurrentIndex(index.row());
        mediaPlayer.play();
    }
}

void player::stopClicked()
{
    mediaPlayer.stop();
}

void player::previousClicked()
{
    if(mediaPlayer.position() <= 5000)
        playlist->previous();

    else
        mediaPlayer.setPosition(0);
}

void player::nextClicked()
{
    playlist->next();
}

void player::volumeClicked()
{
    if (mediaPlayer.isMuted())
    {
    mediaPlayer.setMuted(false);
    volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    volumeSlider->setEnabled(true);
    } else {
        mediaPlayer.setMuted(true);
        volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        volumeSlider->setEnabled(false);
    }
}

void player::volumeChanged(int volume)
{
    mediaPlayer.setVolume(volume);
}

void player::seekForward()
{
    positionSlider->triggerAction(QSlider::SliderPageStepAdd);
}

void player::seekBackward()
{
    positionSlider->triggerAction(QSlider::SliderPageStepSub);
}

void player::updateState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
        playButton->setToolTip(tr("Pause"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        playButton->setToolTip(tr("Play"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void player::updatePosition(qint64 position)
{
    positionSlider->setValue(position);

    QTime duration(0, position / 60000, qRound((position % 60000) / 1000.0));
    positionLabel->setText(duration.toString(tr("mm:ss")));
}

void player::updateDuration(qint64 duration)
{
    positionSlider->setRange(0, duration);
    positionSlider->setEnabled(duration > 0);
    positionSlider->setPageStep(duration / 10);
}

void player::setPosition(int position)
{
    if (qAbs(mediaPlayer.position() - position) > 99)
        mediaPlayer.setPosition(position);
}

void player::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void player::updateInfo()
{
    QStringList info;
    QString author = mediaPlayer.metaData("Author").toString();
    if (!author.isEmpty())
        info += author;
    QString title = mediaPlayer.metaData("Title").toString();
    if (!title.isEmpty())
        info += title;
    if (!info.isEmpty())
        infoLabel->setText(info.join(tr(" - ")));
}

void player::handleError()
{
    playButton->setEnabled(false);
    infoLabel->setText(tr("Error: %1").arg(mediaPlayer.errorString()));
}

void player::createWidgets()
{
    playButton = new QToolButton(this);
    playButton->setEnabled(false);
    playButton->setToolTip("Play the music..");
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton, SIGNAL(clicked()), this, SLOT(togglePlayback()));

    stopButton = new QToolButton(this);
    stopButton->setToolTip("Stop the music..");
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopClicked()));

    nextButton = new QToolButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));

    previousButton = new QToolButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(previousButton, SIGNAL(clicked()), this, SLOT(previousClicked()));

    volumeButton = new QToolButton(this);
    volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(volumeButton, SIGNAL(clicked()), this, SLOT(volumeClicked()));

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0,100);
    volumeSlider->setSliderPosition(100);
    volumeSlider->hasTracking();
    volumeSlider->setToolTip(tr("Volume"));
    connect(volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChanged(int)));

    QAbstractButton *openButton = new QToolButton(this);
    openButton->setText(tr("Open Files"));
    openButton->setToolTip(tr("Open a file..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    QAbstractButton *removeButton = new QToolButton(this);
    removeButton->setText(tr("Remove All Files"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setEnabled(false);
    positionSlider->setToolTip(tr("Seek"));
    connect(positionSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));

    infoLabel = new QLabel(this);
    infoLabel->setMinimumWidth(600);

    QFont f("Arial", 12, QFont::Normal);
    infoLabel->setFont(f);
    positionLabel = new QLabel(tr("00:00"), this);
    positionLabel->setMinimumWidth(positionLabel->sizeHint().width());

    playlist = new QMediaPlaylist();
    mediaPlayer.setPlaylist(playlist);
    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);

    playlistView = new QListView(this);
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));
    playlistView->setMinimumHeight(300);
    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    QBoxLayout *playlistLayout = new QHBoxLayout;
    playlistLayout->addWidget(playlistView);


    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(removeButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(previousButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(nextButton);
    controlLayout->addWidget(volumeButton);
    controlLayout->addWidget(volumeSlider);
    controlLayout->addSpacing(300);
    //controlLayout->addWidget(positionSlider);
    //controlLayout->addWidget(positionLabel);
    QBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(positionSlider);
    sliderLayout->addWidget(positionLabel);


    QBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(playlistLayout);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addLayout(controlLayout);
}































