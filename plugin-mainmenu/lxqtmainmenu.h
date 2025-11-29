/* plugin-mainmenu/lxqtmainmenu.h
 * Main menu plugin implementation
 */

#ifndef LXQT_MAINMENU_H
#define LXQT_MAINMENU_H

#include "../panel/ilxqtpanelplugin.h"
#include <XdgMenu>

#include <QLabel>
#include <QToolButton>
#include <QDomElement>
#include <QAction>
#include <QTimer>
#include <QKeySequence>

#include "menustyle.h"

class QMenu;
class QWidgetAction;
class QLineEdit;
class ActionView;
class LXQtBar;

namespace LXQt {
class PowerManager;
class ScreenSaver;
}  // namespace LXQt

class LXQtMainMenu : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtMainMenu(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtMainMenu();

  QString themeId() const { return QStringLiteral("MainMenu"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return HaveConfigDialog; }

  QWidget* widget() { return &mButton; }
  QDialog* configureDialog();

  bool isSeparate() const { return true; }

 protected:
  bool eventFilter(QObject* obj, QEvent* event);

 private:
  void setMenuFontSize();
  void setButtonIcon();
  void addContextMenu(QMenu* menu);

 private:
  QToolButton mButton;
  QString mLogDir;
  QMenu* mMenu;
  MenuStyle mTopMenuStyle;
  QWidgetAction* mSearchEditAction;
  QLineEdit* mSearchEdit;
  QWidgetAction* mSearchViewAction;
  ActionView* mSearchView;
  QAction* mMakeDirtyAction;
  bool mFilterMenu;          //!< searching should perform hiding nonmatching items in menu
  bool mFilterShow;          //!< searching should list matching items in top menu
  bool mFilterClear;         //!< search field should be cleared upon showing the menu
  bool mFilterShowHideMenu;  //!< while searching all (original) menu entries should be hidden
  bool mHeavyMenuChanges;    //!< flag for filtering some mMenu events while heavy changes are performed

  XdgMenu mXdgMenu;

  QTimer mDelayedPopup;
  QTimer mHideTimer;
  QTimer mSearchTimer;
  QString mMenuFile;

 protected slots:

  virtual void settingsChanged();
  void buildMenu();

 private slots:
  void showMenu();
  void showHideMenu();
  void searchMenu();
  void setSearchFocus(QAction* action);
  void onRequestingCustomMenu(const QPoint& p, QObject* sender);
};

class LXQtMainMenuPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtMainMenu(startupInfo);
  }
};

#endif