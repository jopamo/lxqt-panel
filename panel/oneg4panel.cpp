/* panel/oneg4panel.cpp
 * Main panel implementation, window management
 */

#include "oneg4panel.h"
#include "oneg4panellimits.h"
#include "ioneg4panelplugin.h"
#include "oneg4panelapplication.h"
#include "oneg4panellayout.h"
#include "config/configpaneldialog.h"
#include "popupmenu.h"
#include "plugin.h"
#include "panelpluginsmodel.h"
#include "windownotifier.h"
#include <OneG4/Application.h>
#include <OneG4/PluginInfo.h>

#include <QScreen>
#include <QWindow>
#include <QDebug>
#include <QString>
#include <QMenu>
#include <QMessageBox>
#include <QDropEvent>
#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <XdgIcon.h>
#include <XdgDirs.h>

#include <KWindowSystem>
#include <KX11Extras>
#include <NETWM>

#include <algorithm>

#include "backends/ioneg4abstractwmiface.h"

// Turn on this to show the time required to load each plugin during startup
// #define DEBUG_PLUGIN_LOADTIME
#ifdef DEBUG_PLUGIN_LOADTIME
#include <QElapsedTimer>
#endif

// Config keys and groups
#define CFG_KEY_SCREENNUM "desktop"
#define CFG_KEY_POSITION "position"
#define CFG_KEY_PANELSIZE "panelSize"
#define CFG_KEY_ICONSIZE "iconSize"
#define CFG_KEY_LINECNT "lineCount"
#define CFG_KEY_LENGTH "width"
#define CFG_KEY_PERCENT "width-percent"
#define CFG_KEY_ALIGNMENT "alignment"
#define CFG_KEY_FONTCOLOR "font-color"
#define CFG_KEY_BACKGROUNDCOLOR "background-color"
#define CFG_KEY_BACKGROUNDIMAGE "background-image"
#define CFG_KEY_OPACITY "opacity"
#define CFG_KEY_RESERVESPACE "reserve-space"
#define CFG_KEY_PLUGINS "plugins"
#define CFG_KEY_HIDABLE "hidable"
#define CFG_KEY_VISIBLE_MARGIN "visible-margin"
#define CFG_KEY_HIDE_ON_OVERLAP "hide-on-overlap"
#define CFG_KEY_ANIMATION "animation-duration"
#define CFG_KEY_SHOW_DELAY "show-delay"
#define CFG_KEY_LOCKPANEL "lockPanel"

/************************************************
 Returns the Position by the string
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive
 If the string is not correct, returns defaultValue
 ************************************************/
IOneG4Panel::Position OneG4Panel::strToPosition(const QString& str, IOneG4Panel::Position defaultValue) {
  if (str.toUpper() == QLatin1String("TOP"))
    return OneG4Panel::PositionTop;
  if (str.toUpper() == QLatin1String("LEFT"))
    return OneG4Panel::PositionLeft;
  if (str.toUpper() == QLatin1String("RIGHT"))
    return OneG4Panel::PositionRight;
  if (str.toUpper() == QLatin1String("BOTTOM"))
    return OneG4Panel::PositionBottom;
  return defaultValue;
}

/************************************************
 Return  string representation of the position
 ************************************************/
QString OneG4Panel::positionToStr(IOneG4Panel::Position position) {
  switch (position) {
    case OneG4Panel::PositionTop:
      return QStringLiteral("Top");
    case OneG4Panel::PositionLeft:
      return QStringLiteral("Left");
    case OneG4Panel::PositionRight:
      return QStringLiteral("Right");
    case OneG4Panel::PositionBottom:
      return QStringLiteral("Bottom");
  }

  return QString();
}

/************************************************

 ************************************************/
