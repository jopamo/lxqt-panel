/* plugin-mount/actions/ejectaction_nothing.cpp
 * Mount plugin implementation
 */

#include "ejectaction_nothing.h"

EjectActionNothing::EjectActionNothing(LXQtMountPlugin* plugin, QObject* parent) : EjectAction(plugin, parent) {}

void EjectActionNothing::doEjectPressed(void) {}