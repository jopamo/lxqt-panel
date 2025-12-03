/* plugin-volume/oneg4volume.cpp
 * Volume control plugin implementation
 */

#include "oneg4volume.h"

#include "audiodevice.h"
#include "audioengine.h"
#include "oneg4volumeconfiguration.h"
#ifdef USE_PIPEWIRE
#include "pipewireengine.h"
#endif
#ifdef USE_PULSEAUDIO
#include "pulseaudioengine.h"
#endif
#include "volumebutton.h"
#include "volumepopup.h"
#include "1g4-mixer.h"

#include <OneG4/Notification.h>
#include <QDialog>
#include <QMessageBox>
#include <QtGlobal>
#include <XdgIcon.h>

#include <algorithm>

OneG4Volume::OneG4Volume(const IOneG4PanelPluginStartupInfo& startupInfo)
    : QObject(),
      IOneG4PanelPlugin(startupInfo),
      m_engine(nullptr),
      m_defaultSinkIndex(0),
      m_defaultSink(nullptr),
      m_configDialog(nullptr),
      m_mixerDialog(nullptr),
      m_alwaysShowNotifications(SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS),
      m_showKeyboardNotifications(SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS) {
  m_volumeButton = new VolumeButton(this);

  m_notification = new OneG4::Notification(QString(), this);

  connect(m_volumeButton, &VolumeButton::mixerRequested, this, &OneG4Volume::openMixer);

  settingsChanged();
}

OneG4Volume::~OneG4Volume() {
  delete m_mixerDialog;
}

void OneG4Volume::setAudioEngine(AudioEngine* engine) {
  if (!engine) {
    return;
  }

  if (m_engine && m_engine->backendName() == engine->backendName()) {
    delete engine;
    return;
  }

  if (m_engine) {
    if (m_defaultSink) {
      disconnect(m_defaultSink, nullptr, this, nullptr);
      m_defaultSink = nullptr;
    }

    if (m_volumeButton && m_volumeButton->volumePopup()) {
      m_volumeButton->volumePopup()->setDevice(nullptr);
    }

    disconnect(m_engine, nullptr, this, nullptr);
    delete m_engine;
    m_engine = nullptr;
    m_defaultSink = nullptr;
  }

  m_engine = engine;

  connect(m_engine, &AudioEngine::sinkListChanged, this, &OneG4Volume::handleSinkListChanged);

  handleSinkListChanged();
}

