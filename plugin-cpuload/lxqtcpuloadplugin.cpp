/* plugin-cpuload/lxqtcpuloadplugin.cpp
 * Plugin implementation for plugin-cpuload
 */

#include "lxqtcpuloadplugin.h"
#include "lxqtcpuload.h"
#include "lxqtcpuloadconfiguration.h"

#include <QVBoxLayout>

LXQtCpuLoadPlugin::LXQtCpuLoadPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  mWidget = new QWidget();
  mContent = new LXQtCpuLoad(this, mWidget);
  QVBoxLayout* layout = new QVBoxLayout(mWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(mContent);
  layout->setStretchFactor(mContent, 1);
}

LXQtCpuLoadPlugin::~LXQtCpuLoadPlugin() {
  delete mWidget;
}

QWidget* LXQtCpuLoadPlugin::widget() {
  return mWidget;
}

QDialog* LXQtCpuLoadPlugin::configureDialog() {
  return new LXQtCpuLoadConfiguration(settings());
}

void LXQtCpuLoadPlugin::settingsChanged() {
  mContent->settingsChanged();
}