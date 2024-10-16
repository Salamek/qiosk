#include "mainwindow.h"



MainWindow::MainWindow(Configuration *config, QWidget *parent)
    : QMainWindow(parent),
    config(config)
{
    this->isIdle = false;
    this->resetHistoryLock = false;
    this->refreshWebAction = QWebEnginePage::Reload;
    this->setHomePageUrl(this->config->getUrl());
    this->lastUserActivity = QDateTime::currentSecsSinceEpoch(); // Set last user activity to NOW

    // Start websocket control server
    this->websocketControl = new WebsocketControl(QHostAddress::LocalHost, 1791, false, this->config);

    // Initialize connection checker
    this->connectionChecker = new ConnectionChecker(this);

    this->webView = new WebView();

    QWebEngineProfile *profile = (this->config->getProfileName() == "default" ? QWebEngineProfile::defaultProfile() : new QWebEngineProfile(this->config->getProfileName()));

    // Make sure correct cookie settings are set

    profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    // Set AcceptLanguage header
    QString acceptLanguage;
    if (this->config->getAcceptLanguage().isNull()) {
#if QT_VERSION <= QT_VERSION_CHECK(6, 7, 0)
        acceptLanguage = QLocale().name().replace("_", "-");
#else
        acceptLanguage = QLocale().name(QLocale::TagSeparator::Dash);
#endif
    } else {
        acceptLanguage = this->config->getAcceptLanguage();
    }

    profile->setHttpAcceptLanguage(acceptLanguage);

    // Set UserAgent header
    QString userAgent;
    if (this->config->getUserAgent().isNull()) {
        // User-Agent not set, set our default
        userAgent = profile->httpUserAgent();

        QStringList newAppString(QCoreApplication::applicationName());
        newAppString << QCoreApplication::applicationVersion();

        static QRegularExpression re = QRegularExpression("QtWebEngine/\\S+");
        userAgent = userAgent.replace(re, newAppString.join("/"));
    } else {
        userAgent = this->config->getUserAgent();
    }

    profile->setHttpUserAgent(userAgent);


    WebPage *webPage = new WebPage(profile, this->webView);
    webPage->setWhiteList(this->config->getWhiteList());
    webPage->setPermissions(this->config->getPermissions());
    QWebEngineSettings *settings = webPage->settings();

    settings->setAttribute(QWebEngineSettings::ShowScrollBars, this->config->isDisplayScrollBars());

    settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    settings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
#endif

    this->webView->setPage(webPage);
    this->webView->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);


    this->mainLayout = new QVBoxLayout();
    this->mainLayout->setContentsMargins(0, 0, 0, 0);
    this->mainLayout->setSpacing(0);

    QAction *favAction = new QAction(this);
    this->addressBar = new QLineEdit(this);
    this->addressBar->setClearButtonEnabled(true);
    this->addressBar->addAction(favAction, QLineEdit::LeadingPosition);
    this->setDisplayAddressBar(this->config->isDisplayAddressBar());

    this->mainLayout->addWidget(this->addressBar);

    this->mainLayout->addWidget(this->webView);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    this->barWidget = new BarWidget(this);
    this->setDisplayNavBar(this->config->isDisplayNavBar());

    this->setUnderlayNavBar(this->config->isUnderlayNavBar());

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

    connect(this->webView, &QWebEngineView::urlChanged, this, [this](const QUrl &url) {
        this->addressBar->setText(url.toDisplayString());
    });
    connect(addressBar, &QLineEdit::returnPressed, this, [this]() {
        this->webView->setUrl(QUrl::fromUserInput(this->addressBar->text()));
    });

    connect(this->webView, &QWebEngineView::iconChanged, favAction, &QAction::setIcon);

    //Reset timer events
    connect(this->resetTimer, &ResetTimer::timeout, this, &MainWindow::checkReset);

    // Websocket control events
    connect(this->websocketControl, &WebsocketControl::urlChange, this->webView, &WebView::setUrl);
    connect(this->websocketControl, &WebsocketControl::homePageUrlChange, this, [this](QString homePageUrl) {
        this->setHomePageUrl(QUrl(homePageUrl));
    });
    connect(this->websocketControl, &WebsocketControl::fullscreenChange, this, [this](bool fullscreen) {
        fullscreen ? showFullScreen() : showNormal();
    });
    connect(this->websocketControl, &WebsocketControl::idleTimeChange, this, &MainWindow::setIdleTime);
    connect(this->websocketControl, &WebsocketControl::whiteListChange, webPage, &WebPage::setWhiteList);
    connect(this->websocketControl, &WebsocketControl::permissionsChange, webPage, &WebPage::setPermissions);
    connect(this->websocketControl, &WebsocketControl::navbarVerticalPositionsChange, this->barWidget, &BarWidget::setVerticalPosition);
    connect(this->websocketControl, &WebsocketControl::navbarHorizontalPositionsChange, this->barWidget, &BarWidget::setHorizontalPosition);
    connect(this->websocketControl, &WebsocketControl::navbarWidthChange, this->barWidget, &BarWidget::setWidth);
    connect(this->websocketControl, &WebsocketControl::navbarHeightChange, this->barWidget, &BarWidget::setHeight);
    connect(this->websocketControl, &WebsocketControl::displayAddressBarChange, this, &MainWindow::setDisplayAddressBar);
    connect(this->websocketControl, &WebsocketControl::displayNavBarChange, this, &MainWindow::setDisplayNavBar);
    connect(this->websocketControl, &WebsocketControl::underlayNavBarChange, this, &MainWindow::setUnderlayNavBar);

    // Connection checker events
    connect(this->connectionChecker, &ConnectionChecker::urlAccessible, this, [this]() {
        qDebug() << "URL is accessible. Reloading the page.";
        this->doReload();
    });



    // ============================
    // Initial Configuration
    // ============================
    this->barWidget->setHeight(this->config->getNavbarHeight()); //%
    this->barWidget->setWidth(this->config->getNavbarWidth()); //%
    this->barWidget->setHorizontalPosition(this->config->getNavbarHorizontalPosition());
    this->barWidget->setVerticalPosition(this->config->getNavbarVerticalPosition());
    this->barWidget->setNavBarEnabledButtons(this->config->getNavBarEnabledButtons());

    // Configure reset timer, disable this if timeout==0
    this->setIdleTime(this->config->getIdleTime());

    // ============================
    // End of initial configuration
    // ============================

    //Load URL
    this->webView->setUrl(this->initialUrl);

    //!FIXME Hack
    //! Hack to fix bar position when sometines resizeEvent fails to trigger with correct window size when starting in fullscreen, so bar is calculated in wrong position
    QTimer::singleShot(2000, this, &MainWindow::plotTimerHack);
}


