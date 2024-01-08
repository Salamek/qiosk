#include "webview.h"
#include "mainwindow.h"
#include <QMenu>
#include <QMessageBox>
#include <QWebEnginePage>
#include <QTimer>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {

        bool doReload = false;

        // When render proccess terminates, reload
        // Reload if last crash was more then one minute ago, if not, stop and display dialog
        if (this->lastCrashReloadTime == -1 || this->lastCrashReloadTime + 60 < QDateTime::currentSecsSinceEpoch()) {
            doReload = true;
        } else {
            QString status;
            switch (termStatus) {
            case QWebEnginePage::NormalTerminationStatus:
                status = tr("Render process normal exit");
                break;
            case QWebEnginePage::AbnormalTerminationStatus:
                status = tr("Render process abnormal exit");
                break;
            case QWebEnginePage::CrashedTerminationStatus:
                status = tr("Render process crashed");
                break;
            case QWebEnginePage::KilledTerminationStatus:
                status = tr("Render process killed");
                break;
            }
            QMessageBox::StandardButton btn = QMessageBox::question(window(), status,
                                                                    tr("Render process exited with code: %1\n"
                                                                       "Do you want to reload the page ?").arg(statusCode));

            doReload = btn == QMessageBox::Yes;
        }

        if (doReload) {
            QTimer::singleShot(0, this, SLOT(reload()));
            this->lastCrashReloadTime = QDateTime::currentSecsSinceEpoch();
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
