#include "websocketcontrol.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"


WebsocketControl::WebsocketControl(quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Qiosk Control Server"), QWebSocketServer::NonSecureMode, this)),
    m_debug(debug)
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        if (m_debug) {
            qDebug() << "Qiosk Control Server listening on port" << port;
        }

        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &WebsocketControl::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WebsocketControl::closed);
    }
}

WebsocketControl::~WebsocketControl()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void WebsocketControl::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebsocketControl::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebsocketControl::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebsocketControl::socketDisconnected);

    m_clients << pSocket;
}

void WebsocketControl::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (!pClient) {
        qWarning() << "WTH? no client detected";
        return;
    }


    if (m_debug)
        qDebug() << "Message received:" << message;

    QJsonDocument jsonRequest = QJsonDocument::fromJson(message.toUtf8());
    if (jsonRequest.isNull())
    {
        pClient->sendTextMessage(this->buildResponse(false, "Request is not a valid JSON!"));
        return;
    }

    if (!jsonRequest.isObject())
    {
        pClient->sendTextMessage(this->buildResponse(false, "Request is not a valid JSON object!"));
        return;
    }

    QJsonObject jsonObject = jsonRequest.object();
    QJsonValue commandValue = jsonObject.value("command");
    if (!commandValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "Command has to be string!"));
        return;
    }
    QString commandName = commandValue.toString();
    WebsocketControl::Command command = this->commandNameToCommand(commandName);
    if (command == WebsocketControl::Command::Unknown) {
        pClient->sendTextMessage(this->buildResponse(false, "Unknown command"));
        return;
    }

    QJsonValue optionsValue = jsonObject.value("options");
    if (!optionsValue.isObject()) {
        pClient->sendTextMessage(this->buildResponse(false, "Options has to be object", command));
        return;
    }
    QJsonObject options = optionsValue.toObject();

    switch(command){
        case WebsocketControl::Command::SetUrl:
            this->commandSetUrl(options, pClient);
            break;
        case WebsocketControl::Command::SetWindowMode:
            this->commandSetWindowMode(options, pClient);
            break;
        case WebsocketControl::Command::SetIdleTime:
            this->commandSetIdleTime(options, pClient);
            break;
        case WebsocketControl::Command::SetWhiteList:
            this->commandSetWhiteList(options, pClient);
            break;
        case WebsocketControl::Command::SetPermissions:
            this->commandSetPermissions(options, pClient);
            break;
        case WebsocketControl::Command::SetNavbarVerticalPosition:
            this->commandSetNavbarVerticalPosition(options, pClient);
            break;
        case WebsocketControl::Command::SetNavbarHorizontalPosition:
            this->commandSetNavbarHorizontalPosition(options, pClient);
            break;
        case WebsocketControl::Command::SetNavbarWidth:
            this->commandSetNavbarWidth(options, pClient);
            break;
        case WebsocketControl::Command::SetNavbarHeight:
            this->commandSetNavbarHeight(options, pClient);
            break;
        case WebsocketControl::Command::SetDisplayAddressBar:
            this->commandSetDisplayAddressBar(options, pClient);
            break;
        case WebsocketControl::Command::SetDisplayNavBar:
            this->commandSetDisplayNavBar(options, pClient);
            break;
        case WebsocketControl::Command::SetUnderlayNavBar:
            this->commandSetUnderlayNavBar(options, pClient);
            break;
        case WebsocketControl::Command::SetOptions:
            this->commandSetOptions(options, pClient);
            break;
        case WebsocketControl::Command::Unknown:
            Q_ASSERT(command == WebsocketControl::Command::Unknown);
            return;
            break;
    }
}