OneG4Panel::OneG4Panel(const QString& configGroup, OneG4::Settings* settings, QWidget* parent)
    : QFrame(parent),
      mSettings(settings),
      mConfigGroup(configGroup),
      mPlugins{nullptr},
      mStandaloneWindows{new WindowNotifier},
      mPanelSize(0),
      mIconSize(0),
      mLineCount(0),
      mLength(0),
      mAlignment(AlignmentLeft),
      mPosition(IOneG4Panel::PositionBottom),
      mScreenNum(0),
      mActualScreenNum(0),
      mHidable(false),
      mVisibleMargin(true),
      mHideOnOverlap(false),
      mHidden(false),
      mAnimationTime(0),
      mReserveSpace(true),
      mAnimation(nullptr),
      mLockPanel(false) {
  // You can find information about the flags and widget attributes in Qt documentation or at
  // https://doc.qt.io/qt-5/qt.html Qt::FramelessWindowHint produces a borderless window, the user cannot move or resize
  // a borderless window via the window system
  Qt::WindowFlags flags = Qt::FramelessWindowHint;

  // NOTE: by PCMan
  // In Qt 4 the window is not activated if it has Qt::WA_X11NetWmWindowTypeDock
  // Since Qt 5 the default behaviour is changed and a window is always activated on mouse click
  // This new behaviour caused oneg4 bug #161 - Cannot minimize windows from panel 1 when two task managers are open
  // Besides this breaks minimizing or restoring windows when clicking on the taskbar buttons
  // To workaround this regression bug we need to add this window flag here
  // However since the panel gets no keyboard focus this may decrease accessibility because it is not possible to use
  // the panel with keyboards
  Qt::WindowFlags noFocusFlag = Qt::WindowDoesNotAcceptFocus;
  flags |= noFocusFlag;

  setWindowFlags(flags);
  // Adds _NET_WM_WINDOW_TYPE_DOCK to the window's _NET_WM_WINDOW_TYPE X11 window property
  // See https://standards.freedesktop.org/wm-spec/ for more details
  setAttribute(Qt::WA_X11NetWmWindowTypeDock);
  // Enables tooltips for inactive windows
  setAttribute(Qt::WA_AlwaysShowToolTips);
  // Indicates that the widget should have a translucent background, any non-opaque regions of the widgets will be
  // translucent because the widget will have an alpha channel
  setAttribute(Qt::WA_TranslucentBackground);
  // Allows data from drag and drop operations to be dropped onto the widget
  setAttribute(Qt::WA_AcceptDrops);

  setWindowTitle(QStringLiteral("OneG4 Panel"));
  setObjectName(QStringLiteral("OneG4Panel %1").arg(configGroup));

  // OneG4Panel (inherits QFrame) -> lav (QGridLayout) -> OneG4PanelWidget (QFrame) -> OneG4PanelLayout
  OneG4PanelWidget = new QFrame(this);
  OneG4PanelWidget->setObjectName(QStringLiteral("BackgroundWidget"));
  QGridLayout* lav = new QGridLayout();
  lav->setContentsMargins(0, 0, 0, 0);
  setLayout(lav);
  this->layout()->addWidget(OneG4PanelWidget);

  mLayout = new OneG4PanelLayout(OneG4PanelWidget);
  connect(mLayout, &OneG4PanelLayout::pluginMoved, this, &OneG4Panel::pluginMoved);
  OneG4PanelWidget->setLayout(mLayout);
  mLayout->setLineCount(mLineCount);

  mDelaySave.setSingleShot(true);
  mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
  connect(&mDelaySave, &QTimer::timeout, this, [this] { saveSettings(); });

  mHideTimer.setSingleShot(true);
  mHideTimer.setInterval(PANEL_HIDE_DELAY);
  connect(&mHideTimer, &QTimer::timeout, this, &OneG4Panel::hidePanelWork);

  mShowDelayTimer.setSingleShot(true);
  mShowDelayTimer.setInterval(PANEL_SHOW_DELAY);
  connect(&mShowDelayTimer, &QTimer::timeout, this, [this] { showPanel(mAnimationTime > 0); });

  // screen updates
  connect(qApp, &QApplication::screenAdded, this, [this](QScreen* newScreen) {
    connect(newScreen, &QScreen::virtualGeometryChanged, this, &OneG4Panel::ensureVisible);
    connect(newScreen, &QScreen::geometryChanged, this, &OneG4Panel::ensureVisible);
    ensureVisible();
  });
  connect(qApp, &QApplication::screenRemoved, this, [this](QScreen* oldScreen) {
    disconnect(oldScreen, &QScreen::virtualGeometryChanged, this, &OneG4Panel::ensureVisible);
    disconnect(oldScreen, &QScreen::geometryChanged, this, &OneG4Panel::ensureVisible);
    // wait until the screen is really removed because it may contain the panel
    QTimer::singleShot(0, this, &OneG4Panel::ensureVisible);
  });
  const auto screens = QApplication::screens();
  for (const auto& screen : screens) {
    connect(screen, &QScreen::virtualGeometryChanged, this, &OneG4Panel::ensureVisible);
    connect(screen, &QScreen::geometryChanged, this, &OneG4Panel::ensureVisible);
  }

  connect(OneG4::Settings::globalSettings(), &OneG4::GlobalSettings::settingsChanged, this, [this] { update(); });
  if (auto* oneg4App = qobject_cast<OneG4::Application*>(qApp))
    connect(oneg4App, &OneG4::Application::themeChanged, this, &OneG4Panel::realign);

  connect(mStandaloneWindows.get(), &WindowNotifier::firstShown, this, [this] { showPanel(true); });
  connect(mStandaloneWindows.get(), &WindowNotifier::lastHidden, this, &OneG4Panel::hidePanel);

  readSettings();

  ensureVisible();

  loadPlugins();

  // Some X11 WMs may need the geometry to be set before QWidget::show()
  setPanelGeometry();

  show();

  // show it the first time despite setting
  if (mHidable) {
    showPanel(false);
    QTimer::singleShot(PANEL_HIDE_FIRST_TIME, this, SLOT(hidePanel()));
  }

  OneG4PanelApplication* a = reinterpret_cast<OneG4PanelApplication*>(qApp);
  auto wmBackend = a->getWMBackend();

  connect(wmBackend, &IOneG4AbstractWMInterface::windowAdded, this, [this] {
    if (mHidable && mHideOnOverlap && !mHidden) {
      mShowDelayTimer.stop();
      hidePanel();
    }
  });
  connect(wmBackend, &IOneG4AbstractWMInterface::windowRemoved, this, [this] {
    if (mHidable && mHideOnOverlap && mHidden && !isPanelOverlapped())
      mShowDelayTimer.start();
  });
  connect(wmBackend, &IOneG4AbstractWMInterface::currentWorkspaceChanged, this, [this] {
    if (mHidable && mHideOnOverlap) {
      if (!mHidden) {
        mShowDelayTimer.stop();
        hidePanel();
      }
      else if (!isPanelOverlapped())
        mShowDelayTimer.start();
      else
        mShowDelayTimer.stop();
    }
  });
  connect(wmBackend, &IOneG4AbstractWMInterface::windowPropertyChanged, this, [this](WId /* id */, int prop) {
    if (mHidable && mHideOnOverlap &&
        (prop == int(OneG4TaskBarWindowProperty::Geometry) || prop == int(OneG4TaskBarWindowProperty::State))) {
      if (!mHidden) {
        mShowDelayTimer.stop();
        hidePanel();
      }
      else if (!isPanelOverlapped())
        mShowDelayTimer.start();
      else
        mShowDelayTimer.stop();
    }
  });
}

/************************************************

 ************************************************/
