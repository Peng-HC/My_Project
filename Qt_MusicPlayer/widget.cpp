#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMessageBox>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //构造函数
    ui->setupUi(this);
    setStyleSheet("border-image:url(:/assets/background.png);");

    //如何播放音乐
    //1.先加载multimedia模块:在工程文件Qt_MusicPlayer.pro中添加multimedia
    //2.new一个音频播放对象
    audioOutput = new QAudioOutput(this);
    //3.new一个媒体播放对象
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);

    //4.给播放器设置音乐源
//    mediaPlayer->setSource(
//    QUrl::fromLocalFile(playList[1]);
//    mediaPlayer->play();
    //获取媒体总时长，通过信号、槽机制
    //信号发送方,信号,信号接收方,槽
    connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration) {
        ui->totalTimeLabel->setText(QString("%1:%2")
                    .arg(duration/1000/60,2,10,QChar('0'))
                    .arg(duration/1000%10,2,10,QChar('0')));
        musicDuration=duration;
        //滑块
        ui->timeSlider->setRange(0,duration);
    });
    //获取媒体当前时长，实时更新
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 position) {
        ui->currTimeLabel->setText(QString("%1:%2")
                .arg(position/1000/60,2,10,QChar('0'))
                .arg(position/1000%10,2,10,QChar('0')));
        //滑块
        ui->timeSlider->setValue(position);
    });

    //拖动滑块，改变音乐播放的进度
    connect(ui->timeSlider,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);
}

Widget::~Widget()
{
    delete ui;
}

//打开文件夹对话框
void Widget::on_openDir_pushButton_clicked()
{
    qInfo()<<"打开文件夹对话框";
    //打开文件对话框，让用户选择选择音乐所在的目录
    //默认打开的目录：../Qt_MusicPlayer/assets/music
    QString path = QFileDialog::getExistingDirectory(this,"请选择音乐所在的目录","../Qt_MusicPlayer/assets/music");
    //qInfo()<<path;
    QDir dir(path);
    //根据拿到的路径，获取里面所有的mp3,wav格式的文件
    QStringList musicList = dir.entryList(QStringList()<<"*.mp3"<<"*.wav");
    //qInfo()<<musicList;
    //把音乐名字放到列表上展示
    ui->musicListWidget->addItems(musicList);
    //默认选中列表的第一个
    ui->musicListWidget->setCurrentRow(0);
    //把音乐的完整路径保存起来
    for(auto musicFileURL : musicList) {
        playList.append(QUrl::fromLocalFile(path+"/"+musicFileURL));
    }
//    qInfo()<<playList;
}

//播放或暂停音乐功能
void Widget::on_play_pushButton_clicked()
{
    qInfo()<<"播放或暂停音乐";
    //未添加音乐
    if(playList.empty()) {
        //弹窗告警
        QMessageBox::warning(this,"警告","请添加需要播放的音乐！");
        return;
    }
    switch(mediaPlayer->playbackState()) {
    case QMediaPlayer::PlaybackState::StoppedState: {//停止状态
//        qInfo()<<"停止状态"<<QMediaPlayer::PlaybackState::StoppedState;
        //1.获取选中的行号
        curPlayIndex = ui->musicListWidget->currentRow();
//        qInfo()<<"curPlayIndex="<<curPlayIndex;
        //2.播放对应下标的音乐
        mediaPlayer->setSource(playList[curPlayIndex]);
        mediaPlayer->play();
        break;
    }
    case QMediaPlayer::PlaybackState::PlayingState:{//播放状态
//        qInfo()<<"播放状态"<<QMediaPlayer::PlaybackState::PlayingState;
        mediaPlayer->pause();
        break;
    }
    case QMediaPlayer::PlaybackState::PausedState: {//暂停状态
//        qInfo()<<"暂停状态"<<QMediaPlayer::PlaybackState::PausedState;
        mediaPlayer->play();
        break;
    }
    }
}

//上一曲
void Widget::on_previous_pushButton_clicked()
{
    qInfo()<<"上一曲";
    //未添加音乐
    if(playList.empty()) {
        //弹窗告警
        QMessageBox::warning(this,"警告","请添加需要播放的音乐！");
        return;
    }
    if(0==curPlayIndex) {
        curPlayIndex=playList.size()-1;
    } else {
        curPlayIndex--;
    }
    ui->musicListWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource((playList[curPlayIndex]));
    mediaPlayer->play();
}

//下一曲
void Widget::on_next_pushButton_clicked()
{
    qInfo()<<"下一曲";
    //未添加音乐
    if(playList.empty()) {
        //弹窗告警
        QMessageBox::warning(this,"警告","请添加需要播放的音乐！");
        return;
    }
    curPlayIndex=(curPlayIndex+1)%playList.size();
    ui->musicListWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource((playList[curPlayIndex]));
    mediaPlayer->play();
}


//用户双击播放音乐
void Widget::on_musicListWidget_doubleClicked(const QModelIndex &index)
{
    qInfo()<<"双击播放音乐";
    curPlayIndex = index.row();
    ui->musicListWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
}

