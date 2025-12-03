/* panel/config/configpaneldialog.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPANELDIALOG_H
#define CONFIGPANELDIALOG_H

#include "configplacement.h"
#include "configstyling.h"
#include "configpluginswidget.h"
#include "../oneg4panel.h"

#include <OneG4/ConfigDialog.h>

class ConfigPanelDialog : public OneG4::ConfigDialog {
  Q_OBJECT

 public:
  ConfigPanelDialog(OneG4Panel* panel, QWidget* parent = nullptr);

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