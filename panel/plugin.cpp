/* panel/plugin.cpp
 * Base plugin class implementation
 */

#include "plugin.h"
#include "ioneg4panelplugin.h"
#include "pluginsettings_p.h"
#include "oneg4panel.h"

#include <KX11Extras>

#include <QDebug>
#include <QProcessEnvironment>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QGridLayout>
#include <QDialog>
#include <QCoreApplication>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>
#include <QWindow>
#include <memory>

#include <OneG4/Settings.h>
#include <XdgIcon.h>

// statically linked built-in plugins
#if defined(WITH_DESKTOPSWITCH_PLUGIN)
#include "../plugin-desktopswitch/desktopswitch.h"  // desktopswitch
#endif
#if defined(WITH_FANCYMENU_PLUGIN)
#include "../plugin-fancymenu/oneg4fancymenu.h"  // fancymenu
#endif
#if defined(WITH_MAINMENU_PLUGIN)
#include "../plugin-mainmenu/oneg4mainmenu.h"  // mainmenu
#endif
#if defined(WITH_QUICKLAUNCH_PLUGIN)
#include "../plugin-quicklaunch/oneg4quicklaunchplugin.h"  // quicklaunch
#endif
#if defined(WITH_SHOWDESKTOP_PLUGIN)
#include "../plugin-showdesktop/showdesktop.h"  // showdesktop
#endif
#if defined(WITH_SPACER_PLUGIN)
#include "../plugin-spacer/spacer.h"  // spacer
#endif
#if defined(WITH_STATUSNOTIFIER_PLUGIN)
#include "../plugin-statusnotifier/statusnotifier.h"  // statusnotifier
#endif
#if defined(WITH_TASKBAR_PLUGIN)
#include "../plugin-taskbar/oneg4taskbarplugin.h"  // taskbar
#endif
#if defined(WITH_WORLDCLOCK_PLUGIN)
#include "../plugin-worldclock/oneg4worldclock.h"  // worldclock
#endif

QColor Plugin::mMoveMarkerColor = QColor(255, 0, 0, 255);

/************************************************

 ************************************************/
Plugin::Plugin(const OneG4::PluginInfo& desktopFile,
               OneG4::Settings* settings,
               const QString& settingsGroup,
               OneG4Panel* panel)
    : QFrame(panel),
      mDesktopFile(desktopFile),
      mPluginLoader(nullptr),
      mPlugin(nullptr),
      mPluginWidget(nullptr),
      mAlignment(AlignLeft),
      mPanel(panel) {
  mSettings = PluginSettingsFactory::create(settings, settingsGroup);

  setWindowTitle(desktopFile.name());
  mName = desktopFile.name();

  QStringList dirs;
  dirs << QProcessEnvironment::systemEnvironment()
              .value(QStringLiteral("ONEG4PANEL_PLUGIN_PATH"))
              .split(QStringLiteral(":"));
  // When running directly from the build tree, plugins live next to the panel
  // binary under ../plugin-<id>/lib<id>.so. Prefer that path before the
  // installed location so development builds can run without extra setup.
  const QString buildDir = QDir(QCoreApplication::applicationDirPath())
                               .absoluteFilePath(QStringLiteral("../plugin-%1").arg(desktopFile.id()));
  dirs << buildDir;
  dirs << QStringLiteral(PLUGIN_DIR);

  bool found = false;
  if (IOneG4PanelPluginLibrary const* pluginLib = findStaticPlugin(desktopFile.id())) {
    // this is a static plugin
    found = true;
    loadLib(pluginLib);
  }
  else {
    // this plugin is a dynamically loadable module
    QString baseName = QStringLiteral("lib%1.so").arg(desktopFile.id());
    for (const QString& dirName : std::as_const(dirs)) {
      QFileInfo fi(QDir(dirName), baseName);
      if (fi.exists()) {
        found = true;
        if (loadModule(fi.absoluteFilePath()))
          break;
      }
    }
  }

  if (!isLoaded()) {
    if (!found)
      qWarning() << QStringLiteral("Plugin %1 not found in the").arg(desktopFile.id()) << dirs;

    return;
  }

  setObjectName(mPlugin->themeId() + QStringLiteral("Plugin"));

  // plugin handle for easy context menu
  setProperty("NeedsHandle", mPlugin->flags().testFlag(IOneG4PanelPlugin::NeedsHandle));

  QString s = mSettings->value(QStringLiteral("alignment")).toString();

  // Return default value
  if (s.isEmpty()) {
    mAlignment =
        (mPlugin->flags().testFlag(IOneG4PanelPlugin::PreferRightAlignment)) ? Plugin::AlignRight : Plugin::AlignLeft;
  }
  else {
    mAlignment = (s.toUpper() == QLatin1String("RIGHT")) ? Plugin::AlignRight : Plugin::AlignLeft;
  }

  if (mPluginWidget) {
    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    layout->addWidget(mPluginWidget, 0, 0);
  }

  saveSettings();

  // delay the connection to settingsChanged to avoid conflicts
  // while the plugin is still being initialized
  connect(mSettings, &PluginSettings::settingsChanged, this, &Plugin::settingsChanged);

  connect(mPanel, &OneG4Panel::realigned, this, &Plugin::realign);
}

