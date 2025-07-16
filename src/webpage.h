#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineFullScreenRequest>
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
        AllowDesktopAudioVideoCapture,
        Unknown
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    void setWhiteList(QStringList whiteList);
    void setPermissions(WebPage::Permissions permissions);
    static WebPage::Permissions namesToWebPagePermissions(QStringList allowPermissionNames);
    static QMap<QString, WebPage::Permission> getPermissionOptionMap();
    static QStringList permissionTonames(WebPage::Permissions permissions);
private:
    bool isUrlInWhiteList(QUrl url);
    QList<QRegularExpression> whiteListRegexps;
    Permissions permissions;
    bool isFeatureAllowed(Feature feature);
protected:
#if QT_VERSION <=QT_VERSION_CHECK(6, 0, 0)
    bool certificateError(const QWebEngineCertificateError &error) override;
#endif
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
private slots:
    void handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
    void handleFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    void handleProxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);
    void handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);
#if !defined(QT_NO_SSL) && QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    void handleSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);
#endif
#if QT_VERSION >=QT_VERSION_CHECK(6, 0, 0)
    bool certificateError(const QWebEngineCertificateError &error);
#endif
    void handleFullScreenRequest(QWebEngineFullScreenRequest request);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(WebPage::Permissions)

#endif // WEBPAGE_H
