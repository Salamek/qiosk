#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineHistory>
#include <QResizeEvent>
#include <QProgressBar>

#include "enum.h"
#include "barwidget.h"
#include "webview.h"
#include "progressbarwidget.h"
#include "resettimer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void onUserActivity();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
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
private slots:
    void goHome();
    void doReload();
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void doReset();
};
#endif // MAINWINDOW_H
