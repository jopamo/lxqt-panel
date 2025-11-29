/* plugin-mount/actions/ejectaction_optical.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_EJECTACTION_OPTICAL_H
#define LXQT_PLUGIN_MOUNT_EJECTACTION_OPTICAL_H

#include "ejectaction.h"

#include <QWidget>

class EjectActionOptical : public EjectAction {
  Q_OBJECT
 public:
  explicit EjectActionOptical(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual ActionId Type() const throw() { return ActionOptical; }

 protected:
  void doEjectPressed(void);
};

#endif  // EJECTACTION_OPTICAL_H