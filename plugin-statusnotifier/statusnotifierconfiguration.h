/* plugin-statusnotifier/statusnotifierconfiguration.h
 * Configuration interface for plugin-statusnotifier
 */

#ifndef STATUSNOTIFIERCONFIGURATION_H
#define STATUSNOTIFIERCONFIGURATION_H

#include "../panel/oneg4panelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

namespace Ui {
class StatusNotifierConfiguration;
}

class StatusNotifierConfiguration : public OneG4PanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit StatusNotifierConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~StatusNotifierConfiguration();

  void addItems(const QStringList& items);

 private:
  Ui::StatusNotifierConfiguration* ui;

  QStringList mAutoHideList;
  QStringList mHideList;

  void loadSettings();

  void dialogButtonsAction(QAbstractButton* btn);

 private slots:
  void saveSettings();
};

#endif  // STATUSNOTIFIERCONFIGURATION_H