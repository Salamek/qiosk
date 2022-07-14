#include "mainwindow.h"
#include "userinputeventfilter.h"
#include "configuration.h"
#include "barwidget.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    //qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9988"); //https://developer.chrome.com/docs/devtools/
    //qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging");
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setOrganizationName("Adam Schubert");
    QCoreApplication::setApplicationName("qiosk");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    QStringList navbarHorizontalPositionOptions;
    navbarHorizontalPositionOptions << "left" << "right" << "center";

    QStringList navbarVerticalPositionOptions;
    navbarVerticalPositionOptions << "top" << "bottom";

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



    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser written in QT");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url", QCoreApplication::translate("main", "URL to display."));

    // A boolean option with multiple names (-f, --fullscreen)
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", QCoreApplication::translate("main", "Run browser in fullscreen mode."));
    parser.addOption(fullscreenOption);

    // A int option with multiple names (-i, --idle)
    QCommandLineOption idleTimeOption(QStringList() << "i" << "idle", QCoreApplication::translate("main", "Timeout to reset") ,QCoreApplication::translate("main", "seconds"), "0");
    parser.addOption(idleTimeOption);

    // A int option with multiple names (-w, --white-list)
    QCommandLineOption whiteListOption(QStringList() << "w" << "white-list", QCoreApplication::translate("main", "Whitelist with glob syntax") ,QCoreApplication::translate("main", "string"));
    parser.addOption(whiteListOption);

    // A string option (--navbar-vertical-position)
    QCommandLineOption navbarVerticalPositionOption(QString("navbar-vertical-position"), QCoreApplication::translate("main", "Navbar vertical position"), navbarVerticalPositionOptions.join("|"), "bottom");
    parser.addOption(navbarVerticalPositionOption);

    // A string option (--navbar-horizontal-position)
    QCommandLineOption navbarHorizontalPositionOption(QString("navbar-horizontal-position"), QCoreApplication::translate("main", "Navbar horizontal position") ,navbarHorizontalPositionOptions.join("|"), "center");
    parser.addOption(navbarHorizontalPositionOption);

    // A string option (--navbar-width)
    QCommandLineOption navbarWidthOption(QString("navbar-width"), QCoreApplication::translate("main", "Navbar width") ,QCoreApplication::translate("main", "%"), "100");
    parser.addOption(navbarWidthOption);

    // A string option (--navbar-height)
    QCommandLineOption navbarHeightOption(QString("navbar-height"), QCoreApplication::translate("main", "Navbar height") ,QCoreApplication::translate("main", "%"), "5");
    parser.addOption(navbarHeightOption);

    // A string option with multiple names (-a, --allow-feature)
    QStringList permissionOptionsMapKeys = permissionOptionsMap.keys();
    QCommandLineOption allowFeatureOption(QStringList() << "a" << "allow-feature", QCoreApplication::translate("main", "Allow feature"), permissionOptionsMapKeys.join("|"));
    parser.addOption(allowFeatureOption);

    // Process the actual command line arguments given by the user
    parser.process(a);

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

    if (parser.isSet(whiteListOption)) {
        config->setWhiteList(parser.values(whiteListOption));
    }

    switch(navbarHorizontalPositionOptions.indexOf(parser.value(navbarHorizontalPositionOption))){
      case 0:
        config->setNavbarHorizontalPosition(BarWidget::HorizontalPosition::Left);
        break;

      case 1:
        config->setNavbarHorizontalPosition(BarWidget::HorizontalPosition::Right);
        break;

      case 2:
        config->setNavbarHorizontalPosition(BarWidget::HorizontalPosition::Center);
        break;
      default:
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided horizontal position is unknown, use one of left|right|center.")));
        parser.showHelp(1);
        break;
    }

    switch(navbarVerticalPositionOptions.indexOf(parser.value(navbarVerticalPositionOption))){
      case 0:
        config->setNavbarVerticalPosition(BarWidget::VerticalPosition::Top);
        break;

      case 1:
        config->setNavbarVerticalPosition(BarWidget::VerticalPosition::Bottom);
        break;

      default:
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided vertical position is unknown, use one of top|bottom.")));
        parser.showHelp(1);
        break;
    }

    QString allowFeature;
    WebPage::Permissions permissions;
    foreach(allowFeature, parser.values(allowFeatureOption)) {
        if (permissionOptionsMap.contains(allowFeature)) {
            permissions |= permissionOptionsMap.value(allowFeature);
        } else {
            fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided permission is unknown, use one of %s.")));
            parser.showHelp(1);
            break;
        }
    }

    config->setPermissions(permissions);


    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->setUseForGlobalCertificateVerification();
#endif

    MainWindow w(config);
    if (config->isFullscreen()) {
        w.showFullScreen();
    } else {
        w.show();
    }

    UserInputEventFilter userInputEventFilter;

    QObject::connect(&userInputEventFilter, &UserInputEventFilter::userActivity, &w, &MainWindow::onUserActivity);

    a.installEventFilter(&userInputEventFilter);
    return a.exec();
}
