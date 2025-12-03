/* plugin-taskbar/oneg4taskbarplugin.cpp
 * Taskbar plugin implementation
 */

#include "oneg4taskbarplugin.h"

#include "oneg4taskbar.h"

#include "oneg4taskbarconfiguration.h"

OneG4TaskBarPlugin::OneG4TaskBarPlugin(const IOneG4PanelPluginStartupInfo& startupInfo)
    : QObject(),
      IOneG4PanelPlugin(startupInfo)

{
  mTaskBar = new OneG4TaskBar(this);
}

OneG4TaskBarPlugin::~OneG4TaskBarPlugin() {
  delete mTaskBar;
}

QWidget* OneG4TaskBarPlugin::widget() {
  return mTaskBar;
}

QDialog* OneG4TaskBarPlugin::configureDialog() {
  return new OneG4TaskbarConfiguration(settings());
}

void OneG4TaskBarPlugin::settingsChanged() {
  mTaskBar->settingsChanged();
}

void OneG4TaskBarPlugin::realign() {
  mTaskBar->realign();
}