void OneG4Panel::readSettings() {
  // Read settings
  mSettings->beginGroup(mConfigGroup);

  // Let Hidability be the first thing we read
  // so that every call to realign() is without side-effect
  mHidable = mSettings->value(QStringLiteral(CFG_KEY_HIDABLE), mHidable).toBool();
  mHidden = mHidable;

  mVisibleMargin = mSettings->value(QStringLiteral(CFG_KEY_VISIBLE_MARGIN), mVisibleMargin).toBool();

  mHideOnOverlap = mSettings->value(QStringLiteral(CFG_KEY_HIDE_ON_OVERLAP), mHideOnOverlap).toBool();

  mAnimationTime = mSettings->value(QStringLiteral(CFG_KEY_ANIMATION), mAnimationTime).toInt();
  mShowDelayTimer.setInterval(mSettings->value(QStringLiteral(CFG_KEY_SHOW_DELAY), mShowDelayTimer.interval()).toInt());

  // By default we are using size & count from theme
  setPanelSize(mSettings->value(QStringLiteral(CFG_KEY_PANELSIZE), PANEL_DEFAULT_SIZE).toInt(), false);
  setIconSize(mSettings->value(QStringLiteral(CFG_KEY_ICONSIZE), PANEL_DEFAULT_ICON_SIZE).toInt(), false);
  setLineCount(mSettings->value(QStringLiteral(CFG_KEY_LINECNT), PANEL_DEFAULT_LINE_COUNT).toInt(), false);

  setLength(mSettings->value(QStringLiteral(CFG_KEY_LENGTH), 100).toInt(),
            mSettings->value(QStringLiteral(CFG_KEY_PERCENT), true).toBool(), false);

  const auto screens = QApplication::screens();
  mScreenNum = std::clamp<int>(mSettings->value(QStringLiteral(CFG_KEY_SCREENNUM), 0).toInt(), 0, screens.size() - 1);
  setPosition(mScreenNum, strToPosition(mSettings->value(QStringLiteral(CFG_KEY_POSITION)).toString(), PositionBottom),
              false);

  setAlignment(Alignment(mSettings->value(QStringLiteral(CFG_KEY_ALIGNMENT), mAlignment).toInt()), false);

  QColor color = mSettings->value(QStringLiteral(CFG_KEY_FONTCOLOR), QString()).value<QColor>();
  if (color.isValid())
    setFontColor(color, false);

  setOpacity(mSettings->value(QStringLiteral(CFG_KEY_OPACITY), 100).toInt(), false);
  mReserveSpace = mSettings->value(QStringLiteral(CFG_KEY_RESERVESPACE), true).toBool();
  color = mSettings->value(QStringLiteral(CFG_KEY_BACKGROUNDCOLOR), QString()).value<QColor>();
  if (color.isValid())
    setBackgroundColor(color, false);

  QString image = mSettings->value(QStringLiteral(CFG_KEY_BACKGROUNDIMAGE), QString()).toString();
  if (!image.isEmpty())
    setBackgroundImage(image, false);

  mLockPanel = mSettings->value(QStringLiteral(CFG_KEY_LOCKPANEL), false).toBool();

  mSettings->endGroup();
}

/************************************************

 ************************************************/
void OneG4Panel::saveSettings(bool later) {
  mDelaySave.stop();
  if (later) {
    mDelaySave.start();
    return;
  }

  mSettings->beginGroup(mConfigGroup);

  mSettings->setValue(QStringLiteral(CFG_KEY_PANELSIZE), mPanelSize);
  mSettings->setValue(QStringLiteral(CFG_KEY_ICONSIZE), mIconSize);
  mSettings->setValue(QStringLiteral(CFG_KEY_LINECNT), mLineCount);

  mSettings->setValue(QStringLiteral(CFG_KEY_LENGTH), mLength);
  mSettings->setValue(QStringLiteral(CFG_KEY_PERCENT), mLengthInPercents);

  mSettings->setValue(QStringLiteral(CFG_KEY_SCREENNUM), mScreenNum);
  mSettings->setValue(QStringLiteral(CFG_KEY_POSITION), positionToStr(mPosition));

  mSettings->setValue(QStringLiteral(CFG_KEY_ALIGNMENT), mAlignment);

  mSettings->setValue(QStringLiteral(CFG_KEY_FONTCOLOR), mFontColor.isValid() ? mFontColor : QColor());
  mSettings->setValue(QStringLiteral(CFG_KEY_BACKGROUNDCOLOR),
                      mBackgroundColor.isValid() ? mBackgroundColor : QColor());
  mSettings->setValue(QStringLiteral(CFG_KEY_BACKGROUNDIMAGE),
                      QFileInfo::exists(mBackgroundImage) ? mBackgroundImage : QString());
  mSettings->setValue(QStringLiteral(CFG_KEY_OPACITY), mOpacity);
  mSettings->setValue(QStringLiteral(CFG_KEY_RESERVESPACE), mReserveSpace);

  mSettings->setValue(QStringLiteral(CFG_KEY_HIDABLE), mHidable);
  mSettings->setValue(QStringLiteral(CFG_KEY_VISIBLE_MARGIN), mVisibleMargin);
  mSettings->setValue(QStringLiteral(CFG_KEY_HIDE_ON_OVERLAP), mHideOnOverlap);
  mSettings->setValue(QStringLiteral(CFG_KEY_ANIMATION), mAnimationTime);
  mSettings->setValue(QStringLiteral(CFG_KEY_SHOW_DELAY), mShowDelayTimer.interval());

  mSettings->setValue(QStringLiteral(CFG_KEY_LOCKPANEL), mLockPanel);

  mSettings->endGroup();
}

/************************************************

 ************************************************/
void OneG4Panel::ensureVisible() {
  if (!canPlacedOn(mScreenNum, mPosition))
    setPosition(findAvailableScreen(mPosition), mPosition, false);
  else
    mActualScreenNum = mScreenNum;

  // the screen size might be changed
  realign();
}

/************************************************

 ************************************************/
OneG4Panel::~OneG4Panel() {
  mLayout->setEnabled(false);
  delete mAnimation;
  delete mConfigDialog.data();
  // do not save settings because of "user deleted panel" functionality
}

/************************************************

 ************************************************/
