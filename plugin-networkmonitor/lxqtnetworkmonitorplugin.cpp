/* plugin-networkmonitor/lxqtnetworkmonitorplugin.cpp
 * Network monitor plugin implementation
 */

#include "lxqtnetworkmonitorplugin.h"
#include "lxqtnetworkmonitor.h"
#include "lxqtnetworkmonitorconfiguration.h"

LXQtNetworkMonitorPlugin::LXQtNetworkMonitorPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo), mWidget(new LXQtNetworkMonitor(this)) {}

LXQtNetworkMonitorPlugin::~LXQtNetworkMonitorPlugin() {
  delete mWidget;
}

QWidget* LXQtNetworkMonitorPlugin::widget() {
  return mWidget;
}

QDialog* LXQtNetworkMonitorPlugin::configureDialog() {
  return new LXQtNetworkMonitorConfiguration(settings());
}

void LXQtNetworkMonitorPlugin::settingsChanged() {
  mWidget->settingsChanged();
}