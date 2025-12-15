#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QAudioOutput>
#include <QMediaDevices>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:    
    void durationChanged(qint64 duration);
    void positionChanged(qint64 duration);
    void onVolumeChanged(int volume);
    void on_actionOpen_triggered();

    void on_horizontalSlider_Duration_valueChanged(int value);

    void on_pushButton_Play_Pause_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_Volume_clicked();

    void on_horizontalSlider_Volume_valueChanged(int value);

    void on_pushButton_Seek_Backward_clicked();

    void on_pushButton_Seek_Forward_clicked();
    void onMediaError(QMediaPlayer::Error error, const QString &errorString);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);


private:
    Ui::MainWindow *ui;
    QMediaPlayer *Player;
    QVideoWidget *Video;
    qint64 mDuration;
    QAudioOutput *m_audioOutput;
    bool IS_Muted = false;
     void setControlsEnabled(bool enabled);
    void updateDuration(qint64 Duration);
};
#endif // MAINWINDOW_H
