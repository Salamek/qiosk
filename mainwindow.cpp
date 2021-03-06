#include "mainwindow.h"


MainWindow::MainWindow(Configuration *config, QWidget *parent)
    : QMainWindow(parent)
{
    this->config = config;
    this->resetHistoryLock = false;
    this->refreshWebAction = QWebEnginePage::Reload;
    this->initialUrl = this->config->getUrl();
    this->lastUserActivity = QDateTime::currentSecsSinceEpoch(); // Set last user activity to NOW

    this->webView = new WebView();
    WebPage *webPage = new WebPage(QWebEngineProfile::defaultProfile(), this->webView);
    webPage->setWhiteList(this->config->getWhiteList());
    webPage->setPermissions(this->config->getPermissions());
    this->webView->setPage(webPage);
    this->webView->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QAction *favAction = new QAction(this);
    QLineEdit *addressBar = new QLineEdit(this);
    addressBar->setClearButtonEnabled(true);
    addressBar->addAction(favAction, QLineEdit::LeadingPosition);
    if (!this->config->isDisplayAddressBar()) {
        addressBar->hide();
    }

    mainLayout->addWidget(addressBar);

    mainLayout->addWidget(this->webView);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    this->barWidget = new BarWidget(this);

    if (!this->config->isDisplayNavBar()) {
        this->barWidget->hide();
    }

    this->progressBar = new ProgressBarWidget(this);
    this->resetTimer = new ResetTimer(this);

    //Bar events
    connect(this->barWidget->homeButton, &QPushButton::clicked, this, &MainWindow::goHome);
    connect(this->barWidget->reloadButton, &QPushButton::clicked, this, &MainWindow::doReload);
    connect(this->barWidget->forwardButton, &QPushButton::clicked, this->webView, &QWebEngineView::forward);
    connect(this->barWidget->backButton, &QPushButton::clicked, this->webView, &QWebEngineView::back);

    //Enable Buttons that are not needed to be disabled
    this->barWidget->homeButton->setEnabled(true);
    this->barWidget->reloadButton->setEnabled(true);

    //WebView events
    connect(this->webView, &QWebEngineView::loadStarted, this, &MainWindow::handleLoadStarted);
    connect(this->webView, &QWebEngineView::loadProgress, this, &MainWindow::handleWebViewLoadProgress);
    connect(this->webView, &QWebEngineView::loadFinished, this, &MainWindow::handleLoadFinished);
    connect(this->webView, &WebView::webActionEnabledChanged, this, &MainWindow::handleWebActionEnabledChanged);

    connect(this->webView, &QWebEngineView::urlChanged, this, [this, addressBar](const QUrl &url) {
        addressBar->setText(url.toDisplayString());
    });
    connect(addressBar, &QLineEdit::returnPressed, this, [this, addressBar]() {
        this->webView->setUrl(QUrl::fromUserInput(addressBar->text()));
    });

    connect(this->webView, &QWebEngineView::iconChanged, favAction, &QAction::setIcon);

    //Reset timer events
    connect(this->resetTimer, &ResetTimer::timeout, this, &MainWindow::checkReset);

    // ============================
    // Initial Configuration
    // ============================
    this->barWidget->setHeight(this->config->getNavbarHeight()); //%
    this->barWidget->setWidth(this->config->getNavbarWidth()); //%
    this->barWidget->setHorizontalPosition(this->config->getNavbarHorizontalPosition());
    this->barWidget->setVerticalPosition(this->config->getNavbarVerticalPosition());

    // Configure reset timer, disable this if timeout==0
    if (this->config->getIdleTime() != 0) {
        this->resetTimer->setTimeout(1); //Check each 1s
        this->resetTimer->start();
    }

    // ============================
    // End of initial configuration
    // ============================

    //Plot initial geometry
    /*QRect windowGeometry = this->geometry();
    qDebug() << windowGeometry;
    this->barWidget->plot(windowGeometry.width(), windowGeometry.height());
    this->progressBar->plot(windowGeometry.width(), windowGeometry.height());*/

    //Load URL
    this->webView->setUrl(this->initialUrl);
}


void MainWindow::goHome() {
    this->webView->load(this->initialUrl);
}

void MainWindow::doReload() {
    this->webView->triggerPageAction(this->refreshWebAction);
}


void MainWindow::checkReset() {
    if (this->config->getIdleTime() > 0 && this->lastUserActivity + this->config->getIdleTime() <= QDateTime::currentSecsSinceEpoch()) {
        this->doReset();
    }
}

/**
 * @brief MainWindow::doReset
 * Reset browser to initial state
 */
void MainWindow::doReset() {
    // Load initial page if there is any history
    // History == someone was working on kiosk == we have to reset his shit
    if (this->webView->history()->canGoBack() || this->webView->history()->canGoForward()) {
        this->webView->load(this->initialUrl);
        this->resetHistoryLock = true;
    }

    // Reset scroll
    this->webView->scrollTo(0, 0);

    // Reset zoom
    this->webView->setZoomFactor(1.0);
}


void MainWindow::onUserActivity() {
    this->lastUserActivity = QDateTime::currentSecsSinceEpoch();
}

void MainWindow::plot(QSize size) {
    this->barWidget->plot(size.width(), size.height());
    this->progressBar->plot(size.width(), size.height());
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    if (event->spontaneous()) { // Handle resize event after window system renders the window
        this->plot(event->size());
    }

    QMainWindow::resizeEvent(event);
}

void MainWindow::changeEvent(QEvent *event){
    if (event->type() == QEvent::WindowStateChange) {
        this->plot(this->size());
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event) {
    this->plot(this->size());
    QMainWindow::showEvent(event);
}

void MainWindow::handleLoadStarted() {
    this->setupReloadStopButton(true);
}

void MainWindow::handleWebViewLoadProgress(int progress)
{
    if (0 < progress && progress < 100) {
        this->progressBar->setValue(progress);
    } else {
        this->progressBar->setValue(0);
    }
}

void MainWindow::handleLoadFinished(bool ok) {
    Q_UNUSED(ok);
    this->setupReloadStopButton(false);
    if (this->resetHistoryLock == true) {
        this->webView->history()->clear();
        this->resetHistoryLock = false;
    }
}

void MainWindow::setupReloadStopButton(bool loading) {
    static QIcon stopIcon(QStringLiteral(":stop.png"));
    static QIcon stopIconDisabled(QStringLiteral(":stop_disabled.png"));
    static QIcon reloadIcon(QStringLiteral(":refresh.png"));
    static QIcon reloadIconDisabled(QStringLiteral(":refresh_disabled.png"));

    if (loading) {
        this->refreshWebAction = QWebEnginePage::Stop;
        this->barWidget->reloadButton->setEnabledIcon(stopIcon);
        this->barWidget->reloadButton->setDisabledIcon(stopIconDisabled);
    } else {
        this->refreshWebAction = QWebEnginePage::Reload;
        this->barWidget->reloadButton->setEnabledIcon(reloadIcon);
        this->barWidget->reloadButton->setDisabledIcon(reloadIconDisabled);
    }
}

void MainWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        this->barWidget->backButton->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        this->barWidget->forwardButton->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
    case QWebEnginePage::Stop:
        // Ignored events
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}

MainWindow::~MainWindow()
{
    delete this->webView;
    delete this->config;
}


