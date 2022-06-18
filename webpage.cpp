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
#if !defined(QT_NO_SSL) || QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    connect(this, &QWebEnginePage::selectClientCertificate, this, &WebPage::handleSelectClientCertificate);
#endif
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

void WebPage::setWhiteList(QStringList whiteList) {
    this->whiteListRegexps.clear();
    QString whiteListItem;
    foreach(whiteListItem, whiteList) {
        this->whiteListRegexps << QRegularExpression(Tools::wildcardToRegularExpression(whiteListItem));
    }
}

bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
    bool ignoreCertificateError = true; //@TODO
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
    if (false) {
        /*auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());*/
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

inline QString questionForFeature(QWebEnginePage::Feature feature)
{
    switch (feature) {
    case QWebEnginePage::Geolocation:
        return WebPage::tr("Allow %1 to access your location information?");
    case QWebEnginePage::MediaAudioCapture:
        return WebPage::tr("Allow %1 to access your microphone?");
    case QWebEnginePage::MediaVideoCapture:
        return WebPage::tr("Allow %1 to access your webcam?");
    case QWebEnginePage::MediaAudioVideoCapture:
        return WebPage::tr("Allow %1 to access your microphone and webcam?");
    case QWebEnginePage::MouseLock:
        return WebPage::tr("Allow %1 to lock your mouse cursor?");
    case QWebEnginePage::DesktopVideoCapture:
        return WebPage::tr("Allow %1 to capture video of your desktop?");
    case QWebEnginePage::DesktopAudioVideoCapture:
        return WebPage::tr("Allow %1 to capture audio and video of your desktop?");
    case QWebEnginePage::Notifications:
        return WebPage::tr("Allow %1 to show notification on your desktop?");
    }
    return QString();
}

void WebPage::handleFeaturePermissionRequested(const QUrl &securityOrigin, Feature feature)
{
    QString title = tr("Permission Request");
    QString question = questionForFeature(feature).arg(securityOrigin.host());
    if (!question.isEmpty() && QMessageBox::question(view()->window(), title, question) == QMessageBox::Yes)
        setFeaturePermission(securityOrigin, feature, PermissionGrantedByUser);
    else
        setFeaturePermission(securityOrigin, feature, PermissionDeniedByUser);
}

void WebPage::handleProxyAuthenticationRequired(const QUrl &, QAuthenticator *auth, const QString &proxyHost)
{
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
    auto answer = QMessageBox::question(
        view()->window(),
        tr("Permission Request"),
        tr("Allow %1 to open all %2 links?")
        .arg(request.origin().host())
        .arg(request.scheme()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
//! [registerProtocolHandlerRequested]

#if !defined(QT_NO_SSL) || QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
void WebPage::handleSelectClientCertificate(QWebEngineClientCertificateSelection selection)
{
    // Just select one.
    selection.select(selection.certificates().at(0));
}
#endif
