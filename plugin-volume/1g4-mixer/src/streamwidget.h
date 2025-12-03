/* plugin-volume/1g4-mixer/src/streamwidget.h
 * Implementation of streamwidget.h
 */

#ifndef streamwidget_h
#define streamwidget_h

#include "1g4-mixer.h"

#include "minimalstreamwidget.h"
#include "ui_streamwidget.h"
#include <QTimer>

class MainWindow;
class Channel;
class QAction;

class StreamWidget : public MinimalStreamWidget, public Ui::StreamWidget {
  Q_OBJECT
 public:
  StreamWidget(MainWindow* parent);

  void setChannelMap(const pa_channel_map& m, bool can_decibel);
  void setVolume(const pa_cvolume& volume, bool force = false);
  virtual void updateChannelVolume(int channel, pa_volume_t v);

  void hideLockedChannels(bool hide = true);

  pa_channel_map channelMap;
  pa_cvolume volume;

  Channel* channels[PA_CHANNELS_MAX];

  virtual void onMuteToggleButton();
  virtual void onLockToggleButton();
  virtual void onDeviceChangePopup();
  // virtual bool onContextTriggerEvent(GdkEventButton*);

  QTimer timeout;

  bool timeoutEvent();

  virtual void executeVolumeUpdate();
  virtual void onKill();

 protected:
  MainWindow* mpMainWindow;

  QAction* terminate;
};

#endif
