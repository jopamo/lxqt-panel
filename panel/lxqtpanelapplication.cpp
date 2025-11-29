#include "lxqtpanelapplication.h"
#include "lxqtpanelapplication_p.h"

#include "config/configpaneldialog.h"
#include "lxqtpanel.h"

#include <QCommandLineParser>
#include <QScreen>
#include <QUuid>
#include <QWindow>
#include <QtDebug>
#include <LXQt/Settings>

#include <QPluginLoader>
#include <QDir>
#include <QProcessEnvironment>

#include "backends/lxqtdummywmbackend.h"

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
              .value(QStringLiteral("LXQTPANEL_PLUGIN_PATH"))
              .split(QStringLiteral(":"), Qt::SkipEmptyParts);
  dirs << QStringLiteral(PLUGIN_DIR);

  for (const QString& dir : std::as_const(dirs)) {
    QDir backendsDir(dir);
    if (QFile::exists(dir + QStringLiteral("/backend"))) {
      backendsDir.cd(QLatin1String("backend"));
    }

    if (backendsDir.exists(name))
      return backendsDir.absoluteFilePath(name);
  }

  return QString();
}

static inline bool testBackend(const QString& backendName) {
  const QString backendPath = getBackendFilePath(backendName);
  if (backendPath.isEmpty())
    return false;

  QPluginLoader loader(backendPath);
  if (!loader.load()) {
    qWarning() << "Backend error:" << loader.errorString();
    return false;
  }

  QObject* plugin = loader.instance();
  if (!plugin) {
    qWarning() << "Failed to create the plugin instance";
    loader.unload();
    return false;
  }

  ILXQtWMBackendLibrary* backend = qobject_cast<ILXQtWMBackendLibrary*>(plugin);
  const bool okay = backend != nullptr;

  loader.unload();

  return okay;
}

LXQtPanelApplicationPrivate::LXQtPanelApplicationPrivate(LXQtPanelApplication* q)
    : mSettings(nullptr), mWMBackend(nullptr), q_ptr(q) {}

ILXQtPanel::Position LXQtPanelApplicationPrivate::computeNewPanelPosition(const LXQtPanel* p, const int screenNum) {
  Q_Q(LXQtPanelApplication);
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

  return static_cast<ILXQtPanel::Position>(availablePosition);
}

void LXQtPanelApplicationPrivate::loadBackend() {
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
      ILXQtWMBackendLibrary* backend = qobject_cast<ILXQtWMBackendLibrary*>(plugin);
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
    mWMBackend = new LXQtDummyWMBackend;

    qWarning() << "\n"
               << "ERROR: Could not create a backend for window managment operations\n"
               << "Falling back to dummy backend. Some functions will not be available\n"
               << "\n";
  }

  mWMBackend->setParent(q_ptr);
}

