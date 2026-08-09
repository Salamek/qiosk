#include "webview.h"
#include "webpage.h"
#include <QAction>
#include <QWebEnginePage>
#include <QTimer>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        if (this->rendererRecoveryInProgress) {
            return;
        }

        this->rendererRecoveryInProgress = true;
        const quint64 recoveryGeneration = ++this->rendererRecoveryGeneration;
        emit rendererRecoveryStarted(termStatus, statusCode);

        // Give Qt WebEngine a moment to tear down the failed renderer before
        // asking it to create a replacement. Immediate reloads can repeatedly
        // target the same dying process.
        QTimer::singleShot(1000, this, SLOT(reload()));

        // If no successful load follows, permit one controlled retry later.
        QTimer::singleShot(30000, this, [this, recoveryGeneration]() {
            if (this->rendererRecoveryGeneration == recoveryGeneration) {
                this->rendererRecoveryInProgress = false;
            }
        });
    });

    connect(this, &QWebEngineView::loadFinished, this, [this](bool ok) {
        if (ok && this->rendererRecoveryInProgress) {
            this->rendererRecoveryInProgress = false;
            ++this->rendererRecoveryGeneration;
        }
    });
}

void WebView::setPage(WebPage *page)
{
    createWebActionTrigger(page,QWebEnginePage::Forward);
    createWebActionTrigger(page,QWebEnginePage::Back);
    createWebActionTrigger(page,QWebEnginePage::Reload);
    createWebActionTrigger(page,QWebEnginePage::Stop);
    QWebEngineView::setPage(page);
}

void WebView::createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, this, [this, action, webAction]{
        emit webActionEnabledChanged(webAction, action->isEnabled());
    });
}

bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}

void WebView::scrollTo(int scrollX, int scrollY) {
    page()->runJavaScript(QString("window.scrollTo({ top: %1, left: %2, behavior: 'smooth' });").arg(scrollX).arg(scrollY));
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    switch (type) {
    case QWebEnginePage::WebBrowserTab: {
        return this;
    }
    case QWebEnginePage::WebBrowserBackgroundTab: {
        return this;
    }
    case QWebEnginePage::WebBrowserWindow: {
        return this;
    }
    case QWebEnginePage::WebDialog: {
        return nullptr;
    }
    }
    return nullptr;
}
