#include "mainwindow.h"
#include "userinputeventfilter.h"
#include "configuration.h"
#include "barwidget.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QIcon>
#include <QVersionNumber>


int main(int argc, char *argv[])
{
    //qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    //qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9988"); //https://developer.chrome.com/docs/devtools/
    //qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging");
    if (qgetenv("QT_IM_MODULE") == "qtvirtualkeyboard") {
        qputenv("QT_VIRTUALKEYBOARD_DESKTOP_DISABLE", QByteArray("1"));
    }

    QCoreApplication::setOrganizationName("Salamek");
    QCoreApplication::setApplicationName("qiosk");
    QCoreApplication::setApplicationVersion("1.17.0");
#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);


    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser written in QT");
    parser.addHelpOption();
    QCommandLineOption versionOption(QStringList() << QStringLiteral("v") << QStringLiteral("version"), QCoreApplication::translate("main", "Displays version information."));
    parser.addOption(versionOption);

    QCommandLineOption configOption(QStringLiteral("config"), QCoreApplication::translate("main", "Use an alternative configuration file instead of /etc/qiosk/qiosk.conf."), QCoreApplication::translate("main", "file"));
    parser.addOption(configOption);

    parser.addPositionalArgument("url", QCoreApplication::translate("main", "URL to display."));

    // A boolean option with multiple names (-f, --fullscreen)
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", QCoreApplication::translate("main", "Run browser in fullscreen mode."));
    parser.addOption(fullscreenOption);

    QCommandLineOption windowModeOption(QStringList() << "m" << "window-mode", QCoreApplication::translate("main", "Set window mode.") , Configuration::windowModeOptions.join("|"), "automaticvisibility");
    parser.addOption(windowModeOption);

    QCommandLineOption windowTitleOption(QStringLiteral("window-title"), QCoreApplication::translate("main", "Set the window title."), QCoreApplication::translate("main", "title"));
    parser.addOption(windowTitleOption);

    QCommandLineOption windowIconOption(QStringLiteral("window-icon"), QCoreApplication::translate("main", "Set the window icon from an image file."), QCoreApplication::translate("main", "path"));
    parser.addOption(windowIconOption);

    // A int option with multiple names (-i, --idle)
    QCommandLineOption idleTimeOption(QStringList() << "i" << "idle", QCoreApplication::translate("main", "Timeout to reset to home page.") ,QCoreApplication::translate("main", "seconds"), "0");
    parser.addOption(idleTimeOption);

    // A int option with multiple names (-w, --white-list)
    QCommandLineOption whiteListOption(QStringList() << "w" << "white-list", QCoreApplication::translate("main", "Whitelist with glob syntax.") ,QCoreApplication::translate("main", "string"));
    parser.addOption(whiteListOption);

    // A string option (--navbar-vertical-position)
    QCommandLineOption navbarVerticalPositionOption(QString("navbar-vertical-position"), QCoreApplication::translate("main", "Navbar vertical position."), BarWidget::navbarVerticalPositionOptions.join("|"), "bottom");
    parser.addOption(navbarVerticalPositionOption);

    // A string option (--navbar-horizontal-position)
    QCommandLineOption navbarHorizontalPositionOption(QString("navbar-horizontal-position"), QCoreApplication::translate("main", "Navbar horizontal position.") , BarWidget::navbarHorizontalPositionOptions.join("|"), "center");
    parser.addOption(navbarHorizontalPositionOption);

    // A string option (--navbar-width)
    QCommandLineOption navbarWidthOption(QString("navbar-width"), QCoreApplication::translate("main", "Navbar width.") ,QCoreApplication::translate("main", "%"), "100");
    parser.addOption(navbarWidthOption);

    // A string option (--navbar-height)
    QCommandLineOption navbarHeightOption(QString("navbar-height"), QCoreApplication::translate("main", "Navbar height.") ,QCoreApplication::translate("main", "%"), "5");
    parser.addOption(navbarHeightOption);

    // A string option with multiple names (-a, --allow-feature)
    QStringList permissionOptionsMapKeys = WebPage::getPermissionOptionMap().keys();
    QCommandLineOption allowFeatureOption(QStringList() << "a" << "allow-feature", QCoreApplication::translate("main", "Allow feature."), permissionOptionsMapKeys.join("|"));
    parser.addOption(allowFeatureOption);

    // A boolean option (--display-navbar)
    QCommandLineOption displayNavBarOption(QString("display-navbar"), QCoreApplication::translate("main", "Display navbar with navigation buttons."));
    parser.addOption(displayNavBarOption);

    // A boolean option (--display-scroll-bars)
    QCommandLineOption displayScrollBarsOption(QString("display-scroll-bars"), QCoreApplication::translate("main", "Display scroll bars."));
    parser.addOption(displayScrollBarsOption);

    // A boolean option (--underlay-navbar)
    QCommandLineOption underlayNavBarOption(QString("underlay-navbar"), QCoreApplication::translate("main", "Display navbar under the webview."));
    parser.addOption(underlayNavBarOption);

    // A boolean option (--display-addressbar)
    QCommandLineOption displayAddressBarOption(QString("display-addressbar"), QCoreApplication::translate("main", "Display address bar."));
    parser.addOption(displayAddressBarOption);

    // A string option (--profile-name)
    QCommandLineOption profileNameOption(QString("profile-name"), QCoreApplication::translate("main", "Profile name, if not specified or default, default off-record profile is used."), QCoreApplication::translate("main", "profile_name."), "default");
    parser.addOption(profileNameOption);

    // A boolean option (--hide-cursor)
    QCommandLineOption hideCursor(QString("hide-cursor"), QCoreApplication::translate("main", "Hide cursor."));
    parser.addOption(hideCursor);

    // A string option (--navbar-enable-button)
    QStringList enableButtonOptionsMapKeys = BarWidget::navbarEnabledButtonOptions;
    QCommandLineOption navbarEnableButtonOption(QString("navbar-enable-buttons"), QCoreApplication::translate("main", "Navbar enabled button."), enableButtonOptionsMapKeys.join(","), enableButtonOptionsMapKeys.join(","));
    parser.addOption(navbarEnableButtonOption);

    // A string option (--accept-language), sets content of Accept-Language header
    QCommandLineOption acceptLanguage(QString("accept-language"), QCoreApplication::translate("main", "Set content of Accept-Language header, autodetect when not set."),  QCoreApplication::translate("main", "accept_language"));
    parser.addOption(acceptLanguage);

    // A string option (--user-agent), sets content of User-Agent header
    QCommandLineOption userAgent(QString("user-agent"), QCoreApplication::translate("main", "Set content of User-Agent header, autodetect when not set."),  QCoreApplication::translate("main", "user_agent"));
    parser.addOption(userAgent);

    // A URL option (--connection-test-url), overrides origin autodetection
    QCommandLineOption connectionTestUrl(QString("connection-test-url"), QCoreApplication::translate("main", "URL used to detect when the target web server becomes available; defaults to the target URL origin."), QCoreApplication::translate("main", "url"));
    parser.addOption(connectionTestUrl);

    // Process the actual command line arguments given by the user
    parser.process(a);

    if (parser.isSet(versionOption)) {
        QTextStream(stdout) << QCoreApplication::applicationName() << ": " << QCoreApplication::applicationVersion() << "\n";
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        QTextStream(stdout) << "Chromium: " << qWebEngineChromiumVersion() << "\nQWebEngine: " << qWebEngineVersion() << "\n";
#endif
        ::exit(EXIT_SUCCESS);
    }

    const QStringList args = parser.positionalArguments();
    if (args.size() > 1) {
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Only one URL may be specified.")));
        parser.showHelp(1);
    }

    // Load built-in defaults, then the selected configuration file.
    Configuration *config = new Configuration();
    const bool customConfig = parser.isSet(configOption);
    const QString configFile = customConfig ? parser.value(configOption) : QStringLiteral("/etc/qiosk/qiosk.conf");
    if (customConfig || QFileInfo::exists(configFile)) {
        QString errorMessage;
        if (!QFileInfo::exists(configFile) || !config->loadFromFile(configFile, &errorMessage)) {
            if (errorMessage.isEmpty())
                errorMessage = QCoreApplication::translate("main", "Configuration file does not exist: %1").arg(configFile);
            fprintf(stderr, "%s\n", qPrintable(errorMessage));
            return EXIT_FAILURE;
        }
    }

    // Explicit command-line values overlay file configuration.
    if (!args.isEmpty())
        config->setUrl(args.first());
    if (parser.isSet(idleTimeOption))
        config->setIdleTime(parser.value(idleTimeOption).toInt());
    if (parser.isSet(navbarWidthOption))
        config->setNavbarWidth(parser.value(navbarWidthOption).toInt());
    if (parser.isSet(navbarHeightOption))
        config->setNavbarHeight(parser.value(navbarHeightOption).toInt());

    if (parser.isSet(displayNavBarOption))
        config->setDisplayNavBar(true);
    if (parser.isSet(displayAddressBarOption))
        config->setDisplayAddressBar(true);
    if (parser.isSet(displayScrollBarsOption))
        config->setDisplayScrollBars(true);
    if (parser.isSet(underlayNavBarOption))
        config->setUnderlayNavBar(true);
    if (parser.isSet(profileNameOption))
        config->setProfileName(parser.value(profileNameOption));

    if (parser.isSet(whiteListOption)) {
        config->setWhiteList(parser.values(whiteListOption));
    }

    if (parser.isSet(navbarHorizontalPositionOption)) {
        BarWidget::HorizontalPosition horizontalPosition = BarWidget::nameToBarWidgetHorizontalPosition(parser.value(navbarHorizontalPositionOption));
        if (horizontalPosition == BarWidget::HorizontalPosition::Unknown) {
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided horizontal position is unknown, use one of left|right|center.")));
            parser.showHelp(1);
        }
        config->setNavbarHorizontalPosition(horizontalPosition);
    }

    if (parser.isSet(navbarVerticalPositionOption)) {
        BarWidget::VerticalPosition verticalPosition = BarWidget::nameToBarWidgetVerticalPosition(parser.value(navbarVerticalPositionOption));
        if (verticalPosition == BarWidget::VerticalPosition::Unknown) {
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided vertical position is unknown, use one of top|bottom.")));
            parser.showHelp(1);
        }
        config->setNavbarVerticalPosition(verticalPosition);
    }

    if (parser.isSet(allowFeatureOption)) {
        WebPage::Permissions permissions = WebPage::namesToWebPagePermissions(parser.values(allowFeatureOption));
        if(permissions.testFlag(WebPage::Permission::Unknown)){
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided permission is unknown, use one of %1.").arg(permissionOptionsMapKeys.join("|"))));
            parser.showHelp(1);
        }
        config->setPermissions(permissions);
    }

    if (parser.isSet(navbarEnableButtonOption)) {
        QList<BarWidget::Button> buttons = BarWidget::buttonNamesToButtons(parser.value(navbarEnableButtonOption).split(","));
        if(buttons.contains(BarWidget::Button::Unknown)){
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided enabled-button is unknown, use one of %1 separated by comma.").arg(enableButtonOptionsMapKeys.join("|"))));
            parser.showHelp(1);
        }
        config->setNavbarEnabledButtons(buttons);
    }


    if (parser.isSet(fullscreenOption)) {
        config->setWindowMode(QWindow::Visibility::FullScreen);
    } else if (parser.isSet(windowModeOption)) {
        QString windowMode = parser.value(windowModeOption);
        if (!Configuration::windowModeOptions.contains(windowMode))
        {
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided window-mode is unknown, use one of %1.").arg(Configuration::windowModeOptions.join("|"))));
            parser.showHelp(1);
        }
        QWindow::Visibility visibility = Configuration::nameToWindowMode(windowMode);
        config->setWindowMode(visibility);
    }

    if (parser.isSet(acceptLanguage)) {
        config->setAcceptLanguage(parser.value(acceptLanguage));
    }

    if (parser.isSet(userAgent)) {
        config->setUserAgent(parser.value(userAgent));
    }

    if (parser.isSet(connectionTestUrl)) {
        QUrl testUrl = Configuration::normalizeUrl(QUrl(parser.value(connectionTestUrl)));
        if (!testUrl.isValid() || testUrl.scheme().isEmpty() || testUrl.host().isEmpty()) {
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided connection-test-url is not a valid absolute URL.")));
            parser.showHelp(1);
        }
        config->setConnectionTestUrl(testUrl);
    }

    if (parser.isSet(windowTitleOption)) {
        config->setWindowTitle(parser.value(windowTitleOption));
    }

    if (parser.isSet(windowIconOption)) {
        config->setWindowIcon(parser.value(windowIconOption));
    }


    QIcon applicationIcon(config->getWindowIcon());
    if (applicationIcon.isNull()) {
        qWarning() << "Unable to load window icon" << config->getWindowIcon()
                   << "using the built-in qiosk icon instead.";
        applicationIcon = QIcon(QStringLiteral(":/icon.png"));
    }
    a.setWindowIcon(applicationIcon);

    MainWindow w(config);

    w.setWindowMode(config->getWindowMode());

    UserInputEventFilter userInputEventFilter;

    QObject::connect(&userInputEventFilter, &UserInputEventFilter::userActivity, &w, &MainWindow::onUserActivity);

    a.installEventFilter(&userInputEventFilter);

    if (parser.isSet(hideCursor))
      config->setHideCursor(true);

    if (config->isHideCursor()){
      a.setOverrideCursor(Qt::BlankCursor);
    }

    return a.exec();
}
