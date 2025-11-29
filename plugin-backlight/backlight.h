/* plugin-backlight/backlight.h
 * Header file for backlight
 */

#ifndef LXQTBACKLIGHT_H
#define LXQTBACKLIGHT_H

#include <QToolButton>
#include <QWheelEvent>
#include <QTimer>
#include "../panel/ilxqtpanelplugin.h"
#include "sliderdialog.h"

namespace LXQt {
class Notification;
}
namespace GlobalKeyShortcut {
class Action;
}

class BacklightButton : public QToolButton {
  Q_OBJECT
 public:
  BacklightButton(QWidget* parent = nullptr);
  ~BacklightButton() {};

 signals:
  void wheel(bool up);

 protected:
  void wheelEvent(QWheelEvent* e) override;

 private:
  int m_mouseWheelThresholdCounter;
};

class LXQtBacklight : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtBacklight(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtBacklight();

  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("Backlight"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment; }

 protected Q_SLOTS:
  void toggleSlider();

 private:
  BacklightButton* m_backlightButton;
  SliderDialog* m_backlightSlider;
  QTimer m_updateTimer;
};

class LXQtBacklightPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtBacklight(startupInfo);
  }
};

#endif  // LXQTBACKLIGHT_H