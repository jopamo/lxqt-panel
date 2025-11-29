#include "lxqtvolume.h"

#include "volumebutton.h"
#include "volumepopup.h"
#include "lxqtvolumeconfiguration.h"
#include "audiodevice.h"
#ifdef USE_PULSEAUDIO
#include "pulseaudioengine.h"
#endif
#ifdef USE_ALSA
#include "alsaengine.h"
#endif
#include "ossengine.h"

#include <QMessageBox>
#include <XdgIcon>
#include <LXQt/Notification>
#include <algorithm>

LXQtVolume::LXQtVolume(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(),
      ILXQtPanelPlugin(startupInfo),
      m_engine(nullptr),
      m_defaultSinkIndex(0),
      m_defaultSink(nullptr),
      m_alwaysShowNotifications(SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS),
      m_showKeyboardNotifications(SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS) {
  m_volumeButton = new VolumeButton(this);

  m_notification = new LXQt::Notification(QLatin1String(""), this);

  settingsChanged();
}

LXQtVolume::~LXQtVolume() {
  delete m_volumeButton;
}

void LXQtVolume::setAudioEngine(AudioEngine* engine) {
  if (!engine)
    return;

  if (m_engine && m_engine->backendName() == engine->backendName()) {
    delete engine;
    return;
  }

  if (m_engine) {
    if (m_defaultSink) {
      disconnect(m_defaultSink, nullptr, this, nullptr);
      m_defaultSink = nullptr;
    }
    m_volumeButton->volumePopup()->setDevice(nullptr);

    disconnect(m_engine, nullptr, nullptr, nullptr);
    delete m_engine;
    m_engine = nullptr;
  }

  m_engine = engine;
  connect(m_engine, &AudioEngine::sinkListChanged, this, &LXQtVolume::handleSinkListChanged);

  handleSinkListChanged();
}

void LXQtVolume::settingsChanged() {
  m_defaultSinkIndex = settings()->value(QStringLiteral(SETTINGS_DEVICE), SETTINGS_DEFAULT_DEVICE).toInt();
  const QString engineName =
      settings()
          ->value(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral(SETTINGS_DEFAULT_AUDIO_ENGINE))
          .toString();
  const bool newEngine = !m_engine || m_engine->backendName() != engineName;
  if (newEngine) {
#if defined(USE_PULSEAUDIO) && defined(USE_ALSA)
    if (engineName == QLatin1String("PulseAudio"))
      setAudioEngine(new PulseAudioEngine(this));
    else if (engineName == QLatin1String("Alsa"))
      setAudioEngine(new AlsaEngine(this));
    else
      setAudioEngine(new OssEngine(this));  // fallback to OSS
#elif defined(USE_PULSEAUDIO)
    if (engineName == QLatin1String("PulseAudio"))
      setAudioEngine(new PulseAudioEngine(this));
    else
      setAudioEngine(new OssEngine(this));  // fallback to OSS
#elif defined(USE_ALSA)
    if (engineName == QLatin1String("Alsa"))
      setAudioEngine(new AlsaEngine(this));
    else
      setAudioEngine(new OssEngine(this));  // fallback to OSS
#else
    // no other backends are available, fallback to OSS
    setAudioEngine(new OssEngine(this));
#endif
  }

  m_volumeButton->setMuteOnMiddleClick(
      settings()->value(QStringLiteral(SETTINGS_MUTE_ON_MIDDLECLICK), SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());
  m_volumeButton->setMixerCommand(
      settings()
          ->value(QStringLiteral(SETTINGS_MIXER_COMMAND), QStringLiteral(SETTINGS_DEFAULT_MIXER_COMMAND))
          .toString());
  m_volumeButton->volumePopup()->setSliderStep(
      settings()->value(QStringLiteral(SETTINGS_STEP), SETTINGS_DEFAULT_STEP).toInt());

  m_alwaysShowNotifications =
      settings()
          ->value(QStringLiteral(SETTINGS_ALWAYS_SHOW_NOTIFICATIONS), SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS)
          .toBool();
  m_showKeyboardNotifications =
      settings()
          ->value(QStringLiteral(SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS), SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS)
          .toBool()
      // in case the config file was edited manually (see LXQtVolumeConfiguration)
      || m_alwaysShowNotifications;

  if (!newEngine)
    handleSinkListChanged();
}

void LXQtVolume::handleSinkListChanged() {
  if (!m_engine)
    return;

  const auto& sinks = m_engine->sinks();
  if (!sinks.isEmpty()) {
    const qsizetype idx = std::clamp<qsizetype>(m_defaultSinkIndex, 0, sinks.count() - 1);
    AudioDevice* newDefaultSink = sinks.at(idx);

    if (newDefaultSink != m_defaultSink) {
      if (m_defaultSink)
        disconnect(m_defaultSink, nullptr, this, nullptr);

      m_defaultSink = newDefaultSink;
      m_volumeButton->volumePopup()->setDevice(m_defaultSink);

      connect(m_defaultSink, &AudioDevice::volumeChanged, this, [this] { LXQtVolume::showNotification(false); });
      connect(m_defaultSink, &AudioDevice::muteChanged, this, [this] { LXQtVolume::showNotification(false); });
    }

    m_engine->setIgnoreMaxVolume(
        settings()->value(QStringLiteral(SETTINGS_IGNORE_MAX_VOLUME), SETTINGS_DEFAULT_IGNORE_MAX_VOLUME).toBool());
  }
  else {
    if (m_defaultSink) {
      disconnect(m_defaultSink, nullptr, this, nullptr);
      m_defaultSink = nullptr;
      m_volumeButton->volumePopup()->setDevice(nullptr);
    }
  }

  if (m_configDialog)
    m_configDialog->setSinkList(sinks);
}

QWidget* LXQtVolume::widget() {
  return m_volumeButton;
}

void LXQtVolume::realign() {}

QDialog* LXQtVolume::configureDialog() {
  if (!m_configDialog) {
    const bool ossAvailable = (m_engine && m_engine->backendName() == QLatin1String("Oss"))
                                  ? !m_engine->sinks().isEmpty()
                                  : !OssEngine().sinks().isEmpty();

    m_configDialog = new LXQtVolumeConfiguration(settings(), ossAvailable);
    m_configDialog->setAttribute(Qt::WA_DeleteOnClose, true);

    if (m_engine)
      m_configDialog->setSinkList(m_engine->sinks());
  }
  return m_configDialog;
}

void LXQtVolume::showNotification(bool forceShow) const {
  // force only if volume change should be notified with keyboard
  if ((forceShow && m_showKeyboardNotifications) || m_alwaysShowNotifications) {
    if (Q_LIKELY(m_defaultSink)) {
      m_notification->setSummary(tr("Volume: %1%%2")
                                     .arg(QString::number(m_defaultSink->volume()))
                                     .arg(m_defaultSink->mute() ? tr("(muted)") : QLatin1String("")));
      m_notification->update();
    }
  }
}
