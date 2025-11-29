/* plugin-mount/actions/deviceaction_nothing.cpp
 * Mount plugin implementation
 */

#include "deviceaction_nothing.h"

DeviceActionNothing::DeviceActionNothing(LXQtMountPlugin* plugin, QObject* parent) : DeviceAction(plugin, parent) {}

void DeviceActionNothing::doDeviceAdded(Solid::Device /*device*/) {}

void DeviceActionNothing::doDeviceRemoved(Solid::Device /*device*/) {}