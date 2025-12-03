/* plugin-volume/1g4-mixer/src/minimalstreamwidget.h
 * Implementation of minimalstreamwidget.h
 */

#ifndef minimalstreamwidget_h
#define minimalstreamwidget_h

#include "1g4-mixer.h"
#include <QWidget>

class QProgressBar;
class QGridLayout;

class MinimalStreamWidget : public QWidget {
  Q_OBJECT
 public:
  MinimalStreamWidget(QWidget* parent = nullptr);
  ~MinimalStreamWidget() override;
  void initPeakProgressBar(QGridLayout* channelsGrid);

  QProgressBar* peakProgressBar;
  double lastPeak;
  pa_stream* peak;

  bool updating;

  virtual void onMuteToggleButton() = 0;
  virtual void onLockToggleButton() = 0;
  virtual void updateChannelVolume(int channel, pa_volume_t v) = 0;

  bool volumeMeterEnabled;
  void enableVolumeMeter();
  void updatePeak(double v);
  void setVolumeMeterVisible(bool v);

 private:
  bool volumeMeterVisible;
};

#endif
