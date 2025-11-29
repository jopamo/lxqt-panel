/* plugin-tray/xtestsender.cpp
 * Implementation file for xtestsender
 */

#include "xtestsender.h"
#include <X11/extensions/XTest.h>

void sendXTestPressed(Display* display, int button) {
  XTestFakeButtonEvent(display, button, true, 0);
}

void sendXTestReleased(Display* display, int button) {
  XTestFakeButtonEvent(display, button, false, 0);
}