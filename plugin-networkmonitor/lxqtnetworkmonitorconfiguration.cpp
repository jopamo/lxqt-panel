/* plugin-networkmonitor/lxqtnetworkmonitorconfiguration.cpp
 * Network monitor plugin implementation
 */

#include "lxqtnetworkmonitorconfiguration.h"
#include "ui_lxqtnetworkmonitorconfiguration.h"

#include <algorithm>

extern "C" {
#include <statgrab.h>
}

#ifdef __sg_public
// since libstatgrab 0.90 this macro is defined, so we use it for version check
#define STATGRAB_NEWER_THAN_0_90 1
#endif

LXQtNetworkMonitorConfiguration::LXQtNetworkMonitorConfiguration(PluginSettings* settings, QWidget* parent)
    : LXQtPanelPluginConfigDialog(settings, parent),
      ui(new Ui::LXQtNetworkMonitorConfiguration),
      mLockSettingChanges(false) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("NetworkMonitorConfigurationWindow"));
  ui->setupUi(this);

  connect(ui->buttons, &QDialogButtonBox::clicked, this, &LXQtNetworkMonitorConfiguration::dialogButtonsAction);
  connect(ui->iconCB, &QComboBox::currentIndexChanged, this, &LXQtNetworkMonitorConfiguration::saveSettings);
  connect(ui->interfaceCB, &QComboBox::currentIndexChanged, this, &LXQtNetworkMonitorConfiguration::saveSettings);

  loadSettings();
}

LXQtNetworkMonitorConfiguration::~LXQtNetworkMonitorConfiguration() {
  delete ui;
}

void LXQtNetworkMonitorConfiguration::saveSettings() {
  if (!mLockSettingChanges) {
    settings().setValue(QStringLiteral("icon"), ui->iconCB->currentIndex());
    settings().setValue(QStringLiteral("interface"), ui->interfaceCB->currentText());
  }
}

void LXQtNetworkMonitorConfiguration::loadSettings() {
  mLockSettingChanges = true;

  ui->iconCB->setCurrentIndex(settings().value(QStringLiteral("icon"), 1).toInt());

  int count;
#ifdef STATGRAB_NEWER_THAN_0_90
  size_t ret_count;
  sg_network_iface_stats* stats = sg_get_network_iface_stats(&ret_count);
  count = (int)ret_count;
#else
  sg_network_iface_stats* stats = sg_get_network_iface_stats(&count);
#endif
  for (int ix = 0; ix < count; ix++)
    ui->interfaceCB->addItem(QLatin1String(stats[ix].interface_name));

  QString interface = settings().value(QStringLiteral("interface")).toString();
  ui->interfaceCB->setCurrentIndex(std::max(std::min(0, count - 1), ui->interfaceCB->findText(interface)));

  mLockSettingChanges = false;
}