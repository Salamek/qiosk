#include "configuration.h"

Configuration::Configuration(QObject *parent)
    : QObject{parent}
{
    this->url = QUrl("https://salamek.github.io/chromium-kiosk/");
    this->fullscreen = false;
    this->touchscreen = true;
    this->idleTime = 0;
    this->whiteList = QStringList();
    this->navbarVerticalPosition = BarVerticalPositionEnum::Bottom;
    this->navbarHorizontalPosition = BarHorizontalPositionEnum::Center;
    this->navbarWidth = 25; //%
    this->navbarHeight = 5; //%
}


void Configuration::setUrl(QUrl url) {
    this->url = url;
}

void Configuration::setFullscreen(bool fullscreen) {
    this->fullscreen = fullscreen;
}

void Configuration::setTouchscreen(bool touchscreen) {
    this->touchscreen = touchscreen;
}

void Configuration::setIdleTime(int idleTime) {
    this->idleTime = idleTime;
}

void Configuration::setWhiteList(QStringList whiteList) {
    this->whiteList = whiteList;
}

void Configuration::setNavbarVerticalPosition(BarVerticalPositionEnum navbarVerticalPosition) {
    this->navbarVerticalPosition = navbarVerticalPosition;
}

void Configuration::setNavbarHorizontalPosition(BarHorizontalPositionEnum navbarHorizontalPosition) {
    this->navbarHorizontalPosition = navbarHorizontalPosition;
}

void Configuration::setNavbarWidth(int navbarWidth) {
    if (navbarWidth > 100 || navbarWidth < 1) {
        qFatal("Width cant be bigger then 100 or smaller then 1");
    }
    this->navbarWidth = navbarWidth;
}

void Configuration::setNavbarHeight(int navbarHeight) {
    if (navbarHeight > 100 || navbarHeight < 1) {
        qFatal("Height cant be bigger then 100 or smaller then 1");
    }
    this->navbarHeight = navbarHeight;
}


bool Configuration::isFullscreen() {
    return this->fullscreen;
}

QUrl Configuration::getUrl() {
    return this->url;
}

int Configuration::getNavbarWidth() {
    return this->navbarWidth;
}

int Configuration::getNavbarHeight() {
    return this->navbarHeight;
}

int Configuration::getIdleTime() {
    return this->idleTime;
}

BarVerticalPositionEnum Configuration::getNavbarVerticalPosition() {
    return this->navbarVerticalPosition;
}

BarHorizontalPositionEnum Configuration::getNavbarHorizontalPosition() {
    return this->navbarHorizontalPosition;
}

QStringList Configuration::getWhiteList() {
    return this->whiteList;
}
