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
#include "enum.h"
#include "barbutton.h"

class BarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarWidget(QWidget *parent = nullptr);

    void plot(int parentWidth, int parentHeight);
    void setVerticalPosition(BarVerticalPositionEnum positionVertical);
    void setHorizontalPosition(BarHorizontalPositionEnum positionHorizontal);
    void setWidth(int widthPercent);
    void setHeight(int heightPercent);

    BarButton *homeButton;
    BarButton *reloadButton;
    BarButton *forwardButton;
    BarButton *backButton;

private:
    BarVerticalPositionEnum positionVertical;
    BarHorizontalPositionEnum positionHorizontal;
    int widthPercent;
    int heightPercent;
signals:

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // BARWIDGET_H
