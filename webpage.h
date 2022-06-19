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

    enum Permission {
        NoPermission,
        AllowInvalidCertificate,
        AllowNotifications,
        AllowGeolocation,
        AllowMediaAudioCapture,
        AllowMediaVideoCapture,
        AllowMediaAudioVideoCapture,
        AllowMouseLock,
        AllowDesktopVideoCapture,
        AllowDesktopAudioVideoCapture
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    void setWhiteList(QStringList whiteList);
    void setPermissions(WebPage::Permissions permissions);
private:
    bool isUrlInWhiteList(QUrl url);
    QList<QRegularExpression> whiteListRegexps;
    Permissions permissions;
    bool isFeatureAllowed(Feature feature);
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

Q_DECLARE_OPERATORS_FOR_FLAGS(WebPage::Permissions)

#endif // WEBPAGE_H
