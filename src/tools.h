#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include "barwidget.h"

class Tools : public QObject
{
    Q_OBJECT
public:
    explicit Tools(QObject *parent = nullptr);

    static QString wildcardToRegularExpression(QString pattern);
signals:

};

#endif // TOOLS_H
