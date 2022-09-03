#include "tools.h"

Tools::Tools(QObject *parent)
    : QObject{parent}
{

}

QString Tools::wildcardToRegularExpression(QString pattern) {
    QString regex;

    int patternLength = pattern.length();

    for (int i = 0; i < patternLength; i++) {
        QChar c = pattern.at(i);

       if (QString("/$^+.()=!|").contains(c)) {
           regex.append("\\");
           regex.append(c);
       } else if (QString("?[]{},").contains(c)) {
           regex.append("\\");
           regex.append(c);
       } else if (QString("*").contains(c)) {
           regex.append(".*");
       } else {
           regex.append(c);
       }
    }
    return regex;
}

