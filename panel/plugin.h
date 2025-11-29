/* panel/plugin.h
 * Base plugin class implementation
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QFrame>
#include <QString>
#include <QPointer>
#include <LXQt/PluginInfo>
#include <LXQt/Settings>
#include "ilxqtpanel.h"
#include "lxqtpanelglobals.h"
#include "pluginsettings.h"

class QPluginLoader;
class QSettings;
class ILXQtPanelPlugin;
class ILXQtPanelPluginLibrary;
class LXQtPanel;
class QMenu;

class LXQT_PANEL_API Plugin : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor moveMarkerColor READ moveMarkerColor WRITE setMoveMarkerColor)
 public:
  enum Alignment { AlignLeft, AlignRight };

  explicit Plugin(const LXQt::PluginInfo& desktopFile,
                  LXQt::Settings* settings,
                  const QString& settingsGroup,
                  LXQtPanel* panel);
  ~Plugin();

  bool isLoaded() const { return mPlugin != 0; }
  Alignment alignment() const { return mAlignment; }
  void setAlignment(Alignment alignment);

  QString settingsGroup() const { return mSettings->group(); }

  void saveSettings();

  QMenu* popupMenu() const;
  const ILXQtPanelPlugin* iPlugin() const { return mPlugin; }

  const LXQt::PluginInfo desktopFile() const { return mDesktopFile; }

  bool isSeparate() const;
  bool isExpandable() const;

  QWidget* widget() { return mPluginWidget; }

  QString name() const { return mName; }

  virtual bool eventFilter(QObject* watched, QEvent* event);

  // For QSS properties ..................
  static QColor moveMarkerColor() { return mMoveMarkerColor; }
  static void setMoveMarkerColor(QColor color) { mMoveMarkerColor = color; }

 public slots:
  void realign();
  void showConfigureDialog();
  void requestRemove();

 signals:
  void startMove();
  void remove();
  /*!
   * \brief Signal emitted when this widget or some of its children
   * get the DragLeave event delivered.
   */
  void dragLeft();

 protected:
  void contextMenuEvent(QContextMenuEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void showEvent(QShowEvent* event);

 private:
  bool loadLib(ILXQtPanelPluginLibrary const* pluginLib);
  bool loadModule(const QString& libraryName);
  ILXQtPanelPluginLibrary const* findStaticPlugin(const QString& libraryName);
  void watchWidgets(QObject* const widget);
  void unwatchWidgets(QObject* const widget);

  const LXQt::PluginInfo mDesktopFile;
  QPluginLoader* mPluginLoader;
  ILXQtPanelPlugin* mPlugin;
  QWidget* mPluginWidget;
  Alignment mAlignment;
  PluginSettings* mSettings;
  LXQtPanel* mPanel;
  static QColor mMoveMarkerColor;
  QString mName;
  QPointer<QDialog> mConfigDialog;  //!< plugin's config dialog (if any)

 private slots:
  void settingsChanged();
};

Q_DECLARE_METATYPE(Plugin const*)

#endif  // PLUGIN_H