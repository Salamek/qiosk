#include "connectionchecker.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

ConnectionChecker::ConnectionChecker(QObject *parent) : QObject(parent) {
    this->currentInterval = this->initialInterval;
    this->networkManager = new QNetworkAccessManager(this);
    connect(this->networkManager, &QNetworkAccessManager::finished, this, &ConnectionChecker::handleNetworkReply);

    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &ConnectionChecker::doCheckRequest);
}

ConnectionChecker::~ConnectionChecker() {
    // Destructor
}

void ConnectionChecker::doCheckRequest() {
    QNetworkRequest request(this->url);
    networkManager->get(request);
}


void ConnectionChecker::checkUrlAccessibility(QUrl url) {
    this->url = url;
    this->doCheckRequest();
}

void ConnectionChecker::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        emit urlAccessible();
        this->currentInterval = this->initialInterval; // Reset interval on success
    } else {
        qDebug() << "URL is not accessible.";
        if (this->currentInterval < this->maxInterval) {
            this->currentInterval = qMin(this->currentInterval + this->incrementStep, this->maxInterval);
        }
    }
    timer->start(this->currentInterval); // Adjust the interval
    reply->deleteLater();
}
