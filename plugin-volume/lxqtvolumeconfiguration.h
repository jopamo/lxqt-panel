/* plugin-volume/lxqtvolumeconfiguration.h
 * Volume control plugin implementation
 */

#ifndef LXQTVOLUMECONFIGURATION_H
#define LXQTVOLUMECONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

#include <QList>

#define SETTINGS_MIXER_COMMAND "mixerCommand"
#define SETTINGS_MUTE_ON_MIDDLECLICK "showOnMiddleClick"
#define SETTINGS_DEVICE "device"
#define SETTINGS_STEP "volumeAdjustStep"
#define SETTINGS_IGNORE_MAX_VOLUME "ignoreMaxVolume"
#define SETTINGS_AUDIO_ENGINE "audioEngine"
#define SETTINGS_ALWAYS_SHOW_NOTIFICATIONS "alwaysShowNotifications"
#define SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS "showKeyboardNotifications"

#define SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK true
#define SETTINGS_DEFAULT_DEVICE 0
#define SETTINGS_DEFAULT_STEP 3
#ifdef USE_PULSEAUDIO
#define SETTINGS_DEFAULT_MIXER_COMMAND "pavucontrol-qt"
#define SETTINGS_DEFAULT_AUDIO_ENGINE "PulseAudio"
#elif defined(USE_ALSA)
#define SETTINGS_DEFAULT_MIXER_COMMAND "qasmixer"
#define SETTINGS_DEFAULT_AUDIO_ENGINE "Alsa"
#else
#define SETTINGS_DEFAULT_MIXER_COMMAND ""
#define SETTINGS_DEFAULT_AUDIO_ENGINE "Oss"
#endif
#define SETTINGS_DEFAULT_IGNORE_MAX_VOLUME false
#define SETTINGS_DEFAULT_IGNORE_MAX_VOLUME false
#define SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS false
#define SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS true

class AudioDevice;

namespace Ui {
class LXQtVolumeConfiguration;
}

class LXQtVolumeConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtVolumeConfiguration(PluginSettings* settings, bool ossAvailable, QWidget* parent = nullptr);
  ~LXQtVolumeConfiguration();

 public slots:
  void setSinkList(const QList<AudioDevice*> sinks);
  void audioEngineChanged(bool checked);
  void sinkSelectionChanged(int index);
  void muteOnMiddleClickChanged(bool state);
  void mixerLineEditChanged(const QString& command);
  void stepSpinBoxChanged(int step);
  void ignoreMaxVolumeCheckBoxChanged(bool state);
  void alwaysShowNotificationsCheckBoxChanged(bool state);
  void showKeyboardNotificationsCheckBoxChanged(bool state);

 protected slots:
  virtual void loadSettings();

 private:
  Ui::LXQtVolumeConfiguration* ui;
  bool mLockSettingChanges;
};

#endif  // LXQTVOLUMECONFIGURATION_H