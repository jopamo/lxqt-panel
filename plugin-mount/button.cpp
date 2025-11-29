/* plugin-mount/button.cpp
 * Mount plugin implementation
 */

#include "button.h"
#include <XdgIcon>

Button::Button(QWidget* parent) : QToolButton(parent) {
  // Note: don't use the QStringLiteral here as it is causing a SEGFAULT in static finalization time
  //(the string is released upon our *.so removal, but the reference is still in held in libqtxdg...)
  setIcon(XdgIcon::fromTheme(QLatin1String("drive-removable-media")));
  setToolTip(tr("Removable media/devices manager"));
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setAutoRaise(true);
}

Button::~Button() = default;