WebsocketControl::Command WebsocketControl::commandNameToCommand(QString name){
    QStringList commandNames = {
        "setUrl",
        "setWindowMode",
        "setIdleTime",
        "setWhiteList",
        "setPermissions",
        "setNavbarVerticalPosition",
        "setNavbarHorizontalPosition",
        "setNavbarWidth",
        "setNavbarHeight",
        "setDisplayAddressBar",
        "setDisplayNavBar",
        "setUnderlayNavBar",
        "setOptions"
    };

    switch(commandNames.indexOf(name)){
        case 0:
            return WebsocketControl::Command::SetUrl;
            break;
        case 1:
            return WebsocketControl::Command::SetWindowMode;
            break;
        case 2:
            return WebsocketControl::Command::SetIdleTime;
            break;
        case 3:
            return WebsocketControl::Command::SetWhiteList;
            break;
        case 4:
            return WebsocketControl::Command::SetPermissions;
            break;
        case 5:
            return WebsocketControl::Command::SetNavbarVerticalPosition;
            break;
        case 6:
            return WebsocketControl::Command::SetNavbarHorizontalPosition;
            break;
        case 7:
            return WebsocketControl::Command::SetNavbarWidth;
            break;
        case 8:
            return WebsocketControl::Command::SetNavbarHeight;
            break;
        case 9:
            return WebsocketControl::Command::SetDisplayAddressBar;
            break;
        case 10:
            return WebsocketControl::Command::SetDisplayNavBar;
            break;
        case 11:
            return WebsocketControl::Command::SetUnderlayNavBar;
            break;
        case 12:
            return WebsocketControl::Command::SetOptions;
            break;
        default:
            return WebsocketControl::Command::Unknown;
            break;
    }
}

QString WebsocketControl::commandToCommandName(WebsocketControl::Command command) {
    switch(command){
        case WebsocketControl::Command::SetUrl:
            return "setUrl";
            break;
        case WebsocketControl::Command::SetWindowMode:
            return "setWindowMode";
            break;
        case WebsocketControl::Command::SetIdleTime:
            return "setIdleTime";
            break;
        case WebsocketControl::Command::SetWhiteList:
            return "setWhiteList";
            break;
        case WebsocketControl::Command::SetPermissions:
            return "setPermissions";
            break;
        case WebsocketControl::Command::SetNavbarVerticalPosition:
            return "setNavbarVerticalPosition";
            break;
        case WebsocketControl::Command::SetNavbarHorizontalPosition:
            return "setNavbarHorizontalPosition";
            break;
        case WebsocketControl::Command::SetNavbarWidth:
            return "setNavbarWidth";
            break;
        case WebsocketControl::Command::SetNavbarHeight:
            return "setNavbarHeight";
            break;
        case WebsocketControl::Command::SetDisplayAddressBar:
            return "setDisplayAddressBar";
            break;
        case WebsocketControl::Command::SetDisplayNavBar:
            return "setDisplayNavBar";
            break;
        case WebsocketControl::Command::SetUnderlayNavBar:
            return "setUnderlayNavBar";
            break;
        case WebsocketControl::Command::SetOptions:
            return "setOptions";
            break;
        case WebsocketControl::Command::Unknown:
            Q_ASSERT(command == WebsocketControl::Command::Unknown);
            return "unknown"; // Just make lint happy
            break;
    }
}


QString WebsocketControl::buildResponse(bool isOk, QString message, WebsocketControl::Command command) {
    QJsonObject errordObject;
    errordObject.insert("isOk", QJsonValue::fromVariant(isOk));
    errordObject.insert("message", QJsonValue::fromVariant(message));
    errordObject.insert("command", QJsonValue::fromVariant(this->commandToCommandName(command)));

    QJsonDocument jsonDocument(errordObject);

    return jsonDocument.toJson();
}

QString WebsocketControl::buildResponse(bool isOk, QString message) {
    QJsonObject errordObject;
    errordObject.insert("isOk", QJsonValue::fromVariant(isOk));
    errordObject.insert("message", QJsonValue::fromVariant(message));

    QJsonDocument jsonDocument(errordObject);

    return jsonDocument.toJson();
}

void WebsocketControl::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void WebsocketControl::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

/**********************
 * Commands
 **********************/

void WebsocketControl::commandSetUrl(QJsonObject options, QWebSocket *pClient) {
    // set_url has only one option and that is url
    QJsonValue urlValue = options.value("url");
    if (!urlValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "url has to be string", WebsocketControl::Command::SetUrl));
        return;
    }

    QString url = urlValue.toString();
    // Url is string, emit event and notify user
    emit urlChange(url);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetUrl));
}


void WebsocketControl::commandSetWindowMode(QJsonObject options, QWebSocket *pClient) {
    //showNormal() : showFullScreen();
    // setWindowMode has only one option and that is fullscreen
    QJsonValue fullscreenValue = options.value("fullscreen");
    if (!fullscreenValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "fullscreenValue has to be boolean", WebsocketControl::Command::SetWindowMode));
        return;
    }

    bool fullscreen = fullscreenValue.toBool();
    // Url is string, emit event and notify user
    emit fullscreenChange(fullscreen);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetWindowMode));
}

