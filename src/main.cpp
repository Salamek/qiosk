#include "mainwindow.h"
#include "userinputeventfilter.h"
#include "configuration.h"
#include "barwidget.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QCommandLineParser>
#include <QVersionNumber>

#include <QWindow>
#include <cstring>


static void handleVisibleChanged(){
    if (!QGuiApplication::inputMethod()->isVisible())
        return;
    for(QWindow * w: QGuiApplication::allWindows()){
        if(std::strcmp(w->metaObject()->className(), "QtVirtualKeyboard::InputView") == 0){
            if(QObject *keyboard = w->findChild<QObject *>("keyboard")){
                int offset = 170;
                QRect r = w->geometry();
                r.setHeight(r.height() - offset);
                r.moveTop(keyboard->property("y").toDouble() - offset);
                w->setMask(r);
                return;
            }
        }
    }
}


int main(int argc, char *argv[])
{
    //qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    //qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9988"); //https://developer.chrome.com/docs/devtools/
    //qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging");
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setOrganizationName("Salamek");
    QCoreApplication::setApplicationName("qiosk");
    QCoreApplication::setApplicationVersion("1.11.0");
#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);

    if(qgetenv("XDG_CURRENT_DESKTOP") == "") {
        // Hack to ~fix virtual keyboard viewport on openbox
        QObject::connect(QGuiApplication::inputMethod(), &QInputMethod::visibleChanged, &handleVisibleChanged);
    }


    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser written in QT");
    parser.addHelpOption();
    QCommandLineOption versionOption(QStringList() << QStringLiteral("v") << QStringLiteral("version"), QCoreApplication::translate("main", "Displays version information."));
    parser.addOption(versionOption);

    parser.addPositionalArgument("url", QCoreApplication::translate("main", "URL to display."));

    // A boolean option with multiple names (-f, --fullscreen)
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", QCoreApplication::translate("main", "Run browser in fullscreen mode."));
    parser.addOption(fullscreenOption);

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
    // url is args.at(0)
    if (args.size() < 1) {
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Must specify an argument.")));
        parser.showHelp(1);
    }

    // Prefill Configuration
    Configuration *config = new Configuration();
    config->setUrl(args.at(0));
    config->setFullscreen(parser.isSet(fullscreenOption));
    config->setIdleTime(parser.value(idleTimeOption).toInt());

    config->setNavbarWidth(parser.value(navbarWidthOption).toInt());
    config->setNavbarHeight(parser.value(navbarHeightOption).toInt());

    config->setDisplayNavBar(parser.isSet(displayNavBarOption));
    config->setDisplayAddressBar(parser.isSet(displayAddressBarOption));
    config->setDisplayScrollBars(parser.isSet(displayScrollBarsOption));
    config->setUnderlayNavBar(parser.isSet(underlayNavBarOption));
    config->setProfileName(parser.value(profileNameOption));

    if (parser.isSet(whiteListOption)) {
        config->setWhiteList(parser.values(whiteListOption));
    }

    BarWidget::HorizontalPosition horizontalPosition = BarWidget::nameToBarWidgetHorizontalPosition(parser.value(navbarHorizontalPositionOption));
    if (horizontalPosition == BarWidget::HorizontalPosition::Unknown) {
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided horizontal position is unknown, use one of left|right|center.")));
        parser.showHelp(1);
    }
    config->setNavbarHorizontalPosition(horizontalPosition);

    BarWidget::VerticalPosition verticalPosition = BarWidget::nameToBarWidgetVerticalPosition(parser.value(navbarVerticalPositionOption));
    if (verticalPosition == BarWidget::VerticalPosition::Unknown) {
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided vertical position is unknown, use one of top|bottom.")));
        parser.showHelp(1);
    }
    config->setNavbarVerticalPosition(verticalPosition);

    WebPage::Permissions permissions = WebPage::namesToWebPagePermissions(parser.values(allowFeatureOption));
    if(permissions.testFlag(WebPage::Permission::Unknown)){
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided permission is unknown, use one of %s.", permissionOptionsMapKeys.join("|").toLatin1())));
        parser.showHelp(1);
    }
    config->setPermissions(permissions);


    QList<BarWidget::Button> buttons = BarWidget::buttonNamesToButtons(parser.value(navbarEnableButtonOption).split(","));
    if(buttons.contains(BarWidget::Button::Unknown)){
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided enabled-button is unknown, use one of %s separated by comma.", enableButtonOptionsMapKeys.join("|").toLatin1())));
        parser.showHelp(1);
    }
    config->setNavbarEnabledButtons(buttons);

    if (parser.isSet(acceptLanguage)) {
        config->setAcceptLanguage(parser.value(acceptLanguage));
    }

    if (parser.isSet(userAgent)) {
        config->setUserAgent(parser.value(userAgent));
    }


    MainWindow w(config);

    if (config->isFullscreen()) {
        w.showFullScreen();
    } else {
        w.show();
    }

    UserInputEventFilter userInputEventFilter;

    QObject::connect(&userInputEventFilter, &UserInputEventFilter::userActivity, &w, &MainWindow::onUserActivity);

    a.installEventFilter(&userInputEventFilter);

    if (parser.isSet(hideCursor)){
      a.setOverrideCursor(Qt::BlankCursor);
    }

    return a.exec();
}
