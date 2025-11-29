/* plugin-tray/lxqttrayplugin.h
 * Plugin interface for plugin-tray
 */

#pragma once

#include "../panel/ilxqtpanelplugin.h"

#include <QObject>

#include <QDebug>

class FdoSelectionManager;
class LXQtTrayPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  explicit LXQtTrayPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtTrayPlugin();

  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("Tray"); }
  virtual Flags flags() const { return PreferRightAlignment | SingleInstance | NeedsHandle; }

  bool isSeparate() const { return true; }

 private:
  std::unique_ptr<FdoSelectionManager> mManager;
};

class LXQtTrayPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const;
};