/************************************************

 ************************************************/
Plugin::~Plugin() {
  if (mConfigDialog)
    delete mConfigDialog.data();
  delete mPlugin;
  delete mPluginLoader;
  delete mSettings;
}

void Plugin::setAlignment(Plugin::Alignment alignment) {
  mAlignment = alignment;
  saveSettings();
}

/************************************************

 ************************************************/
namespace {
// helper types for static plugins storage & binary search
typedef std::unique_ptr<IOneG4PanelPluginLibrary> plugin_ptr_t;
typedef std::tuple<QString, plugin_ptr_t> plugin_tuple_t;

// NOTE: Please keep the plugins sorted by name while adding new plugins.
static plugin_tuple_t const static_plugins[] = {
#if defined(WITH_DESKTOPSWITCH_PLUGIN)
    std::make_tuple(QLatin1String("desktopswitch"), plugin_ptr_t{new DesktopSwitchPluginLibrary}),  // desktopswitch
#endif
#if defined(WITH_FANCYMENU_PLUGIN)
    std::make_tuple(QLatin1String("fancymenu"), plugin_ptr_t{new OneG4FancyMenuPluginLibrary}),  // fancymenu
#endif
#if defined(WITH_MAINMENU_PLUGIN)
    std::make_tuple(QLatin1String("mainmenu"), plugin_ptr_t{new OneG4MainMenuPluginLibrary}),  // mainmenu
#endif
#if defined(WITH_QUICKLAUNCH_PLUGIN)
    std::make_tuple(QLatin1String("quicklaunch"), plugin_ptr_t{new OneG4QuickLaunchPluginLibrary}),  // quicklaunch
#endif
#if defined(WITH_SHOWDESKTOP_PLUGIN)
    std::make_tuple(QLatin1String("showdesktop"), plugin_ptr_t{new ShowDesktopLibrary}),  // showdesktop
#endif
#if defined(WITH_SPACER_PLUGIN)
    std::make_tuple(QLatin1String("spacer"), plugin_ptr_t{new SpacerPluginLibrary}),  // spacer
#endif
#if defined(WITH_STATUSNOTIFIER_PLUGIN)
    std::make_tuple(QLatin1String("statusnotifier"), plugin_ptr_t{new StatusNotifierLibrary}),  // statusnotifier
#endif
#if defined(WITH_TASKBAR_PLUGIN)
    std::make_tuple(QLatin1String("taskbar"), plugin_ptr_t{new OneG4TaskBarPluginLibrary}),  // taskbar
#endif
#if defined(WITH_TRAY_PLUGIN)
    std::make_tuple(QLatin1String("tray"), plugin_ptr_t{new OneG4TrayPluginLibrary}),  // tray
#endif
#if defined(WITH_WORLDCLOCK_PLUGIN)
    std::make_tuple(QLatin1String("worldclock"), plugin_ptr_t{new OneG4WorldClockLibrary}),  // worldclock
#endif
};
static constexpr plugin_tuple_t const* const plugins_begin = static_plugins;
static constexpr plugin_tuple_t const* const plugins_end =
    static_plugins + sizeof(static_plugins) / sizeof(static_plugins[0]);

struct assert_helper {
  assert_helper() {
    Q_ASSERT(std::is_sorted(plugins_begin, plugins_end, [](plugin_tuple_t const& p1, plugin_tuple_t const& p2) -> bool {
      return std::get<0>(p1) < std::get<0>(p2);
    }));
  }
};
static assert_helper h;
}  // namespace

