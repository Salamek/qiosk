#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>


class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = nullptr);
    bool isWebActionEnabled(QWebEnginePage::WebAction webAction) const;
    void scrollTo(int scrollX, int scrollY);
protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
signals:
    void webActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
private:
    void createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);
};

#endif
