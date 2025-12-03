/* plugin-volume/1g4-mixer/src/rolewidget.h
 * Implementation of rolewidget.h
 */

#ifndef rolewidget_h
#define rolewidget_h

#include "1g4-mixer.h"

#include "streamwidget.h"

class RoleWidget : public StreamWidget {
  Q_OBJECT
 public:
  RoleWidget(MainWindow* parent);

  QByteArray role;
  QByteArray device;

  virtual void onMuteToggleButton();
  virtual void executeVolumeUpdate();
};

#endif
