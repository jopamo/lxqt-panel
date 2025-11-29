/* plugin-fancymenu/lxqtfancymenu.h
 * Fancy menu plugin implementation
 */

#ifndef LXQT_FANCYMENU_H
#define LXQT_FANCYMENU_H

#include "../panel/ilxqtpanelplugin.h"
#include <XdgMenu>

#include <QLabel>
#include <QToolButton>
#include <QDomElement>
#include <QAction>
#include <QTimer>
#include <QKeySequence>

class LXQtFancyMenuWindow;
class LXQtBar;

namespace LXQt {
class PowerManager;
class ScreenSaver;
}  // namespace LXQt

class LXQtFancyMenu : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtFancyMenu(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtFancyMenu();

  QString themeId() const { return QStringLiteral("FancyMenu"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return HaveConfigDialog; }

  QWidget* widget() { return &mButton; }
  QDialog* configureDialog();

  bool isSeparate() const { return true; }

 protected:
  bool eventFilter(QObject* obj, QEvent* event);

 private:
  void setMenuFontSize();
  void setButtonIcon();

 private:
  QToolButton mButton;
  QString mLogDir;
  LXQtFancyMenuWindow* mWindow;
  bool mFilterClear;  //!< search field should be cleared upon showing the menu

  XdgMenu mXdgMenu;

  QTimer mDelayedPopup;
  QTimer mHideTimer;
  QString mMenuFile;

 protected slots:

  virtual void settingsChanged();
  void buildMenu();

  void loadFavorites();
  void saveFavorites();

 private slots:
  void showMenu();
  void showHideMenu();
};

class LXQtFancyMenuPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtFancyMenu(startupInfo);
  }
};

#endif