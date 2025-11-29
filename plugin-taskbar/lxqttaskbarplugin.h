/* plugin-taskbar/lxqttaskbarplugin.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKBARPLUGIN_H
#define LXQTTASKBARPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"

class LXQtTaskBar;

class LXQtTaskBarPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtTaskBarPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtTaskBarPlugin();

  QString themeId() const { return QStringLiteral("TaskBar"); }
  virtual Flags flags() const { return HaveConfigDialog | NeedsHandle; }

  QWidget* widget();
  QDialog* configureDialog();

  void settingsChanged();
  void realign();

  bool isSeparate() const { return true; }
  bool isExpandable() const { return true; }

 private:
  LXQtTaskBar* mTaskBar;
};

class LXQtTaskBarPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtTaskBarPlugin(startupInfo);
  }
};

#endif  // LXQTTASKBARPLUGIN_H