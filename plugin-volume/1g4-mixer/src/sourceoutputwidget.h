/* plugin-volume/1g4-mixer/src/sourceoutputwidget.h
 * Implementation of sourceoutputwidget.h
 */

#ifndef sourceoutputwidget_h
#define sourceoutputwidget_h

#include "1g4-mixer.h"

#include "streamwidget.h"
#include <QAction>

class MainWindow;
class QMenu;

class SourceOutputWidget : public StreamWidget {
  Q_OBJECT
 public:
  SourceOutputWidget(MainWindow* parent);
  ~SourceOutputWidget(void);

  SourceOutputType type;

  uint32_t index, clientIndex;
  void setSourceIndex(uint32_t idx);
  uint32_t sourceIndex();
#if HAVE_SOURCE_OUTPUT_VOLUMES
  virtual void executeVolumeUpdate();
  virtual void onMuteToggleButton();
#endif
  virtual void onDeviceChangePopup();
  virtual void onKill();

 private:
  uint32_t mSourceIndex;

  void clearMenu();
  void buildMenu();

  QMenu* menu;

  struct SourceMenuItem : public QAction {
    SourceMenuItem(SourceOutputWidget* w, const char* label, uint32_t i, bool active, QObject* parent = nullptr)
        : QAction{QString::fromUtf8(label), parent}, widget(w), index(i) {
      setCheckable(true);
      setChecked(active);
      connect(this, &QAction::toggled, [this] { onToggle(); });
    }

    SourceOutputWidget* widget;
    uint32_t index;
    void onToggle();
  };
};

#endif
