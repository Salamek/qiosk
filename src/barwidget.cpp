#include "barwidget.h"


QStringList BarWidget::navbarHorizontalPositionOptions = {"left", "right", "center"};
QStringList BarWidget::navbarVerticalPositionOptions = {"top", "bottom"};
QStringList BarWidget::navbarEnabledButtonOptions = {"back", "forward", "reload", "home"};


BarWidget::BarWidget(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout *buttonBarLayout = new QHBoxLayout();
    buttonBarLayout->setContentsMargins(0, 0, 0, 0);

    static QIcon backIcon(QStringLiteral(":back.png"));
    static QIcon backIconDisabled(QStringLiteral(":back_disabled.png"));
    static QIcon forwardIcon(QStringLiteral(":forward.png"));
    static QIcon forwardIconDisabled(QStringLiteral(":forward_disabled.png"));
    static QIcon reloadIcon(QStringLiteral(":refresh.png"));
    static QIcon reloadIconDisabled(QStringLiteral(":refresh_disabled.png"));
    static QIcon homeIcon(QStringLiteral(":home.png"));
    static QIcon homeIconDisabled(QStringLiteral(":home_disabled.png"));

    backButton = new BarButton(backIcon, backIconDisabled);
    forwardButton = new BarButton(forwardIcon, forwardIconDisabled);
    reloadButton = new BarButton(reloadIcon, reloadIconDisabled);
    reloadButton->setShortcut(Qt::Key_F5);
    homeButton = new BarButton(homeIcon, homeIconDisabled);
    QSpacerItem *horizontalSpacerOne = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonBarLayout->addItem(horizontalSpacerOne);

    buttonBarLayout->addWidget(backButton);
    buttonBarLayout->addWidget(forwardButton);
    buttonBarLayout->addWidget(reloadButton);
    buttonBarLayout->addWidget(homeButton);


    QSpacerItem *horizontalSpacerTwo = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonBarLayout->addItem(horizontalSpacerTwo);

    this->setLayout(buttonBarLayout);
}

void BarWidget::setDisplayBackButton(bool display){
    if (display) {
        this->backButton->show();
    } else {
        this->backButton->hide();
    }
}

void BarWidget::setDisplayForwardButton(bool display){
    if (display) {
        this->forwardButton->show();
    } else {
        this->forwardButton->hide();
    }
}

void BarWidget::setDisplayReloadButton(bool display){
    if (display) {
        this->reloadButton->show();
    } else {
        this->reloadButton->hide();
    }
}

void BarWidget::setDisplayHomeButton(bool display){
    if (display) {
        this->homeButton->show();
    } else {
        this->homeButton->hide();
    }
}

void BarWidget::setNavBarEnabledButtons(QList<BarWidget::Button> enabledButtons) {
    this->setDisplayBackButton(enabledButtons.contains(BarWidget::Button::Back));
    this->setDisplayForwardButton(enabledButtons.contains(BarWidget::Button::Forward));
    this->setDisplayReloadButton(enabledButtons.contains(BarWidget::Button::Reload));
    this->setDisplayHomeButton(enabledButtons.contains(BarWidget::Button::Home));
}

void BarWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BarWidget::setVerticalPosition(VerticalPosition positionVertical) {
    this->positionVertical = positionVertical;
    this->plot();
}

void BarWidget::setHorizontalPosition(HorizontalPosition positionHorizontal) {
    this->positionHorizontal = positionHorizontal;
    this->plot();
}

void BarWidget::setWidth(int widthPercent) {
    this->widthPercent = widthPercent;
    this->plot();
}

void BarWidget::setHeight(int heightPercent) {
    this->heightPercent = heightPercent;
    this->plot();
}


void BarWidget::plot() {
    this->plot(this->parentWidget()->size());
}


