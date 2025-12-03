#include "Notification.h"

#include <QGuiApplication>
#include <QIcon>
#include <QSystemTrayIcon>

namespace OneG4 {

Notification::Notification(const QString& iconName, QObject* parent)
    : QObject(parent), mIconName(iconName) {}

void Notification::setSummary(const QString& summary) {
  mSummary = summary;
}

void Notification::update() {
  static QSystemTrayIcon tray;
  if (!tray.isVisible()) {
    tray.setIcon(QIcon::fromTheme(mIconName, QIcon::fromTheme(QStringLiteral("dialog-information"))));
    tray.show();
  }
  tray.showMessage(QGuiApplication::applicationDisplayName(), mSummary, QSystemTrayIcon::Information, 1000);
}

}  // namespace OneG4
