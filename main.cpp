#include "mainwindow.h"
#include "userinputeventfilter.h"
#include "configuration.h"
#include "enum.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setOrganizationName("Adam Schubert");
    QCoreApplication::setApplicationName("qt-kiosk");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

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
    QCommandLineOption navbarVerticalPositionOption(QString("navbar-vertical-position"), QCoreApplication::translate("main", "Navbar vertical position") ,QCoreApplication::translate("main", "top|bottom"), "bottom");
    parser.addOption(navbarVerticalPositionOption);

    // A string option (--navbar-horizontal-position)
    QCommandLineOption navbarHorizontalPositionOption(QString("navbar-horizontal-position"), QCoreApplication::translate("main", "Navbar horizontal position") ,QCoreApplication::translate("main", "left|right|center"), "center");
    parser.addOption(navbarHorizontalPositionOption);

    // A string option (--navbar-width)
    QCommandLineOption navbarWidthOption(QString("navbar-width"), QCoreApplication::translate("main", "Navbar width") ,QCoreApplication::translate("main", "%"), "100");
    parser.addOption(navbarWidthOption);

    // A string option (--navbar-height)
    QCommandLineOption navbarHeightOption(QString("navbar-height"), QCoreApplication::translate("main", "Navbar height") ,QCoreApplication::translate("main", "%"), "5");
    parser.addOption(navbarHeightOption);

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

    QStringList navbarHorizontalPositionOptions;
    navbarHorizontalPositionOptions << "left" << "right" << "center";
    switch(navbarHorizontalPositionOptions.indexOf(parser.value(navbarHorizontalPositionOption))){
      case 0:
        config->setNavbarHorizontalPosition(BarHorizontalPositionEnum::Left);
        break;

      case 1:
        config->setNavbarHorizontalPosition(BarHorizontalPositionEnum::Right);
        break;

      case 2:
        config->setNavbarHorizontalPosition(BarHorizontalPositionEnum::Center);
        break;
      default:
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided horizontal position is not allowed, use one of left|right|center.")));
        parser.showHelp(1);
        break;
    }

    QStringList navbarVerticalPositionOptions;
    navbarVerticalPositionOptions << "top" << "bottom";
    switch(navbarVerticalPositionOptions.indexOf(parser.value(navbarVerticalPositionOption))){
      case 0:
        config->setNavbarVerticalPosition(BarVerticalPositionEnum::Top);
        break;

      case 1:
        config->setNavbarVerticalPosition(BarVerticalPositionEnum::Bottom);
        break;

      default:
        fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Provided vertical position is not allowed, use one of top|bottom.")));
        parser.showHelp(1);
        break;
    }


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
