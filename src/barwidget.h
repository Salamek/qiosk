#ifndef BARWIDGET_H
#define BARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QGraphicsDropShadowEffect>
#include "barbutton.h"

class BarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarWidget(QWidget *parent = nullptr);

    enum class HorizontalPosition {
        Center,
        Left,
        Right,
        Unknown
    };

    enum class VerticalPosition {
        Top,
        Bottom,
        Unknown
    };

    enum class Button {
        Home,
        Reload,
        Forward,
        Back,
        Unknown
    };

    static QStringList navbarHorizontalPositionOptions;
    static QStringList navbarVerticalPositionOptions;
    static QStringList navbarEnabledButtonOptions;

    void plot(QSize parentSize);
    void setVerticalPosition(VerticalPosition positionVertical);
    void setHorizontalPosition(HorizontalPosition positionHorizontal);
    void setWidth(int widthPercent);
    void setHeight(int heightPercent);
    void setNavBarEnabledButtons(QList<BarWidget::Button> enabledButtons);

    BarButton *homeButton;
    BarButton *reloadButton;
    BarButton *forwardButton;
    BarButton *backButton;


    static BarWidget::HorizontalPosition nameToBarWidgetHorizontalPosition(QString name);
    static BarWidget::VerticalPosition nameToBarWidgetVerticalPosition(QString name);
    static BarWidget::Button nameToBarWidgetButton(QString name);
    static QString verticalPositionToName(BarWidget::VerticalPosition verticalPosition);
    static QString horizontalPositionToName(BarWidget::HorizontalPosition horizontalPosition);
    static QString buttonToName(BarWidget::Button button);
    static QList<BarWidget::Button> buttonNamesToButtons(QStringList buttonNames);

    void plot();
    void setDisplayBackButton(bool display);
    void setDisplayForwardButton(bool display);
    void setDisplayReloadButton(bool display);
    void setDisplayHomeButton(bool display);
private:
    VerticalPosition positionVertical;
    HorizontalPosition positionHorizontal;
    int widthPercent;
    int heightPercent;
signals:

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // BARWIDGET_H
