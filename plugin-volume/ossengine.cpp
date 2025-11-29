#include "ossengine.h"
#include "audiodevice.h"

#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <linux/soundcard.h>

OssEngine::OssEngine(QObject* parent)
    : AudioEngine(parent), m_mixer(-1), m_device(nullptr), m_leftVolume(0), m_rightVolume(0) {
  qDebug() << "OssEngine";
  initMixer();
}

OssEngine::~OssEngine() {
  if (m_mixer >= 0)
    close(m_mixer);
}

void OssEngine::initMixer() {
  m_mixer = open("/dev/mixer", O_RDWR | O_CLOEXEC);
  if (m_mixer < 0) {
    qDebug() << "/dev/mixer cannot be opened";
    return;
  }
  qDebug() << "InitMixer:" << m_mixer;

  m_device = new AudioDevice(Sink, this);
  m_device->setName(QStringLiteral("Master"));
  m_device->setIndex(0);
  m_device->setDescription(QStringLiteral("Master Volume"));
  m_device->setMuteNoCommit(false);
  updateVolume();

  m_sinks.append(m_device);
  emit sinkListChanged();
}

void OssEngine::updateVolume() {
  if (m_mixer < 0 || !m_device)
    return;

  int volumes = 0;
  if (ioctl(m_mixer, MIXER_READ(SOUND_MIXER_VOLUME), &volumes) < 0) {
    qDebug() << "updateVolume() failed" << errno;
    return;
  }

  m_leftVolume = volumes & 0xff;          // left
  m_rightVolume = (volumes >> 8) & 0xff;  // right
  qDebug() << "volume:" << m_leftVolume << m_rightVolume;

  m_device->setVolumeNoCommit(m_leftVolume);
}

void OssEngine::setVolume(int volume) {
  if (m_mixer < 0)
    return;

  const int volumes = (volume << 8) | volume;
  if (ioctl(m_mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &volumes) < 0)
    qDebug() << "setVolume() failed" << errno;
  else
    qDebug() << "setVolume()" << volume;
}

void OssEngine::commitDeviceVolume(AudioDevice* device) {
  if (!device)
    return;

  setVolume(device->volume());
}

void OssEngine::setMute(AudioDevice* device, bool state) {
  Q_UNUSED(device)

  if (state)
    setVolume(0);
  else
    setVolume(m_leftVolume);
}

void OssEngine::setIgnoreMaxVolume(bool ignore) {
  Q_UNUSED(ignore)
  // OSS backend does not support configurable max volume mapping yet
}