void WebsocketControl::commandSetIdleTime(QJsonObject options, QWebSocket *pClient) {
    // setIdleTime has only one option and that is idleTime
    QJsonValue idleTimeValue = options.value("idleTime");
    if (!idleTimeValue.isDouble()) {
        pClient->sendTextMessage(this->buildResponse(false, "idleTime has to be number", WebsocketControl::Command::SetIdleTime));
        return;
    }

    int idleTime = idleTimeValue.toInt();
    // Url is string, emit event and notify user
    emit idleTimeChange(idleTime);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetIdleTime));
}

void WebsocketControl::commandSetWhiteList(QJsonObject options, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue whitelistValue = options.value("whitelist");
    if (!whitelistValue.isArray()) {
        pClient->sendTextMessage(this->buildResponse(false, "whitelist has to be array", WebsocketControl::Command::SetWhiteList));
        return;
    }

    QStringList whitelist = whitelistValue.toVariant().toStringList();

    // Url is string, emit event and notify user
    emit whiteListChange(whitelist);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetWhiteList));
}

void WebsocketControl::commandSetPermissions(QJsonObject options, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue permissionsValue = options.value("permissions");
    if (!permissionsValue.isArray()) {
        pClient->sendTextMessage(this->buildResponse(false, "permissions has to be array", WebsocketControl::Command::SetPermissions));
        return;
    }

    // Url is string, emit event and notify user
    WebPage::Permissions permissions = WebPage::namesToWebPagePermissions(permissionsValue.toVariant().toStringList());
    if(permissions.testFlag(WebPage::Permission::Unknown)){
        pClient->sendTextMessage(this->buildResponse(false, "Unknown permission was provided", WebsocketControl::Command::SetPermissions));
        return;
    }
    emit permissionsChange(permissions);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetPermissions));
}

void WebsocketControl::commandSetNavbarVerticalPosition(QJsonObject options, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue navbarVerticalPositionValue = options.value("navbarVerticalPosition");
    if (!navbarVerticalPositionValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarVerticalPosition has to be string", WebsocketControl::Command::SetNavbarVerticalPosition));
        return;
    }

    BarWidget::VerticalPosition barWidgetVerticalPosition = BarWidget::nameToBarWidgetVerticalPosition(navbarVerticalPositionValue.toString());
    if (barWidgetVerticalPosition == BarWidget::VerticalPosition::Unknown) {
        pClient->sendTextMessage(this->buildResponse(false, "invalid navbarVerticalPosition", WebsocketControl::Command::SetNavbarVerticalPosition));
        return;
    }

    emit navbarVerticalPositionsChange(barWidgetVerticalPosition);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetNavbarVerticalPosition));
}

void WebsocketControl::commandSetNavbarHorizontalPosition(QJsonObject options, QWebSocket *pClient) {
    QJsonValue navbarHorizontalPositionValue = options.value("navbarHorizontalPosition");
    if (!navbarHorizontalPositionValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarHorizontalPosition has to be string", WebsocketControl::Command::SetNavbarHorizontalPosition));
        return;
    }

    BarWidget::HorizontalPosition barWidgetHorizontalPosition = BarWidget::nameToBarWidgetHorizontalPosition(navbarHorizontalPositionValue.toString());
    if (barWidgetHorizontalPosition == BarWidget::HorizontalPosition::Unknown) {
        pClient->sendTextMessage(this->buildResponse(false, "invalid navbarHorizontalPosition", WebsocketControl::Command::SetNavbarHorizontalPosition));
        return;
    }

    emit navbarHorizontalPositionsChange(barWidgetHorizontalPosition);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetNavbarHorizontalPosition));
}

void WebsocketControl::commandSetNavbarWidth(QJsonObject options, QWebSocket *pClient) {
    QJsonValue navbarWidthValue = options.value("navbarWidth");
    if (!navbarWidthValue.isDouble()) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarWidth has to be number", WebsocketControl::Command::SetNavbarWidth));
        return;
    }

    int navbarWidth = navbarWidthValue.toInt();
    if (navbarWidth < 0 || navbarWidth > 100) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarWidth has to be bigger then 0 and smaller then 100", WebsocketControl::Command::SetNavbarWidth));
        return;
    }

    emit navbarWidthChange(navbarWidth);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetNavbarWidth));
}

