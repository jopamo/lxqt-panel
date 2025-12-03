/* panel/oneg4panelapplication.cpp
 * Main panel implementation, window management
 */

#include "oneg4panelapplication.h"
#include "oneg4panelapplication_p.h"

#include "config/configpaneldialog.h"
#include "oneg4panel.h"

#include <QCommandLineParser>
#include <QScreen>
#include <QUuid>
#include <QWindow>
#include <QtDebug>
#include <OneG4/Settings.h>
#include <OneG4/Globals.h>

#include <QPluginLoader>
#include <QDir>
#include <QProcessEnvironment>
#include <QCoreApplication>

#include "backends/oneg4dummywmbackend.h"

static inline QString getBackendFilePath(QString name) {
  // if we do not have a full library name like libwmbackend_xcb.so
  // then build a name based on default heuristic: libwmbackend_<name>.so
  if (!name.startsWith(QStringLiteral("lib")) || !name.endsWith(QStringLiteral(".so"))) {
    if (!name.startsWith(QStringLiteral("libwmbackend_"))) {
      name = QStringLiteral("libwmbackend_%1").arg(name);
    }
    if (!name.endsWith(QStringLiteral(".so"))) {
      name = QStringLiteral("%1.so").arg(name);
    }
  }

  QStringList dirs;
  dirs << QProcessEnvironment::systemEnvironment()
              .value(QStringLiteral("ONEG4PANEL_PLUGIN_PATH"))
              .split(QStringLiteral(":"), Qt::SkipEmptyParts);
  // In the build tree backends live in a sibling directory to the panel binary
  dirs << QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("backends"));
  dirs << QStringLiteral(PLUGIN_DIR);

  for (const QString& dir : std::as_const(dirs)) {
    QDir backendsDir(dir);
    QStringList candidateBases;
    candidateBases << backendsDir.absolutePath();
    if (backendsDir.cd(QStringLiteral("backend")))
      candidateBases << backendsDir.absolutePath();
    backendsDir.setPath(dir);
    if (backendsDir.cd(QStringLiteral("backends")))
      candidateBases << backendsDir.absolutePath();

    for (const QString& base : std::as_const(candidateBases)) {
      QDir baseDir(base);
      if (baseDir.exists(name))
        return baseDir.absoluteFilePath(name);

      // handle layouts like backends/xcb/libwmbackend_xcb.so
      const auto subDirs = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
      for (const QString& sub : subDirs) {
        QDir subDir(baseDir);
        if (subDir.cd(sub) && subDir.exists(name))
          return subDir.absoluteFilePath(name);
      }
    }
  }

  return QString();
}



OneG4PanelApplicationPrivate::OneG4PanelApplicationPrivate(OneG4PanelApplication* q)
    : mSettings(nullptr), mWMBackend(nullptr), q_ptr(q) {}

IOneG4Panel::Position OneG4PanelApplicationPrivate::computeNewPanelPosition(const OneG4Panel* p, const int screenNum) {
  Q_Q(OneG4PanelApplication);
  QList<bool> screenPositions(4, false);  // false means not occupied

  for (int i = 0; i < q->mPanels.size(); ++i) {
    if (p != q->mPanels.at(i)) {
      // we are not the newly added one
      if (screenNum == q->mPanels.at(i)->screenNum()) {  // panels on the same screen
        const int posIndex = static_cast<int>(q->mPanels.at(i)->position());
        if (posIndex >= 0 && posIndex < screenPositions.size())
          screenPositions[posIndex] = true;  // occupied
      }
    }
  }

  int availablePosition = 0;

  // Bottom, Top, Left, Right
  for (int i = 0; i < 4; ++i) {
    if (!screenPositions[i]) {
      availablePosition = i;
      break;
    }
  }

  return static_cast<IOneG4Panel::Position>(availablePosition);
}

