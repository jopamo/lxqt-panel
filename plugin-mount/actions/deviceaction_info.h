/* plugin-mount/actions/deviceaction_info.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_DEVICEACTION_INFO_H
#define LXQT_PLUGIN_MOUNT_DEVICEACTION_INFO_H

#include "deviceaction.h"

#include <QWidget>
#include <QTimer>

class Popup;

class DeviceActionInfo : public DeviceAction {
  Q_OBJECT
 public:
  explicit DeviceActionInfo(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual ActionId Type() const throw() { return ActionInfo; }

 protected:
  void doDeviceAdded(Solid::Device device);
  void doDeviceRemoved(Solid::Device device);

 private:
  void showMessage(const QString& text);
};

#endif  // DEVICEACTION_INFO_H