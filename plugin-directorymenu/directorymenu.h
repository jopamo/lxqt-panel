/* plugin-directorymenu/directorymenu.h
 * Header file for directorymenu
 */

#ifndef DIRECTORYMENU_H
#define DIRECTORYMENU_H

#include "../panel/ilxqtpanelplugin.h"
#include "directorymenuconfiguration.h"

#include <QLabel>
#include <QToolButton>
#include <QDomElement>
#include <QAction>
#include <QDir>
#include <QSignalMapper>
#include <QSettings>
#include <QMenu>

class DirectoryMenu : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

 public:
  DirectoryMenu(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~DirectoryMenu();

  virtual QWidget* widget() { return &mButton; }
  virtual QString themeId() const { return QStringLiteral("DirectoryMenu"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return HaveConfigDialog; }
  QDialog* configureDialog();
  void settingsChanged();

 private slots:
  void showMenu();
  void openDirectory(const QString& path);
  void openInTerminal(const QString& path);
  void addMenu(QString path);

 protected slots:
  void buildMenu(const QString& path);

 private:
  void addActions(QMenu* menu, const QString& path);

  QToolButton mButton;
  QMenu* mMenu;
  QSignalMapper* mOpenDirectorySignalMapper;
  QSignalMapper* mOpenTerminalSignalMapper;  // New signal mapper to opening directory in term
  QSignalMapper* mMenuSignalMapper;

  QDir mBaseDirectory;
  QIcon mDefaultIcon;
  std::vector<QString> mPathStrings;
  QString mDefaultTerminal;
};

class DirectoryMenuLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new DirectoryMenu(startupInfo);
  }
};

#endif