IOneG4PanelPluginLibrary const* Plugin::findStaticPlugin(const QString& libraryName) {
  // find a static plugin library by name -> binary search
  plugin_tuple_t const* plugin = std::lower_bound(
      plugins_begin, plugins_end, libraryName,
      [](plugin_tuple_t const& plugin, QString const& name) -> bool { return std::get<0>(plugin) < name; });
  if (plugins_end != plugin && libraryName == std::get<0>(*plugin))
    return std::get<1>(*plugin).get();
  return nullptr;
}

// load a plugin from a library
bool Plugin::loadLib(IOneG4PanelPluginLibrary const* pluginLib) {
  IOneG4PanelPluginStartupInfo startupInfo;
  startupInfo.settings = mSettings;
  startupInfo.desktopFile = &mDesktopFile;
  startupInfo.oneg4Panel = mPanel;

  mPlugin = pluginLib->instance(startupInfo);
  if (!mPlugin) {
    qWarning()
        << QStringLiteral("Can't load plugin \"%1\". Plugin can't build IOneG4PanelPlugin.").arg(mDesktopFile.id());
    return false;
  }

  mPluginWidget = mPlugin->widget();
  if (mPluginWidget) {
    mPluginWidget->setObjectName(mPlugin->themeId());
    watchWidgets(mPluginWidget);
  }
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  return true;
}

// load dynamic plugin from a *.so module
bool Plugin::loadModule(const QString& libraryName) {
  mPluginLoader = new QPluginLoader(libraryName);

  if (!mPluginLoader->load()) {
    qWarning() << mPluginLoader->errorString();
    return false;
  }

  QObject* obj = mPluginLoader->instance();
  if (!obj) {
    qWarning() << mPluginLoader->errorString();
    return false;
  }

  IOneG4PanelPluginLibrary* pluginLib = qobject_cast<IOneG4PanelPluginLibrary*>(obj);
  if (!pluginLib) {
    qWarning() << QStringLiteral("Can't load plugin \"%1\". Plugin is not a IOneG4PanelPluginLibrary.")
                      .arg(mPluginLoader->fileName());
    delete obj;
    return false;
  }
  return loadLib(pluginLib);
}

/************************************************

 ************************************************/
void Plugin::watchWidgets(QObject* const widget) {
  // the QWidget might not be fully constructed yet, but we can rely on the isWidgetType()
  if (!widget->isWidgetType())
    return;
  widget->installEventFilter(this);
  // watch also children (recursive)
  for (auto const& child : widget->children()) {
    watchWidgets(child);
  }
}

/************************************************

 ************************************************/
void Plugin::unwatchWidgets(QObject* const widget) {
  widget->removeEventFilter(this);
  // unwatch also children (recursive)
  for (auto const& child : widget->children()) {
    unwatchWidgets(child);
  }
}

/************************************************

 ************************************************/
void Plugin::settingsChanged() {
  mPlugin->settingsChanged();
}

/************************************************

 ************************************************/
void Plugin::saveSettings() {
  bool syncSettings = false;
  const QString alignment(mAlignment == AlignLeft ? QStringLiteral("Left") : QStringLiteral("Right"));
  if (mSettings->value(QStringLiteral("alignment")).toString() != alignment) {
    mSettings->setValue(QStringLiteral("alignment"), alignment);
    syncSettings = true;
  }
  if (mSettings->value(QStringLiteral("type")).toString() != mDesktopFile.id()) {
    mSettings->setValue(QStringLiteral("type"), mDesktopFile.id());
    syncSettings = true;
  }
  if (syncSettings)
    mSettings->sync();
}