void OneG4Panel::show() {
  QWidget::show();
  if (qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
    KX11Extras::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
}

/************************************************

 ************************************************/
QStringList pluginDesktopDirs() {
  QStringList dirs;
  dirs << QString::fromLocal8Bit(qgetenv("ONEG4_PANEL_PLUGINS_DIR")).split(QLatin1Char(':'), Qt::SkipEmptyParts);
  dirs << QStringLiteral("%1/%2").arg(XdgDirs::dataHome(), QStringLiteral("/oneg4/1g4-panel"));
  // When running from the build tree, desktop files live under ../plugin-*/ next to the panel binary.
  const QDir appDir(QCoreApplication::applicationDirPath());
  const QDir buildRoot(appDir.absoluteFilePath(QStringLiteral("..")));
  const QStringList buildPluginDirs =
      buildRoot.entryList(QStringList() << QStringLiteral("plugin-*"), QDir::Dirs | QDir::NoSymLinks);
  for (const QString& entry : buildPluginDirs)
    dirs << buildRoot.absoluteFilePath(entry);
  dirs << QStringLiteral(PLUGIN_DESKTOPS_DIR);
  return dirs;
}

/************************************************

 ************************************************/
void OneG4Panel::loadPlugins() {
  QString names_key(mConfigGroup);
  names_key += QLatin1Char('/');
  names_key += QLatin1String(CFG_KEY_PLUGINS);
  mPlugins.reset(new PanelPluginsModel(this, settings(), names_key, pluginDesktopDirs()));

  connect(mPlugins.get(), &PanelPluginsModel::pluginAdded, mLayout, &OneG4PanelLayout::addPlugin);
  connect(mPlugins.get(), &PanelPluginsModel::pluginMovedUp, mLayout, &OneG4PanelLayout::moveUpPlugin);
  // reemit signals
  connect(mPlugins.get(), &PanelPluginsModel::pluginAdded, this, &OneG4Panel::pluginAdded);
  connect(mPlugins.get(), &PanelPluginsModel::pluginRemoved, this, &OneG4Panel::pluginRemoved);

  const auto plugins = mPlugins->plugins();
  for (auto const& plugin : plugins) {
    mLayout->addPlugin(plugin);
    connect(plugin, &Plugin::dragLeft, this, [this] {
      mShowDelayTimer.stop();
      hidePanel();
    });
  }
}

/************************************************

 ************************************************/
int OneG4Panel::getReserveDimension() {
  return mHidable ? PANEL_HIDE_SIZE : std::max(PANEL_MINIMUM_SIZE, mPanelSize);
}

void OneG4Panel::setPanelGeometry(bool animate) {
  const auto screens = QApplication::screens();
  if (mActualScreenNum >= screens.size())
    return;
  const QRect currentScreen = screens.at(mActualScreenNum)->geometry();

  QRect rect;

  if (isHorizontal()) {
    // horizontal panel
    rect.setHeight(std::max(PANEL_MINIMUM_SIZE, mPanelSize));
    if (mLengthInPercents)
      rect.setWidth(currentScreen.width() * mLength / 100.0);
    else {
      if (mLength <= 0)
        rect.setWidth(currentScreen.width() + mLength);
      else
        rect.setWidth(mLength);
    }

    rect.setWidth(std::max(rect.size().width(), mLayout->minimumSize().width()));

    switch (mAlignment) {
      case OneG4Panel::AlignmentLeft:
        rect.moveLeft(currentScreen.left());
        break;

      case OneG4Panel::AlignmentCenter:
        rect.moveCenter(currentScreen.center());
        break;

      case OneG4Panel::AlignmentRight:
        rect.moveRight(currentScreen.right());
        break;
    }

    if (mPosition == IOneG4Panel::PositionTop) {
      if (mHidden)
        rect.moveBottom(currentScreen.top() + PANEL_HIDE_SIZE - 1);
      else
        rect.moveTop(currentScreen.top());
    }
    else {
      if (mHidden)
        rect.moveTop(currentScreen.bottom() - PANEL_HIDE_SIZE + 1);
      else
        rect.moveBottom(currentScreen.bottom());
    }
  }
  else {
    // vertical panel
    rect.setWidth(std::max(PANEL_MINIMUM_SIZE, mPanelSize));
    if (mLengthInPercents)
      rect.setHeight(currentScreen.height() * mLength / 100.0);
    else {
      if (mLength <= 0)
        rect.setHeight(currentScreen.height() + mLength);
      else
        rect.setHeight(mLength);
    }

    rect.setHeight(std::max(rect.size().height(), mLayout->minimumSize().height()));

    switch (mAlignment) {
      case OneG4Panel::AlignmentLeft:
        rect.moveTop(currentScreen.top());
        break;

      case OneG4Panel::AlignmentCenter:
        rect.moveCenter(currentScreen.center());
        break;

      case OneG4Panel::AlignmentRight:
        rect.moveBottom(currentScreen.bottom());
        break;
    }

    if (mPosition == IOneG4Panel::PositionLeft) {
      if (mHidden)
        rect.moveRight(currentScreen.left() + PANEL_HIDE_SIZE - 1);
      else
        rect.moveLeft(currentScreen.left());
    }
    else {
      if (mHidden)
        rect.moveLeft(currentScreen.right() - PANEL_HIDE_SIZE + 1);
      else
        rect.moveRight(currentScreen.right());
    }
  }

  if (!mHidden || !mGeometry.isValid())
    mGeometry = rect;
  if (rect != geometry()) {
    setFixedSize(rect.size());
    if (animate) {
      if (mAnimation == nullptr) {
        mAnimation = new QPropertyAnimation(this, "geometry");
        mAnimation->setEasingCurve(QEasingCurve::Linear);
        // for hiding, the margins are set after animation is finished
        connect(mAnimation, &QAbstractAnimation::finished, this, [this] {
          if (mHidden)
            setMargins();
        });
      }
      mAnimation->setDuration(mAnimationTime);
      mAnimation->setStartValue(geometry());
      mAnimation->setEndValue(rect);
      // for showing, the margins are removed instantly
      if (!mHidden)
        setMargins();
      mAnimation->start();
    }
    else {
      setMargins();
      setGeometry(rect);
    }
  }
}

void OneG4Panel::setMargins() {
  if (mHidden) {
    if (isHorizontal()) {
      if (mPosition == IOneG4Panel::PositionTop)
        mLayout->setContentsMargins(0, 0, 0, PANEL_HIDE_SIZE);
      else
        mLayout->setContentsMargins(0, PANEL_HIDE_SIZE, 0, 0);
    }
    else {
      if (mPosition == IOneG4Panel::PositionLeft)
        mLayout->setContentsMargins(0, 0, PANEL_HIDE_SIZE, 0);
      else
        mLayout->setContentsMargins(PANEL_HIDE_SIZE, 0, 0, 0);
    }
    if (!mVisibleMargin)
      setWindowOpacity(0.0);
  }
  else {
    mLayout->setContentsMargins(0, 0, 0, 0);
    if (!mVisibleMargin)
      setWindowOpacity(1.0);
  }
}

void OneG4Panel::realign() {
  if (!isVisible())
    return;

  setPanelGeometry();

  // reserve our space on the screen
  // it is possible that our geometry is not changed but screen resolution is changed
  // so resetting WM_STRUT is still needed, we always do it
  updateWmStrut();
}

// Update the _NET_WM_PARTIAL_STRUT and _NET_WM_STRUT properties for the window
void OneG4Panel::updateWmStrut() {
  WId wid = effectiveWinId();
  if (wid == 0 || !isVisible())
    return;

  if (qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
    if (mReserveSpace && QApplication::primaryScreen()) {
      const QRect wholeScreen = QApplication::primaryScreen()->virtualGeometry();
      const QRect rect = geometry();
      // NOTE: https://standards.freedesktop.org/wm-spec/wm-spec-latest.html
      // The strut is relative to the screen edge, and not the edge of the xinerama monitor
      // So use the geometry of the whole screen to calculate the strut rather than using the geometry of individual
      // monitors
      switch (mPosition) {
        case OneG4Panel::PositionTop:
          KX11Extras::setExtendedStrut(wid,
                                       /* Left   */ 0, 0, 0,
                                       /* Right  */ 0, 0, 0,
                                       /* Top    */ rect.top() + getReserveDimension(), rect.left(), rect.right(),
                                       /* Bottom */ 0, 0, 0);
          break;

        case OneG4Panel::PositionBottom:
          KX11Extras::setExtendedStrut(wid,
                                       /* Left   */ 0, 0, 0,
                                       /* Right  */ 0, 0, 0,
                                       /* Top    */ 0, 0, 0,
                                       /* Bottom */ wholeScreen.bottom() - rect.bottom() + getReserveDimension(),
                                       rect.left(), rect.right());
          break;

        case OneG4Panel::PositionLeft:
          KX11Extras::setExtendedStrut(wid,
                                       /* Left   */ rect.left() + getReserveDimension(), rect.top(), rect.bottom(),
                                       /* Right  */ 0, 0, 0,
                                       /* Top    */ 0, 0, 0,
                                       /* Bottom */ 0, 0, 0);

          break;

        case OneG4Panel::PositionRight:
          KX11Extras::setExtendedStrut(wid,
                                       /* Left   */ 0, 0, 0,
                                       /* Right  */ wholeScreen.right() - rect.right() + getReserveDimension(),
                                       rect.top(), rect.bottom(),
                                       /* Top    */ 0, 0, 0,
                                       /* Bottom */ 0, 0, 0);
          break;
      }
    }
    else {
      KX11Extras::setExtendedStrut(wid,
                                   /* Left   */ 0, 0, 0,
                                   /* Right  */ 0, 0, 0,
                                   /* Top    */ 0, 0, 0,
                                   /* Bottom */ 0, 0, 0);
    }
  }
}

/************************************************
  This function checks if the panel can be placed on
  the display @screenNum at @position
  The panel can be placed only at screen edges
  but no part of it should be between two screens
 ************************************************/
bool OneG4Panel::canPlacedOn(int screenNum, OneG4Panel::Position position) {
  const auto screens = QApplication::screens();
  if (screens.size() > screenNum) {
    const QRect screenGeometry = screens.at(screenNum)->geometry();
    switch (position) {
      case OneG4Panel::PositionTop:
        for (const auto& screen : screens) {
          if (screen->geometry().top() < screenGeometry.top()) {
            QRect r = screenGeometry.adjusted(0, screen->geometry().top() - screenGeometry.top(), 0, 0);
            if (screen->geometry().intersects(r))
              return false;
          }
        }
        return true;

      case OneG4Panel::PositionBottom:
        for (const auto& screen : screens) {
          if (screen->geometry().bottom() > screenGeometry.bottom()) {
            QRect r = screenGeometry.adjusted(0, 0, 0, screen->geometry().bottom() - screenGeometry.bottom());
            if (screen->geometry().intersects(r))
              return false;
          }
        }
        return true;

      case OneG4Panel::PositionLeft:
        for (const auto& screen : screens) {
          if (screen->geometry().left() < screenGeometry.left()) {
            QRect r = screenGeometry.adjusted(screen->geometry().left() - screenGeometry.left(), 0, 0, 0);
            if (screen->geometry().intersects(r))
              return false;
          }
        }
        return true;

      case OneG4Panel::PositionRight:
        for (const auto& screen : screens) {
          if (screen->geometry().right() > screenGeometry.right()) {
            QRect r = screenGeometry.adjusted(0, 0, screen->geometry().right() - screenGeometry.right(), 0);
            if (screen->geometry().intersects(r))
              return false;
          }
        }
        return true;
    }
  }

  return false;
}

/************************************************

 ************************************************/
int OneG4Panel::findAvailableScreen(OneG4Panel::Position position) {
  int current = mScreenNum;

  for (int i = current; i < QApplication::screens().size(); ++i)
    if (canPlacedOn(i, position))
      return i;

  for (int i = 0; i < current; ++i)
    if (canPlacedOn(i, position))
      return i;

  return 0;
}

/************************************************

 ************************************************/
void OneG4Panel::showConfigDialog() {
  if (mConfigDialog.isNull())
    mConfigDialog = new ConfigPanelDialog(this, nullptr);

  mConfigDialog->showConfigPlacementPage();
  mStandaloneWindows->observeWindow(mConfigDialog.data());
  mConfigDialog->show();
  mConfigDialog->raise();
  mConfigDialog->activateWindow();
  WId wid = mConfigDialog->windowHandle()->winId();

  KX11Extras::activateWindow(wid);
  KX11Extras::setOnDesktop(wid, KX11Extras::currentDesktop());
}

/************************************************

 ************************************************/
void OneG4Panel::showAddPluginDialog() {
  if (mConfigDialog.isNull())
    mConfigDialog = new ConfigPanelDialog(this, nullptr);

  mConfigDialog->showConfigPluginsPage();
  mStandaloneWindows->observeWindow(mConfigDialog.data());
  mConfigDialog->show();
  mConfigDialog->raise();
  mConfigDialog->activateWindow();
  WId wid = mConfigDialog->windowHandle()->winId();

  KX11Extras::activateWindow(wid);
  KX11Extras::setOnDesktop(wid, KX11Extras::currentDesktop());
}

/************************************************

 ************************************************/
void OneG4Panel::updateStyleSheet() {
  // workaround for Qt >= 5.13, which might not completely
  // update the style sheet if it is not reset first
  setStyleSheet(QString());

  QStringList sheet;
  sheet << QStringLiteral("Plugin > QAbstractButton, OneG4Tray { qproperty-iconSize: %1px %1px; }").arg(mIconSize);
  sheet << QStringLiteral("Plugin > * > QAbstractButton, TrayIcon { qproperty-iconSize: %1px %1px; }").arg(mIconSize);

  if (mFontColor.isValid())
    sheet << QString(QStringLiteral("Plugin * { color: ") + mFontColor.name() + QStringLiteral("; }"));

  if (mBackgroundColor.isValid()) {
    QString color = QStringLiteral("%1, %2, %3, %4")
                        .arg(mBackgroundColor.red())
                        .arg(mBackgroundColor.green())
                        .arg(mBackgroundColor.blue())
                        .arg(static_cast<float>(mOpacity) / 100);
    sheet << QString(QStringLiteral("OneG4Panel #BackgroundWidget { background-color: rgba(") + color +
                     QStringLiteral("); }"));
  }

  if (QFileInfo::exists(mBackgroundImage))
    sheet << QString(QStringLiteral("OneG4Panel #BackgroundWidget { background-image: url('") + mBackgroundImage +
                     QStringLiteral("');}"));

  setStyleSheet(sheet.join(QStringLiteral("\n")));
}

/************************************************

 ************************************************/
void OneG4Panel::setPanelSize(int value, bool save) {
  if (mPanelSize != value) {
    mPanelSize = value;
    realign();

    if (save)
      saveSettings(true);
  }
}

/************************************************

 ************************************************/
void OneG4Panel::setIconSize(int value, bool save) {
  if (mIconSize != value) {
    mIconSize = value;
    updateStyleSheet();
    mLayout->setLineSize(mIconSize);

    if (save)
      saveSettings(true);

    realign();
  }
}

/************************************************

 ************************************************/
void OneG4Panel::setLineCount(int value, bool save) {
  if (mLineCount != value) {
    mLineCount = value;
    mLayout->setEnabled(false);
    mLayout->setLineCount(mLineCount);
    mLayout->setEnabled(true);

    if (save)
      saveSettings(true);

    realign();
  }
}

/************************************************

 ************************************************/
void OneG4Panel::setLength(int length, bool inPercents, bool save) {
  if (mLength == length && mLengthInPercents == inPercents)
    return;

  mLength = length;
  mLengthInPercents = inPercents;

  if (save)
    saveSettings(true);

  realign();
}

/************************************************

 ************************************************/
void OneG4Panel::setPosition(int screen, IOneG4Panel::Position position, bool save) {
  if (mScreenNum == screen && mPosition == position)
    return;

  mActualScreenNum = screen;
  mPosition = position;
  mLayout->setPosition(mPosition);

  if (save) {
    mScreenNum = screen;
    saveSettings(true);
  }

  // on Qt 5 use QScreen API for setting the screen of a QWindow
  if (windowHandle())
    windowHandle()->setScreen(qApp->screens().at(screen));

  realign();
}

/************************************************
 *
 ************************************************/
void OneG4Panel::setAlignment(Alignment value, bool save) {
  if (mAlignment == value)
    return;

  mAlignment = value;

  if (save)
    saveSettings(true);

  realign();
}

/************************************************
 *
 ************************************************/
void OneG4Panel::setFontColor(QColor color, bool save) {
  mFontColor = color;
  updateStyleSheet();

  if (save)
    saveSettings(true);
}

/************************************************

 ************************************************/
void OneG4Panel::setBackgroundColor(QColor color, bool save) {
  mBackgroundColor = color;
  updateStyleSheet();

  if (save)
    saveSettings(true);
}

/************************************************

 ************************************************/
void OneG4Panel::setBackgroundImage(QString path, bool save) {
  mBackgroundImage = std::move(path);
  updateStyleSheet();

  if (save)
    saveSettings(true);
}

/************************************************
 *
 ************************************************/
void OneG4Panel::setOpacity(int opacity, bool save) {
  mOpacity = std::clamp(opacity, 0, 100);
  updateStyleSheet();

  if (save)
    saveSettings(true);
}

/************************************************
 *
 ************************************************/
void OneG4Panel::setReserveSpace(bool reserveSpace, bool save) {
  if (mReserveSpace == reserveSpace)
    return;

  mReserveSpace = reserveSpace;

  if (save)
    saveSettings(true);

  updateWmStrut();
}

/************************************************

 ************************************************/
QRect OneG4Panel::globalGeometry() const {
  // panel is the top-most widget/window, no calculation needed
  return geometry();
}

/************************************************

 ************************************************/
bool OneG4Panel::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::ContextMenu:
      showPopupMenu(static_cast<QContextMenuEvent*>(event)->globalPos());
      break;

    case QEvent::LayoutRequest:
      emit realigned();
      break;

    case QEvent::WinIdChange: {
      if (qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
        if (effectiveWinId() == 0)
          break;

        // Sometimes Qt needs to re-create the underlying window of the widget and
        // the winId() may be changed at runtime, so we need to reset all X11 properties
        qDebug() << "WinIdChange" << Qt::hex << effectiveWinId() << "handle" << windowHandle()
                 << windowHandle()->screen();

        // Qt::WA_X11NetWmWindowTypeDock becomes ineffective in Qt 5
        // See QTBUG-39887, use KWindowSystem instead
        KX11Extras::setType(effectiveWinId(), NET::Dock);

        updateWmStrut();
        KX11Extras::setOnAllDesktops(effectiveWinId(), true);
      }
      break;
    }
    case QEvent::DragEnter:
      dynamic_cast<QDropEvent*>(event)->setDropAction(Qt::IgnoreAction);
      event->accept();
#if __cplusplus >= 201703L
      [[fallthrough]];
#endif
    case QEvent::Enter:
      mShowDelayTimer.start();
      break;

    case QEvent::Leave:
    case QEvent::DragLeave:
      mShowDelayTimer.stop();
      hidePanel();
      break;

    default:
      break;
  }

  return QFrame::event(event);
}

