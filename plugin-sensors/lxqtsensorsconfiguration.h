/* plugin-sensors/lxqtsensorsconfiguration.h
 * System sensors plugin implementation
 */

#ifndef LXQTSENSORSCONFIGURATION_H
#define LXQTSENSORSCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"
#include <QAbstractButton>
#include <QButtonGroup>
#include <QDateTime>
#include <QLocale>

namespace Ui {
class LXQtSensorsConfiguration;
}

class LXQtSensorsConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtSensorsConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtSensorsConfiguration();

 private:
  Ui::LXQtSensorsConfiguration* ui;
  bool mLockSettingChanges;

  /*
    Read settings from conf file and put data into controls.
  */
  void loadSettings();

 private slots:
  /*
    Saves settings in conf file.
  */
  void saveSettings();
  void changeProgressBarColor();
  void detectedChipSelected(int index);
};

#endif  // LXQTSENSORSCONFIGURATION_H