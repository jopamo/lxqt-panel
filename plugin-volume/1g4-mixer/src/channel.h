/* plugin-volume/1g4-mixer/src/channel.h
 * Implementation of channel.h
 */

#ifndef channel_h
#define channel_h

#include <QObject>
#include "1g4-mixer.h"

class QGridLayout;
class QLabel;
class QSlider;
class MinimalStreamWidget;

class Channel : public QObject {
  Q_OBJECT
 public:
  Channel(QGridLayout* parent = nullptr);

  void setVolume(pa_volume_t volume);
  void setVisible(bool visible);
  void setEnabled(bool enabled);

  int channel;
  MinimalStreamWidget* minimalStreamWidget;

 protected Q_SLOTS:
  void onVolumeScaleValueChanged(int value);
  void onVolumeScaleSliderMoved(int value);

 public:
  bool can_decibel;
  bool volumeScaleEnabled;
  bool last;

  QLabel* channelLabel;
  QSlider* volumeScale;
  QLabel* volumeLabel;

  // virtual void set_sensitive(bool enabled);
  virtual void setBaseVolume(pa_volume_t);
};

#endif