/************************************************

 ************************************************/

void OneG4Panel::showEvent(QShowEvent* event) {
  QFrame::showEvent(event);
  realign();
}

/************************************************

 ************************************************/
void OneG4Panel::showPopupMenu(const QPoint& cursorPos, Plugin* plugin) {
  PopupMenu* menu = new PopupMenu(tr("Panel"), this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  menu->setIcon(XdgIcon::fromTheme(QStringLiteral("configure-toolbars")));

  // Plugin menu
  if (plugin) {
    QMenu* m = plugin->popupMenu();

    if (m) {
      menu->addTitle(plugin->windowTitle());
      const auto actions = m->actions();
      for (auto const& action : actions) {
        action->setParent(menu);
        action->setDisabled(mLockPanel);
        menu->addAction(action);
      }
      delete m;
    }
  }

  // Panel menu

  menu->addTitle(QIcon(), tr("Panel"));

  menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")), tr("Configure Panel"), this,
                  &OneG4Panel::showConfigDialog)
      ->setDisabled(mLockPanel);

  menu->addAction(XdgIcon::fromTheme(QStringLiteral("preferences-plugin")), tr("Manage Widgets"), this,
                  &OneG4Panel::showAddPluginDialog)
      ->setDisabled(mLockPanel);

  OneG4PanelApplication* a = reinterpret_cast<OneG4PanelApplication*>(qApp);
  menu->addAction(XdgIcon::fromTheme(QLatin1String("list-add")), tr("Add New Panel"), a,
                  &OneG4PanelApplication::addNewPanel);

  if (a->count() > 1) {
    menu->addAction(XdgIcon::fromTheme(QLatin1String("list-remove")), tr("Remove Panel", "Menu Item"), this,
                    &OneG4Panel::userRequestForDeletion)
        ->setDisabled(mLockPanel);
  }

  QAction* act_lock = menu->addAction(tr("Lock This Panel"));
  act_lock->setCheckable(true);
  act_lock->setChecked(mLockPanel);
  connect(act_lock, &QAction::triggered, this, [this] {
    mLockPanel = !mLockPanel;
    saveSettings(false);
  });

#ifdef DEBUG
  menu->addSeparator();
  menu->addAction("Exit (debug only)", qApp, &QApplication::quit);
#endif

  // In multihead & multipanel setup QMenu::popup/exec may place the window wrongly
  // so place it manually
  menu->setGeometry(calculatePopupWindowPos(cursorPos, menu->sizeHint()));
  willShowWindow(menu);
  menu->show();
}

