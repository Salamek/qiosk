#include "configuration.h"
#include <QRegularExpression>
#include <QSettings>

QStringList Configuration::windowModeOptions = {"hidden", "automaticvisibility", "windowed", "minimized", "maximized", "fullscreen"};

QUrl Configuration::normalizeUrl(QUrl url) {
    const QString value = url.toString();
    if (value.startsWith(QStringLiteral("//"))) {
        url.setScheme(QStringLiteral("https"));
        return url;
    }

    // QUrl interprets values such as "localhost:8080" as having a
    // "localhost" scheme. Recognize the host:port form before trusting it.
    static const QRegularExpression hostWithPort(
        QStringLiteral("^[^:/?#]+:[0-9]+(?:[/?#]|$)"));
    if (!url.scheme().isEmpty() && !hostWithPort.match(value).hasMatch()) {
        return url;
    }

    return QUrl(QStringLiteral("https://") + value);
}

bool Configuration::parseWindowPosition(QString value, QPoint *position) {
    if (!position) {
        return false;
    }

    static const QRegularExpression expression(
        QStringLiteral("^\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*$"));
    const QRegularExpressionMatch match = expression.match(value);
    if (!match.hasMatch()) {
        return false;
    }

    bool xValid = false;
    bool yValid = false;
    const int x = match.captured(1).toInt(&xValid);
    const int y = match.captured(2).toInt(&yValid);
    if (!xValid || !yValid) {
        return false;
    }

    *position = QPoint(x, y);
    return true;
}

bool Configuration::parseWindowSize(QString value, QSize *size) {
    if (!size) {
        return false;
    }

    static const QRegularExpression expression(
        QStringLiteral("^\\s*([1-9]\\d*)\\s*[xX]\\s*([1-9]\\d*)\\s*$"));
    const QRegularExpressionMatch match = expression.match(value);
    if (!match.hasMatch()) {
        return false;
    }

    bool widthValid = false;
    bool heightValid = false;
    const int width = match.captured(1).toInt(&widthValid);
    const int height = match.captured(2).toInt(&heightValid);
    if (!widthValid || !heightValid) {
        return false;
    }

    *size = QSize(width, height);
    return true;
}

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
    this->windowTitle = QStringLiteral("qiosk");
    this->windowIcon = QStringLiteral(":/icon.png");
    this->windowPositionSet = false;
    this->navbarWidth = 100; //%
    this->navbarHeight = 5; //%
    this->displayAddressBar = false;
    this->displayNavBar = false;
    this->displayScrollBars = false;
    this->underlayNavBar = false;
    this->profileName = "default";
    this->permissions = WebPage::Permissions();
    this->navBarEnabledButtons = {
        BarWidget::Button::Back,
        BarWidget::Button::Forward,
        BarWidget::Button::Reload,
        BarWidget::Button::Home
    };
    this->hideCursor = false;
}


