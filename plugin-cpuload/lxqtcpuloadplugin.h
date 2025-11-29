/* plugin-cpuload/lxqtcpuloadplugin.h
 * Plugin interface for plugin-cpuload
 */

#ifndef LXQTCPULOADPLUGIN_H
#define LXQTCPULOADPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include <QObject>
class LXQtCpuLoad;

class LXQtCpuLoadPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  explicit LXQtCpuLoadPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtCpuLoadPlugin();

  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("CpuLoad"); }

  bool isSeparate() const { return true; }
  QDialog* configureDialog();

 protected:
  virtual void settingsChanged();

 private:
  QWidget* mWidget;
  LXQtCpuLoad* mContent;
};

class LXQtCpuLoadPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtCpuLoadPlugin(startupInfo);
  }
};

#endif  // LXQTCPULOADPLUGIN_H