#include "mainwindow.h"
#include "webpage.h"
#include "webview.h"
#include <QAuthenticator>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>
#include <QWebEngineCertificateError>

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
    connect(this, &QWebEnginePage::authenticationRequired, this, &WebPage::handleAuthenticationRequired);
    connect(this, &QWebEnginePage::featurePermissionRequested, this, &WebPage::handleFeaturePermissionRequested);
    connect(this, &QWebEnginePage::proxyAuthenticationRequired, this, &WebPage::handleProxyAuthenticationRequired);
    connect(this, &QWebEnginePage::registerProtocolHandlerRequested, this, &WebPage::handleRegisterProtocolHandlerRequested);
#if !defined(QT_NO_SSL) && QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    connect(this, &QWebEnginePage::selectClientCertificate, this, &WebPage::handleSelectClientCertificate);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(this, &QWebEnginePage::certificateError, this, &WebPage::certificateError);
#endif
}

void WebPage::setWhiteList(QStringList whiteList) {
    this->whiteListRegexps.clear();
    QString whiteListItem;
    foreach(whiteListItem, whiteList) {
        this->whiteListRegexps << QRegularExpression(Tools::wildcardToRegularExpression(whiteListItem));
    }
}

void WebPage::setPermissions(WebPage::Permissions permissions) {
    this->permissions = permissions;
}

bool WebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    Q_UNUSED(type);
    Q_UNUSED(isMainFrame);
    if (this->whiteListRegexps.count() > 0) {
        return this->isUrlInWhiteList(url);
    }

    return true; // Allow URL
}

bool WebPage::isUrlInWhiteList(QUrl url) {
    QRegularExpression whiteListRegex;
    foreach(whiteListRegex, this->whiteListRegexps) {
        QRegularExpressionMatch result = whiteListRegex.match(url.toString());
        if (result.hasMatch()) {
            return true;
        }
    }

    return false;
}


bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
    bool ignoreCertificateError = this->permissions.testFlag(Permission::AllowInvalidCertificate);

    //QWidget *mainWindow = this->parent();
    if (error.isOverridable()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QWebEngineCertificateError deferredError = error;
        //deferredError.defer();
        if (ignoreCertificateError) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            deferredError.ignoreCertificateError();
#else
            deferredError.acceptCertificate();
#endif
        } else {
            deferredError.rejectCertificate();
        }
        return true;
#else
        return ignoreCertificateError;
#endif
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    //QMessageBox::critical(mainWindow, tr("Certificate Error"), error.errorDescription());
#else
    //QMessageBox::critical(mainWindow, tr("Certificate Error"), error.description());
#endif

    return false;
}

void WebPage::handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
    Q_UNUSED(requestUrl);
    //@TODO provide option for HTTP auth
    if (false) {
        /*auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());*/
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

bool WebPage::isFeatureAllowed(Feature feature) {
    switch (feature) {
    case QWebEnginePage::Geolocation:
        return this->permissions.testFlag(Permission::AllowGeolocation);
    case QWebEnginePage::MediaAudioCapture:
        return this->permissions.testFlag(Permission::AllowMediaAudioCapture);
    case QWebEnginePage::MediaVideoCapture:
        return this->permissions.testFlag(Permission::AllowMediaVideoCapture);
    case QWebEnginePage::MediaAudioVideoCapture:
        return this->permissions.testFlag(Permission::AllowMediaAudioVideoCapture);
    case QWebEnginePage::MouseLock:
        return this->permissions.testFlag(Permission::AllowMouseLock);
    case QWebEnginePage::DesktopVideoCapture:
        return this->permissions.testFlag(Permission::AllowDesktopVideoCapture);
    case QWebEnginePage::DesktopAudioVideoCapture:
        return this->permissions.testFlag(Permission::AllowDesktopAudioVideoCapture);
    case QWebEnginePage::Notifications:
        return this->permissions.testFlag(Permission::AllowNotifications);
    }

    return false;
}

void WebPage::handleFeaturePermissionRequested(const QUrl &securityOrigin, Feature feature)
{
    bool allowed = this->isFeatureAllowed(feature);
    if (allowed) {
        setFeaturePermission(securityOrigin, feature, PermissionGrantedByUser);
    } else {
        setFeaturePermission(securityOrigin, feature, PermissionDeniedByUser);
    }
}

void WebPage::handleProxyAuthenticationRequired(const QUrl &, QAuthenticator *auth, const QString &proxyHost)
{
    Q_UNUSED(proxyHost);
    //@TODO provide option for proxy auth
    if (false) {
        /*auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());*/
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

//! [registerProtocolHandlerRequested]
void WebPage::handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request)
{
    request.reject();
}
//! [registerProtocolHandlerRequested]

#if !defined(QT_NO_SSL) && QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
void WebPage::handleSelectClientCertificate(QWebEngineClientCertificateSelection selection)
{
    // Just select one.
    selection.select(selection.certificates().at(0));
}
#endif

QMap<QString, WebPage::Permission> WebPage::getPermissionOptionMap() {
    QMap<QString, WebPage::Permission> permissionOptionsMap;
    permissionOptionsMap.insert("invalid-certificate", WebPage::Permission::AllowInvalidCertificate);
    permissionOptionsMap.insert("notifications", WebPage::Permission::AllowNotifications);
    permissionOptionsMap.insert("geolocation", WebPage::Permission::AllowGeolocation);
    permissionOptionsMap.insert("media-audio-capture", WebPage::Permission::AllowMediaAudioCapture);
    permissionOptionsMap.insert("media-video-capture", WebPage::Permission::AllowMediaVideoCapture);
    permissionOptionsMap.insert("media-audio-video-capture", WebPage::Permission::AllowMediaAudioVideoCapture);
    permissionOptionsMap.insert("mouse-lock", WebPage::Permission::AllowMouseLock);
    permissionOptionsMap.insert("desktop-video-capture", WebPage::Permission::AllowDesktopVideoCapture);
    permissionOptionsMap.insert("desktop-audio-video-capture", WebPage::Permission::AllowDesktopAudioVideoCapture);
    return permissionOptionsMap;
}


WebPage::Permissions WebPage::namesToWebPagePermissions(QStringList allowPermissionNames){
    QMap<QString, WebPage::Permission>permissionOptionsMap = WebPage::getPermissionOptionMap();
    QString allowFeature;
    WebPage::Permissions permissions;
    foreach(allowFeature, allowPermissionNames) {
        if (permissionOptionsMap.contains(allowFeature)) {
            permissions |= permissionOptionsMap.value(allowFeature);
        } else {
            permissions |= WebPage::Permission::Unknown;
            break;
        }
    }

    return permissions;
}

QStringList WebPage::permissionTonames(WebPage::Permissions permissions){
    QMap<QString, WebPage::Permission>permissionOptionsMap = WebPage::getPermissionOptionMap();

    QStringList permissionNames;
    WebPage::Permission permission;
    foreach(permission, permissionOptionsMap.values()) {
        if (permissions.testFlag(permission)) {
            permissionNames << permissionOptionsMap.key(permission);
        }
    }

    return permissionNames;
}
