#include "XdgIcon.h"

#include <QApplication>

namespace XdgIcon {

QIcon fromTheme(const QString& name, const QIcon& fallback) {
  const QIcon icon = QIcon::fromTheme(name);
  return icon.isNull() ? fallback : icon;
}

QIcon defaultApplicationIcon() {
  QIcon icon = QIcon::fromTheme(QStringLiteral("application-x-executable"));
  if (icon.isNull())
    icon = QApplication::windowIcon();
  return icon;
}

}  // namespace XdgIcon
