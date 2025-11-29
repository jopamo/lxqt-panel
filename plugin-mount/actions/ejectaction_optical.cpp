/* plugin-mount/actions/ejectaction_optical.cpp
 * Mount plugin implementation
 */

#include "../lxqtmountplugin.h"
#include "ejectaction_optical.h"

#include <Solid/StorageAccess>
#include <Solid/StorageDrive>
#include <Solid/DeviceNotifier>
#include <Solid/OpticalDrive>

#include <LXQt/Notification>
// #include <QDebug>

EjectActionOptical::EjectActionOptical(LXQtMountPlugin* plugin, QObject* parent) : EjectAction(plugin, parent) {}

void EjectActionOptical::doEjectPressed(void) {
  for (const Solid::Device& device : Solid::Device::listFromType(Solid::DeviceInterface::OpticalDrive)) {
    Solid::Device it;
    if (device.isValid()) {
      it = device;
      // qDebug() << "device : " << it.udi() << "\n";
      //  search for parent drive
      for (; !it.udi().isEmpty(); it = it.parent())
        if (it.is<Solid::OpticalDrive>())
          it.as<Solid::OpticalDrive>()->eject();
    }
  }
  LXQt::Notification::notify(tr("Removable media/devices manager"), tr("Ejected all optical drives"),
                             mPlugin->icon().name());
}