void OneG4Volume::settingsChanged() {
  m_defaultSinkIndex = settings()->value(QStringLiteral(SETTINGS_DEVICE), SETTINGS_DEFAULT_DEVICE).toInt();

  const QString engineName =
      settings()
          ->value(QStringLiteral(SETTINGS_AUDIO_ENGINE), QStringLiteral(SETTINGS_DEFAULT_AUDIO_ENGINE))
          .toString();

  const bool newEngine = !m_engine || m_engine->backendName() != engineName;
  if (newEngine) {
    AudioEngine* engine = nullptr;
    if (engineName == QLatin1String("PipeWire")) {
#ifdef USE_PIPEWIRE
      engine = new PipeWireEngine(this);
#endif
    }
    else if (engineName == QLatin1String("PulseAudio")) {
#ifdef USE_PULSEAUDIO
      engine = new PulseAudioEngine(this);
#endif
    }
    if (!engine) {
      // fallback to first available backend
#ifdef USE_PULSEAUDIO
      engine = new PulseAudioEngine(this);
#elif USE_PIPEWIRE
      engine = new PipeWireEngine(this);
#endif
    }
    if (engine) {
      setAudioEngine(engine);
    }
  }

  m_volumeButton->setMuteOnMiddleClick(
      settings()->value(QStringLiteral(SETTINGS_MUTE_ON_MIDDLECLICK), SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());

  m_volumeButton->volumePopup()->setSliderStep(
      settings()->value(QStringLiteral(SETTINGS_STEP), SETTINGS_DEFAULT_STEP).toInt());

  m_alwaysShowNotifications =
      settings()
          ->value(QStringLiteral(SETTINGS_ALWAYS_SHOW_NOTIFICATIONS), SETTINGS_DEFAULT_ALWAYS_SHOW_NOTIFICATIONS)
          .toBool();

  m_showKeyboardNotifications =
      settings()
          ->value(QStringLiteral(SETTINGS_SHOW_KEYBOARD_NOTIFICATIONS), SETTINGS_DEFAULT_SHOW_KEYBOARD_NOTIFICATIONS)
          .toBool() ||
      m_alwaysShowNotifications;

  if (!newEngine) {
    handleSinkListChanged();
  }
}

void OneG4Volume::handleSinkListChanged() {
  if (!m_engine) {
    return;
  }

  const auto& sinks = m_engine->sinks();
  if (!sinks.isEmpty()) {
    const qsizetype idx = std::clamp<qsizetype>(m_defaultSinkIndex, 0, sinks.count() - 1);
    AudioDevice* newDefaultSink = sinks.at(idx);

    if (newDefaultSink != m_defaultSink) {
      if (m_defaultSink) {
        disconnect(m_defaultSink, nullptr, this, nullptr);
      }

      m_defaultSink = newDefaultSink;

      if (m_volumeButton && m_volumeButton->volumePopup()) {
        m_volumeButton->volumePopup()->setDevice(m_defaultSink);
      }

      if (auto* sink = m_defaultSink.data()) {
        connect(sink, &AudioDevice::volumeChanged, this, [this] { showNotification(false); });
        connect(sink, &AudioDevice::muteChanged, this, [this] { showNotification(false); });
      }
    }

    m_engine->setIgnoreMaxVolume(
        settings()->value(QStringLiteral(SETTINGS_IGNORE_MAX_VOLUME), SETTINGS_DEFAULT_IGNORE_MAX_VOLUME).toBool());
  }
  else {
    if (m_defaultSink) {
      disconnect(m_defaultSink, nullptr, this, nullptr);
      m_defaultSink = nullptr;
    }

    if (m_volumeButton && m_volumeButton->volumePopup()) {
      m_volumeButton->volumePopup()->setDevice(nullptr);
    }
  }

  if (m_configDialog) {
    m_configDialog->setSinkList(sinks);
  }
}

QWidget* OneG4Volume::widget() {
  return m_volumeButton;
}

void OneG4Volume::realign() {}

QDialog* OneG4Volume::configureDialog() {
  if (!m_configDialog) {
    m_configDialog = new OneG4VolumeConfiguration(settings(), false);
    m_configDialog->setAttribute(Qt::WA_DeleteOnClose, true);

    if (m_engine) {
      m_configDialog->setSinkList(m_engine->sinks());
    }

    connect(m_configDialog, &QObject::destroyed, this, [this] { m_configDialog = nullptr; });
  }

  return m_configDialog;
}

void OneG4Volume::showNotification(bool forceShow) const {
  if ((forceShow && m_showKeyboardNotifications) || m_alwaysShowNotifications) {
    if (Q_LIKELY(m_defaultSink)) {
      const int vol = m_defaultSink->volume();
      const bool muted = m_defaultSink->mute();

      m_notification->setSummary(tr("Volume: %1%2").arg(QString::number(vol), muted ? tr(" (muted)") : QString()));

      m_notification->update();
    }
  }
}

void OneG4Volume::openMixer() {
  if (!m_engine) {
    QMessageBox::warning(m_volumeButton, tr("Audio"), tr("No audio engine is available"));
    return;
  }

  if (!m_mixerDialog) {
    m_mixerDialog = create_1g4_mixer_dialog();
    if (!m_mixerDialog) {
      QMessageBox::warning(m_volumeButton, tr("Audio"), tr("Failed to create mixer dialog"));
      return;
    }
    // Don't parent to volume button to avoid focus issues with panel window
    m_mixerDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    connect(m_mixerDialog, &QObject::destroyed, this, [this] { m_mixerDialog = nullptr; });
  }

  m_mixerDialog->show();
  m_mixerDialog->raise();
}
