/* panel/config/configpaneldialog.cpp
 * Configuration dialogs and widgets
 */

#include "configpaneldialog.h"

ConfigPanelDialog::ConfigPanelDialog(OneG4Panel* panel, QWidget* parent)
    : OneG4::ConfigDialog(tr("Configure Panel"), panel->settings(), parent),
      mPlacementPage(nullptr),
      mStylingPage(nullptr),
      mPluginsPage(nullptr) {
  setAttribute(Qt::WA_DeleteOnClose);

  mPlacementPage = new ConfigPlacement(panel, this);
  addPage(mPlacementPage, tr("Placement"), QLatin1String("configure-toolbars"));
  connect(this, &ConfigPanelDialog::reset, mPlacementPage, &ConfigPlacement::reset);

  mStylingPage = new ConfigStyling(panel, this);
  addPage(mStylingPage, tr("Styling"), QLatin1String("colormanagement"));
  connect(this, &ConfigPanelDialog::reset, mStylingPage, &ConfigStyling::reset);

  mPluginsPage = new ConfigPluginsWidget(panel, this);
  addPage(mPluginsPage, tr("Widgets"), QLatin1String("preferences-plugin"));
  connect(this, &ConfigPanelDialog::reset, mPluginsPage, &ConfigPluginsWidget::reset);

  connect(this, &ConfigPanelDialog::accepted, panel, [panel] { panel->saveSettings(); });
}

void ConfigPanelDialog::showConfigPlacementPage() {
  showPage(mPlacementPage);
}

void ConfigPanelDialog::showConfigStylingPage() {
  showPage(mStylingPage);
}

void ConfigPanelDialog::showConfigPluginsPage() {
  showPage(mPluginsPage);
}

void ConfigPanelDialog::updateIconThemeSettings() {
  mStylingPage->updateIconThemeSettings();
}