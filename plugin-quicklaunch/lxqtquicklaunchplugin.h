/* plugin-quicklaunch/lxqtquicklaunchplugin.h
 * Quick launch plugin implementation
 */

#ifndef LXQTQUICKLAUNCHPLUGIN_H
#define LXQTQUICKLAUNCHPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include <QObject>

class LXQtQuickLaunch;

class LXQtQuickLaunchPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  explicit LXQtQuickLaunchPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtQuickLaunchPlugin();

  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("QuickLaunch"); }
  virtual Flags flags() const { return NeedsHandle; }

  void realign();

  bool isSeparate() const { return true; }

 private:
  LXQtQuickLaunch* mWidget;
};

class LXQtQuickLaunchPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtQuickLaunchPlugin(startupInfo);
  }
};
#endif  // LXQTQUICKLAUNCHPLUGIN_H