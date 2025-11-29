/* plugin-sensors/lxqtsensorsplugin.h
 * System sensors plugin implementation
 */

#ifndef LXQTSENSORSPLUGIN_H
#define LXQTSENSORSPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include <QObject>

class LXQtSensors;

class LXQtSensorsPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  explicit LXQtSensorsPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtSensorsPlugin();

  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("Sensors"); }

  bool isSeparate() const { return true; }
  QDialog* configureDialog();

  void realign();

 protected:
  virtual void settingsChanged();

 private:
  LXQtSensors* mWidget;
};

class LXQtSensorsPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtSensorsPlugin(startupInfo);
  }
};

#endif  // LXQTSENSORSPLUGIN_H