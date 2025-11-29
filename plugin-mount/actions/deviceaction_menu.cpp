/* plugin-mount/actions/deviceaction_menu.cpp
 * Mount plugin implementation
 */

#include "deviceaction_menu.h"
#include "../lxqtmountplugin.h"
#include "../popup.h"

DeviceActionMenu::DeviceActionMenu(LXQtMountPlugin* plugin, QObject* parent) : DeviceAction(plugin, parent) {
  mPopup = plugin->popup();

  mHideTimer.setSingleShot(true);
  mHideTimer.setInterval(5000);
  connect(&mHideTimer, &QTimer::timeout, mPopup, &Popup::hide);
}

void DeviceActionMenu::doDeviceAdded(Solid::Device /*device*/) {
  mHideTimer.start();
  mPopup->show();
}

void DeviceActionMenu::doDeviceRemoved(Solid::Device /*device*/) {}