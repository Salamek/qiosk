#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>


QT_FORWARD_DECLARE_CLASS(WebPage)
//class WebPage;

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
    void rendererRecoveryStarted(QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode);
private:
    void createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);
    bool rendererRecoveryInProgress = false;
    quint64 rendererRecoveryGeneration = 0;
};

#endif
