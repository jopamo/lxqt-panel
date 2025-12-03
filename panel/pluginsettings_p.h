/* panel/pluginsettings_p.h
 * Base plugin class implementation
 */

#ifndef PLUGIN_SETTINGS_P_H
#define PLUGIN_SETTINGS_P_H

#include "pluginsettings.h"

class PluginSettingsFactory {
 public:
  static PluginSettings* create(OneG4::Settings* settings, const QString& group, QObject* parent = nullptr);
};

#endif  // PLUGIN_SETTINGS_P_H