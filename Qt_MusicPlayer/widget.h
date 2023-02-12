#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUrl>

class QMediaPlayer;
class QAudioOutput;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_openDir_pushButton_clicked();

    void on_play_pushButton_clicked();

    void on_previous_pushButton_clicked();

    void on_next_pushButton_clicked();

    void on_musicListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::Widget *ui;
    //播放列表(存放播放地址)
    QList<QUrl> playList;

    QAudioOutput* audioOutput;
    QMediaPlayer* mediaPlayer;
    //当前播放下标
    int curPlayIndex;
    //媒体总时长
    qint64 musicDuration;


};
#endif // WIDGET_H
