/* plugin-networkmonitor/lxqtnetworkmonitorconfiguration.h
 * Network monitor plugin implementation
 */

#ifndef LXQTNETWORKMONITORCONFIGURATION_H
#define LXQTNETWORKMONITORCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QAbstractButton;

namespace Ui {
class LXQtNetworkMonitorConfiguration;
}

class LXQtNetworkMonitorConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtNetworkMonitorConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtNetworkMonitorConfiguration();

 private:
  Ui::LXQtNetworkMonitorConfiguration* ui;
  bool mLockSettingChanges;

 private slots:
  /*
    Saves settings in conf file.
  */
  void saveSettings();
  void loadSettings();
};

#endif  // LXQTNETWORKMONITORCONFIGURATION_H