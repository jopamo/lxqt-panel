/* plugin-desktopswitch/desktopswitchconfiguration.cpp
 * Desktop switcher plugin implementation
 */

#include "desktopswitchconfiguration.h"
#include "ui_desktopswitchconfiguration.h"

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqtabstractwmiface.h"

#include <KX11Extras>
#include <QScreen>
#include <QTimer>

DesktopSwitchConfiguration::DesktopSwitchConfiguration(PluginSettings* settings, QWidget* parent)
    : LXQtPanelPluginConfigDialog(settings, parent), ui(new Ui::DesktopSwitchConfiguration) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("DesktopSwitchConfigurationWindow"));
  ui->setupUi(this);

  connect(ui->buttons, &QDialogButtonBox::clicked, this, &DesktopSwitchConfiguration::dialogButtonsAction);

  loadSettings();

  connect(ui->rowsSB, &QSpinBox::valueChanged, this, &DesktopSwitchConfiguration::rowsChanged);
  connect(ui->labelTypeCB, &QComboBox::currentIndexChanged, this, &DesktopSwitchConfiguration::labelTypeChanged);
  connect(ui->showOnlyActiveCB, &QAbstractButton::toggled, this,
          [this](bool checked) { this->settings().setValue(QStringLiteral("showOnlyActive"), checked); });

  loadDesktopsNames();
}

DesktopSwitchConfiguration::~DesktopSwitchConfiguration() {
  delete ui;
}

void DesktopSwitchConfiguration::loadSettings() {
  ui->rowsSB->setValue(settings().value(QStringLiteral("rows"), 1).toInt());
  ui->labelTypeCB->setCurrentIndex(settings().value(QStringLiteral("labelType"), 0).toInt());
  ui->showOnlyActiveCB->setChecked(settings().value(QStringLiteral("showOnlyActive"), false).toBool());
}

void DesktopSwitchConfiguration::loadDesktopsNames() {
  LXQtPanelApplication* a = reinterpret_cast<LXQtPanelApplication*>(qApp);
  auto wmBackend = a->getWMBackend();

  int n = wmBackend->getWorkspacesCount(screen());
  for (int i = 1; i <= n; i++) {
    auto deskName = wmBackend->getWorkspaceName(i, screen() ? screen()->name() : QString());
    if (deskName.isEmpty())
      deskName = tr("Desktop %1").arg(i);
    QLineEdit* edit = new QLineEdit(deskName, this);
    ((QFormLayout*)ui->namesGroupBox->layout())->addRow(tr("Desktop %1:").arg(i), edit);

    // C++11 rocks!
    QTimer* timer = new QTimer(this);
    timer->setInterval(400);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=] { KX11Extras::setDesktopName(i, edit->text()); });
    connect(edit, &QLineEdit::textEdited, this, [=] { timer->start(); });
  }
}

void DesktopSwitchConfiguration::rowsChanged(int value) {
  settings().setValue(QStringLiteral("rows"), value);
}

void DesktopSwitchConfiguration::labelTypeChanged(int type) {
  settings().setValue(QStringLiteral("labelType"), type);
}