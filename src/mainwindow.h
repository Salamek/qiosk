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

#include "enum.h"
#include "barwidget.h"
#include "configuration.h"
#include "webview.h"
#include "webpage.h"
#include "progressbarwidget.h"
#include "resettimer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Configuration *config, QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void onUserActivity();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
private:
    qint64 lastUserActivity;
    Configuration *config;
    bool resetHistoryLock;
    QUrl initialUrl;
    WebView *webView;
    BarWidget *barWidget;
    ProgressBarWidget *progressBar;
    QWebEnginePage::WebAction refreshWebAction;
    ResetTimer *resetTimer;
    void handleWebViewLoadProgress(int progress);
    void handleLoadFinished(bool ok);
    void handleLoadStarted();
    void setupReloadStopButton(bool loading);
    void checkReset();
    void plot(QSize size);
private slots:
    void goHome();
    void doReload();
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void doReset();
    void plotTimerHack();
};
#endif // MAINWINDOW_H