void MainWindow::goHome() {
    this->webView->load(this->initialUrl);

    //Reset zoom on home button press
    this->webView->setZoomFactor(1.0);
}

void MainWindow::doReload() {
    this->webView->triggerPageAction(this->refreshWebAction);
}


void MainWindow::checkReset() {
    if (this->idleTime > 0 && this->lastUserActivity + this->idleTime <= QDateTime::currentSecsSinceEpoch() && !this->isIdle) {
        this->doReset();
        this->websocketControl->onIdle();
        this->isIdle = true;
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

    // Reset cookies
    this->webView->page()->profile()->cookieStore()->deleteAllCookies();

    // Clear visited links
    this->webView->page()->profile()->clearAllVisitedLinks();
}


void MainWindow::onUserActivity() {
    // Check if we are comming out from idle state or not
    if (this->isIdle) {
        this->websocketControl->onActive();
        this->isIdle = false;
    }

    this->lastUserActivity = QDateTime::currentSecsSinceEpoch();
}

void MainWindow::plotTimerHack() {
    this->plot(this->size());
}

void MainWindow::plot(QSize size) {
    this->barWidget->plot(size);
    this->progressBar->plot(size);
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
    if (!ok) {
        qDebug() << "Load finished with error. Checking URL accessibility.";
        this->connectionChecker->checkUrlAccessibility(this->webView->url());
    }

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

void MainWindow::setIdleTime(int idleTime){
    this->idleTime = idleTime;
    if (this->idleTime != 0) {
        this->resetTimer->setTimeout(1); //Check each 1s
        this->resetTimer->start();
    } else {
        this->resetTimer->stop();
    }
}

void MainWindow::setDisplayAddressBar(bool displayAddressBar){
    if (displayAddressBar) {
        this->addressBar->show();
    } else {
        this->addressBar->hide();
    }
}

void MainWindow::setDisplayNavBar(bool displayNavBar){
    if (displayNavBar) {
        this->barWidget->show();
    } else {
        this->barWidget->hide();
    }
}

void MainWindow::setUnderlayNavBar(bool underlayNavBar) {
    // State is same, no nothing
    if (this->underlayNavBar == underlayNavBar) {
        return;
    }

    if (underlayNavBar) {
        this->mainLayout->addWidget(this->barWidget);
    } else {
        this->mainLayout->removeWidget(this->barWidget);
    }

    this->underlayNavBar = underlayNavBar;
}


void MainWindow::setNavBarEnabledButtons(QStringList enabledButtons) {
    this->barWidget->setNavBarEnabledButtons(BarWidget::buttonNamesToButtons(enabledButtons));
}

void MainWindow::setHomePageUrl(QUrl homePageUrl) {
    this->initialUrl = homePageUrl;
}

MainWindow::~MainWindow()
{
    delete this->websocketControl;
    delete this->webView;
    delete this->config;
}


