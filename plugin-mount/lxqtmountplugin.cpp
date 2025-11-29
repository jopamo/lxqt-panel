/* plugin-mount/lxqtmountplugin.cpp
 * Mount plugin implementation
 */

#include <QGuiApplication>

#include "lxqtmountplugin.h"
#include "configuration.h"

#include <LXQt/Notification>

#include <Solid/DeviceNotifier>

LXQtMountPlugin::LXQtMountPlugin(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo), mPopup(nullptr), mDeviceAction(nullptr), mEjectAction(nullptr) {
  mButton = new Button;
  mPopup = new Popup(this);

  connect(mButton, &QToolButton::clicked, mPopup, &Popup::showHide);
  connect(mPopup, &Popup::visibilityChanged, mButton, &QToolButton::setDown);
  // Note: postpone creation of the mDeviceAction to not fire it in startup time
  QTimer::singleShot(0, this, &LXQtMountPlugin::settingsChanged);
}

LXQtMountPlugin::~LXQtMountPlugin() {
  delete mButton;
  delete mPopup;
}

QDialog* LXQtMountPlugin::configureDialog() {
  if (mPopup)
    mPopup->hide();

  Configuration* configWindow = new Configuration(settings());
  configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
  return configWindow;
}

void LXQtMountPlugin::realign() {
  // nothing to do
}

void LXQtMountPlugin::settingsChanged() {
  QString s = settings()->value(QLatin1String(CFG_KEY_ACTION)).toString();
  DeviceAction::ActionId devActionId = DeviceAction::stringToActionId(s, DeviceAction::ActionMenu);

  if (mDeviceAction == nullptr || mDeviceAction->Type() != devActionId) {
    delete mDeviceAction;
    mDeviceAction = DeviceAction::create(devActionId, this, this);

    connect(mPopup, &Popup::deviceAdded, mDeviceAction, &DeviceAction::onDeviceAdded);
    connect(mPopup, &Popup::deviceRemoved, mDeviceAction, &DeviceAction::onDeviceRemoved);
  }

  s = settings()->value(QLatin1String(CFG_EJECT_ACTION)).toString();
  EjectAction::ActionId ejActionId = EjectAction::stringToActionId(s, EjectAction::ActionNothing);

  if (mEjectAction == nullptr || mEjectAction->Type() != ejActionId) {
    delete mEjectAction;
    mEjectAction = EjectAction::create(ejActionId, this, this);
  }
}