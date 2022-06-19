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
    int idleTime; // Timeout to reset the browser to initial state, 0 is disabled
    QStringList whiteList; // List of whitelisted URLs in GLOB format
    //bool navbarEnabled; // Is navbar displayed
    //QStringList navbarButtons; // Enabled buttons on navbar, order matters ['home', 'reload', 'back', 'forward']
    BarWidget::VerticalPosition navbarVerticalPosition;
    BarWidget::HorizontalPosition navbarHorizontalPosition;
    WebPage::Permissions permissions;
    int navbarWidth; //%
    int navbarHeight; //%


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

    bool isFullscreen();
    QUrl getUrl();
    int getNavbarWidth();
    int getNavbarHeight();
    int getIdleTime();
    QStringList getWhiteList();
    BarWidget::VerticalPosition getNavbarVerticalPosition();
    BarWidget::HorizontalPosition getNavbarHorizontalPosition();
    WebPage::Permissions getPermissions();

signals:

};

#endif // CONFIGURATION_H
