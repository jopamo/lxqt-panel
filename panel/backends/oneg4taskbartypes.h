/* panel/backends/oneg4taskbartypes.h
 * Window manager backend interfaces
 */

#ifndef ONEG4TASKBARTYPES_H
#define ONEG4TASKBARTYPES_H

#include <QtGlobal>

typedef quintptr WId;

enum class OneG4TaskBarBackendAction {
  Move = 0,
  Resize,
  Maximize,
  MaximizeVertically,
  MaximizeHorizontally,
  Minimize,
  RollUp,
  FullScreen,
  DesktopSwitch,
  MoveToDesktop,
  MoveToLayer,
  MoveToOutput
};

enum class OneG4TaskBarWindowProperty { Title = 0, Icon, State, Geometry, Urgency, WindowClass, Workspace };

enum class OneG4TaskBarWindowState {
  Hidden = 0,
  FullScreen,
  Minimized,
  Maximized,
  MaximizedVertically,
  MaximizedHorizontally,
  Normal,
  RolledUp  // Shaded
};

enum class OneG4TaskBarWindowLayer { KeepBelow = 0, Normal, KeepAbove };

#endif  // ONEG4TASKBARTYPES_H