#include "barbutton.h"

BarButton::BarButton(QIcon enabledIcon, QIcon disabledIcon)
    : QPushButton{enabledIcon, ""}
{
    this->enabledIcon = enabledIcon;
    this->disabledIcon = disabledIcon;
    //Create as disabled, browser will set them
    this->setEnabled(false);
    this->setFlat(true);
    this->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
    this->setMaximumSize(QSize(50, 16777215));
    this->setMinimumWidth(50);
    this->setStyleSheet("QPushButton {background-color: transparent}");
    this->setIconSize(QSize(48, 48));

}


void BarButton::setDisabledIcon(QIcon disabledIcon) {
    this->disabledIcon = disabledIcon;
}

void BarButton::setEnabledIcon(QIcon enabledIcon) {
    this->enabledIcon = enabledIcon;
}

void BarButton::setEnabled(bool enabled) {
    this->setIcon((enabled == true ? this->enabledIcon : this->disabledIcon));
    QPushButton::setEnabled(enabled);
}
