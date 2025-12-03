/* plugin-volume/audiodevice.cpp
 * Volume control plugin implementation
 */

#include "audiodevice.h"
#include "audioengine.h"

AudioDevice::AudioDevice(AudioDeviceType t, AudioEngine* engine, QObject* parent)
    : QObject(parent),
      m_engine(engine),
      m_volume(0),
      m_mute(false),
      m_enabled(true),
      m_type(t),
      m_index(0),
      m_profileName(),
      m_cardId(-1) {}

AudioDevice::~AudioDevice() = default;

void AudioDevice::setName(const QString& name) {
  if (m_name == name)
    return;

  m_name = name;
  emit nameChanged(m_name);
}

void AudioDevice::setDescription(const QString& description) {
  if (m_description == description)
    return;

  m_description = description;
  emit descriptionChanged(m_description);
}

void AudioDevice::setProfileName(const QString& profile) {
  if (m_profileName == profile)
    return;

  m_profileName = profile;
  emit profileNameChanged(m_profileName);
}

void AudioDevice::setIndex(uint index) {
  if (m_index == index)
    return;

  m_index = index;
  emit indexChanged(index);
}

void AudioDevice::setType(AudioDeviceType type) {
  if (m_type == type)
    return;

  m_type = type;
  emit typeChanged(m_type);
}

// this is just for setting the internal volume
void AudioDevice::setVolumeNoCommit(int volume) {
  if (m_engine)
    volume = m_engine->volumeBounded(volume, this);

  if (m_volume == volume)
    return;

  m_volume = volume;
  emit volumeChanged(m_volume);
}

void AudioDevice::toggleMute() {
  setMute(!m_mute);
}

void AudioDevice::setMute(bool state) {
  if (m_mute == state)
    return;

  setMuteNoCommit(state);

  if (m_engine)
    m_engine->setMute(this, state);
}

void AudioDevice::setMuteNoCommit(bool state) {
  if (m_mute == state)
    return;

  m_mute = state;
  emit muteChanged(m_mute);
}

// this performs a volume change on the device
void AudioDevice::setVolume(int volume) {
  if (m_volume == volume)
    return;

  setVolumeNoCommit(volume);

  if (m_engine)
    m_engine->commitDeviceVolume(this);
}

void AudioDevice::setEnabled(bool enabled) {
  if (m_enabled == enabled)
    return;

  m_enabled = enabled;
  emit enabledChanged(m_enabled);
}

void AudioDevice::setCardId(int cardId) {
  if (m_cardId == cardId)
    return;

  m_cardId = cardId;
  emit cardIdChanged(m_cardId);
}
