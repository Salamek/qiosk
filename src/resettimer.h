#ifndef RESETTIMER_H
#define RESETTIMER_H

#include <QObject>
#include <QTimer>

class ResetTimer : public QObject
{
    Q_OBJECT
public:
    explicit ResetTimer(QObject *parent = nullptr);
    QTimer *timer;

    void start();
    void reset();
    void setTimeout(int seconds);
    void stop();
private:
    int timeoutMs;
signals:
    void timeout();


};

#endif // RESETTIMER_H