/************************************************

 ************************************************/
void Plugin::contextMenuEvent(QContextMenuEvent* event) {
  mPanel->showPopupMenu(event->globalPos(), this);
}

/************************************************

 ************************************************/
void Plugin::mousePressEvent(QMouseEvent* event) {
  switch (event->button()) {
    case Qt::LeftButton:
      mPlugin->activated(IOneG4PanelPlugin::Trigger);
      break;

    case Qt::MiddleButton:
      mPlugin->activated(IOneG4PanelPlugin::MiddleClick);
      break;

    default:
      break;
  }
}

/************************************************

 ************************************************/
void Plugin::mouseDoubleClickEvent(QMouseEvent*) {
  mPlugin->activated(IOneG4PanelPlugin::DoubleClick);
}

/************************************************

 ************************************************/
void Plugin::showEvent(QShowEvent*) {
  // ensure that plugin widgets have correct sizes at startup
  if (mPluginWidget) {
    mPluginWidget->updateGeometry();  // needed for widgets with style sizes (like buttons)
    mPluginWidget->adjustSize();
  }
}

/************************************************

 ************************************************/
QMenu* Plugin::popupMenu() const {
  QString name = this->name().replace(QLatin1String("&"), QLatin1String("&&"));
  QMenu* menu = new QMenu(windowTitle());

  if (mPlugin->flags().testFlag(IOneG4PanelPlugin::HaveConfigDialog)) {
    QAction* configAction =
        new QAction(XdgIcon::fromTheme(QLatin1String("preferences-other")), tr("Configure \"%1\"").arg(name), menu);
    menu->addAction(configAction);
    connect(configAction, &QAction::triggered, this, &Plugin::showConfigureDialog);
  }

  QAction* moveAction =
      new QAction(XdgIcon::fromTheme(QStringLiteral("transform-move")), tr("Move \"%1\"").arg(name), menu);
  menu->addAction(moveAction);
  connect(moveAction, &QAction::triggered, this, &Plugin::startMove);

  menu->addSeparator();

  QAction* removeAction =
      new QAction(XdgIcon::fromTheme(QLatin1String("list-remove")), tr("Remove \"%1\"").arg(name), menu);
  menu->addAction(removeAction);
  connect(removeAction, &QAction::triggered, this, &Plugin::requestRemove);

  return menu;
}

/************************************************

 ************************************************/
bool Plugin::isSeparate() const {
  return mPlugin->isSeparate();
}

/************************************************

 ************************************************/
bool Plugin::isExpandable() const {
  return mPlugin->isExpandable();
}

/************************************************

 ************************************************/
bool Plugin::eventFilter(QObject* /*watched*/, QEvent* event) {
  switch (event->type()) {
    case QEvent::DragLeave:
      emit dragLeft();
      break;
    case QEvent::ChildAdded:
      watchWidgets(dynamic_cast<QChildEvent*>(event)->child());
      break;
    case QEvent::ChildRemoved:
      unwatchWidgets(dynamic_cast<QChildEvent*>(event)->child());
      break;
    default:
      break;
  }
  return false;
}

/************************************************

 ************************************************/
void Plugin::realign() {
  if (mPlugin)
    mPlugin->realign();
}

/************************************************

 ************************************************/
void Plugin::showConfigureDialog() {
  if (!mConfigDialog)
    mConfigDialog = mPlugin->configureDialog();

  if (!mConfigDialog)
    return;

  mPanel->willShowWindow(mConfigDialog);
  mConfigDialog->show();
  mConfigDialog->raise();
  mConfigDialog->activateWindow();

  WId wid = mConfigDialog->windowHandle()->winId();
  KX11Extras::activateWindow(wid);
  KX11Extras::setOnDesktop(wid, KX11Extras::currentDesktop());
}

/************************************************

 ************************************************/
void Plugin::requestRemove() {
  emit remove();
  deleteLater();
}
