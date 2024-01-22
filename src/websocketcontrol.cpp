#include "websocketcontrol.h"



WebsocketControl::WebsocketControl(QHostAddress host, quint16 port, bool debug, Configuration *configuration, QObject *parent) :
    QObject(parent),
    pWebSocketServer(new QWebSocketServer(QStringLiteral("Qiosk Control Server"), QWebSocketServer::NonSecureMode, this)),
    host(host),
    port(port),
    debug(debug),
    configuration(configuration)
{
    if (this->host != QHostAddress::Null && this->pWebSocketServer->listen(this->host, this->port)) {
        if (this->debug) {
            qDebug() << "Qiosk Control Server listening on port" << this->port;
        }

        connect(this->pWebSocketServer, &QWebSocketServer::newConnection, this, &WebsocketControl::onNewConnection);
        connect(this->pWebSocketServer, &QWebSocketServer::closed, this, &WebsocketControl::closed);
    }
}


WebsocketControl::~WebsocketControl()
{
    this->pWebSocketServer->close();
    qDeleteAll(this->clients.begin(), this->clients.end());
}

void WebsocketControl::onNewConnection()
{
    QWebSocket *pSocket = this->pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebsocketControl::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebsocketControl::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebsocketControl::socketDisconnected);

    this->clients << pSocket;
}


void WebsocketControl::onIdle() {
    this->onEvent("idle", "Browser went to idle state");
}

void WebsocketControl::onActive() {
    this->onEvent("active", "Browser went to active state");
}

void WebsocketControl::onEvent(QString eventName, QString eventMessage) {
    QJsonObject errordObject;
    errordObject.insert("message", QJsonValue::fromVariant(eventMessage));
    errordObject.insert("event", QJsonValue::fromVariant(eventName));

    QJsonDocument jsonDocument(errordObject);

    this->broadcastMessage(jsonDocument.toJson());
}


void WebsocketControl::broadcastMessage(QString message){
    if (this->clients.isEmpty()) {
        return;
    }

    for (QWebSocket *pClient : this->clients)
    {
        pClient->sendTextMessage(message);
    }
}


void WebsocketControl::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (!pClient) {
        qWarning() << "WTH? no client detected";
        return;
    }

    if (this->debug)
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

    QJsonValue dataValue = jsonObject.value("data");
    if (!dataValue.isObject()) {
        pClient->sendTextMessage(this->buildResponse(false, "data has to be object", command));
        return;
    }
    QJsonObject data = dataValue.toObject();

    switch(command){
        case WebsocketControl::Command::SetUrl:
            this->commandSetUrl(data, pClient);
            break;
        case WebsocketControl::Command::SetWindowMode:
            this->commandSetWindowMode(data, pClient);
            break;
        case WebsocketControl::Command::SetIdleTime:
            this->commandSetIdleTime(data, pClient);
            break;
        case WebsocketControl::Command::SetWhiteList:
            this->commandSetWhiteList(data, pClient);
            break;
        case WebsocketControl::Command::SetPermissions:
            this->commandSetPermissions(data, pClient);
            break;
        case WebsocketControl::Command::SetNavbarVerticalPosition:
            this->commandSetNavbarVerticalPosition(data, pClient);
            break;
        case WebsocketControl::Command::SetNavbarHorizontalPosition:
            this->commandSetNavbarHorizontalPosition(data, pClient);
            break;
        case WebsocketControl::Command::SetNavbarWidth:
            this->commandSetNavbarWidth(data, pClient);
            break;
        case WebsocketControl::Command::SetNavbarHeight:
            this->commandSetNavbarHeight(data, pClient);
            break;
        case WebsocketControl::Command::SetDisplayAddressBar:
            this->commandSetDisplayAddressBar(data, pClient);
            break;
        case WebsocketControl::Command::SetDisplayNavBar:
            this->commandSetDisplayNavBar(data, pClient);
            break;
        case WebsocketControl::Command::SetUnderlayNavBar:
            this->commandSetUnderlayNavBar(data, pClient);
            break;
        case WebsocketControl::Command::SetHomePage:
            this->commandSetHomePage(data, pClient);
            break;
        case WebsocketControl::Command::GetConfiguration:
            this->commandGetConfiguration(data, pClient);
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
        "setHomePage",
        "getConfiguration"
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
            return WebsocketControl::Command::SetHomePage;
            break;
        case 13:
            return WebsocketControl::Command::GetConfiguration;
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
        case WebsocketControl::Command::SetHomePage:
            return "setHomePage";
            break;
        case WebsocketControl::Command::GetConfiguration:
            return "getConfiguration";
            break;
        case WebsocketControl::Command::Unknown:
            Q_ASSERT(command == WebsocketControl::Command::Unknown);
            return "unknown"; // Just make lint happy
            break;
    }
}