void WebsocketControl::commandSetNavbarHeight(QJsonObject options, QWebSocket *pClient) {
    QJsonValue navbarHeightValue = options.value("navbarHeight");
    if (!navbarHeightValue.isDouble()) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarHeight has to be number", WebsocketControl::Command::SetNavbarHeight));
        return;
    }

    int navbarHeight = navbarHeightValue.toInt();
    if (navbarHeight < 0 || navbarHeight > 100) {
        pClient->sendTextMessage(this->buildResponse(false, "navbarHeight has to be bigger then 0 and smaller then 100", WebsocketControl::Command::SetNavbarHeight));
        return;
    }

    emit navbarHeightChange(navbarHeight);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetNavbarHeight));
}

void WebsocketControl::commandSetDisplayAddressBar(QJsonObject options, QWebSocket *pClient) {
    QJsonValue displayAddressBarValue = options.value("displayAddressBar");
    if (!displayAddressBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "displayAddressBar has to be bool", WebsocketControl::Command::SetDisplayAddressBar));
        return;
    }

    emit displayAddressBarChange(displayAddressBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetDisplayAddressBar));
}

void WebsocketControl::commandSetDisplayNavBar(QJsonObject options, QWebSocket *pClient) {
    QJsonValue displayNavBarValue = options.value("displayNavBar");
    if (!displayNavBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "displayNavBar has to be bool", WebsocketControl::Command::SetDisplayNavBar));
        return;
    }

    emit displayNavBarChange(displayNavBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetDisplayNavBar));
}

void WebsocketControl::commandSetUnderlayNavBar(QJsonObject options, QWebSocket *pClient) {
    QJsonValue underlayNavBarValue = options.value("underlayNavBar");
    if (!underlayNavBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "underlayNavBar has to be bool", WebsocketControl::Command::SetUnderlayNavBar));
        return;
    }

    emit underlayNavBarChange(underlayNavBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetUnderlayNavBar));
}

void WebsocketControl::commandSetOptions(QJsonObject options, QWebSocket *pClient) {
    QJsonValue urlValue = options.value("url");
    if (!urlValue.isUndefined()) {
        this->commandSetUrl(options, pClient);
    }

    QJsonValue fullscreenValue = options.value("fullscreen");
    if (!fullscreenValue.isUndefined()) {
        this->commandSetWindowMode(options, pClient);
    }

    QJsonValue idleTimeValue = options.value("idleTime");
    if (!idleTimeValue.isUndefined()) {
        this->commandSetIdleTime(options, pClient);
    }

    QJsonValue whitelistValue = options.value("whitelist");
    if (!whitelistValue.isUndefined()) {
        this->commandSetWhiteList(options, pClient);
    }

    QJsonValue permissionsValue = options.value("permissions");
    if (!permissionsValue.isUndefined()) {
        this->commandSetPermissions(options, pClient);
    }

    QJsonValue navbarVerticalPositionValue = options.value("navbarVerticalPosition");
    if (!navbarVerticalPositionValue.isUndefined()) {
        this->commandSetNavbarVerticalPosition(options, pClient);
    }

    QJsonValue navbarHorizontalPositionValue = options.value("navbarHorizontalPosition");
    if (!navbarHorizontalPositionValue.isUndefined()) {
        this->commandSetNavbarHorizontalPosition(options, pClient);
    }

    QJsonValue navbarWidthValue = options.value("navbarWidth");
    if (!navbarWidthValue.isUndefined()) {
        this->commandSetNavbarWidth(options, pClient);
    }

    QJsonValue navbarHeightValue = options.value("navbarHeight");
    if (!navbarHeightValue.isUndefined()) {
        this->commandSetNavbarHeight(options, pClient);
    }

    QJsonValue displayAddressBarValue = options.value("displayAddressBar");
    if (!displayAddressBarValue.isUndefined()) {
        this->commandSetDisplayAddressBar(options, pClient);
    }

    QJsonValue displayNavBarValue = options.value("displayNavBar");
    if (!displayNavBarValue.isUndefined()) {
        this->commandSetDisplayNavBar(options, pClient);
    }

    QJsonValue underlayNavBarValue = options.value("underlayNavBar");
    if (!underlayNavBarValue.isUndefined()) {
        this->commandSetUnderlayNavBar(options, pClient);
    }

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetOptions));
}