void OneG4PanelApplicationPrivate::loadBackend() {
  // only X11/XCB backend is supported
  const QString preferredBackend = QStringLiteral("xcb");

  QPluginLoader loader;

  if (!preferredBackend.isEmpty()) {
    const QString backendPath = getBackendFilePath(preferredBackend);
    if (!backendPath.isEmpty())
      loader.setFileName(backendPath);

    if (loader.fileName().isEmpty() || !loader.load()) {
      qWarning() << "Failed to load backend" << preferredBackend << ":" << loader.errorString();
    }
    else {
      QObject* plugin = loader.instance();
      IOneG4WMBackendLibrary* backend = qobject_cast<IOneG4WMBackendLibrary*>(plugin);
      if (backend) {
        mWMBackend = backend->instance();
      }
      else {
        // plugin not valid
        loader.unload();
      }
    }
  }

  if (mWMBackend) {
    qDebug() << "\nPanel backend:" << preferredBackend << "\n";
  }
  else {
    // if no backend can be found fall back to dummy backend
    loader.unload();
    mWMBackend = new OneG4DummyWMBackend;

    qWarning() << "\n"
               << "ERROR: Could not create a backend for window managment operations\n"
               << "Falling back to dummy backend. Some functions will not be available\n"
               << "\n";
  }

  mWMBackend->setParent(q_ptr);
}

OneG4PanelApplication::OneG4PanelApplication(int& argc, char** argv)
    : OneG4::Application(argc, argv, true), d_ptr(new OneG4PanelApplicationPrivate(this)) {
  Q_D(OneG4PanelApplication);

  QCoreApplication::setApplicationName(QLatin1String("1g4-panel"));
  const QString compatVersion = QStringLiteral("%1.%2").arg(OneG4::VersionMajor).arg(OneG4::VersionMinor);
  const QString VERINFO =
      QStringLiteral(ONEG4_PANEL_VERSION "\ncompat   %1\nQt        " QT_VERSION_STR).arg(compatVersion);

  QCoreApplication::setApplicationVersion(VERINFO);

  QCommandLineParser parser;
  parser.setApplicationDescription(QLatin1String("OneG4 Panel"));
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption configFileOption(
      QStringList() << QLatin1String("c") << QLatin1String("config") << QLatin1String("configfile"),
      QCoreApplication::translate("main", "Use alternate configuration file."),
      QCoreApplication::translate("main", "Configuration file"));
  parser.addOption(configFileOption);

  parser.process(*this);

  const QString configFile = parser.value(configFileOption);

  if (configFile.isEmpty())
    d->mSettings = new OneG4::Settings(QLatin1String("panel"), this);
  else
    d->mSettings = new OneG4::Settings(configFile, QSettings::IniFormat, this);

  d->loadBackend();

  const auto allScreens = screens();

  // workaround for historical multihead screen handling bugs in Qt
  for (QScreen* screen : allScreens) {
    connect(screen, &QScreen::destroyed, this, &OneG4PanelApplication::screenDestroyed);
  }
  connect(this, &QGuiApplication::screenAdded, this, &OneG4PanelApplication::handleScreenAdded);

  connect(this, &QCoreApplication::aboutToQuit, this, &OneG4PanelApplication::cleanup);

  QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();

  // giving a separate icon theme to the panel can have side effects
  // it is optional and can be used as the last resort for avoiding a low contrast
  // in the case of symbolic SVG icons
  mGlobalIconTheme = QIcon::themeName();
  const QString iconTheme = d->mSettings->value(QStringLiteral("iconTheme")).toString();
  if (!iconTheme.isEmpty())
    QIcon::setThemeName(iconTheme);

  if (panels.isEmpty())
    panels << QStringLiteral("panel1");

  for (const QString& i : std::as_const(panels)) {
    addPanel(i);
  }
}

OneG4PanelApplication::~OneG4PanelApplication() {
  delete d_ptr;
}

void OneG4PanelApplication::cleanup() {
  qDeleteAll(mPanels);
}

void OneG4PanelApplication::addNewPanel() {
  Q_D(OneG4PanelApplication);

  const QString name = QStringLiteral("panel_") + QUuid::createUuid().toString();

  OneG4Panel* p = addPanel(name);
  const int screenNum = p->screenNum();
  const IOneG4Panel::Position newPanelPosition = d->computeNewPanelPosition(p, screenNum);
  p->setPosition(screenNum, newPanelPosition, true);
  QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
  panels << name;
  d->mSettings->setValue(QStringLiteral("panels"), panels);

  // popup the configuration dialog to allow user configuration right away
  p->showConfigDialog();
}

