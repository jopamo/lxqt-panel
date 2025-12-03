/* panel/oneg4panelapplication.h
 * Main panel implementation, window management
 */

#ifndef ONEG4PANELAPPLICATION_H
#define ONEG4PANELAPPLICATION_H

#include <OneG4/Application.h>
#include "ioneg4panelplugin.h"

class QScreen;

class OneG4Panel;
class OneG4PanelApplicationPrivate;

class IOneG4AbstractWMInterface;

/*!
 * \brief The OneG4PanelApplication class inherits from OneG4::Application and
 * is therefore the QApplication that we will create and execute in our
 * main()-function.
 *
 * OneG4PanelApplication itself is not a visible panel, rather it is only
 * the container which holds the visible panels. These visible panels are
 * OneG4Panel objects which are stored in mPanels. This approach enables us
 * to have more than one panel (for example one panel at the top and one
 * panel at the bottom of the screen) without additional effort.
 */
class OneG4PanelApplication : public OneG4::Application {
  Q_OBJECT
 public:
  /*!
   * \brief Creates a new OneG4PanelApplication with the given command line
   * arguments. Performs the following steps:
   * 1. Initializes the OneG4::Application, sets application name and version.
   * 2. Handles command line arguments. Currently, the only cmdline argument
   * is -c = -config = -configfile which chooses a different config file
   * for the OneG4::Settings.
   * 3. Creates the OneG4::Settings.
   * 4. Connects QCoreApplication::aboutToQuit to cleanup().
   * 5. Calls addPanel() for each panel found in the config file. If there is
   * none, adds a new panel.
   * \param argc
   * \param argv
   */
  explicit OneG4PanelApplication(int& argc, char** argv);
  ~OneG4PanelApplication();

  void setIconTheme(const QString& iconTheme);

  /*!
   * \brief Determines the number of OneG4Panel objects
   * \return the current number of OneG4Panel objects
   */
  int count() const { return mPanels.count(); }

  /*!
   * \brief Checks if a given Plugin is running and has the
   * IOneG4PanelPlugin::SingleInstance flag set. As Plugins are added to
   * OneG4Panel instances, this method only iterates over these OneG4Panel
   * instances and lets them check the conditions.
   * \param pluginId Plugin Identifier which is the basename of the .desktop
   * file that specifies the plugin.
   * \return true if the Plugin is running and has the
   * IOneG4PanelPlugin::SingleInstance flag set, false otherwise.
   */
  bool isPluginSingletonAndRunning(QString const& pluginId) const;

  IOneG4AbstractWMInterface* getWMBackend() const;

 public slots:
  /*!
   * \brief Adds a new OneG4Panel which consists of the following steps:
   * 1. Create id/name.
   * 2. Create the OneG4Panel: call addPanel(name).
   * 3. Update the config file (add the new panel id to the list of panels).
   * 4. Show the panel configuration dialog so that the user can add plugins.
   *
   * This method will create a new OneG4Panel with a new name and add this
   * to the config file. So this should only be used while the application
   * is running and the user decides to add a new panel. At application
   * startup, addPanel() should be used instead.
   *
   * \note This slot will be used from the OneG4Panel right-click menu. As we
   * can only add new panels from a visible panel, we should never run
   * 1g4-panel without an OneG4Panel. Without a panel, we have just an
   * invisible application.
   */
  void addNewPanel();

 signals:
  /*!
   * \brief Signal that re-emits the signal pluginAdded() from OneG4Panel.
   */
  void pluginAdded();
  /*!
   * \brief Signal that re-emits the signal pluginRemoved() from OneG4Panel.
   */
  void pluginRemoved();

 private:
  /*!
   * \brief Holds all the instances of OneG4Panel.
   */
  QList<OneG4Panel*> mPanels;
  /*!
   * \brief The global icon theme used by all apps (except for panels perhaps).
   */
  QString mGlobalIconTheme;
  /*!
   * \brief Creates a new OneG4Panel with the given name and connects the
   * appropriate signals and slots.
   * This method can be used at application startup.
   * \param name Name of the OneG4Panel as it is used in the config file.
   * \return The newly created OneG4Panel.
   */
  OneG4Panel* addPanel(const QString& name);

 private slots:
  /*!
   * \brief Removes the given OneG4Panel which consists of the following
   * steps:
   * 1. Remove the panel from mPanels.
   * 2. Remove the panel from the config file.
   * 3. Schedule the QObject for deletion: QObject::deleteLater().
   * \param panel OneG4Panel instance that should be removed.
   */
  void removePanel(OneG4Panel* panel);

  /*!
   * \brief Connects the QScreen::destroyed signal of a new screen to
   * the screenDestroyed() slot so that we can handle this screens'
   * destruction as soon as it happens.
   * \param newScreen The QScreen that was created and added.
   */
  void handleScreenAdded(QScreen* newScreen);
  /*!
   * \brief Handles screen destruction. This is a workaround for a Qt bug.
   * For further information, see the implementation notes.
   * \param screenObj The QScreen that was destroyed.
   */
  void screenDestroyed(QObject* screenObj);
  /*!
   * \brief Reloads the panels. This is the second part of the workaround
   * mentioned above.
   */
  void reloadPanelsAsNeeded();
  /*!
   * \brief Deletes all OneG4Panel instances that are stored in mPanels.
   */
  void cleanup();

 private:
  /*!
   * \brief mSettings is the OneG4::Settings object that is used for the
   * current instance of 1g4-panel. Normally, this refers to the config file
   * $HOME/.config/oneg4/panel.conf (on Unix systems). This behaviour can be
   * changed with the -c command line option.
   */

  OneG4PanelApplicationPrivate* const d_ptr;

  Q_DECLARE_PRIVATE(OneG4PanelApplication)
  Q_DISABLE_COPY(OneG4PanelApplication)
};

#endif  // ONEG4PANELAPPLICATION_H