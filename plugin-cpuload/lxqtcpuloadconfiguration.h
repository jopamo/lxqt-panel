/* plugin-cpuload/lxqtcpuloadconfiguration.h
 * Configuration interface for plugin-cpuload
 */

#ifndef LXQTCPULOADCONFIGURATION_H
#define LXQTCPULOADCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QSettings;
class QAbstractButton;

namespace Ui {
class LXQtCpuLoadConfiguration;
}

class LXQtCpuLoadConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtCpuLoadConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtCpuLoadConfiguration();

 private:
  Ui::LXQtCpuLoadConfiguration* ui;
  bool mLockSettingChanges;

  /*
    Fills Bar orientation combobox
  */
  void fillBarOrientations();

 private slots:
  /*
    Saves settings in conf file.
  */
  void loadSettings();
  void showTextChanged(bool value);
  void barWidthChanged(int value);
  void updateIntervalChanged(double value);
  void barOrientationChanged(int index);
};

#endif  // LXQTCPULOADCONFIGURATION_H