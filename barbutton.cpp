#include "barbutton.h"

BarButton::BarButton(QIcon icon)
    : QPushButton{icon, ""}
{
    //Create as disabled, browser will set them
    this->setEnabled(false);
    this->setFlat(true);
    this->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
    this->setMaximumSize(QSize(50, 16777215));
    this->setMinimumWidth(50);
    this->setStyleSheet("QPushButton {background-color: transparent}");
    this->setIconSize(QSize(48, 48));
}