QString WebsocketControl::buildResponse(bool isOk, QString message, WebsocketControl::Command command, QJsonObject data) {
    QJsonObject errordObject;
    errordObject.insert("isOk", QJsonValue::fromVariant(isOk));
    errordObject.insert("message", QJsonValue::fromVariant(message));
    errordObject.insert("command", QJsonValue::fromVariant(this->commandToCommandName(command)));
    errordObject.insert("data", QJsonValue::fromVariant(data));

    QJsonDocument jsonDocument(errordObject);

    return jsonDocument.toJson();
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
    if (this->debug)
        qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void WebsocketControl::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (this->debug)
        qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        this->clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

/**********************
 * Commands
 **********************/

void WebsocketControl::commandSetUrl(QJsonObject data, QWebSocket *pClient) {
    // set_url has only one option and that is url
    QJsonValue urlValue = data.value("url");
    if (!urlValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "url has to be string", WebsocketControl::Command::SetUrl));
        return;
    }

    QString url = urlValue.toString();
    // Url is string, emit event and notify user
    emit urlChange(url);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetUrl));
}


void WebsocketControl::commandSetWindowMode(QJsonObject data, QWebSocket *pClient) {
    //showNormal() : showFullScreen();
    // setWindowMode has only one option and that is fullscreen
    QJsonValue fullscreenValue = data.value("fullscreen");
    if (!fullscreenValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "fullscreenValue has to be boolean", WebsocketControl::Command::SetWindowMode));
        return;
    }

    bool fullscreen = fullscreenValue.toBool();
    // Url is string, emit event and notify user
    emit fullscreenChange(fullscreen);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetWindowMode));
}

void WebsocketControl::commandSetIdleTime(QJsonObject data, QWebSocket *pClient) {
    // setIdleTime has only one option and that is idleTime
    QJsonValue idleTimeValue = data.value("idleTime");
    if (!idleTimeValue.isDouble()) {
        pClient->sendTextMessage(this->buildResponse(false, "idleTime has to be number", WebsocketControl::Command::SetIdleTime));
        return;
    }

    int idleTime = idleTimeValue.toInt();
    // Url is string, emit event and notify user
    emit idleTimeChange(idleTime);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetIdleTime));
}

void WebsocketControl::commandSetWhiteList(QJsonObject data, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue whitelistValue = data.value("whitelist");
    if (!whitelistValue.isArray()) {
        pClient->sendTextMessage(this->buildResponse(false, "whitelist has to be array", WebsocketControl::Command::SetWhiteList));
        return;
    }

    QStringList whitelist = whitelistValue.toVariant().toStringList();

    // Url is string, emit event and notify user
    emit whiteListChange(whitelist);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetWhiteList));
}

void WebsocketControl::commandSetPermissions(QJsonObject data, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue permissionsValue = data.value("permissions");
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

void WebsocketControl::commandSetNavbarVerticalPosition(QJsonObject data, QWebSocket *pClient) {
    // SetWhiteList has only one option and that is whitelist
    QJsonValue navbarVerticalPositionValue = data.value("navbarVerticalPosition");
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

void WebsocketControl::commandSetNavbarHorizontalPosition(QJsonObject data, QWebSocket *pClient) {
    QJsonValue navbarHorizontalPositionValue = data.value("navbarHorizontalPosition");
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

void WebsocketControl::commandSetNavbarWidth(QJsonObject data, QWebSocket *pClient) {
    QJsonValue navbarWidthValue = data.value("navbarWidth");
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

void WebsocketControl::commandSetNavbarHeight(QJsonObject data, QWebSocket *pClient) {
    QJsonValue navbarHeightValue = data.value("navbarHeight");
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

void WebsocketControl::commandSetDisplayAddressBar(QJsonObject data, QWebSocket *pClient) {
    QJsonValue displayAddressBarValue = data.value("displayAddressBar");
    if (!displayAddressBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "displayAddressBar has to be bool", WebsocketControl::Command::SetDisplayAddressBar));
        return;
    }

    emit displayAddressBarChange(displayAddressBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetDisplayAddressBar));
}

void WebsocketControl::commandSetDisplayNavBar(QJsonObject data, QWebSocket *pClient) {
    QJsonValue displayNavBarValue = data.value("displayNavBar");
    if (!displayNavBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "displayNavBar has to be bool", WebsocketControl::Command::SetDisplayNavBar));
        return;
    }

    emit displayNavBarChange(displayNavBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetDisplayNavBar));
}

void WebsocketControl::commandSetUnderlayNavBar(QJsonObject data, QWebSocket *pClient) {
    QJsonValue underlayNavBarValue = data.value("underlayNavBar");
    if (!underlayNavBarValue.isBool()) {
        pClient->sendTextMessage(this->buildResponse(false, "underlayNavBar has to be bool", WebsocketControl::Command::SetUnderlayNavBar));
        return;
    }

    emit underlayNavBarChange(underlayNavBarValue.toBool());

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetUnderlayNavBar));
}


