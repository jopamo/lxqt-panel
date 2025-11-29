#include "lxqtvolumeconfiguration.h"
#include "ui_lxqtvolumeconfiguration.h"

#include "audiodevice.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QRadioButton>
#include <QAbstractButton>
#include <QVariant>

LXQtVolumeConfiguration::LXQtVolumeConfiguration(PluginSettings* settings, bool ossAvailable, QWidget* parent)
    : LXQtPanelPluginConfigDialog(settings, parent), ui(new Ui::LXQtVolumeConfiguration), mLockSettingChanges(false) {
  ui->setupUi(this);

  loadSettings();
  connect(ui->devAddedCombo, &QComboBox::currentIndexChanged, this, &LXQtVolumeConfiguration::sinkSelectionChanged);
  connect(ui->buttons, &QDialogButtonBox::clicked, this, &LXQtVolumeConfiguration::dialogButtonsAction);
  connect(ui->muteOnMiddleClickCheckBox, &QCheckBox::toggled, this, &LXQtVolumeConfiguration::muteOnMiddleClickChanged);
  connect(ui->mixerLineEdit, &QLineEdit::textChanged, this, &LXQtVolumeConfiguration::mixerLineEditChanged);
  connect(ui->stepSpinBox, &QSpinBox::valueChanged, this, &LXQtVolumeConfiguration::stepSpinBoxChanged);
  connect(ui->ignoreMaxVolumeCheckBox, &QCheckBox::toggled, this,
          &LXQtVolumeConfiguration::ignoreMaxVolumeCheckBoxChanged);
  connect(ui->alwaysShowNotificationsCheckBox, &QAbstractButton::toggled, this,
          &LXQtVolumeConfiguration::alwaysShowNotificationsCheckBoxChanged);
  connect(ui->showKeyboardNotificationsCheckBox, &QAbstractButton::toggled, this,
          &LXQtVolumeConfiguration::showKeyboardNotificationsCheckBoxChanged);

  if (ossAvailable)
    connect(ui->ossRadioButton, &QRadioButton::toggled, this, &LXQtVolumeConfiguration::audioEngineChanged);
  else
    ui->ossRadioButton->setVisible(false);

#ifdef USE_PULSEAUDIO
  connect(ui->pulseAudioRadioButton, &QRadioButton::toggled, this, &LXQtVolumeConfiguration::audioEngineChanged);
#else
  ui->pulseAudioRadioButton->setVisible(false);
#endif

#ifdef USE_ALSA
  connect(ui->alsaRadioButton, &QRadioButton::toggled, this, &LXQtVolumeConfiguration::audioEngineChanged);
#else
  ui->alsaRadioButton->setVisible(false);
#endif
}

LXQtVolumeConfiguration::~LXQtVolumeConfiguration() {
  delete ui;
}

void LXQtVolumeConfiguration::setSinkList(const QList<AudioDevice*> sinks) {
  // preserve selection based on stored device index while updating the list
  const QVariant savedIndex = settings().value(QStringLiteral(SETTINGS_DEVICE), SETTINGS_DEFAULT_DEVICE);

  const bool oldBlock = ui->devAddedCombo->blockSignals(true);
  ui->devAddedCombo->clear();

  for (const AudioDevice* dev : std::as_const(sinks))
    ui->devAddedCombo->addItem(dev->description(), dev->index());

  int comboIndex = 0;
  for (int i = 0; i < ui->devAddedCombo->count(); ++i) {
    if (ui->devAddedCombo->itemData(i) == savedIndex) {
      comboIndex = i;
      break;
    }
  }

  ui->devAddedCombo->setCurrentIndex(comboIndex);
  ui->devAddedCombo->blockSignals(oldBlock);
}

void LXQtVolumeConfiguration::audioEngineChanged(bool checked) {
  if (!checked)
    return;

  bool canIgnoreMaxVolume = false;

  if (ui->pulseAudioRadioButton->isChecked()) {
    if (!mLockSettingChanges)
      settings().setValue(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral("PulseAudio"));
    canIgnoreMaxVolume = true;
  }
  else if (!mLockSettingChanges) {
    if (ui->alsaRadioButton->isChecked())
      settings().setValue(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral("Alsa"));
    else
      settings().setValue(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral("Oss"));
  }

  ui->ignoreMaxVolumeCheckBox->setEnabled(canIgnoreMaxVolume);
}

