/* plugin-worldclock/oneg4worldclockconfiguration.h
 * Configuration interface for plugin-worldclock
 */

#ifndef ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_H
#define ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_H

#include "../panel/oneg4panelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"
#include <QAbstractButton>
#include <QFont>
#include <QMap>

namespace Ui {
class OneG4WorldClockConfiguration;
}

class OneG4WorldClockConfigurationTimeZones;
class WorldClockManualFormatConfig;
class QTableWidgetItem;

class OneG4WorldClockConfiguration : public OneG4PanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit OneG4WorldClockConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~OneG4WorldClockConfiguration();

 public slots:
  void saveSettings();

 private:
  Ui::OneG4WorldClockConfiguration* ui;

  /*
    Read settings from conf file and put data into controls.
  */
  void loadSettings();

 private slots:
  void timeFormatChanged(int);
  void dateGroupToggled(bool);
  void dateFormatChanged(int);
  void advancedFormatToggled(bool);
  void customiseManualFormatClicked();
  void manualFormatChanged();

  void updateTimeZoneButtons();
  void addTimeZone();
  void removeTimeZone();
  void setTimeZoneAsDefault();
  void editTimeZoneCustomName();
  void moveTimeZoneUp();
  void moveTimeZoneDown();

 private:
  QString mDefaultTimeZone;

  bool mLockCascadeSettingChanges;

  OneG4WorldClockConfigurationTimeZones* mConfigurationTimeZones;
  WorldClockManualFormatConfig* mConfigurationManualFormat;

  QString mManualFormat;

  void setDefault(int);
  void setBold(QTableWidgetItem*, bool);
  void setBold(int row, bool value);
  int findTimeZone(const QString& timeZone);
};

#endif  // ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_H