void WebsocketControl::commandSetHomePage(QJsonObject data, QWebSocket *pClient) {
    // set_url has only one option and that is url
    QJsonValue homePageUrlValue = data.value("homePageUrl");
    if (!homePageUrlValue.isString()) {
        pClient->sendTextMessage(this->buildResponse(false, "homePageUrl has to be string", WebsocketControl::Command::SetHomePage));
        return;
    }

    QString homePageUrl = homePageUrlValue.toString();
    // Url is string, emit event and notify user
    emit homePageUrlChange(homePageUrl);

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetHomePage));
}

/*
void WebsocketControl::commandSetOptions(QJsonObject data, QWebSocket *pClient) {
    QJsonValue urlValue = data.value("url");
    if (!urlValue.isUndefined()) {
        this->commandSetUrl(data, pClient);
    }

    QJsonValue fullscreenValue = data.value("fullscreen");
    if (!fullscreenValue.isUndefined()) {
        this->commandSetWindowMode(data, pClient);
    }

    QJsonValue idleTimeValue = data.value("idleTime");
    if (!idleTimeValue.isUndefined()) {
        this->commandSetIdleTime(data, pClient);
    }

    QJsonValue whitelistValue = data.value("whitelist");
    if (!whitelistValue.isUndefined()) {
        this->commandSetWhiteList(data, pClient);
    }

    QJsonValue permissionsValue = data.value("permissions");
    if (!permissionsValue.isUndefined()) {
        this->commandSetPermissions(data, pClient);
    }

    QJsonValue navbarVerticalPositionValue = data.value("navbarVerticalPosition");
    if (!navbarVerticalPositionValue.isUndefined()) {
        this->commandSetNavbarVerticalPosition(data, pClient);
    }

    QJsonValue navbarHorizontalPositionValue = data.value("navbarHorizontalPosition");
    if (!navbarHorizontalPositionValue.isUndefined()) {
        this->commandSetNavbarHorizontalPosition(data, pClient);
    }

    QJsonValue navbarWidthValue = data.value("navbarWidth");
    if (!navbarWidthValue.isUndefined()) {
        this->commandSetNavbarWidth(data, pClient);
    }

    QJsonValue navbarHeightValue = data.value("navbarHeight");
    if (!navbarHeightValue.isUndefined()) {
        this->commandSetNavbarHeight(data, pClient);
    }

    QJsonValue displayAddressBarValue = data.value("displayAddressBar");
    if (!displayAddressBarValue.isUndefined()) {
        this->commandSetDisplayAddressBar(data, pClient);
    }

    QJsonValue displayNavBarValue = data.value("displayNavBar");
    if (!displayNavBarValue.isUndefined()) {
        this->commandSetDisplayNavBar(data, pClient);
    }

    QJsonValue underlayNavBarValue = data.value("underlayNavBar");
    if (!underlayNavBarValue.isUndefined()) {
        this->commandSetUnderlayNavBar(data, pClient);
    }

    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::SetOptions));
}*/

void WebsocketControl::commandGetConfiguration(QJsonObject data, QWebSocket *pClient) {
    QJsonObject dataObject;
    dataObject.insert("url", QJsonValue::fromVariant(this->configuration->getUrl()));
    dataObject.insert("isFullscreen", QJsonValue::fromVariant(this->configuration->isFullscreen()));
    dataObject.insert("idleTime", QJsonValue::fromVariant(this->configuration->getIdleTime()));
    dataObject.insert("whitelist", QJsonValue::fromVariant(this->configuration->getWhiteList()));
    dataObject.insert("permissions", QJsonValue::fromVariant(WebPage::permissionTonames(this->configuration->getPermissions())));
    dataObject.insert("navbarVerticalPosition", QJsonValue::fromVariant(BarWidget::verticalPositionToName(this->configuration->getNavbarVerticalPosition())));
    dataObject.insert("navbarHorizontalPosition", QJsonValue::fromVariant(BarWidget::horizontalPositionToName(this->configuration->getNavbarHorizontalPosition())));
    dataObject.insert("navbarWidth", QJsonValue::fromVariant(this->configuration->getNavbarWidth()));
    dataObject.insert("navbarHeight", QJsonValue::fromVariant(this->configuration->getNavbarHeight()));
    dataObject.insert("isDisplayAddressBar", QJsonValue::fromVariant(this->configuration->isDisplayAddressBar()));
    dataObject.insert("isDisplayNavBar", QJsonValue::fromVariant(this->configuration->isDisplayNavBar()));
    dataObject.insert("isUnderlayNavBar", QJsonValue::fromVariant(this->configuration->isUnderlayNavBar()));


    pClient->sendTextMessage(this->buildResponse(true, "OK", WebsocketControl::Command::GetConfiguration, dataObject));
}
