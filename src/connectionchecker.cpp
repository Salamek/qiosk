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
    this->timer->setSingleShot(true);
    connect(this->timer, &QTimer::timeout, this, &ConnectionChecker::doCheckRequest);
}

ConnectionChecker::~ConnectionChecker() {
    // Destructor
}

void ConnectionChecker::doCheckRequest() {
    if (!this->active || this->currentReply) {
        return;
    }

    QNetworkRequest request(this->url);
    this->currentReply = networkManager->get(request);
}


void ConnectionChecker::checkUrlAccessibility(QUrl url) {
    if (this->active && this->url == url) {
        return;
    }

    this->stop();
    this->url = url;
    this->active = true;
    this->connectionFailureObserved = false;
    this->currentInterval = this->initialInterval;
    this->doCheckRequest();
}

void ConnectionChecker::stop() {
    this->active = false;
    this->connectionFailureObserved = false;
    this->timer->stop();
    if (this->currentReply) {
        QNetworkReply *reply = this->currentReply;
        this->currentReply = nullptr;
        reply->abort();
        reply->deleteLater();
    }
}

void ConnectionChecker::handleNetworkReply(QNetworkReply *reply) {
    if (reply != this->currentReply) {
        reply->deleteLater();
        return;
    }

    this->currentReply = nullptr;
    if (!this->active) {
        reply->deleteLater();
        return;
    }

    // Any HTTP response proves that the web server is reachable. Only a
    // connection-level failure should cause another probe.
    const bool serverResponded = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid();
    if (reply->error() == QNetworkReply::NoError || serverResponded) {
        const bool recoveredFromConnectionFailure = this->connectionFailureObserved;
        this->active = false;
        this->connectionFailureObserved = false;
        this->timer->stop();
        this->currentInterval = this->initialInterval;
        if (recoveredFromConnectionFailure) {
            emit urlAccessible();
        } else {
            qDebug() << "Connection test URL is already accessible; not reloading failed page:" << this->url;
        }
    } else {
        this->connectionFailureObserved = true;
        qDebug() << "Connection test URL is not accessible:" << this->url << reply->errorString();
        if (this->currentInterval < this->maxInterval) {
            this->currentInterval = qMin(this->currentInterval + this->incrementStep, this->maxInterval);
        }

        this->timer->start(this->currentInterval); // Adjust the interval
    }

    reply->deleteLater();
}
