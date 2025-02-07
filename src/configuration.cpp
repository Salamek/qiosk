#include "configuration.h"

QStringList Configuration::windowModeOptions = {"hidden", "automaticvisibility", "windowed", "minimized", "maximized", "fullscreen"};

Configuration::Configuration(QObject *parent)
    : QObject{parent}
{
    this->url = QUrl("https://salamek.github.io/chromium-kiosk/");
    this->touchscreen = true;
    this->idleTime = 0;
    this->whiteList = QStringList();
    this->navbarVerticalPosition = BarWidget::VerticalPosition::Bottom;
    this->navbarHorizontalPosition = BarWidget::HorizontalPosition::Center;
    this->windowMode = QWindow::Visibility::AutomaticVisibility;
    this->navbarWidth = 25; //%
    this->navbarHeight = 5; //%
    this->displayAddressBar = false;
    this->displayNavBar = false;
    this->displayScrollBars = false;
    this->underlayNavBar = false;
    this->profileName = "default";
    //this->permissions = WebPage::Permission::AllowGeolocation | WebPage::Permission::AllowInvalidCertificate | WebPage::Permission::AllowNotifications | WebPage::Permission::AllowDesktopVideoCapture;
}


void Configuration::setUrl(QUrl url) {
    this->url = url;
}

void Configuration::setWindowMode(QWindow::Visibility windowMode) {
    this->windowMode = windowMode;
}

void Configuration::setTouchscreen(bool touchscreen) {
    this->touchscreen = touchscreen;
}

void Configuration::setIdleTime(int idleTime) {
    this->idleTime = idleTime;
}

void Configuration::setWhiteList(QStringList whiteList) {
    this->whiteList = whiteList;
}

void Configuration::setNavbarVerticalPosition(BarWidget::VerticalPosition navbarVerticalPosition) {
    this->navbarVerticalPosition = navbarVerticalPosition;
}

void Configuration::setNavbarHorizontalPosition(BarWidget::HorizontalPosition navbarHorizontalPosition) {
    this->navbarHorizontalPosition = navbarHorizontalPosition;
}

void Configuration::setNavbarWidth(int navbarWidth) {
    if (navbarWidth > 100 || navbarWidth < 1) {
        qFatal("Width cant be bigger then 100 or smaller then 1");
    }
    this->navbarWidth = navbarWidth;
}

void Configuration::setNavbarHeight(int navbarHeight) {
    if (navbarHeight > 100 || navbarHeight < 1) {
        qFatal("Height cant be bigger then 100 or smaller then 1");
    }
    this->navbarHeight = navbarHeight;
}

void Configuration::setPermissions(WebPage::Permissions permissions) {
    this->permissions = permissions;
}

void Configuration::setDisplayAddressBar(bool displayAddressBar) {
    this->displayAddressBar = displayAddressBar;
}

void Configuration::setDisplayNavBar(bool displayNavBar) {
    this->displayNavBar = displayNavBar;
}

void Configuration::setDisplayScrollBars(bool displayScrollBars) {
    this->displayScrollBars = displayScrollBars;
}

void Configuration::setUnderlayNavBar(bool underlayNavBar) {
    this->underlayNavBar = underlayNavBar;
}

void Configuration::setProfileName(QString profileName) {
    this->profileName = profileName;
}

void Configuration::setNavbarEnabledButtons(QList<BarWidget::Button> navBarEnabledButtons) {
    this->navBarEnabledButtons = navBarEnabledButtons;
}

void Configuration::setAcceptLanguage(QString acceptLanguage) {
    this->acceptLanguage = acceptLanguage;
}

void Configuration::setUserAgent(QString userAgent) {
    this->userAgent = userAgent;
}


QUrl Configuration::getUrl() {
    return this->url;
}

int Configuration::getNavbarWidth() {
    return this->navbarWidth;
}

int Configuration::getNavbarHeight() {
    return this->navbarHeight;
}

int Configuration::getIdleTime() {
    return this->idleTime;
}

BarWidget::VerticalPosition Configuration::getNavbarVerticalPosition() {
    return this->navbarVerticalPosition;
}

BarWidget::HorizontalPosition Configuration::getNavbarHorizontalPosition() {
    return this->navbarHorizontalPosition;
}

QStringList Configuration::getWhiteList() {
    return this->whiteList;
}

WebPage::Permissions Configuration::getPermissions() {
    return this->permissions;
}

QList<BarWidget::Button> Configuration::getNavBarEnabledButtons() {
    return this->navBarEnabledButtons;
}

bool Configuration::isDisplayAddressBar() {
    return this->displayAddressBar;
}

bool Configuration::isDisplayNavBar() {
    return this->displayNavBar;
}

bool Configuration::isDisplayScrollBars() {
    return this->displayScrollBars;
}

bool Configuration::isUnderlayNavBar() {
    return this->underlayNavBar;
}

QString Configuration::getProfileName() {
    return this->profileName;
}

QString Configuration::getAcceptLanguage() {
    return this->acceptLanguage;
}

QString Configuration::getUserAgent() {
    return this->userAgent;
}

QWindow::Visibility Configuration::getWindowMode() {
    return this->windowMode;
}

QWindow::Visibility Configuration::nameToWindowMode(QString name){
    switch(Configuration::windowModeOptions.indexOf(name)){
    case 0:
        return QWindow::Visibility::Hidden;
        break;

    case 1:
        return QWindow::Visibility::AutomaticVisibility;
        break;

    case 2:
        return QWindow::Visibility::Windowed;
        break;

    case 3:
        return QWindow::Visibility::Minimized;
        break;

    case 4:
        return QWindow::Visibility::Maximized;
        break;

    case 5:
        return QWindow::Visibility::FullScreen;
        break;

    default:
        return QWindow::Visibility::AutomaticVisibility;
        break;
    }
}

QString Configuration::windowModeToName(QWindow::Visibility visibility){
    switch(visibility){
    case QWindow::Visibility::Hidden:
        return Configuration::windowModeOptions.at(0);
        break;

    case QWindow::Visibility::AutomaticVisibility:
        return Configuration::windowModeOptions.at(1);
        break;

    case QWindow::Visibility::Windowed:
        return Configuration::windowModeOptions.at(2);
        break;

    case QWindow::Visibility::Minimized:
        return Configuration::windowModeOptions.at(3);
        break;

    case QWindow::Visibility::Maximized:
        return Configuration::windowModeOptions.at(4);
        break;

    case QWindow::Visibility::FullScreen:
        return Configuration::windowModeOptions.at(5);
        break;

    }

    return Configuration::windowModeOptions.at(1);
}
