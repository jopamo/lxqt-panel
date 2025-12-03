/* panel/oneg4panelglobals.h
 * Main panel implementation, window management
 */

#ifndef __ONEG4_PANEL_GLOBALS_H__
#define __ONEG4_PANEL_GLOBALS_H__

#include <QtGlobal>

#ifdef COMPILE_ONEG4_PANEL
#define ONEG4_PANEL_API Q_DECL_EXPORT
#else
#define ONEG4_PANEL_API Q_DECL_IMPORT
#endif

#endif  // __ONEG4_PANEL_GLOBALS_H__