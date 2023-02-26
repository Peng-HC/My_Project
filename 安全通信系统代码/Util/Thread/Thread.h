//
// Created by ruiy on 18-3-8.
//

#ifndef ROI_THREAD_H
#define ROI_THREAD_H

#include <QtCore>

class Thread :public QThread{
    Q_OBJECT
public:
    Thread(QObject *parent=0);
    ~Thread();
    void stop();
    void launchWorker(QObject *worker);
private:
    QObject *worker;
    QSignalMapper *shutdown_helper;
    QWaitCondition waitCondition;
    QMutex mutex;
private slots:
    void stopExecutor();
    void setReadyStatus();
signals:
    void aboutToStop();
};


#endif //ROI_THREAD_H
