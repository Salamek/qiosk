#include "progressbarwidget.h"


ProgressBarWidget::ProgressBarWidget(QWidget *parent)
    : QProgressBar{parent}
{
    this->setTextVisible(false);
    this->setStyleSheet(QStringLiteral("QProgressBar {border: 0px; background-color: rgba(0, 0, 0, 0)} QProgressBar::chunk {background-color: #da4453}"));
    this->move(0, 0);
}

void ProgressBarWidget::plot(int parentWidth, int parentHeight) {
    this->resize(parentWidth, 1);
}

void ProgressBarWidget::setValue(int value) {
    this->setHidden((value == 0 ? true : false));
    QProgressBar::setValue(value);
}
