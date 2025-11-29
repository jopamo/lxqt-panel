/* plugin-mount/configuration.cpp
 * Mount plugin implementation
 */

#include "configuration.h"
#include "ui_configuration.h"

#include <QComboBox>
#include <QDialogButtonBox>

Configuration::Configuration(PluginSettings* settings, QWidget* parent)
    : LXQtPanelPluginConfigDialog(settings, parent), ui(new Ui::Configuration), mLockSettingChanges(false) {
  ui->setupUi(this);

  // Set size policies
  QSizePolicy sp = ui->devAddedLabel->sizePolicy();
  sp.setHorizontalStretch(1);
  ui->devAddedLabel->setSizePolicy(sp);

  sp = ui->devAddedCombo->sizePolicy();
  sp.setHorizontalStretch(1);
  ui->devAddedCombo->setSizePolicy(sp);

  sp = ui->ejectPressedLabel->sizePolicy();
  sp.setHorizontalStretch(1);
  ui->ejectPressedLabel->setSizePolicy(sp);

  sp = ui->ejectPressedCombo->sizePolicy();
  sp.setHorizontalStretch(1);
  ui->ejectPressedCombo->setSizePolicy(sp);

  // Fill combo boxes
  ui->devAddedCombo->addItem(tr("Popup menu"), QLatin1String(ACT_SHOW_MENU));
  ui->devAddedCombo->addItem(tr("Show info"), QLatin1String(ACT_SHOW_INFO));
  ui->devAddedCombo->addItem(tr("Do nothing"), QLatin1String(ACT_NOTHING));

  ui->ejectPressedCombo->addItem(tr("Do nothing"), QLatin1String(ACT_NOTHING));
  ui->ejectPressedCombo->addItem(tr("Eject All Optical Drives"), QLatin1String(ACT_EJECT_OPTICAL));

  adjustSize();

  loadSettings();
  connect(ui->devAddedCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &Configuration::devAddedChanged);
  connect(ui->ejectPressedCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &Configuration::ejectPressedChanged);
  connect(ui->buttons, &QDialogButtonBox::clicked, this, &Configuration::dialogButtonsAction);
}

Configuration::~Configuration() {
  delete ui;
}

void Configuration::loadSettings() {
  mLockSettingChanges = true;

  QVariant value = settings().value(QLatin1String(CFG_KEY_ACTION), QLatin1String(ACT_SHOW_INFO));
  setComboboxIndexByData(ui->devAddedCombo, value, 1);

  value = settings().value(QLatin1String(CFG_EJECT_ACTION), QLatin1String(ACT_NOTHING));
  setComboboxIndexByData(ui->ejectPressedCombo, value, 0);

  mLockSettingChanges = false;
}

void Configuration::devAddedChanged(int index) {
  if (!mLockSettingChanges) {
    QString s = ui->devAddedCombo->itemData(index).toString();
    settings().setValue(QLatin1String(CFG_KEY_ACTION), s);
  }
}

void Configuration::ejectPressedChanged(int index) {
  if (!mLockSettingChanges) {
    QString s = ui->ejectPressedCombo->itemData(index).toString();
    settings().setValue(QLatin1String(CFG_EJECT_ACTION), s);
  }
}