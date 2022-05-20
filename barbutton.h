#ifndef BARBUTTON_H
#define BARBUTTON_H

#include <QPushButton>

class BarButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BarButton(QIcon icon);
};

#endif // BARBUTTON_H
