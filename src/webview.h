#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>

class WebPage;

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = nullptr);
    bool isWebActionEnabled(QWebEnginePage::WebAction webAction) const;
    void scrollTo(int scrollX, int scrollY);
    void setPage(WebPage *page);
protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
signals:
    void webActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
private:
    void createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);
    qint64 lastCrashReloadTime = -1;
};

#endif
