#include "widget.h"
#include <QIcon>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon("../Qt_MusicPlayer/assets/musicIcon.png");
    Widget w;
    w.setWindowTitle("PHC的音乐播放器");
    w.setWindowIcon(icon);
    w.show();
    return a.exec();
}
