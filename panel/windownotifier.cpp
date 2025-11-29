/* panel/windownotifier.cpp
 * Implementation file for windownotifier
 */

#include "windownotifier.h"
#include <QWidget>
#include <QEvent>

void WindowNotifier::observeWindow(QWidget* w) {
  // installing the same filter object multiple times doesn't harm
  w->installEventFilter(this);
}

bool WindowNotifier::eventFilter(QObject* watched, QEvent* event) {
  QWidget* widget = qobject_cast<QWidget*>(watched);  // we're observing only QWidgetw
  auto it = std::lower_bound(mShownWindows.begin(), mShownWindows.end(), widget);
  switch (event->type()) {
    case QEvent::Close:
      watched->removeEventFilter(this);
#if __cplusplus >= 201703L
      [[fallthrough]];
#endif
      // fall through
    case QEvent::Hide:
      if (mShownWindows.end() != it)
        mShownWindows.erase(it);
      if (mShownWindows.isEmpty())
        emit lastHidden();
      break;
    case QEvent::Show: {
      const bool first_shown = mShownWindows.isEmpty();
      mShownWindows.insert(it, widget);  // we keep the mShownWindows sorted
      if (first_shown)
        emit firstShown();
    }
    default:
      break;
  }
  return false;
}