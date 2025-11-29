/* plugin-desktopswitch/desktopswitchconfiguration.h
 * Desktop switcher plugin implementation
 */

#ifndef DESKTOPSWITCHCERCONFIGURATION_H
#define DESKTOPSWITCHCERCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

#include <QFormLayout>
#include <QLineEdit>

class QAbstractButton;

namespace Ui {
class DesktopSwitchConfiguration;
}

class DesktopSwitchConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit DesktopSwitchConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~DesktopSwitchConfiguration();

 private:
  Ui::DesktopSwitchConfiguration* ui;

 private slots:
  /*
     Saves settings in conf file.
  */
  void loadSettings();
  void loadDesktopsNames();
  void rowsChanged(int value);
  void labelTypeChanged(int type);
};

#endif  // DESKTOPSWITCHCERCONFIGURATION_H