Plugin* OneG4Panel::findPlugin(const IOneG4PanelPlugin* iPlugin) const {
  const auto plugins = mPlugins->plugins();
  for (auto const& plug : plugins)
    if (plug->iPlugin() == iPlugin)
      return plug;
  return nullptr;
}

/************************************************

 ************************************************/
QRect OneG4Panel::calculatePopupWindowPos(QPoint const& absolutePos, QSize const& windowSize) const {
  // Use local coordinates and then map them to global coordinates
  QPoint localPos = mapFromGlobal(absolutePos);
  int x = localPos.x(), y = localPos.y();

  switch (position()) {
    case IOneG4Panel::PositionTop:
      y = mGeometry.height();
      break;

    case IOneG4Panel::PositionBottom:
      y = -windowSize.height();
      break;

    case IOneG4Panel::PositionLeft:
      x = mGeometry.width();
      break;

    case IOneG4Panel::PositionRight:
      x = -windowSize.width();
      break;
  }

  QRect res(mapToGlobal(QPoint(x, y)), windowSize);

  QRect panelScreen;
  const auto screens = QApplication::screens();
  if (mActualScreenNum < screens.size())
    panelScreen = screens.at(mActualScreenNum)->geometry();
  // Cannot use availableGeometry here because in a multihead setup with different resolutions
  // the work area may be limited by the smallest monitor and much smaller than the current screen
  if (res.right() > panelScreen.right())
    res.moveRight(panelScreen.right());

  if (res.bottom() > panelScreen.bottom())
    res.moveBottom(panelScreen.bottom());

  if (res.left() < panelScreen.left())
    res.moveLeft(panelScreen.left());

  if (res.top() < panelScreen.top())
    res.moveTop(panelScreen.top());

  return res;
}

