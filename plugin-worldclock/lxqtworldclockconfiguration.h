/* plugin-worldclock/lxqtworldclockconfiguration.h
 * Configuration interface for plugin-worldclock
 */

#ifndef LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H
#define LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"
#include <QAbstractButton>
#include <QFont>
#include <QMap>

namespace Ui {
class LXQtWorldClockConfiguration;
}

class LXQtWorldClockConfigurationTimeZones;
class LXQtWorldClockConfigurationManualFormat;
class QTableWidgetItem;

class LXQtWorldClockConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtWorldClockConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtWorldClockConfiguration();

 public slots:
  void saveSettings();

 private:
  Ui::LXQtWorldClockConfiguration* ui;

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

  LXQtWorldClockConfigurationTimeZones* mConfigurationTimeZones;
  LXQtWorldClockConfigurationManualFormat* mConfigurationManualFormat;

  QString mManualFormat;

  void setDefault(int);
  void setBold(QTableWidgetItem*, bool);
  void setBold(int row, bool value);
  int findTimeZone(const QString& timeZone);
};

#endif  // LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H