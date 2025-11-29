/* plugin-volume/lxqtvolume.h
 * Volume control plugin implementation
 */

#ifndef LXQTVOLUME_H
#define LXQTVOLUME_H

#include "../panel/ilxqtpanelplugin.h"
#include <QToolButton>
#include <QSlider>
#include <QPointer>

class VolumeButton;
class AudioEngine;
class AudioDevice;
namespace LXQt {
class Notification;
}

class LXQtVolumeConfiguration;

class LXQtVolume : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtVolume(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtVolume();

  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("Volume"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  void realign();
  QDialog* configureDialog();

  void setAudioEngine(AudioEngine* engine);
 protected slots:
  virtual void settingsChanged();
  void handleSinkListChanged();
  void showNotification(bool forceShow) const;

 private:
  AudioEngine* m_engine;
  VolumeButton* m_volumeButton;
  int m_defaultSinkIndex;
  AudioDevice* m_defaultSink;
  LXQt::Notification* m_notification;
  QPointer<LXQtVolumeConfiguration> m_configDialog;
  bool m_alwaysShowNotifications;
  bool m_showKeyboardNotifications;
};

class LXQtVolumePluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtVolume(startupInfo);
  }
};

#endif  // LXQTVOLUME_H