/************************************************

 ************************************************/
QRect OneG4Panel::calculatePopupWindowPos(const IOneG4PanelPlugin* plugin, const QSize& windowSize) const {
  Plugin* panel_plugin = findPlugin(plugin);
  if (nullptr == panel_plugin) {
    qWarning() << Q_FUNC_INFO << "Wrong logic? Unable to find Plugin* for" << plugin << "known plugins follow...";
    const auto plugins = mPlugins->plugins();
    for (auto const& plug : plugins)
      qWarning() << plug->iPlugin() << plug;

    return QRect();
  }

  // assuming there are no contentMargins around the "BackgroundWidget" (OneG4PanelWidget)
  return calculatePopupWindowPos(mapToGlobal(panel_plugin->geometry().topLeft()), windowSize);
}

/************************************************

 ************************************************/
void OneG4Panel::willShowWindow(QWidget* w) {
  mStandaloneWindows->observeWindow(w);
}

/************************************************

 ************************************************/
void OneG4Panel::pluginFlagsChanged(const IOneG4PanelPlugin* /*plugin*/) {
  mLayout->rebuild();
}

/************************************************

 ************************************************/
QString OneG4Panel::qssPosition() const {
  return positionToStr(position());
}

/************************************************

 ************************************************/
void OneG4Panel::pluginMoved(Plugin* plug) {
  // get new position of the moved plugin
  bool found{false};
  QString plug_is_before;
  for (int i = 0; i < mLayout->count(); ++i) {
    Plugin* plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
    if (plugin) {
      if (found) {
        // we found our plugin in previous cycle, it is before this (or empty as last)
        plug_is_before = plugin->settingsGroup();
        break;
      }
      else
        found = (plug == plugin);
    }
  }
  mPlugins->movePlugin(plug, plug_is_before);
}

