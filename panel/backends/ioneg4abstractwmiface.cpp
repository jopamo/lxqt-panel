/* panel/backends/ioneg4abstractwmiface.cpp
 * Window manager backend interfaces
 */

#include "ioneg4abstractwmiface.h"

IOneG4AbstractWMInterface::IOneG4AbstractWMInterface(QObject* parent) : QObject(parent) {}

void IOneG4AbstractWMInterface::moveApplicationToPrevNextDesktop(WId windowId, bool next) {
  int count = getWorkspacesCount();
  if (count <= 1)
    return;

  int targetWorkspace = getWindowWorkspace(windowId) + (next ? 1 : -1);

  // Wrap around
  if (targetWorkspace > count)
    targetWorkspace = 1;  // Ids are 1-based
  else if (targetWorkspace < 1)
    targetWorkspace = count;

  setWindowOnWorkspace(windowId, targetWorkspace);
}

int IOneG4AbstractWMInterface::onAllWorkspacesEnum() const {
  // Virtual destops have 1-based indexes.
  // NOTE: The real value of this enum may be negative (as in X11).
  return 0;
}