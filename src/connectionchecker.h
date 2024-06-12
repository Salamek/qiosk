#ifndef CONNECTIONCHECKER_H
#define CONNECTIONCHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>

class ConnectionChecker : public QObject {
    Q_OBJECT

public:
    ConnectionChecker(QObject *parent = nullptr);
    ~ConnectionChecker();
    void checkUrlAccessibility(QUrl url);

signals:
    void urlAccessible();

public slots:
    void doCheckRequest();


private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QTimer *timer;
    QUrl url;
    int currentInterval;
    const int initialInterval = 5000; // 5 seconds
    const int maxInterval = 60000;    // 60 seconds
    const int incrementStep = 5000;   // 5 seconds
};

#endif // CONNECTIONCHECKER_H
