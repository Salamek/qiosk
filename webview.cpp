#include "webview.h"
#include "mainwindow.h"
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QWebEnginePage>
#include <QTimer>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
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
        if (btn == QMessageBox::Yes) {
            QTimer::singleShot(0, [this] { reload(); });
        }
    });
    createWebActionTrigger(this->page(),QWebEnginePage::Forward);
    createWebActionTrigger(this->page(),QWebEnginePage::Back);
    createWebActionTrigger(this->page(),QWebEnginePage::Reload);
    createWebActionTrigger(this->page(),QWebEnginePage::Stop);
}


void WebView::createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction]{
        emit webActionEnabledChanged(webAction, action->isEnabled());
    });
}

bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
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
