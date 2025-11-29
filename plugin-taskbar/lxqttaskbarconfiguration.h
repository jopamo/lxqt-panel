/* plugin-taskbar/lxqttaskbarconfiguration.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKBARCONFIGURATION_H
#define LXQTTASKBARCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"

class PluginSettings;

namespace Ui {
class LXQtTaskbarConfiguration;
}

class LXQtTaskbarConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtTaskbarConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtTaskbarConfiguration();

 private:
  Ui::LXQtTaskbarConfiguration* ui;

  /*
    Read settings from conf file and put data into controls.
  */
  void loadSettings();

 private slots:
  void saveSettings();
};

#endif  // LXQTTASKBARCONFIGURATION_H