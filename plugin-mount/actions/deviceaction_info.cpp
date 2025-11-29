/* plugin-mount/actions/deviceaction_info.cpp
 * Mount plugin implementation
 */

#include "../lxqtmountplugin.h"
#include "deviceaction_info.h"

#include <LXQt/Notification>

DeviceActionInfo::DeviceActionInfo(LXQtMountPlugin* plugin, QObject* parent) : DeviceAction(plugin, parent) {}

void DeviceActionInfo::doDeviceAdded(Solid::Device device) {
  showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(device.description()));
}

void DeviceActionInfo::doDeviceRemoved(Solid::Device device) {
  showMessage(tr("The device <b><nobr>\"%1\"</nobr></b> is removed.")
                  .arg(device.description().isEmpty() ? mKnownDeviceDescriptions[device.udi()] : device.description()));
}

void DeviceActionInfo::showMessage(const QString& text) {
  LXQt::Notification::notify(tr("Removable media/devices manager"), text, mPlugin->icon().name());
}