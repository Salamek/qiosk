#ifndef USERINPUTEVENTFILTER_H
#define USERINPUTEVENTFILTER_H

#include <QObject>
#include <QEvent>

class UserInputEventFilter : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
signals:
    void userActivity();
};

#endif // USERINPUTEVENTFILTER_H
