#include "resettimer.h"

ResetTimer::ResetTimer(QObject *parent)
    : QObject{parent}
{
    this->timer = new QTimer(this);

    connect(this->timer, &QTimer::timeout, this, [this]() {
        emit timeout();
    });
}

void ResetTimer::setTimeout(int seconds) {
    this->timeoutMs = seconds * 1000;
}

void ResetTimer::start() {
    this->timer->start(this->timeoutMs);
}

void ResetTimer::stop() {
    this->timer->stop();
}


void ResetTimer::reset() {
    this->timer->stop();
    this->timer->start(this->timeoutMs);
}
