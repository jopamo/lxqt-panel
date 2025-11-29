/* plugin-tray/xtestsender.h
 * Header file for xtestsender
 */

#pragma once

typedef struct _XDisplay Display;

void sendXTestPressed(Display* display, int button);
void sendXTestReleased(Display* display, int button);