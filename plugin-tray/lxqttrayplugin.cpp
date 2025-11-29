/* plugin-tray/lxqttrayplugin.cpp
 * Plugin implementation for plugin-tray
 */

#include "lxqttrayplugin.h"
#include "fdoselectionmanager.h"

#include <QGuiApplication>  // For nativeInterface()

LXQtTrayPlugin::LXQtTrayPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo), mManager{new FdoSelectionManager} {}

LXQtTrayPlugin::~LXQtTrayPlugin() {}

QWidget* LXQtTrayPlugin::widget() {
  return nullptr;
}

ILXQtPanelPlugin* LXQtTrayPluginLibrary::instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
  auto* x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
  if (!x11Application || !x11Application->connection()) {
    // Currently only X11 supported
    qWarning() << "Currently tray plugin supports X11 only. Skipping.";
    return nullptr;
  }

  return new LXQtTrayPlugin(startupInfo);
}