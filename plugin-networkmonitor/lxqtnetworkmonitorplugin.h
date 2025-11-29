/* plugin-networkmonitor/lxqtnetworkmonitorplugin.h
 * Network monitor plugin implementation
 */

#ifndef LXQTNETWORKMONITORPLUGIN_H
#define LXQTNETWORKMONITORPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include <QObject>
class LXQtNetworkMonitor;

class LXQtNetworkMonitorPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  explicit LXQtNetworkMonitorPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtNetworkMonitorPlugin();

  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("NetworkMonitor"); }

  bool isSeparate() const { return false; }
  QDialog* configureDialog();

 protected:
  virtual void settingsChanged();

 private:
  LXQtNetworkMonitor* mWidget;
};

class LXQtNetworkMonitorPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtNetworkMonitorPlugin(startupInfo);
  }
};

#endif  // LXQTNETWORKMONITORPLUGIN_H