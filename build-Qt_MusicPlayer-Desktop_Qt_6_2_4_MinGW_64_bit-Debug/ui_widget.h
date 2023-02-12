/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *openDir_pushButton;
    QPushButton *musicList_pushButton;
    QPushButton *previous_pushButton;
    QPushButton *play_pushButton;
    QPushButton *next_pushButton;
    QPushButton *volume_pushButton;
    QListWidget *musicListWidget;
    QSlider *timeSlider;
    QLabel *currTimeLabel;
    QLabel *totalTimeLabel;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(320, 410);
        Widget->setStyleSheet(QString::fromUtf8("QWidget{\n"
"}\n"
"QListWidget{\n"
"	background-image: url(:/assets/background.png);\n"
"}\n"
"QPushButton {\n"
"	border:none;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color:rgb(0,255,0);\n"
"}"));
        widget = new QWidget(Widget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 348, 281, 44));
        widget->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        openDir_pushButton = new QPushButton(widget);
        openDir_pushButton->setObjectName(QString::fromUtf8("openDir_pushButton"));
        openDir_pushButton->setMinimumSize(QSize(32, 32));
        openDir_pushButton->setMaximumSize(QSize(32, 32));
        openDir_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/openDir.png);"));

        horizontalLayout->addWidget(openDir_pushButton);

        musicList_pushButton = new QPushButton(widget);
        musicList_pushButton->setObjectName(QString::fromUtf8("musicList_pushButton"));
        musicList_pushButton->setMinimumSize(QSize(32, 32));
        musicList_pushButton->setMaximumSize(QSize(32, 32));
        musicList_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/musicLists.png);"));

        horizontalLayout->addWidget(musicList_pushButton);

        previous_pushButton = new QPushButton(widget);
        previous_pushButton->setObjectName(QString::fromUtf8("previous_pushButton"));
        previous_pushButton->setMinimumSize(QSize(32, 32));
        previous_pushButton->setMaximumSize(QSize(32, 32));
        previous_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/previous.png);"));

        horizontalLayout->addWidget(previous_pushButton);

        play_pushButton = new QPushButton(widget);
        play_pushButton->setObjectName(QString::fromUtf8("play_pushButton"));
        play_pushButton->setMinimumSize(QSize(32, 32));
        play_pushButton->setMaximumSize(QSize(32, 32));
        play_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/play.png);"));

        horizontalLayout->addWidget(play_pushButton);

        next_pushButton = new QPushButton(widget);
        next_pushButton->setObjectName(QString::fromUtf8("next_pushButton"));
        next_pushButton->setMinimumSize(QSize(32, 32));
        next_pushButton->setMaximumSize(QSize(32, 32));
        next_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/next.png);"));

        horizontalLayout->addWidget(next_pushButton);

        volume_pushButton = new QPushButton(widget);
        volume_pushButton->setObjectName(QString::fromUtf8("volume_pushButton"));
        volume_pushButton->setMinimumSize(QSize(32, 32));
        volume_pushButton->setMaximumSize(QSize(32, 32));
        volume_pushButton->setStyleSheet(QString::fromUtf8("icon: url(:/assets/volume.png);"));

        horizontalLayout->addWidget(volume_pushButton);

        musicListWidget = new QListWidget(Widget);
        musicListWidget->setObjectName(QString::fromUtf8("musicListWidget"));
        musicListWidget->setGeometry(QRect(0, 0, 321, 301));
        musicListWidget->setStyleSheet(QString::fromUtf8(""));
        timeSlider = new QSlider(Widget);
        timeSlider->setObjectName(QString::fromUtf8("timeSlider"));
        timeSlider->setGeometry(QRect(20, 328, 271, 16));
        timeSlider->setOrientation(Qt::Horizontal);
        currTimeLabel = new QLabel(Widget);
        currTimeLabel->setObjectName(QString::fromUtf8("currTimeLabel"));
        currTimeLabel->setGeometry(QRect(10, 308, 40, 12));
        totalTimeLabel = new QLabel(Widget);
        totalTimeLabel->setObjectName(QString::fromUtf8("totalTimeLabel"));
        totalTimeLabel->setGeometry(QRect(260, 308, 40, 12));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        openDir_pushButton->setText(QString());
        musicList_pushButton->setText(QString());
        previous_pushButton->setText(QString());
        play_pushButton->setText(QString());
        next_pushButton->setText(QString());
        volume_pushButton->setText(QString());
        currTimeLabel->setText(QCoreApplication::translate("Widget", "00\357\274\23200", nullptr));
        totalTimeLabel->setText(QCoreApplication::translate("Widget", "00\357\274\23200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
