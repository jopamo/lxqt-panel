/* plugin-mount/actions/ejectaction.cpp
 * Mount plugin implementation
 */

#include "ejectaction.h"
#include "ejectaction_nothing.h"
#include "ejectaction_optical.h"
#include "../lxqtmountplugin.h"

#define ACT_NOTHING "nothing"
#define ACT_EJECT_OPTICAL "ejectOpticalDrives"

#define ACT_NOTHING_UPPER QStringLiteral(ACT_NOTHING).toUpper()
#define ACT_EJECT_OPTICAL_UPPER QStringLiteral(ACT_EJECT_OPTICAL).toUpper()

EjectAction::EjectAction(LXQtMountPlugin* plugin, QObject* parent) : QObject(parent), mPlugin(plugin) {}

EjectAction::~EjectAction() = default;

EjectAction* EjectAction::create(ActionId id, LXQtMountPlugin* plugin, QObject* parent) {
  switch (id) {
    case ActionNothing:
      return new EjectActionNothing(plugin, parent);

    case ActionOptical:
      return new EjectActionOptical(plugin, parent);
  }

  return nullptr;
}

QString EjectAction::actionIdToString(EjectAction::ActionId id) {
  switch (id) {
    case ActionNothing:
      return QStringLiteral(ACT_NOTHING);
    case ActionOptical:
      return QStringLiteral(ACT_EJECT_OPTICAL);
  }

  return QStringLiteral(ACT_NOTHING);
}

void EjectAction::onEjectPressed(void) {
  doEjectPressed();
}

EjectAction::ActionId EjectAction::stringToActionId(const QString& string, ActionId defaultValue) {
  QString s = string.toUpper();
  if (s == ACT_NOTHING_UPPER)
    return ActionNothing;
  if (s == ACT_EJECT_OPTICAL_UPPER)
    return ActionOptical;

  return defaultValue;
}