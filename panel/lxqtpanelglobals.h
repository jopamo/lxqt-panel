/* panel/lxqtpanelglobals.h
 * Main panel implementation, window management
 */

#ifndef __LXQT_PANEL_GLOBALS_H__
#define __LXQT_PANEL_GLOBALS_H__

#include <QtGlobal>

#ifdef COMPILE_LXQT_PANEL
#define LXQT_PANEL_API Q_DECL_EXPORT
#else
#define LXQT_PANEL_API Q_DECL_IMPORT
#endif

#endif  // __LXQT_PANEL_GLOBALS_H__