//
// Created by ruiy on 18-3-8.
//

#include "Thread.h"

Thread::Thread(QObject *parent) : QThread(parent){
    shutdown_helper = new QSignalMapper;
    shutdown_helper->setMapping(this, 0);
    connect(this, SIGNAL(started()), this, SLOT(setReadyStatus()), Qt::DirectConnection);
    connect(this, SIGNAL(aboutToStop()), shutdown_helper, SLOT(map()));
}

Thread::~Thread(){
    delete shutdown_helper;
}

void Thread::launchWorker(QObject *worker) {
    this->worker = worker;
    start();
    int i=0;
    worker->moveToThread(this);
    shutdown_helper->moveToThread(this);
    connect(shutdown_helper, SIGNAL(mapped(int)), this, SLOT(stopExecutor()), Qt::DirectConnection);
    mutex.lock();
    waitCondition.wait(&mutex);
}

void Thread::stopExecutor() {
    exit();
}

void Thread::setReadyStatus() {
    waitCondition.wakeAll();
}

void Thread::stop(){
    emit aboutToStop();
}