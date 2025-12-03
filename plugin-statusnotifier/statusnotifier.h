/* plugin-statusnotifier/statusnotifier.h
 * Header file for statusnotifier
 */

#ifndef STATUSNOTIFIER_PLUGIN_H
#define STATUSNOTIFIER_PLUGIN_H

#include "../panel/ioneg4panelplugin.h"
#include "statusnotifierwidget.h"
#include "statusnotifierconfiguration.h"

class StatusNotifier : public QObject, public IOneG4PanelPlugin {
  Q_OBJECT
 public:
  StatusNotifier(const IOneG4PanelPluginStartupInfo& startupInfo);

  bool isSeparate() const override { return true; }
  void realign() override;
  QString themeId() const override { return QStringLiteral("StatusNotifier"); }
  Flags flags() const override { return HaveConfigDialog | NeedsHandle; }
  QWidget* widget() override { return m_widget; }

  QDialog* configureDialog() override;

  void settingsChanged() override { m_widget->settingsChanged(); }

 private:
  StatusNotifierWidget* m_widget;
};

class StatusNotifierLibrary : public QObject, public IOneG4PanelPluginLibrary {
  Q_OBJECT
  //     Q_PLUGIN_METADATA(IID "oneg4.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(IOneG4PanelPluginLibrary)
 public:
  IOneG4PanelPlugin* instance(const IOneG4PanelPluginStartupInfo& startupInfo) const {
    return new StatusNotifier(startupInfo);
  }
};

#endif  // STATUSNOTIFIER_PLUGIN_H