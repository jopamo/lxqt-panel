/* plugin-volume/1g4-mixer/src/sinkwidget.h
 * Implementation of sinkwidget.h
 */

#ifndef sinkwidget_h
#define sinkwidget_h

#include "1g4-mixer.h"
#include "devicewidget.h"

#if HAVE_EXT_DEVICE_RESTORE_API
#include <pulse/format.h>

#define PAVU_NUM_ENCODINGS 6

class QCheckBox;

typedef struct {
  pa_encoding encoding;
  QCheckBox* widget;
} encodingList;
#endif

class SinkWidget : public DeviceWidget {
  Q_OBJECT
 public:
  SinkWidget(MainWindow* parent);

  SinkType type;
  uint32_t monitor_index;
  bool can_decibel;

#if HAVE_EXT_DEVICE_RESTORE_API
  encodingList encodings[PAVU_NUM_ENCODINGS];
#endif

  virtual void onMuteToggleButton();
  virtual void executeVolumeUpdate();
  virtual void onDefaultToggleButton();
  void setDigital(bool);

 protected Q_SLOTS:
  virtual void onPortChange();
  virtual void onEncodingsChange();
};

#endif
