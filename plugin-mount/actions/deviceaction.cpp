/* plugin-mount/actions/deviceaction.cpp
 * Mount plugin implementation
 */

#include "deviceaction.h"
#include "deviceaction_info.h"
#include "deviceaction_menu.h"
#include "deviceaction_nothing.h"
#include "../menudiskitem.h"
#include "../lxqtmountplugin.h"

#include <Solid/StorageAccess>

#define ACT_NOTHING "nothing"
#define ACT_INFO "showInfo"
#define ACT_MENU "showMenu"

#define ACT_NOTHING_UPPER QStringLiteral(ACT_NOTHING).toUpper()
#define ACT_INFO_UPPER QStringLiteral(ACT_INFO).toUpper()
#define ACT_MENU_UPPER QStringLiteral(ACT_MENU).toUpper()

DeviceAction::DeviceAction(LXQtMountPlugin* plugin, QObject* parent) : QObject(parent), mPlugin(plugin) {}

DeviceAction::~DeviceAction() = default;

DeviceAction* DeviceAction::create(ActionId id, LXQtMountPlugin* plugin, QObject* parent) {
  switch (id) {
    case ActionNothing:
      return new DeviceActionNothing(plugin, parent);

    case ActionInfo:
      return new DeviceActionInfo(plugin, parent);

    case ActionMenu:
      return new DeviceActionMenu(plugin, parent);
  }

  return nullptr;
}

QString DeviceAction::actionIdToString(DeviceAction::ActionId id) {
  switch (id) {
    case ActionNothing:
      return QStringLiteral(ACT_NOTHING);
    case ActionInfo:
      return QStringLiteral(ACT_INFO);
    case ActionMenu:
      return QStringLiteral(ACT_MENU);
  }

  return QStringLiteral(ACT_INFO);
}

void DeviceAction::onDeviceAdded(Solid::Device device) {
  mKnownDeviceDescriptions[device.udi()] = device.description();
  doDeviceAdded(device);
}

void DeviceAction::onDeviceRemoved(Solid::Device device) {
  doDeviceRemoved(device);
  mKnownDeviceDescriptions.remove(device.udi());
}

DeviceAction::ActionId DeviceAction::stringToActionId(const QString& string, ActionId defaultValue) {
  QString s = string.toUpper();
  if (s == ACT_NOTHING_UPPER)
    return ActionNothing;
  if (s == ACT_INFO_UPPER)
    return ActionInfo;
  if (s == ACT_MENU_UPPER)
    return ActionMenu;

  return defaultValue;
}