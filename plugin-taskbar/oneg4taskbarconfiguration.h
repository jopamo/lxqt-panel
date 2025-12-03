/* plugin-taskbar/oneg4taskbarconfiguration.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKBARCONFIGURATION_H
#define ONEG4TASKBARCONFIGURATION_H

#include "../panel/oneg4panelpluginconfigdialog.h"

class PluginSettings;

namespace Ui {
class OneG4TaskbarConfiguration;
}

class OneG4TaskbarConfiguration : public OneG4PanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit OneG4TaskbarConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~OneG4TaskbarConfiguration();

 private:
  Ui::OneG4TaskbarConfiguration* ui;

  /*
    Read settings from conf file and put data into controls.
  */
  void loadSettings();

 private slots:
  void saveSettings();
};

#endif  // ONEG4TASKBARCONFIGURATION_H