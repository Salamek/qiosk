#ifndef PROGRESSBARWIDGET_H
#define PROGRESSBARWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

class ProgressBarWidget : public QProgressBar
{
    Q_OBJECT
public:
    ProgressBarWidget(QWidget *parent = nullptr);
    void plot(int parentWidth, int parentHeight);
    void setValue(int value);

private:
    int height;
signals:
};

#endif // PROGRESSBARWIDGET_H
