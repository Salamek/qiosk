#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QRegularExpression>
#include "tools.h"

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
    void setWhiteList(QStringList whiteList);
private:
    bool isUrlInWhiteList(QUrl url);
    QList<QRegularExpression> whiteListRegexps;
protected:
    bool certificateError(const QWebEngineCertificateError &error) override;
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
private slots:
    void handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
    void handleFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    void handleProxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);
    void handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);
#if !defined(QT_NO_SSL) || QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    void handleSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);
#endif
};

#endif // WEBPAGE_H
