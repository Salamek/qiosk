#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QWebEngineSettings>
#include <QResizeEvent>
#include <QProgressBar>
#include <QLineEdit>
#include <QAction>
#include <QApplication>
#include <QQuickWidget>
#include <QTimer>

#include "enum.h"
#include "barwidget.h"
#include "configuration.h"
#include "webview.h"
#include "webpage.h"
#include "progressbarwidget.h"
#include "websocketcontrol.h"
#include "connectionchecker.h"
#include <QWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Configuration *config, QWidget *parent = nullptr);
    ~MainWindow();

    void setWindowMode(QWindow::Visibility windowMode);
public slots:
    void onUserActivity();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
private:
    QVBoxLayout *mainLayout;
    QLineEdit *addressBar;
    qint64 lastUserActivity;
    int idleTime;
    Configuration *config;
    bool isIdle;
    bool resetHistoryLock;
    bool underlayNavBar;
    QUrl initialUrl;
    WebView *webView;
    BarWidget *barWidget;
    ProgressBarWidget *progressBar;
    QWebEnginePage::WebAction refreshWebAction;
    QTimer *idleTimer;
    WebsocketControl *websocketControl;
    ConnectionChecker *connectionChecker;
    QQuickWidget *virtualKeyboardWidget;
    void handleWebViewLoadProgress(int progress);
    void handleLoadFinished(bool ok);
    void handleLoadStarted();
    void setupReloadStopButton(bool loading);
    void checkReset();
    void plot(QSize size);
    void setIdleTime(int idleTime);
    void setDisplayAddressBar(bool displayAddressBar);
    void setDisplayNavBar(bool displayNavBar);
    void setUnderlayNavBar(bool underlayNavBar);
    void setHomePageUrl(QUrl homePageUrl);
    QUrl connectionTestUrlFor(QUrl targetUrl) const;
    void setNavBarEnabledButtons(QStringList enabledButtons);
    void setupVirtualKeyboard();
    void updateVirtualKeyboardGeometry();
    void scrollFocusedElementIntoView();
private slots:
    void goHome();
    void doReload();
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void doReset();
    void plotTimerHack();
};
#endif // MAINWINDOW_H
