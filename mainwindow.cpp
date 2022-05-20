#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->refreshWebAction = QWebEnginePage::Reload;
    this->initialUrl = QUrl("https://blacklist.salamek.cz");
    this->webView = new WebView();
    this->webView->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    this->webView->setUrl(this->initialUrl);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(this->webView);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    this->barWidget = new BarWidget(this);
    this->progressBar = new ProgressBarWidget(this);

    //Bar events
    connect(this->barWidget->homeButton, &QPushButton::clicked, this, &MainWindow::goHome);
    connect(this->barWidget->reloadButton, &QPushButton::clicked, this, &MainWindow::doReload);
    connect(this->barWidget->forwardButton, &QPushButton::clicked, this->webView, &QWebEngineView::forward);
    connect(this->barWidget->backButton, &QPushButton::clicked, this->webView, &QWebEngineView::back);

    //Enable Buttons that are not needed to be disabled
    this->barWidget->homeButton->setEnabled(true);
    this->barWidget->reloadButton->setEnabled(true);

    //WebView events
    connect(this->webView, &QWebEngineView::loadProgress, this, &MainWindow::handleWebViewLoadProgress);
    connect(this->webView, &WebView::webActionEnabledChanged, this, &MainWindow::handleWebActionEnabledChanged);

    // Initial Configuration
    this->barWidget->setHeight(5); //%
    this->barWidget->setWidth(50); //%
    this->barWidget->setHorizontalPosition(BarHorizontalPositionEnum::Center);
    this->barWidget->setVerticalPosition(BarVerticalPositionEnum::Bottom);
    QRect windowGeometry = this->geometry();
    this->barWidget->plot(windowGeometry.width(), windowGeometry.height());
    this->progressBar->plot(windowGeometry.width(), windowGeometry.height());
}


void MainWindow::goHome() {
    this->webView->load(this->initialUrl);
}

void MainWindow::doReload() {
    this->webView->triggerPageAction(this->refreshWebAction);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QSize size = event->size();
    this->barWidget->plot(size.width(), size.height());
    this->progressBar->plot(size.width(), size.height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":stop.png"));
    static QIcon reloadIcon(QStringLiteral(":refresh.png"));

    if (0 < progress && progress < 100) {
        this->refreshWebAction = QWebEnginePage::Stop;
        this->barWidget->reloadButton->setIcon(stopIcon);
        this->progressBar->setValue(progress);
    } else {
        this->refreshWebAction = QWebEnginePage::Reload;
        this->barWidget->reloadButton->setIcon(reloadIcon);
        this->progressBar->setValue(0);
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
}


