/* plugin-volume/1g4-mixer/src/sinkinputwidget.h
 * Implementation of sinkinputwidget.h
 */

#ifndef sinkinputwidget_h
#define sinkinputwidget_h

#include "1g4-mixer.h"

#include "streamwidget.h"
#include <QAction>

class MainWindow;
class QMenu;

class SinkInputWidget : public StreamWidget {
  Q_OBJECT
 public:
  SinkInputWidget(MainWindow* parent);
  ~SinkInputWidget(void);

  SinkInputType type;

  uint32_t index, clientIndex;
  void setSinkIndex(uint32_t idx);
  uint32_t sinkIndex();
  virtual void executeVolumeUpdate();
  virtual void onMuteToggleButton();
  virtual void onDeviceChangePopup();
  virtual void onKill();

 private:
  uint32_t mSinkIndex;

  void buildMenu();

  QMenu* menu;

  struct SinkMenuItem : public QAction {
    SinkMenuItem(SinkInputWidget* w, const char* label, uint32_t i, bool active, QObject* parent = nullptr)
        : QAction(QString::fromUtf8(label), parent), widget(w), index(i) {
      setCheckable(true);
      setChecked(active);
      connect(this, &QAction::toggled, [this] { onToggle(); });
    }

    SinkInputWidget* widget;
    uint32_t index;
    void onToggle();
  };
};

#endif
