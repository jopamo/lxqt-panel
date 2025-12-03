/* panel/plugin.h
 * Base plugin class implementation
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QFrame>
#include <QString>
#include <QPointer>
#include <OneG4/PluginInfo.h>
#include <OneG4/Settings.h>
#include "ioneg4panel.h"
#include "oneg4panelglobals.h"
#include "pluginsettings.h"

class QPluginLoader;
class QSettings;
class IOneG4PanelPlugin;
class IOneG4PanelPluginLibrary;
class OneG4Panel;
class QMenu;

class ONEG4_PANEL_API Plugin : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor moveMarkerColor READ moveMarkerColor WRITE setMoveMarkerColor)
 public:
  enum Alignment { AlignLeft, AlignRight };

  explicit Plugin(const OneG4::PluginInfo& desktopFile,
                  OneG4::Settings* settings,
                  const QString& settingsGroup,
                  OneG4Panel* panel);
  ~Plugin();

  bool isLoaded() const { return mPlugin != 0; }
  Alignment alignment() const { return mAlignment; }
  void setAlignment(Alignment alignment);

  QString settingsGroup() const { return mSettings->group(); }

  void saveSettings();

  QMenu* popupMenu() const;
  const IOneG4PanelPlugin* iPlugin() const { return mPlugin; }

  const OneG4::PluginInfo desktopFile() const { return mDesktopFile; }

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
  bool loadLib(IOneG4PanelPluginLibrary const* pluginLib);
  bool loadModule(const QString& libraryName);
  IOneG4PanelPluginLibrary const* findStaticPlugin(const QString& libraryName);
  void watchWidgets(QObject* const widget);
  void unwatchWidgets(QObject* const widget);

  const OneG4::PluginInfo mDesktopFile;
  QPluginLoader* mPluginLoader;
  IOneG4PanelPlugin* mPlugin;
  QWidget* mPluginWidget;
  Alignment mAlignment;
  PluginSettings* mSettings;
  OneG4Panel* mPanel;
  static QColor mMoveMarkerColor;
  QString mName;
  QPointer<QDialog> mConfigDialog;  //!< plugin's config dialog (if any)

 private slots:
  void settingsChanged();
};

Q_DECLARE_METATYPE(Plugin const*)

#endif  // PLUGIN_H