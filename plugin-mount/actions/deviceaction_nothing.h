/* plugin-mount/actions/deviceaction_nothing.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_DEVICEACTION_NOTHING_H
#define LXQT_PLUGIN_MOUNT_DEVICEACTION_NOTHING_H

#include "deviceaction.h"
#include <QWidget>

class DeviceActionNothing : public DeviceAction {
  Q_OBJECT

 public:
  explicit DeviceActionNothing(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual ActionId Type() const throw() { return ActionNothing; };

 protected:
  void doDeviceAdded(Solid::Device device);
  void doDeviceRemoved(Solid::Device device);
};

#endif  // DEVICEACTIONNOTHING_H