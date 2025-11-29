/* plugin-kbindicator/src/settings.h
 * Keyboard indicator plugin implementation
 */

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "../panel/pluginsettings.h"

enum class KeeperType { Global, Window, Application };

class Settings {
 public:
  Settings();
  static Settings& instance();

  void init(PluginSettings* settings);

 public:
  bool showCapLock() const;
  bool showNumLock() const;
  bool showScrollLock() const;
  bool showLayout() const;
  QString layoutFlagPattern() const;
  KeeperType keeperType() const;
  void restore();

 public:
  void setShowCapLock(bool show);
  void setShowNumLock(bool show);
  void setShowScrollLock(bool show);
  void setShowLayout(bool show);
  void setLayoutFlagPattern(const QString& layoutFlagPattern);
  void setKeeperType(KeeperType type) const;

 private:
  PluginSettings* m_settings{nullptr};
};

#endif