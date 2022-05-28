#include "barwidget.h"

BarWidget::BarWidget(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout *buttonBarLayout = new QHBoxLayout();
    buttonBarLayout->setMargin(0);

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
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonBarLayout->addItem(horizontalSpacer);

    buttonBarLayout->addWidget(backButton);
    buttonBarLayout->addWidget(forwardButton);
    buttonBarLayout->addWidget(reloadButton);
    buttonBarLayout->addWidget(homeButton);

    buttonBarLayout->addItem(horizontalSpacer);

    this->setLayout(buttonBarLayout);
}

void BarWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BarWidget::setVerticalPosition(BarVerticalPositionEnum positionVertical) {
    this->positionVertical = positionVertical;
}

void BarWidget::setHorizontalPosition(BarHorizontalPositionEnum positionHorizontal) {
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
    int barWidth = (parentWidth / 100) * this->widthPercent;
    int barHeight = qMax((parentHeight / 100) * this->heightPercent, minHeight);
    int barX;
    int barY;

    switch(this->positionHorizontal) {
    case BarHorizontalPositionEnum::Center:
        barX = (parentWidth / 2) - (barWidth / 2);
        if (this->widthPercent < 100) {
            styleBackground += " border-top-left-radius: 10px; border-top-right-radius: 10px;";
        }
        break;
    case BarHorizontalPositionEnum::Left:
        barX = 0;
        styleBackground += " border-top-right-radius: 10px;";
        break;
    case BarHorizontalPositionEnum::Right:
        barX = parentWidth - barWidth;
        styleBackground += " border-top-left-radius: 10px;";
        break;
    }

    styleBackground += "}";

    switch(this->positionVertical) {
    case BarVerticalPositionEnum::Bottom:
        barY = parentHeight - barHeight;
        break;
    case BarVerticalPositionEnum::Top:
        barY = 0;
        break;
    }

    this->move(barX, barY);
    this->resize(barWidth, barHeight);

    QString style = styleBackground;
    this->setStyleSheet(style);
}
