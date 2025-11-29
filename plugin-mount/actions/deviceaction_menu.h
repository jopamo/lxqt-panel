/* plugin-mount/actions/deviceaction_menu.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_DEVICEACTION_MENU_H
#define LXQT_PLUGIN_MOUNT_DEVICEACTION_MENU_H

#include "deviceaction.h"

#include <QWidget>
#include <QTimer>

class Popup;

class DeviceActionMenu : public DeviceAction {
  Q_OBJECT
 public:
  explicit DeviceActionMenu(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual ActionId Type() const throw() { return ActionMenu; }

 protected:
  void doDeviceAdded(Solid::Device device);
  void doDeviceRemoved(Solid::Device device);

 private:
  Popup* mPopup;
  QTimer mHideTimer;
};

#endif  // DEVICEACTIONMENU_H