#ifndef BARWIDGET_H
#define BARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
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

    static QStringList navbarHorizontalPositionOptions;
    static QStringList navbarVerticalPositionOptions;

    void plot(int parentWidth, int parentHeight);
    void setVerticalPosition(VerticalPosition positionVertical);
    void setHorizontalPosition(HorizontalPosition positionHorizontal);
    void setWidth(int widthPercent);
    void setHeight(int heightPercent);

    BarButton *homeButton;
    BarButton *reloadButton;
    BarButton *forwardButton;
    BarButton *backButton;


    static BarWidget::HorizontalPosition nameToBarWidgetHorizontalPosition(QString name);
    static BarWidget::VerticalPosition nameToBarWidgetVerticalPosition(QString name);
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
