#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QUrl>
#include "enum.h"

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
    BarVerticalPositionEnum navbarVerticalPosition;
    BarHorizontalPositionEnum navbarHorizontalPosition;
    int navbarWidth; //%
    int navbarHeight; //%


    void setUrl(QUrl url);
    void setFullscreen(bool fullscreen);
    void setTouchscreen(bool touchscreen);
    void setIdleTime(int idleTime);
    void setWhiteList(QStringList whiteList);
    void setNavbarVerticalPosition(BarVerticalPositionEnum navbarVerticalPosition);
    void setNavbarHorizontalPosition(BarHorizontalPositionEnum navbarHorizontalPosition);
    void setNavbarWidth(int navbarWidth);
    void setNavbarHeight(int navbarHeight);

    bool isFullscreen();
    QUrl getUrl();
    int getNavbarWidth();
    int getNavbarHeight();
    int getIdleTime();
    QStringList getWhiteList();
    BarVerticalPositionEnum getNavbarVerticalPosition();
    BarHorizontalPositionEnum getNavbarHorizontalPosition();

signals:

};

#endif // CONFIGURATION_H
