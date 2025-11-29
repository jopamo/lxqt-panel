/* plugin-taskbar/lxqttaskbarplugin.cpp
 * Taskbar plugin implementation
 */

#include "lxqttaskbarplugin.h"

#include "lxqttaskbar.h"

#include "lxqttaskbarconfiguration.h"

LXQtTaskBarPlugin::LXQtTaskBarPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(),
      ILXQtPanelPlugin(startupInfo)

{
  mTaskBar = new LXQtTaskBar(this);
}

LXQtTaskBarPlugin::~LXQtTaskBarPlugin() {
  delete mTaskBar;
}

QWidget* LXQtTaskBarPlugin::widget() {
  return mTaskBar;
}

QDialog* LXQtTaskBarPlugin::configureDialog() {
  return new LXQtTaskbarConfiguration(settings());
}

void LXQtTaskBarPlugin::settingsChanged() {
  mTaskBar->settingsChanged();
}

void LXQtTaskBarPlugin::realign() {
  mTaskBar->realign();
}