LXQtPanelApplication::LXQtPanelApplication(int& argc, char** argv)
    : LXQt::Application(argc, argv, true), d_ptr(new LXQtPanelApplicationPrivate(this)) {
  Q_D(LXQtPanelApplication);

  QCoreApplication::setApplicationName(QLatin1String("lxqt-panel"));
  const QString VERINFO = QStringLiteral(LXQT_PANEL_VERSION "\nliblxqt   " LXQT_VERSION "\nQt        " QT_VERSION_STR);

  QCoreApplication::setApplicationVersion(VERINFO);

  QCommandLineParser parser;
  parser.setApplicationDescription(QLatin1String("LXQt Panel"));
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
    d->mSettings = new LXQt::Settings(QLatin1String("panel"), this);
  else
    d->mSettings = new LXQt::Settings(configFile, QSettings::IniFormat, this);

  d->loadBackend();

  const auto allScreens = screens();

  // workaround for historical multihead screen handling bugs in Qt
  for (QScreen* screen : allScreens) {
    connect(screen, &QScreen::destroyed, this, &LXQtPanelApplication::screenDestroyed);
  }
  connect(this, &QGuiApplication::screenAdded, this, &LXQtPanelApplication::handleScreenAdded);

  connect(this, &QCoreApplication::aboutToQuit, this, &LXQtPanelApplication::cleanup);

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

LXQtPanelApplication::~LXQtPanelApplication() {
  delete d_ptr;
}

void LXQtPanelApplication::cleanup() {
  qDeleteAll(mPanels);
}

void LXQtPanelApplication::addNewPanel() {
  Q_D(LXQtPanelApplication);

  const QString name = QStringLiteral("panel_") + QUuid::createUuid().toString();

  LXQtPanel* p = addPanel(name);
  const int screenNum = p->screenNum();
  const ILXQtPanel::Position newPanelPosition = d->computeNewPanelPosition(p, screenNum);
  p->setPosition(screenNum, newPanelPosition, true);
  QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
  panels << name;
  d->mSettings->setValue(QStringLiteral("panels"), panels);

  // popup the configuration dialog to allow user configuration right away
  p->showConfigDialog();
}

LXQtPanel* LXQtPanelApplication::addPanel(const QString& name) {
  Q_D(LXQtPanelApplication);

  LXQtPanel* panel = new LXQtPanel(name, d->mSettings);
  mPanels << panel;

  // reemit signals
  connect(panel, &LXQtPanel::deletedByUser, this, &LXQtPanelApplication::removePanel);
  connect(panel, &LXQtPanel::pluginAdded, this, &LXQtPanelApplication::pluginAdded);
  connect(panel, &LXQtPanel::pluginRemoved, this, &LXQtPanelApplication::pluginRemoved);

  return panel;
}

void LXQtPanelApplication::handleScreenAdded(QScreen* newScreen) {
  // qDebug() << "LXQtPanelApplication::handleScreenAdded" << newScreen;
  connect(newScreen, &QScreen::destroyed, this, &LXQtPanelApplication::screenDestroyed);
}

void LXQtPanelApplication::reloadPanelsAsNeeded() {
  Q_D(LXQtPanelApplication);

  // recreate missing panels which were deleted when their screen was destroyed
  const QStringList names = d->mSettings->value(QStringLiteral("panels")).toStringList();
  for (const QString& name : names) {
    bool found = false;
    for (LXQtPanel* panel : std::as_const(mPanels)) {
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

void LXQtPanelApplication::screenDestroyed(QObject* screenObj) {
  // Workaround for multihead screen changes that would otherwise leave panels in a broken state
  // Destroy panels whose QWindow is associated with the disappearing QScreen and recreate them later
  QScreen* screen = static_cast<QScreen*>(screenObj);
  bool reloadNeeded = false;
  qApp->setQuitOnLastWindowClosed(false);
  for (LXQtPanel* panel : std::as_const(mPanels)) {
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

void LXQtPanelApplication::removePanel(LXQtPanel* panel) {
  Q_D(LXQtPanelApplication);
  Q_ASSERT(mPanels.contains(panel));

  mPanels.removeAll(panel);

  QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
  panels.removeAll(panel->name());
  d->mSettings->setValue(QStringLiteral("panels"), panels);

  panel->deleteLater();
}

bool LXQtPanelApplication::isPluginSingletonAndRunning(QString const& pluginId) const {
  for (auto const& panel : mPanels)
    if (panel->isPluginSingletonAndRunning(pluginId))
      return true;

  return false;
}

ILXQtAbstractWMInterface* LXQtPanelApplication::getWMBackend() const {
  Q_D(const LXQtPanelApplication);
  return d->mWMBackend;
}

// see LXQtPanelApplication::LXQtPanelApplication for why this is not ideal
void LXQtPanelApplication::setIconTheme(const QString& iconTheme) {
  Q_D(LXQtPanelApplication);

  d->mSettings->setValue(QStringLiteral("iconTheme"), iconTheme == mGlobalIconTheme ? QString() : iconTheme);
  const QString newTheme = iconTheme.isEmpty() ? mGlobalIconTheme : iconTheme;
  if (newTheme != QIcon::themeName()) {
    QIcon::setThemeName(newTheme);
    for (LXQtPanel* panel : std::as_const(mPanels)) {
      panel->update();
      panel->updateConfigDialog();
    }
  }
}
