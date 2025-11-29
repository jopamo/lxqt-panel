/* plugin-dom/domplugin.h
 * Plugin interface for plugin-dom
 */

#ifndef DOMPLUGIN_H
#define DOMPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include <QToolButton>

class DomPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  DomPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);

  virtual QWidget* widget() { return &mButton; }
  virtual QString themeId() const { return QStringLiteral("Dom"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment; }

 private slots:
  void showDialog();

 private:
  QToolButton mButton;
};

class DomPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new DomPlugin(startupInfo);
  }
};

#endif  // DOMPLUGIN_H