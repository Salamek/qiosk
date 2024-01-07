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
}

void BarWidget::setHorizontalPosition(HorizontalPosition positionHorizontal) {
    this->positionHorizontal = positionHorizontal;
}

void BarWidget::setWidth(int widthPercent) {
    this->widthPercent = widthPercent;
}

void BarWidget::setHeight(int heightPercent) {
    this->heightPercent = heightPercent;
}

void BarWidget::plot(int parentWidth, int parentHeight) {
    QString styleBackground = "BarWidget{background-color: rgba(53, 53, 53, .85);";
    int minHeight = 50; //px
    int minWidth = 48 * 4 + 10; //px

    int barWidth = qMax(parentWidth * this->widthPercent / 100 , minWidth);
    int barHeight = qMax(parentHeight * this->heightPercent / 100, minHeight);
    int barX;
    int barY;

    switch(this->positionHorizontal) {
        case HorizontalPosition::Center:
            barX = (parentWidth / 2) - (barWidth / 2);
            if (this->widthPercent < 100) {
                styleBackground += " border-top-left-radius: 10px; border-top-right-radius: 10px;";
            }
            break;
        case HorizontalPosition::Left:
            barX = 0;
            styleBackground += " border-top-right-radius: 10px;";
            break;
        case HorizontalPosition::Right:
            barX = parentWidth - barWidth;
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
        barY = parentHeight - barHeight;
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
    //QStringList navbarHorizontalPositionOptions;
    //navbarHorizontalPositionOptions << "left" << "right" << "center";

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
    //QStringList navbarVerticalPositionOptions;
    //navbarVerticalPositionOptions << "top" << "bottom";

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


