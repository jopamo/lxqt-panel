/* plugin-quicklaunch/lxqtquicklaunchplugin.cpp
 * Quick launch plugin implementation
 */

#include "lxqtquicklaunchplugin.h"
#include "lxqtquicklaunch.h"

LXQtQuickLaunchPlugin::LXQtQuickLaunchPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo), mWidget(new LXQtQuickLaunch(this)) {}

LXQtQuickLaunchPlugin::~LXQtQuickLaunchPlugin() {
  delete mWidget;
}

QWidget* LXQtQuickLaunchPlugin::widget() {
  return mWidget;
}

void LXQtQuickLaunchPlugin::realign() {
  mWidget->realign();
}