void BarWidget::plot(QSize parentSize) {


    QString styleBackground = "BarWidget{background-color: rgba(53, 53, 53, .85);";
    int minHeight = 50; //px
    int minWidth = 48 * 4 + 10; //px

    int barWidth = qMax(parentSize.width() * this->widthPercent / 100 , minWidth);
    int barHeight = qMax(parentSize.height() * this->heightPercent / 100, minHeight);
    int barX = 0;
    int barY = 0;

    QString radiusVerticalPosition = "top";
    switch(this->positionVertical) {
    case VerticalPosition::Bottom:
        barY = parentSize.height() - barHeight;
        break;
    case VerticalPosition::Top:
        radiusVerticalPosition = "bottom";
        barY = 0;
        break;
    case VerticalPosition::Unknown:
        // We put it here to make lint happy with our switch...
        Q_ASSERT(this->positionVertical == VerticalPosition::Unknown);
        break;
    }


    switch(this->positionHorizontal) {
        case HorizontalPosition::Center:
            barX = (parentSize.width() / 2) - (barWidth / 2);
            if (this->widthPercent < 100) {
                styleBackground += QString(" border-%1-left-radius: 10px; border-%1-right-radius: 10px;").arg(radiusVerticalPosition);
            }
            break;
        case HorizontalPosition::Left:
            barX = 0;
            styleBackground += QString(" border-%1-right-radius: 10px;").arg(radiusVerticalPosition);
            break;
        case HorizontalPosition::Right:
            barX = parentSize.width() - barWidth;
            styleBackground += QString(" border-%1-left-radius: 10px;").arg(radiusVerticalPosition);
            break;

        case HorizontalPosition::Unknown:
            // We put it here to make lint happy with our switch...
            Q_ASSERT(this->positionHorizontal == HorizontalPosition::Unknown);
            break;
        }

    styleBackground += "}";


    this->move(barX, barY);
    this->resize(barWidth, barHeight);
    this->setFixedSize(barWidth, barHeight);

    QString style = styleBackground;
    this->setStyleSheet(style);
}

BarWidget::Button BarWidget::nameToBarWidgetButton(QString name){
    switch(BarWidget::navbarEnabledButtonOptions.indexOf(name)){
    case 0:
        return BarWidget::Button::Back;
        break;

    case 1:
        return BarWidget::Button::Forward;
        break;

    case 2:
        return BarWidget::Button::Reload;
        break;

    case 3:
        return BarWidget::Button::Home;
        break;

    default:
        return BarWidget::Button::Unknown;
        break;
    }
}

BarWidget::HorizontalPosition BarWidget::nameToBarWidgetHorizontalPosition(QString name){
    switch(BarWidget::navbarHorizontalPositionOptions.indexOf(name)){
    case 0:
        return BarWidget::HorizontalPosition::Left;
        break;

    case 1:
        return BarWidget::HorizontalPosition::Right;
        break;

    case 2:
        return BarWidget::HorizontalPosition::Center;
        break;
    default:
        return BarWidget::HorizontalPosition::Unknown;
        break;
    }
}


BarWidget::VerticalPosition BarWidget::nameToBarWidgetVerticalPosition(QString name){
    switch(BarWidget::navbarVerticalPositionOptions.indexOf(name)){
    case 0:
        return BarWidget::VerticalPosition::Top;
        break;

    case 1:
        return BarWidget::VerticalPosition::Bottom;
        break;

    default:
        return BarWidget::VerticalPosition::Unknown;
        break;
    }
}

QString BarWidget::buttonToName(BarWidget::Button button){
    switch(button){
    case BarWidget::Button::Back:
        return "back";
        break;

    case BarWidget::Button::Forward:
        return "forward";
        break;

    case BarWidget::Button::Reload:
        return "reload";
        break;

    case BarWidget::Button::Home:
        return "home";
        break;

    case BarWidget::Button::Unknown:
        return "unknown";
        break;
    }

    return "unknown";
}


QString BarWidget::verticalPositionToName(BarWidget::VerticalPosition verticalPosition){
    switch(verticalPosition){
    case BarWidget::VerticalPosition::Top:
        return "top";
        break;

    case BarWidget::VerticalPosition::Bottom:
        return "bottom";
        break;

    case BarWidget::VerticalPosition::Unknown:
        return "unknown";
        break;
    }

    return "unknown";
}

QString BarWidget::horizontalPositionToName(BarWidget::HorizontalPosition horizontalPosition){
    switch(horizontalPosition){
    case BarWidget::HorizontalPosition::Center:
        return "center";
        break;

    case BarWidget::HorizontalPosition::Left:
        return "left";
        break;

    case BarWidget::HorizontalPosition::Right:
        return "right";
        break;

    case BarWidget::HorizontalPosition::Unknown:
        return "unknown";
        break;
    }

    return "unknown";
}


QList<BarWidget::Button> BarWidget::buttonNamesToButtons(QStringList buttonNames) {
    QList<BarWidget::Button> buttons;
    foreach (QString buttonName, buttonNames) {
        buttons.append(BarWidget::nameToBarWidgetButton(buttonName));
    }

    return buttons;
}

