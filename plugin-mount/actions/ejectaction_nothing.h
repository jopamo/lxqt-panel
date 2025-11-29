/* plugin-mount/actions/ejectaction_nothing.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_EJECTACTION_NOTHING_H
#define LXQT_PLUGIN_MOUNT_EJECTACTION_NOTHING_H

#include "ejectaction.h"
#include <QWidget>

class EjectActionNothing : public EjectAction {
  Q_OBJECT

 public:
  explicit EjectActionNothing(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual ActionId Type() const throw() { return ActionNothing; };

 protected:
  void doEjectPressed(void);
};

#endif  // EJECTACTION_NOTHING_H