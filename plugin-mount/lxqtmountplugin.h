/* plugin-mount/lxqtmountplugin.h
 * Mount plugin implementation
 */

#ifndef LXQTMOUNTPLUGIN_H
#define LXQTMOUNTPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include "../panel/lxqtpanel.h"
#include "button.h"
#include "popup.h"
#include "actions/deviceaction.h"
#include "actions/ejectaction.h"

#include <QIcon>

/*!
\author Petr Vanek <petr@scribus.info>
*/

class LXQtMountPlugin : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

 public:
  LXQtMountPlugin(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtMountPlugin();

  virtual QWidget* widget() { return mButton; }
  virtual QString themeId() const { return QLatin1String("LXQtMount"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

  Popup* popup() { return mPopup; }
  QIcon icon() { return mButton->icon(); };
  QDialog* configureDialog();

 public slots:
  void realign();

 protected slots:
  virtual void settingsChanged();

 private:
  Button* mButton;
  Popup* mPopup;
  DeviceAction* mDeviceAction;
  EjectAction* mEjectAction;
};

class LXQtMountPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)

 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtMountPlugin(startupInfo);
  }
};

#endif