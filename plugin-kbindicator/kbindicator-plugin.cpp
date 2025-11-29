/* plugin-kbindicator/kbindicator-plugin.cpp
 * Keyboard indicator plugin implementation
 */

#include <QDebug>
#include <QObject>

#include <QGuiApplication>  // For nativeInterface()

#include "src/kbdstate.h"
#include "../panel/ilxqtpanelplugin.h"

class LXQtKbIndicatorPlugin : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ~LXQtKbIndicatorPlugin() override = default;

  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const override {
    auto* x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11Application || !x11Application->connection()) {
      // Currently only X11 supported
      qWarning() << "Currently kbindicator plugin supports X11 only. Skipping.";
      return nullptr;
    }

    return new KbdState(startupInfo);
  }
};

#include "kbindicator-plugin.moc"