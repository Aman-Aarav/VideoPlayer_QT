#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new QMediaPlayer(this);

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
    m_audioOutput->setVolume(1.0f); // Default volume (range float: 0.0 to 1.0)

    Player->setAudioOutput(m_audioOutput);


    ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_Seek_Backward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_Seek_Forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(100);
    connect(ui->horizontalSlider_Volume, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);


    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    //QMediaPlayer::durationChanged, Triggered when: The total duration(milliseconds.) of the media changes.// in this code only triggers when new file is selected

    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
   // QMediaPlayer::positionChanged, Triggered when: The current playback position changes . position (qint64): The current playback position in milliseconds.s

    // qint64 is a Qt-specific typedef for a 64-bit signed integer. It is equivalent to long long int in C++.

    ui->horizontalSlider_Duration->setRange(0, Player->duration() / 1000);
    //QMediaPlayer::duration() Returns the duration of the current media in ms. Returns 0 if the media player doesn't have a valid media file or stream. For live streams, the duration usually changes during playback as more data becomes available.

    connect(Player, &QMediaPlayer::errorOccurred,
            this, &MainWindow::onMediaError);

    connect(Player, &QMediaPlayer::playbackStateChanged, this,
            [this](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::StoppedState)  // IF Video completed change icon back to stop
                    ui->pushButton_Play_Pause->setIcon(
                        style()->standardIcon(QStyle::SP_MediaPlay));
            });



    QFile styleFile(":/player.qss"); // If file is in the same directory
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
    }
    connect(Player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::onMediaStatusChanged);
    setControlsEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onMediaError(QMediaPlayer::Error error, const QString &errorString)
{
    Q_UNUSED(error);

    QMessageBox::critical(
        this,
        "Playback Error",
        "Unable to play this media file.\n\nReason:\n" + errorString
        );
}

void MainWindow::onVolumeChanged(int volume) {
    float volumeFraction = static_cast<float>(volume) / 100.0f; // Convert 0-100 to 0.0-1.0
    m_audioOutput->setVolume(volumeFraction);
}

void MainWindow::durationChanged(qint64 duration)
{
    if (duration <= 0)
    {
        QMessageBox::warning(
            this,
            "Invalid Media",
            "This file does not contain playable audio/video."
            );
        return;
    }
    mDuration = duration / 1000;

    ui->horizontalSlider_Duration->setMaximum(mDuration);
}

void MainWindow::positionChanged(qint64 duration)
{
    if (!ui->horizontalSlider_Duration->isSliderDown())
    {
        ui->horizontalSlider_Duration->blockSignals(true); //blockSignals(true) prevents setValue() from triggering valueChanged().
                                                           //   Ensures that setPosition() is not called unnecessarily.
        ui->horizontalSlider_Duration->setValue(duration / 1000);
        ui->horizontalSlider_Duration->blockSignals(false);
    }
    QCoreApplication::processEvents();
    updateDuration(duration / 1000);
}

void MainWindow::updateDuration(qint64 Duration)
{
    if (Duration || mDuration)
    {
        QTime CurrentTime((Duration / 3600) % 60, (Duration / 60) % 60, Duration % 60, (Duration * 1000) % 1000);
        QTime TotalTime((mDuration / 3600) % 60, (mDuration / 60) % 60, mDuration % 60, (mDuration * 1000) % 1000);
        QString Format ="";
        if (mDuration > 3600) Format = "hh:mm:ss";
        else Format = "mm:ss";

        ui->label_current_Time->setText(CurrentTime.toString(Format));
        ui->label_Total_Time->setText(TotalTime.toString(Format));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("Video Files (*.mp4 *.avi *.mkv *.mov);;All Files (*.*)""MP4 Files (*.mp4)")); //getOpenFileName for selecting one file
                                   //getOpenFileNames for selecting multiplr files and QStringList is required
    if (FileName.isEmpty()) return; // Avoid processing if no file is selected

    QFileInfo fileInfo(FileName);
    // 2️if  File does not exist
    if (!fileInfo.exists())
    {
        QMessageBox::warning(this, "File Error",
                             "The selected file does not exist.");
        return;
    }

    // Cleanup old video widget
    if (Video)  // Delete the existing video widget
    {
        delete Video;
        Video = nullptr;
    }
    Video = new QVideoWidget();
    QVBoxLayout *Layout = new QVBoxLayout();
    ui->groupBox_Video->setLayout(Layout);
    Layout->addWidget(Video);


    Video->setParent(ui->groupBox_Video);

    Player->setVideoOutput(Video);

    Player->setSource(QUrl(FileName));



    Video->show();
}


void MainWindow::on_horizontalSlider_Duration_valueChanged(int value)
{
    Player->setPosition(value * 1000);
}


void MainWindow::on_pushButton_Play_Pause_clicked()
{
    if (Player->playbackState() == QMediaPlayer::PlayingState)
    {
        Player->pause();
        ui->pushButton_Play_Pause->setIcon(
            style()->standardIcon(QStyle::SP_MediaPlay));
    }
    else
    {
        Player->play();
        ui->pushButton_Play_Pause->setIcon(
            style()->standardIcon(QStyle::SP_MediaPause));
    }
}


void MainWindow::on_pushButton_Stop_clicked()
{
    Player->stop();

}


void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
         m_audioOutput->setMuted(true);
    }
    else
    {
        IS_Muted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
         m_audioOutput->setMuted(false);
    }
}


void MainWindow::on_horizontalSlider_Volume_valueChanged(int value)
{
    //Player->setVolume(value);
}


void MainWindow::on_pushButton_Seek_Backward_clicked()
{
    ui->horizontalSlider_Duration->setValue(ui->horizontalSlider_Duration->value() - 20);
    Player->setPosition(ui->horizontalSlider_Duration->value() * 1000);
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    ui->horizontalSlider_Duration->setValue(ui->horizontalSlider_Duration->value() + 20);
    Player->setPosition(ui->horizontalSlider_Duration->value() * 1000);
}
void MainWindow::setControlsEnabled(bool enabled)
{
    ui->pushButton_Play_Pause->setEnabled(enabled);
    ui->pushButton_Stop->setEnabled(enabled);
    ui->pushButton_Seek_Backward->setEnabled(enabled);
    ui->pushButton_Seek_Forward->setEnabled(enabled);
    ui->pushButton_Volume->setEnabled(enabled);

    ui->horizontalSlider_Duration->setEnabled(enabled);
    ui->horizontalSlider_Volume->setEnabled(enabled);
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::LoadedMedia:
         setControlsEnabled(true);
         break;

    case QMediaPlayer::InvalidMedia:
    case QMediaPlayer::NoMedia:
         setControlsEnabled(false);
         break;

    default:
         break;
    }
}