/************************************************

 ************************************************/
void OneG4Panel::userRequestForDeletion() {
  const QMessageBox::StandardButton ret = QMessageBox::warning(
      this, tr("Remove Panel", "Dialog Title"),
      tr("Removing a panel can not be undone.\nDo you want to remove this panel?"), QMessageBox::Yes | QMessageBox::No);

  if (ret != QMessageBox::Yes) {
    return;
  }

  mSettings->beginGroup(mConfigGroup);
  const QStringList plugins = mSettings->value(QStringLiteral("plugins")).toStringList();
  mSettings->endGroup();

  for (const QString& i : plugins)
    if (!i.isEmpty())
      mSettings->remove(i);

  mSettings->remove(mConfigGroup);

  emit deletedByUser(this);
}

bool OneG4Panel::isPanelOverlapped() const {
  OneG4PanelApplication* a = reinterpret_cast<OneG4PanelApplication*>(qApp);

  QRect area = mGeometry;
  return a->getWMBackend()->isAreaOverlapped(area);
}

void OneG4Panel::showPanel(bool animate) {
  if (mHidable) {
    mHideTimer.stop();
    if (mHidden) {
      mHidden = false;
      setPanelGeometry(mAnimationTime > 0 && animate);
    }
  }
}

void OneG4Panel::hidePanel() {
  if (mHidable && !mHidden && !mStandaloneWindows->isAnyWindowShown())
    mHideTimer.start();
}

void OneG4Panel::hidePanelWork() {
  if (!testAttribute(Qt::WA_UnderMouse)) {
    if (!mStandaloneWindows->isAnyWindowShown()) {
      if (!mHideOnOverlap || isPanelOverlapped()) {
        mHidden = true;
        setPanelGeometry(mAnimationTime > 0);
      }
    }
    else {
      mHideTimer.start();
    }
  }
}

void OneG4Panel::setHidable(bool hidable, bool save) {
  if (mHidable == hidable)
    return;

  mHidable = hidable;

  if (save)
    saveSettings(true);

  realign();
}

void OneG4Panel::setVisibleMargin(bool visibleMargin, bool save) {
  if (mVisibleMargin == visibleMargin)
    return;

  mVisibleMargin = visibleMargin;

  if (save)
    saveSettings(true);

  realign();
}

void OneG4Panel::setHideOnOverlap(bool hideOnOverlap, bool save) {
  if (mHideOnOverlap == hideOnOverlap)
    return;

  mHideOnOverlap = hideOnOverlap;

  if (save)
    saveSettings(true);

  realign();
}

void OneG4Panel::setAnimationTime(int animationTime, bool save) {
  if (mAnimationTime == animationTime)
    return;

  mAnimationTime = animationTime;

  if (save)
    saveSettings(true);
}

void OneG4Panel::setShowDelay(int showDelay, bool save) {
  if (mShowDelayTimer.interval() == showDelay)
    return;

  mShowDelayTimer.setInterval(showDelay);

  if (save)
    saveSettings(true);
}

QString OneG4Panel::iconTheme() const {
  return mSettings->value(QStringLiteral("iconTheme")).toString();
}

void OneG4Panel::setIconTheme(const QString& iconTheme) {
  OneG4PanelApplication* a = reinterpret_cast<OneG4PanelApplication*>(qApp);
  a->setIconTheme(iconTheme);
}

void OneG4Panel::updateConfigDialog() const {
  if (!mConfigDialog.isNull() && mConfigDialog->isVisible()) {
    mConfigDialog->updateIconThemeSettings();
    const QList<QWidget*> widgets = mConfigDialog->findChildren<QWidget*>();
    for (QWidget* widget : widgets)
      widget->update();
  }
}

bool OneG4Panel::isPluginSingletonAndRunning(QString const& pluginId) const {
  Plugin const* plugin = mPlugins->pluginByID(pluginId);
  if (nullptr == plugin)
    return false;
  else
    return plugin->iPlugin()->flags().testFlag(IOneG4PanelPlugin::SingleInstance);
}
