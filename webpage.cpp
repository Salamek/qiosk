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
    QWidget *mainWindow = view()->window();
    if (error.isOverridable()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QWebEngineCertificateError deferredError = error;
        //deferredError.defer();
        if (ignoreCertificateError) {
            deferredError.ignoreCertificateError();
        } else {
            deferredError.rejectCertificate();
        }
        return true;
#else
        return ignoreCertificateError;
#endif
    }
    QMessageBox::critical(mainWindow, tr("Certificate Error"), error.errorDescription());

    return false;
}

void WebPage::handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
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
