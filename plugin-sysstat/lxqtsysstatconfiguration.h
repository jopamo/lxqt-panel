/* plugin-sysstat/lxqtsysstatconfiguration.h
 * Configuration interface for plugin-sysstat
 */

#ifndef LXQTSYSSTATCONFIGURATION_H
#define LXQTSYSSTATCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"
#include <QAbstractButton>
#include <QMap>

namespace Ui {
class LXQtSysStatConfiguration;
}

namespace SysStat {
class BaseStat;
}

class LXQtSysStatColours;

class LXQtSysStatConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtSysStatConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtSysStatConfiguration();

 public slots:
  void saveSettings();

  void on_typeCOB_currentIndexChanged(int);
  void on_maximumHS_valueChanged(int);
  void on_customColoursB_clicked();

  void coloursChanged();

 public:
  static const QStringList msStatTypes;

 signals:
  void maximumNetSpeedChanged(QString);

 private:
  Ui::LXQtSysStatConfiguration* ui;

  SysStat::BaseStat* mStat;
  LXQtSysStatColours* mColoursDialog;

  bool mLockSettingChanges;

  void loadSettings();
};

#endif  // LXQTSYSSTATCONFIGURATION_H