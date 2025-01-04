#ifndef WEBSOCKETCONTROL_H
#define WEBSOCKETCONTROL_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include "webpage.h"
#include "configuration.h"


QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketControl : public QObject
{
    Q_OBJECT
public:


    enum class Command {
        SetUrl,
        SetWindowMode,
        SetIdleTime,
        SetWhiteList,
        SetPermissions,
        SetNavbarVerticalPosition,
        SetNavbarHorizontalPosition,
        SetNavbarWidth,
        SetNavbarHeight,
        SetDisplayAddressBar,
        SetDisplayNavBar,
        SetUnderlayNavBar,
        SetHomePage,
        GetConfiguration,
        Unknown
    };


    explicit WebsocketControl(QHostAddress host, quint16 port, bool debug, Configuration *configuration, QObject *parent = nullptr);
    ~WebsocketControl();

    void onIdle();
    void onActive();
Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer *pWebSocketServer;
    QList<QWebSocket *> clients;
    bool debug;
    quint16 port;
    QHostAddress host;
    Configuration *configuration;
    void commandSetUrl(QJsonObject options, QWebSocket *pClient);

    void commandSetWindowMode(QJsonObject options, QWebSocket *pClient);
    void commandSetIdleTime(QJsonObject options, QWebSocket *pClient);
    void commandSetWhiteList(QJsonObject options, QWebSocket *pClient);
    void commandSetPermissions(QJsonObject options, QWebSocket *pClient);
    WebsocketControl::Command commandNameToCommand(QString name);
    QString commandToCommandName(WebsocketControl::Command command);
    QString buildResponse(bool isOk, QString message);
    QString buildResponse(bool isOk, QString message, WebsocketControl::Command command);
    QString buildResponse(bool isOk, QString message, WebsocketControl::Command command, QJsonObject data);
    void commandSetHomePage(QJsonObject options, QWebSocket *pClient);
    void commandSetNavbarVerticalPosition(QJsonObject options, QWebSocket *pClient);
    void commandSetNavbarHorizontalPosition(QJsonObject options, QWebSocket *pClient);
    void commandSetNavbarWidth(QJsonObject options, QWebSocket *pClient);
    void commandSetNavbarHeight(QJsonObject options, QWebSocket *pClient);
    void commandSetDisplayAddressBar(QJsonObject options, QWebSocket *pClient);

    void commandSetDisplayNavBar(QJsonObject options, QWebSocket *pClient);
    void commandSetUnderlayNavBar(QJsonObject options, QWebSocket *pClient);
    void commandGetConfiguration(QJsonObject options, QWebSocket *pClient);
    void broadcastMessage(QString message);
    void onEvent(QString eventName, QString eventMessage);

signals:
    void urlChange(QString url);
    void homePageUrlChange(QString homePageUrl);
    void windowModeChange(QWindow::Visibility windowMode);
    void idleTimeChange(int idleTime);
    void whiteListChange(QStringList whitelist);
    void permissionsChange(WebPage::Permissions permissions);
    void navbarVerticalPositionsChange(BarWidget::VerticalPosition verticalPosition);
    void navbarHorizontalPositionsChange(BarWidget::HorizontalPosition horizontalPosition);
    void navbarWidthChange(int navbarWidth);
    void navbarHeightChange(int navbarHeight);
    void displayAddressBarChange(bool displayAddressBar);
    void displayNavBarChange(bool displayNavBar);
    void underlayNavBarChange(bool underlayNavBar);
};

#endif // WEBSOCKETCONTROL_H