void LXQtVolumeConfiguration::sinkSelectionChanged(int index) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_DEVICE), index >= 0 ? index : 0);
}

void LXQtVolumeConfiguration::muteOnMiddleClickChanged(bool state) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_MUTE_ON_MIDDLECLICK), state);
}

void LXQtVolumeConfiguration::mixerLineEditChanged(const QString& command) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_MIXER_COMMAND), command);
}

void LXQtVolumeConfiguration::stepSpinBoxChanged(int step) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_STEP), step);
}

void LXQtVolumeConfiguration::ignoreMaxVolumeCheckBoxChanged(bool state) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_IGNORE_MAX_VOLUME), state);
}

void LXQtVolumeConfiguration::alwaysShowNotificationsCheckBoxChanged(bool state) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_ALWAYS_SHOW_NOTIFICATIONS), state);

  // since always showing notifications is the sufficient condition for showing them with keyboard,
  // self-consistency requires setting the latter to true whenever the former is toggled by the user
  ui->showKeyboardNotificationsCheckBox->setEnabled(!state);
  if (!ui->showKeyboardNotificationsCheckBox->isChecked())
    ui->showKeyboardNotificationsCheckBox->setChecked(true);
  else if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS), true);
}

void LXQtVolumeConfiguration::showKeyboardNotificationsCheckBoxChanged(bool state) {
  if (!mLockSettingChanges)
    settings().setValue(QStringLiteral(SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS), state);
}

void LXQtVolumeConfiguration::loadSettings() {
  mLockSettingChanges = true;

  const QString engine =
      settings()
          .value(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral(SETTINGS_DEFAULT_AUDIO_ENGINE))
          .toString()
          .toLower();

  if (engine == QLatin1String("pulseaudio"))
    ui->pulseAudioRadioButton->setChecked(true);
  else if (engine == QLatin1String("alsa"))
    ui->alsaRadioButton->setChecked(true);
  else
    ui->ossRadioButton->setChecked(true);

  // currently, this option is only supported by the pulse audio backend
  if (!ui->pulseAudioRadioButton->isChecked())
    ui->ignoreMaxVolumeCheckBox->setEnabled(false);

  setComboboxIndexByData(ui->devAddedCombo, settings().value(QStringLiteral(SETTINGS_DEVICE), SETTINGS_DEFAULT_DEVICE),
                         1);

  ui->muteOnMiddleClickCheckBox->setChecked(
      settings().value(QStringLiteral(SETTINGS_MUTE_ON_MIDDLECLICK), SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());
  ui->mixerLineEdit->setText(
      settings()
          .value(QStringLiteral(SETTINGS_MIXER_COMMAND), QStringLiteral(SETTINGS_DEFAULT_MIXER_COMMAND))
          .toString());
  ui->stepSpinBox->setValue(settings().value(QStringLiteral(SETTINGS_STEP), SETTINGS_DEFAULT_STEP).toInt());
  ui->ignoreMaxVolumeCheckBox->setChecked(
      settings().value(QStringLiteral(SETTINGS_IGNORE_MAX_VOLUME), SETTINGS_DEFAULT_IGNORE_MAX_VOLUME).toBool());
  ui->alwaysShowNotificationsCheckBox->setChecked(
      settings()
          .value(QStringLiteral(SETTINGS_ALWAYS_SHOW_NOTIFICATIONS), SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS)
          .toBool());

  // always showing notifications is the sufficient condition for showing them with keyboard
  if (ui->alwaysShowNotificationsCheckBox->isChecked()) {
    ui->showKeyboardNotificationsCheckBox->setChecked(true);
    ui->showKeyboardNotificationsCheckBox->setEnabled(false);
  }
  else {
    ui->showKeyboardNotificationsCheckBox->setChecked(
        settings()
            .value(QStringLiteral(SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS), SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS)
            .toBool());
  }

  mLockSettingChanges = false;
}
