/* plugin-taskbar/oneg4taskbarplugin.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKBARPLUGIN_H
#define ONEG4TASKBARPLUGIN_H

#include "../panel/ioneg4panelplugin.h"

class OneG4TaskBar;

class OneG4TaskBarPlugin : public QObject, public IOneG4PanelPlugin {
  Q_OBJECT
 public:
  OneG4TaskBarPlugin(const IOneG4PanelPluginStartupInfo& startupInfo);
  ~OneG4TaskBarPlugin();

  QString themeId() const { return QStringLiteral("TaskBar"); }
  virtual Flags flags() const { return HaveConfigDialog | NeedsHandle; }

  QWidget* widget();
  QDialog* configureDialog();

  void settingsChanged();
  void realign();

  bool isSeparate() const { return true; }
  bool isExpandable() const { return true; }

 private:
  OneG4TaskBar* mTaskBar;
};

class OneG4TaskBarPluginLibrary : public QObject, public IOneG4PanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "oneg4.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(IOneG4PanelPluginLibrary)
 public:
  IOneG4PanelPlugin* instance(const IOneG4PanelPluginStartupInfo& startupInfo) const {
    return new OneG4TaskBarPlugin(startupInfo);
  }
};

#endif  // ONEG4TASKBARPLUGIN_H