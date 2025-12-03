/* plugin-volume/1g4-mixer/src/sourcewidget.h
 * Implementation of sourcewidget.h
 */

#ifndef sourcewidget_h
#define sourcewidget_h

#include "1g4-mixer.h"

#include "devicewidget.h"

class SourceWidget : public DeviceWidget {
  Q_OBJECT
 public:
  SourceWidget(MainWindow* parent);
  static SourceWidget* create(MainWindow* mainWindow);

  SourceType type;
  bool can_decibel;

  virtual void onMuteToggleButton();
  virtual void executeVolumeUpdate();
  virtual void onDefaultToggleButton();

 protected:
  virtual void onPortChange();
};

#endif
