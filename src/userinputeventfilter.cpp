#include "userinputeventfilter.h"

bool UserInputEventFilter::eventFilter(QObject *obj, QEvent *ev)
{
  //if (ev->type() != QEvent::Timer) {
  if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::MouseMove || ev->type() == QEvent::Wheel || ev->type() == QEvent::TouchUpdate) {
      emit userActivity();
  }

  return QObject::eventFilter(obj, ev);
}
