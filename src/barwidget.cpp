#include "barwidget.h"


QStringList BarWidget::navbarHorizontalPositionOptions = {"left", "right", "center"};
QStringList BarWidget::navbarVerticalPositionOptions = {"top", "bottom"};


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
    int barX;
    int barY;

    switch(this->positionHorizontal) {
        case HorizontalPosition::Center:
            barX = (parentSize.width() / 2) - (barWidth / 2);
            if (this->widthPercent < 100) {
                styleBackground += " border-top-left-radius: 10px; border-top-right-radius: 10px;";
            }
            break;
        case HorizontalPosition::Left:
            barX = 0;
            styleBackground += " border-top-right-radius: 10px;";
            break;
        case HorizontalPosition::Right:
            barX = parentSize.width() - barWidth;
            styleBackground += " border-top-left-radius: 10px;";
            break;

        case HorizontalPosition::Unknown:
            // We put it here to make lint happy with our switch...
            Q_ASSERT(this->positionHorizontal == HorizontalPosition::Unknown);
            break;
        }

    styleBackground += "}";

    switch(this->positionVertical) {
    case VerticalPosition::Bottom:
        barY = parentSize.height() - barHeight;
        break;
    case VerticalPosition::Top:
        barY = 0;
        break;
    case VerticalPosition::Unknown:
        // We put it here to make lint happy with our switch...
        Q_ASSERT(this->positionVertical == VerticalPosition::Unknown);
        break;
    }

    this->move(barX, barY);
    this->resize(barWidth, barHeight);
    this->setFixedSize(barWidth, barHeight);

    QString style = styleBackground;
    this->setStyleSheet(style);
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


