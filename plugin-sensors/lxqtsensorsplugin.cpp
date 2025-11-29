/* plugin-sensors/lxqtsensorsplugin.cpp
 * System sensors plugin implementation
 */

#include "lxqtsensorsplugin.h"
#include "lxqtsensors.h"
#include "lxqtsensorsconfiguration.h"

LXQtSensorsPlugin::LXQtSensorsPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo), mWidget(new LXQtSensors(this)) {}

LXQtSensorsPlugin::~LXQtSensorsPlugin() {
  delete mWidget;
}

QWidget* LXQtSensorsPlugin::widget() {
  return mWidget;
}

QDialog* LXQtSensorsPlugin::configureDialog() {
  return new LXQtSensorsConfiguration(settings());
}

void LXQtSensorsPlugin::realign() {
  mWidget->realign();
}

void LXQtSensorsPlugin::settingsChanged() {
  mWidget->settingsChanged();
}