#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QUrl>
#include "barwidget.h"
#include "webpage.h"

class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = nullptr);
    QUrl url;
    bool fullscreen; // Fullscreen enabled
    bool touchscreen; // @TODO Touchscreen support enabled
    bool displayAddressBar;  // Is address bar displayed?
    bool displayNavBar;  // Is navbar displayed?
    bool displayScrollBars; // Are scroll bars displayed
    bool underlayNavBar;  // Is navbar floating?
    int idleTime; // Timeout to reset the browser to initial state, 0 is disabled
    QStringList whiteList; // List of whitelisted URLs in GLOB format
    //bool navbarEnabled; // Is navbar displayed
    //QStringList navbarButtons; // Enabled buttons on navbar, order matters ['home', 'reload', 'back', 'forward']
    BarWidget::VerticalPosition navbarVerticalPosition;
    BarWidget::HorizontalPosition navbarHorizontalPosition;
    WebPage::Permissions permissions;
    QList<BarWidget::Button> navBarEnabledButtons;
    int navbarWidth; //%
    int navbarHeight; //%
    QString profileName;
    QString acceptLanguage;
    QString userAgent;


    void setUrl(QUrl url);
    void setFullscreen(bool fullscreen);
    void setTouchscreen(bool touchscreen);
    void setIdleTime(int idleTime);
    void setWhiteList(QStringList whiteList);
    void setNavbarVerticalPosition(BarWidget::VerticalPosition navbarVerticalPosition);
    void setNavbarHorizontalPosition(BarWidget::HorizontalPosition navbarHorizontalPosition);
    void setNavbarWidth(int navbarWidth);
    void setNavbarHeight(int navbarHeight);
    void setPermissions(WebPage::Permissions permissions);
    void setDisplayAddressBar(bool displayAddressBar);
    void setDisplayNavBar(bool displayNavBar);
    void setUnderlayNavBar(bool underlayNavBar);
    void setProfileName(QString profileName);
    void setNavbarEnabledButtons(QList<BarWidget::Button> navBarEnabledButtons);
    void setAcceptLanguage(QString acceptLanguage);
    void setUserAgent(QString userAgent);

    bool isFullscreen();
    QUrl getUrl();
    int getNavbarWidth();
    int getNavbarHeight();
    int getIdleTime();
    QStringList getWhiteList();
    BarWidget::VerticalPosition getNavbarVerticalPosition();
    BarWidget::HorizontalPosition getNavbarHorizontalPosition();
    WebPage::Permissions getPermissions();
    bool isDisplayAddressBar();
    bool isDisplayNavBar();
    bool isUnderlayNavBar();
    QString getProfileName();

    bool isDisplayScrollBars();
    void setDisplayScrollBars(bool displayScrollBars);
    QList<BarWidget::Button> getNavBarEnabledButtons();
    QString getAcceptLanguage();
    QString getUserAgent();
signals:

};

#endif // CONFIGURATION_H
