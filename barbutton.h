#ifndef BARBUTTON_H
#define BARBUTTON_H

#include <QPushButton>

class BarButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BarButton(QIcon enabledIcon, QIcon disabledIcon, QWidget *parent = nullptr);
    void setDisabledIcon(QIcon disabledIcon);
    void setEnabled(bool enabled);
    void setEnabledIcon(QIcon enabledIcon);
private:
    QIcon disabledIcon;
    QIcon enabledIcon;
    void reloadIcon();
};

#endif // BARBUTTON_H
