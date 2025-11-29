/* plugin-mainmenu/menustyle.cpp
 * Main menu plugin implementation
 */

#include "menustyle.h"
#include <QDebug>

/************************************************

 ************************************************/
MenuStyle::MenuStyle() : QProxyStyle() {
  mIconSize = DEFAULT_ICON_SIZE;
}

/************************************************

 ************************************************/
int MenuStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
  if (metric == QStyle::PM_SmallIconSize && mIconSize != DEFAULT_ICON_SIZE)
    return mIconSize;

  return QProxyStyle::pixelMetric(metric, option, widget);
}

/************************************************

 ************************************************/
int MenuStyle::styleHint(StyleHint hint,
                         const QStyleOption* option,
                         const QWidget* widget,
                         QStyleHintReturn* returnData) const {
  // By default, the popup menu will be closed when Alt key
  // is pressed. If SH_MenuBar_AltKeyNavigation style hint returns
  // false, this behavior can be suppressed so let's do it.
  if (hint == QStyle::SH_MenuBar_AltKeyNavigation)
    return 0;
  return QProxyStyle::styleHint(hint, option, widget, returnData);
}