OneG4Panel* OneG4PanelApplication::addPanel(const QString& name) {
  Q_D(OneG4PanelApplication);

  OneG4Panel* panel = new OneG4Panel(name, d->mSettings);
  mPanels << panel;

  // reemit signals
  connect(panel, &OneG4Panel::deletedByUser, this, &OneG4PanelApplication::removePanel);
  connect(panel, &OneG4Panel::pluginAdded, this, &OneG4PanelApplication::pluginAdded);
  connect(panel, &OneG4Panel::pluginRemoved, this, &OneG4PanelApplication::pluginRemoved);

  return panel;
}

void OneG4PanelApplication::handleScreenAdded(QScreen* newScreen) {
  // qDebug() << "OneG4PanelApplication::handleScreenAdded" << newScreen;
  connect(newScreen, &QScreen::destroyed, this, &OneG4PanelApplication::screenDestroyed);
}

void OneG4PanelApplication::reloadPanelsAsNeeded() {
  Q_D(OneG4PanelApplication);

  // recreate missing panels which were deleted when their screen was destroyed
  const QStringList names = d->mSettings->value(QStringLiteral("panels")).toStringList();
  for (const QString& name : names) {
    bool found = false;
    for (OneG4Panel* panel : std::as_const(mPanels)) {
      if (panel->name() == name) {
        found = true;
        break;
      }
    }
    if (!found) {
      qDebug() << "Re-create panel after screen change:" << name;
      addPanel(name);
    }
  }
  qApp->setQuitOnLastWindowClosed(true);
}

void OneG4PanelApplication::screenDestroyed(QObject* screenObj) {
  // Workaround for multihead screen changes that would otherwise leave panels in a broken state
  // Destroy panels whose QWindow is associated with the disappearing QScreen and recreate them later
  QScreen* screen = static_cast<QScreen*>(screenObj);
  bool reloadNeeded = false;
  qApp->setQuitOnLastWindowClosed(false);
  for (OneG4Panel* panel : std::as_const(mPanels)) {
    QWindow* panelWindow = panel->windowHandle();
    if (panelWindow && panelWindow->screen() == screen) {
      // the screen containing the panel is destroyed
      // delete and then re-create the panel ourselves
      const QString name = panel->name();
      panel->saveSettings(false);
      mPanels.removeAll(panel);
      delete panel;
      reloadNeeded = true;
      qDebug() << "Delete panel on screen removal:" << name;
    }
  }
  if (reloadNeeded)
    QTimer::singleShot(1000, this, SLOT(reloadPanelsAsNeeded()));
  else
    qApp->setQuitOnLastWindowClosed(true);
}

void OneG4PanelApplication::removePanel(OneG4Panel* panel) {
  Q_D(OneG4PanelApplication);
  Q_ASSERT(mPanels.contains(panel));

  mPanels.removeAll(panel);

  QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
  panels.removeAll(panel->name());
  d->mSettings->setValue(QStringLiteral("panels"), panels);

  panel->deleteLater();
}

bool OneG4PanelApplication::isPluginSingletonAndRunning(QString const& pluginId) const {
  for (auto const& panel : mPanels)
    if (panel->isPluginSingletonAndRunning(pluginId))
      return true;

  return false;
}

IOneG4AbstractWMInterface* OneG4PanelApplication::getWMBackend() const {
  Q_D(const OneG4PanelApplication);
  return d->mWMBackend;
}

// see OneG4PanelApplication::OneG4PanelApplication for why this is not ideal
void OneG4PanelApplication::setIconTheme(const QString& iconTheme) {
  Q_D(OneG4PanelApplication);

  d->mSettings->setValue(QStringLiteral("iconTheme"), iconTheme == mGlobalIconTheme ? QString() : iconTheme);
  const QString newTheme = iconTheme.isEmpty() ? mGlobalIconTheme : iconTheme;
  if (newTheme != QIcon::themeName()) {
    QIcon::setThemeName(newTheme);
    for (OneG4Panel* panel : std::as_const(mPanels)) {
      panel->update();
      panel->updateConfigDialog();
    }
  }
}