void Configuration::setUrl(QUrl url) {
    this->url = Configuration::normalizeUrl(url);
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

void Configuration::setConnectionTestUrl(QUrl connectionTestUrl) {
    this->connectionTestUrl = Configuration::normalizeUrl(connectionTestUrl);
}

void Configuration::setHideCursor(bool hideCursor) {
    this->hideCursor = hideCursor;
}

void Configuration::setWindowTitle(QString windowTitle) {
    this->windowTitle = windowTitle;
}

void Configuration::setWindowIcon(QString windowIcon) {
    this->windowIcon = windowIcon;
}

void Configuration::setWindowPosition(QPoint windowPosition) {
    this->windowPosition = windowPosition;
    this->windowPositionSet = true;
}

void Configuration::setWindowSize(QSize windowSize) {
    this->windowSize = windowSize;
}

bool Configuration::loadFromFile(QString fileName, QString *errorMessage) {
    QSettings settings(fileName, QSettings::IniFormat);

    if (settings.status() != QSettings::NoError) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Unable to read configuration file %1").arg(fileName);
        }
        return false;
    }

    auto fail = [errorMessage, &fileName](const QString &message) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1 in %2").arg(message, fileName);
        }
        return false;
    };

    if (settings.contains("Browser/url"))
        this->setUrl(settings.value("Browser/url").toString());
    if (settings.contains("Browser/idle"))
        this->setIdleTime(settings.value("Browser/idle").toInt());
    if (settings.contains("Navbar/width")) {
        const int value = settings.value("Navbar/width").toInt();
        if (value < 1 || value > 100)
            return fail(QStringLiteral("Navbar/width must be between 1 and 100"));
        this->setNavbarWidth(value);
    }
    if (settings.contains("Navbar/height")) {
        const int value = settings.value("Navbar/height").toInt();
        if (value < 1 || value > 100)
            return fail(QStringLiteral("Navbar/height must be between 1 and 100"));
        this->setNavbarHeight(value);
    }
    if (settings.contains("Navbar/display"))
        this->setDisplayNavBar(settings.value("Navbar/display").toBool());
    if (settings.contains("Page/display-addressbar"))
        this->setDisplayAddressBar(settings.value("Page/display-addressbar").toBool());
    if (settings.contains("Page/display-scroll-bars"))
        this->setDisplayScrollBars(settings.value("Page/display-scroll-bars").toBool());
    if (settings.contains("Navbar/underlay"))
        this->setUnderlayNavBar(settings.value("Navbar/underlay").toBool());
    if (settings.contains("Browser/profile-name"))
        this->setProfileName(settings.value("Browser/profile-name").toString());
    if (settings.contains("Page/accept-language"))
        this->setAcceptLanguage(settings.value("Page/accept-language").toString());
    if (settings.contains("Page/user-agent"))
        this->setUserAgent(settings.value("Page/user-agent").toString());
    if (settings.contains("Connection/test-url"))
        this->setConnectionTestUrl(settings.value("Connection/test-url").toString());
    if (settings.contains("Browser/hide-cursor"))
        this->setHideCursor(settings.value("Browser/hide-cursor").toBool());
    if (settings.contains("Window/title"))
        this->setWindowTitle(settings.value("Window/title").toString());
    if (settings.contains("Window/icon"))
        this->setWindowIcon(settings.value("Window/icon").toString());
    if (settings.contains("Window/position")) {
        QPoint position;
        const QString value = settings.value("Window/position").toString();
        if (!Configuration::parseWindowPosition(value, &position))
            return fail(QStringLiteral("Window/position must use the X,Y format"));
        this->setWindowPosition(position);
    }
    if (settings.contains("Window/size")) {
        QSize size;
        const QString value = settings.value("Window/size").toString();
        if (!Configuration::parseWindowSize(value, &size))
            return fail(QStringLiteral("Window/size must use the WIDTHxHEIGHT format with positive dimensions"));
        this->setWindowSize(size);
    }
    if (settings.contains("Page/white-list")) {
        QStringList values = settings.value("Page/white-list").toStringList();
        if (values.size() == 1)
            values = values.first().split(',', Qt::SkipEmptyParts);
        for (QString &value : values)
            value = value.trimmed();
        this->setWhiteList(values);
    }

    if (settings.contains("Window/mode")) {
        const QString value = settings.value("Window/mode").toString();
        if (!Configuration::windowModeOptions.contains(value))
            return fail(QStringLiteral("Unknown Window/mode value '%1'").arg(value));
        this->setWindowMode(Configuration::nameToWindowMode(value));
    }

    if (settings.contains("Navbar/horizontal-position")) {
        const QString value = settings.value("Navbar/horizontal-position").toString();
        const auto position = BarWidget::nameToBarWidgetHorizontalPosition(value);
        if (position == BarWidget::HorizontalPosition::Unknown)
            return fail(QStringLiteral("Unknown Navbar/horizontal-position value '%1'").arg(value));
        this->setNavbarHorizontalPosition(position);
    }

    if (settings.contains("Navbar/vertical-position")) {
        const QString value = settings.value("Navbar/vertical-position").toString();
        const auto position = BarWidget::nameToBarWidgetVerticalPosition(value);
        if (position == BarWidget::VerticalPosition::Unknown)
            return fail(QStringLiteral("Unknown Navbar/vertical-position value '%1'").arg(value));
        this->setNavbarVerticalPosition(position);
    }

    if (settings.contains("Page/allow-feature")) {
        QStringList names = settings.value("Page/allow-feature").toStringList();
        if (names.size() == 1)
            names = names.first().split(',', Qt::SkipEmptyParts);
        const auto permissions = WebPage::namesToWebPagePermissions(names);
        if (permissions.testFlag(WebPage::Permission::Unknown))
            return fail(QStringLiteral("Unknown Page/allow-feature value"));
        this->setPermissions(permissions);
    }

    if (settings.contains("Navbar/enabled-buttons")) {
        QStringList names = settings.value("Navbar/enabled-buttons").toStringList();
        if (names.size() == 1)
            names = names.first().split(',', Qt::SkipEmptyParts);
        const auto buttons = BarWidget::buttonNamesToButtons(names);
        if (buttons.contains(BarWidget::Button::Unknown))
            return fail(QStringLiteral("Unknown Navbar/enabled-buttons value"));
        this->setNavbarEnabledButtons(buttons);
    }

    if (settings.status() != QSettings::NoError)
        return fail(QStringLiteral("Unable to parse configuration file"));

    return true;
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

QUrl Configuration::getConnectionTestUrl() {
    return this->connectionTestUrl;
}

bool Configuration::isHideCursor() {
    return this->hideCursor;
}

QString Configuration::getWindowTitle() {
    return this->windowTitle;
}

QString Configuration::getWindowIcon() {
    return this->windowIcon;
}

bool Configuration::hasWindowPosition() {
    return this->windowPositionSet;
}

bool Configuration::hasWindowSize() {
    return this->windowSize.isValid();
}

QPoint Configuration::getWindowPosition() {
    return this->windowPosition;
}

QSize Configuration::getWindowSize() {
    return this->windowSize;
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
