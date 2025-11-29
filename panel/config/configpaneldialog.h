/* panel/config/configpaneldialog.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPANELDIALOG_H
#define CONFIGPANELDIALOG_H

#include "configplacement.h"
#include "configstyling.h"
#include "configpluginswidget.h"
#include "../lxqtpanel.h"

#include <LXQt/ConfigDialog>

class ConfigPanelDialog : public LXQt::ConfigDialog {
  Q_OBJECT

 public:
  ConfigPanelDialog(LXQtPanel* panel, QWidget* parent = nullptr);

  void showConfigPlacementPage();
  void showConfigStylingPage();
  void showConfigPluginsPage();
  void updateIconThemeSettings();

 private:
  ConfigPlacement* mPlacementPage;
  ConfigStyling* mStylingPage;
  ConfigPluginsWidget* mPluginsPage;
};

#endif  // CONFIGPANELDIALOG_H