/* plugin-quicklaunch/quicklaunchaction.h
 * Quick launch plugin implementation
 */

#ifndef QUICKLAUNCHACTION_H
#define QUICKLAUNCHACTION_H

#include <QAction>

class XdgDesktopFile;

/*! \brief Special action representation for LXQtQuickLaunch plugin.
It supports XDG desktop files or "legacy" launching of specified apps.
All process management is handled internally.
\author Petr Vanek <petr@scribus.info>
*/
class QuickLaunchAction : public QAction {
  Q_OBJECT

 public:
  enum ActionType { ActionLegacy, ActionXdg, ActionFile };
  /*! Constructor for "legacy" launchers.
      \warning The XDG way is preferred this is only for older or non-standard apps
      \param name a name to display in tooltip
      \param exec a executable with path
      \param icon a valid QIcon
   */
  QuickLaunchAction(const QString& name, const QString& exec, const QString& icon, QWidget* parent);
  /*! Constructor for XDG desktop handlers.
   */
  QuickLaunchAction(const XdgDesktopFile* xdg, QWidget* parent);
  /*! Constructor for regular files
   */
  QuickLaunchAction(const QString& fileName, QWidget* parent);

  //! Returns true if the action is valid (contains all required properties).
  bool isValid() const { return m_valid; }

  //! Returns the action type (legacy, Xdg, file).
  int type() const { return m_type; }

  QHash<QString, QString> settingsMap() { return m_settingsMap; }

  /*! Returns list of additional actions to present for user (in menu).
   * Currently there are only "Addititional application actions" for the ActionXdg type
   * (the [Desktop Action %s] in .desktop files)
   */
  QList<QAction*> additionalActions() const { return m_additionalActions; }

  /*! Updates the Xdg action by reloading its desktop file.
   * Does nothing if the desktop file is not loadable or suitable.
   */
  void updateXdgAction();

 public slots:
  void execAction(QString additionalAction = QString{});

 private:
  ActionType m_type;
  QString m_data;
  bool m_valid;
  QHash<QString, QString> m_settingsMap;
  QList<QAction*> m_additionalActions;
};

#endif