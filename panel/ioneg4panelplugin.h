/* panel/ioneg4panelplugin.h
 * Plugin interface for panel
 */

#ifndef IONEG4PANELPLUGIN_H
#define IONEG4PANELPLUGIN_H

#include <QtPlugin>
#include "ioneg4panel.h"
#include "oneg4panelglobals.h"

/**
OneG4 panel plugins are standalone sharedlibraries
(*.so) located in PLUGIN_DIR (define provided by CMakeLists.txt).

Plugin for the panel is a library written in C++. One more necessary thing
is a .desktop file describing this plugin. The same may be additional files,
like translations. Themselves plugins will be installed to
/usr/local/lib/1g4-panel or /usr/lib/1g4-panel (dependent on cmake option
-DCMAKE_INSTALL_PREFIX). Desktop files are installed to
/usr/local/share/oneg4/1g4-panel, translations to
/usr/local/share/oneg4/1g4-panel/PLUGIN_NAME.
**/

class QDialog;
class PluginSettings;
namespace OneG4 {
class PluginInfo;
}

struct ONEG4_PANEL_API IOneG4PanelPluginStartupInfo {
  IOneG4Panel* oneg4Panel;
  PluginSettings* settings;
  const OneG4::PluginInfo* desktopFile;
};

/** \brief Base abstract class for OneG4 panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.
**/

class ONEG4_PANEL_API IOneG4PanelPlugin {
 public:
  /**
    This enum describes the properties of a plugin.
   **/
  enum Flag {
    NoFlags = 0,              ///< It does not have any properties set.
    PreferRightAlignment = 1, /**< The plugin prefers right alignment (for example the clock plugin);
                                  otherwise the plugin prefers left alignment (like main menu).
                                  This flag is used only at the first start, later positions of all
                                  plugins are saved in a config, and this saved information is used. */
    HaveConfigDialog = 2,     ///< The plugin have a configuration dialog.
    SingleInstance = 4,       ///< The plugin allows only one instance to run.
    NeedsHandle = 8           ///< The plugin needs a handle for the context menu
  };

  Q_DECLARE_FLAGS(Flags, Flag)

  /**
    This enum describes the reason the plugin was activated.
   **/
  enum ActivationReason {
    Unknown = 0,      ///< Unknown reason
    DoubleClick = 2,  ///<	The plugin entry was double clicked
    Trigger = 3,      ///<	The plugin was clicked
    MiddleClick = 4   ///< The plugin was clicked with the middle mouse button
  };

  /**
   Constructs an IOneG4PanelPlugin object with the given startupInfo. You do not have to worry
   about the startupInfo parameters, IOneG4PanelPlugin processes the parameters itself.
   **/
  IOneG4PanelPlugin(const IOneG4PanelPluginStartupInfo& startupInfo)
      : mSettings(startupInfo.settings), mPanel(startupInfo.oneg4Panel), mDesktopFile(startupInfo.desktopFile) {}

  /**
   Destroys the object.
   **/
  virtual ~IOneG4PanelPlugin() {}

  /**
  Returns the plugin flags.
  The base class implementation returns a NoFlags.
   **/
  virtual Flags flags() const { return NoFlags; }

  /**
  Returns the string that is used in the theme QSS file.
  If you return "WorldClock" string, theme author may write something like `#WorldClock { border: 1px solid red; }`
  to set a custom border for your plugin.
  **/
  virtual QString themeId() const = 0;

  /**
   From the user's point of view, your plugin is some visual widget on the panel. This function returns a pointer to it.
   This method is called only once, so you are free to return the pointer on a class member, or create the widget on the
   fly.
   **/
  virtual QWidget* widget() = 0;

  /**
  Returns the plugin settings dialog. Reimplement this function if your plugin has it.
  The panel does not take ownership of the dialog, it is probably a good idea to set Qt::WA_DeleteOnClose
  attribute for the dialog.
  The default implementation returns 0, no dialog;

  Note that the flags method has to return HaveConfigDialog flag.
  To save the settings you should use a ready-to-use IOneG4PanelPlugin::settings() object.

  **/
  virtual QDialog* configureDialog() { return 0; }

  /**
  This function is called when values are changed in the plugin settings.
  Reimplement this function to your plugin corresponded the new settings.

  The default implementation do nothing.
  **/
  virtual void settingsChanged() {}

  /**
  This function is called when the user activates the plugin. reason specifies the reason for activation.
  IOneG4PanelPlugin::ActivationReason enumerates the various reasons.

  The default implementation do nothing.
   **/
  virtual void activated(ActivationReason /*reason*/) {}

  /**
  This function is called when the panel geometry or lines count are changed.

  The default implementation do nothing.

   **/
  virtual void realign() {}

  /**
  Returns the panel object.
   **/
  IOneG4Panel* panel() const { return mPanel; }

  PluginSettings* settings() const { return mSettings; }
  const OneG4::PluginInfo* desktopFile() const { return mDesktopFile; }

  /**
   Helper functions for calculating global screen position of some popup window with windowSize size.
   If you need to show some popup window, you can use it, to get global screen position for the new window.
   **/
  virtual QRect calculatePopupWindowPos(const QSize& windowSize) {
    return mPanel->calculatePopupWindowPos(this, windowSize);
  }

  /*!
   * \brief By calling this function plugin notifies the panel about showing a (standalone) window/menu.
   *
   * \param w the shown window
   *
   */
  inline void willShowWindow(QWidget* w) { mPanel->willShowWindow(w); }

  /*!
   * \brief By calling this function, a plugin notifies the panel about change of it's "static"
   * configuration
   *
   * \sa isSeparate(), isExpandable
   */
  inline void pluginFlagsChanged() { mPanel->pluginFlagsChanged(this); }

  virtual bool isSeparate() const { return false; }
  virtual bool isExpandable() const { return false; }

 private:
  PluginSettings* mSettings;
  IOneG4Panel* mPanel;
  const OneG4::PluginInfo* mDesktopFile;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOneG4PanelPlugin::Flags)

/**
Every plugin must have the IOneG4PanelPluginLibrary loader. You should only reimplement the instance() method which
should return your plugin. Example:
@code
class OneG4ClockPluginLibrary: public QObject, public IOneG4PanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "oneg4.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IOneG4PanelPluginLibrary)
public:
    IOneG4PanelPlugin *instance(const IOneG4PanelPluginStartupInfo &startupInfo) { return new OneG4Clock(startupInfo);}
};
@endcode
**/
class ONEG4_PANEL_API IOneG4PanelPluginLibrary {
 public:
  /**
   Destroys the IOneG4PanelPluginLibrary object.
   **/
  virtual ~IOneG4PanelPluginLibrary() {}

  /**
  Returns the root component object of the plugin. When the library is finally unloaded, the root component will
  automatically be deleted.
   **/
  virtual IOneG4PanelPlugin* instance(const IOneG4PanelPluginStartupInfo& startupInfo) const = 0;
};

Q_DECLARE_INTERFACE(IOneG4PanelPluginLibrary, "oneg4.org/Panel/PluginInterface/3.0")

#endif  // IONEG4PANELPLUGIN_H