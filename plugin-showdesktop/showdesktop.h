/* plugin-showdesktop/showdesktop.h
 * Show desktop plugin implementation
 */

#ifndef SHOWDESKTOP_H
#define SHOWDESKTOP_H

#include "../panel/ilxqtpanelplugin.h"
#include <QToolButton>
#include <QTimer>

class ShowDesktop : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

 public:
  ShowDesktop(const ILXQtPanelPluginStartupInfo& startupInfo);

  virtual QWidget* widget() override { return &mButton; }
  virtual QString themeId() const override { return QStringLiteral("ShowDesktop"); }

  virtual bool eventFilter(QObject* watched, QEvent* event) override;

 private slots:
  void toggleShowingDesktop();

 private:
  QTimer mDNDTimer;
  QToolButton mButton;
};

class ShowDesktopLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new ShowDesktop(startupInfo);
  }
};

#endif