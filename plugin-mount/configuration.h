/* plugin-mount/configuration.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_CONFIGURATION_H
#define LXQT_PLUGIN_MOUNT_CONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"

#define CFG_KEY_ACTION "newDeviceAction"
#define CFG_EJECT_ACTION "ejectAction"
#define ACT_SHOW_MENU "showMenu"
#define ACT_SHOW_INFO "showInfo"
#define ACT_NOTHING "nothing"
#define ACT_EJECT_OPTICAL "ejectOpticalDrives"

namespace Ui {
class Configuration;
}

class Configuration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit Configuration(PluginSettings* settings, QWidget* parent = nullptr);
  ~Configuration();

 protected slots:
  virtual void loadSettings();
  void devAddedChanged(int index);
  void ejectPressedChanged(int index);

 private:
  Ui::Configuration* ui;
  bool mLockSettingChanges;
};

#endif  // LXQTMOUNTCONFIGURATION_H