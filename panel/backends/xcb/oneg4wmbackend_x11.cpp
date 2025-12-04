/* panel/backends/xcb/oneg4wmbackend_x11.cpp
 * Window manager backend interfaces
 */

#include "oneg4wmbackend_x11.h"

#include <KX11Extras>
#include <KWindowSystem>
#include <KWindowInfo>

// Necessary for closeApplication()
#include <NETWM>

#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QCursor>
#include <QtMath>

// NOTE: Xlib.h defines Bool which conflicts with QJsonValue::Type enum
#include <X11/Xlib.h>
#undef Bool

OneG4WMBackendX11::OneG4WMBackendX11(QObject* parent) : IOneG4AbstractWMInterface(parent) {
  auto* x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
  Q_ASSERT_X(x11Application, "OneG4WMBackendX11", "Constructed without X11 connection");
  m_X11Display = x11Application->display();
  m_xcbConnection = x11Application->connection();

  connect(KX11Extras::self(), &KX11Extras::windowChanged, this, &OneG4WMBackendX11::onWindowChanged);
  connect(KX11Extras::self(), &KX11Extras::windowAdded, this, &OneG4WMBackendX11::onWindowAdded);
  connect(KX11Extras::self(), &KX11Extras::windowRemoved, this, &OneG4WMBackendX11::onWindowRemoved);

  connect(KX11Extras::self(), &KX11Extras::numberOfDesktopsChanged, this,
          &IOneG4AbstractWMInterface::workspacesCountChanged);
  connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this,
          [this](int x) { emit currentWorkspaceChanged(x, QString()); });
  connect(KX11Extras::self(), &KX11Extras::desktopNamesChanged, this, [this]() { emit workspaceNameChanged(-1); });

  connect(KX11Extras::self(), &KX11Extras::activeWindowChanged, this, &IOneG4AbstractWMInterface::activeWindowChanged);
}

/************************************************
 *   Model slots
 ************************************************/
void OneG4WMBackendX11::onWindowChanged(WId windowId, NET::Properties prop, NET::Properties2 prop2) {
  if (!m_windows.contains(windowId)) {
    // if an unknown window changes in a way that makes it acceptable, add it to the taskbar
    if (acceptWindow(windowId))
      onWindowAdded(windowId);
    return;
  }

  if (!acceptWindow(windowId)) {
    // if a known window changes in a way that makes it unacceptable, remove it from the taskbar
    onWindowRemoved(windowId);
    return;
  }

  if (prop.testFlag(NET::WMGeometry))
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Geometry));

  if (prop2.testFlag(NET::WM2WindowClass))
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::WindowClass));

  // window changed virtual desktop
  if (prop.testFlag(NET::WMDesktop))
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Workspace));

  if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Title));

  // we are setting window icon geometry, no need to handle NET::WMIconGeometry
  // icon of the button can be based on windowClass
  if (prop.testFlag(NET::WMIcon) || prop2.testFlag(NET::WM2WindowClass))
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Icon));

  bool update_urgency = false;
  if (prop2.testFlag(NET::WM2Urgency))
    update_urgency = true;

  if (prop.testFlag(NET::WMState)) {
    update_urgency = true;
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::State));
  }

  if (update_urgency)
    emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Urgency));
}

void OneG4WMBackendX11::onWindowAdded(WId windowId) {
  if (m_windows.contains(windowId))
    return;

  if (!acceptWindow(windowId))
    return;

  addWindow_internal(windowId);
}

void OneG4WMBackendX11::onWindowRemoved(WId windowId) {
  const int row = m_windows.indexOf(windowId);
  if (row == -1)
    return;

  m_iconGeometries.remove(windowId);
  m_windows.removeAt(row);

  emit windowRemoved(windowId);
}

/************************************************
 *   Model private functions
 ************************************************/
bool OneG4WMBackendX11::acceptWindow(WId windowId) const {
  QFlags<NET::WindowTypeMask> ignoreList;
  ignoreList |= NET::DesktopMask;
  ignoreList |= NET::DockMask;
  ignoreList |= NET::SplashMask;
  ignoreList |= NET::ToolbarMask;
  ignoreList |= NET::MenuMask;
  ignoreList |= NET::PopupMenuMask;
  ignoreList |= NET::NotificationMask;

  KWindowInfo info(windowId, NET::WMWindowType | NET::WMState, NET::WM2TransientFor);
  if (!info.valid())
    return false;

  if (NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
    return false;

  if (info.state() & NET::SkipTaskbar)
    return false;

  // WM_TRANSIENT_FOR hint not set means a normal window
  WId transFor = info.transientFor();

  WId appRootWindow = XDefaultRootWindow(m_X11Display);

  if (transFor == 0 || transFor == windowId || transFor == appRootWindow)
    return true;

  info = KWindowInfo(transFor, NET::WMWindowType);

  QFlags<NET::WindowTypeMask> normalFlag;
  normalFlag |= NET::NormalMask;
  normalFlag |= NET::DialogMask;
  normalFlag |= NET::UtilityMask;

  return !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), normalFlag);
}

void OneG4WMBackendX11::addWindow_internal(WId windowId) {
  m_windows.append(windowId);
  emit windowAdded(windowId);
}

/************************************************
 *   Windows function
 ************************************************/
bool OneG4WMBackendX11::supportsAction(WId windowId, OneG4TaskBarBackendAction action) const {
  NET::Action x11Action;

  switch (action) {
    case OneG4TaskBarBackendAction::Move:
      x11Action = NET::ActionMove;
      break;

    case OneG4TaskBarBackendAction::Resize:
      x11Action = NET::ActionResize;
      break;

    case OneG4TaskBarBackendAction::Maximize:
      x11Action = NET::ActionMax;
      break;

    case OneG4TaskBarBackendAction::MaximizeVertically:
      x11Action = NET::ActionMaxVert;
      break;

    case OneG4TaskBarBackendAction::MaximizeHorizontally:
      x11Action = NET::ActionMaxHoriz;
      break;

    case OneG4TaskBarBackendAction::Minimize:
      x11Action = NET::ActionMinimize;
      break;

    case OneG4TaskBarBackendAction::RollUp:
      x11Action = NET::ActionShade;
      break;

    case OneG4TaskBarBackendAction::FullScreen:
      x11Action = NET::ActionFullScreen;
      break;

    case OneG4TaskBarBackendAction::DesktopSwitch:
      return true;

    case OneG4TaskBarBackendAction::MoveToDesktop:
      return true;

    case OneG4TaskBarBackendAction::MoveToLayer:
      return true;

    case OneG4TaskBarBackendAction::MoveToOutput:
      return true;

    default:
      return false;
  }

  KWindowInfo info(windowId, NET::Properties(), NET::WM2AllowedActions);
  return info.actionSupported(x11Action);
}

bool OneG4WMBackendX11::reloadWindows() {
  QVector<WId> knownWindows;
  qSwap(knownWindows, m_windows);
  QList<WId> new_list;

  // just add new windows to groups, deleting is up to the groups
  const auto wnds = KX11Extras::stackingOrder();
  for (auto const wnd : wnds) {
    if (acceptWindow(wnd)) {
      new_list << wnd;
      addWindow_internal(wnd);
    }
  }

  // emulate windowRemoved if known window not reported by KWindowSystem
  for (auto i = knownWindows.begin(), i_e = knownWindows.end(); i != i_e; ++i) {
    WId wnd = *i;
    if (!new_list.contains(wnd)) {
      m_iconGeometries.remove(wnd);
      emit windowRemoved(wnd);
    }
  }

  emit reloaded();

  return true;
}

QVector<WId> OneG4WMBackendX11::getCurrentWindows() const {
  return m_windows;
}

QString OneG4WMBackendX11::getWindowTitle(WId windowId) const {
  KWindowInfo info(windowId, NET::WMVisibleName | NET::WMName);
  QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
  return title;
}

bool OneG4WMBackendX11::applicationDemandsAttention(WId windowId) const {
  WId appRootWindow = XDefaultRootWindow(m_X11Display);
  return NETWinInfo(m_xcbConnection, windowId, appRootWindow, NET::Properties{}, NET::WM2Urgency).urgency() ||
         KWindowInfo{windowId, NET::WMState}.hasState(NET::DemandsAttention);
}

QIcon OneG4WMBackendX11::getApplicationIcon(WId windowId, int devicePixels) const {
  return KX11Extras::icon(windowId, devicePixels, devicePixels);
}

QString OneG4WMBackendX11::getWindowClass(WId windowId) const {
  KWindowInfo info(windowId, NET::Properties(), NET::WM2WindowClass);
  return QString::fromUtf8(info.windowClassClass());
}

OneG4TaskBarWindowLayer OneG4WMBackendX11::getWindowLayer(WId windowId) const {
  NET::States state = KWindowInfo(windowId, NET::WMState).state();
  if (state.testFlag(NET::KeepAbove))
    return OneG4TaskBarWindowLayer::KeepAbove;
  else if (state.testFlag(NET::KeepBelow))
    return OneG4TaskBarWindowLayer::KeepBelow;
  return OneG4TaskBarWindowLayer::Normal;
}

bool OneG4WMBackendX11::setWindowLayer(WId windowId, OneG4TaskBarWindowLayer layer) {
  switch (layer) {
    case OneG4TaskBarWindowLayer::KeepAbove:
      KX11Extras::clearState(windowId, NET::KeepBelow);
      KX11Extras::setState(windowId, NET::KeepAbove);
      break;

    case OneG4TaskBarWindowLayer::KeepBelow:
      KX11Extras::clearState(windowId, NET::KeepAbove);
      KX11Extras::setState(windowId, NET::KeepBelow);
      break;

    default:
      KX11Extras::clearState(windowId, NET::KeepBelow);
      KX11Extras::clearState(windowId, NET::KeepAbove);
      break;
  }

  return true;
}

OneG4TaskBarWindowState OneG4WMBackendX11::getWindowState(WId windowId) const {
  KWindowInfo info(windowId, NET::WMState | NET::XAWMState);
  if (info.isMinimized())
    return OneG4TaskBarWindowState::Minimized;

  NET::States state = info.state();
  if (state.testFlag(NET::Hidden))
    return OneG4TaskBarWindowState::Hidden;
  if (state.testFlag(NET::Max))
    return OneG4TaskBarWindowState::Maximized;
  if (state.testFlag(NET::MaxHoriz))
    return OneG4TaskBarWindowState::MaximizedHorizontally;
  if (state.testFlag(NET::MaxVert))
    return OneG4TaskBarWindowState::MaximizedVertically;
  if (state.testFlag(NET::Shaded))
    return OneG4TaskBarWindowState::RolledUp;
  if (state.testFlag(NET::FullScreen))
    return OneG4TaskBarWindowState::FullScreen;

  return OneG4TaskBarWindowState::Normal;
}

bool OneG4WMBackendX11::setWindowState(WId windowId, OneG4TaskBarWindowState state, bool set) {
  // window activation is left to the caller

  NET::State x11State;

  switch (state) {
    case OneG4TaskBarWindowState::Minimized: {
      if (set)
        KX11Extras::minimizeWindow(windowId);
      else
        KX11Extras::unminimizeWindow(windowId);
      return true;
    }
    case OneG4TaskBarWindowState::Maximized: {
      x11State = NET::Max;
      break;
    }
    case OneG4TaskBarWindowState::MaximizedVertically: {
      x11State = NET::MaxVert;
      break;
    }
    case OneG4TaskBarWindowState::MaximizedHorizontally: {
      x11State = NET::MaxHoriz;
      break;
    }
    case OneG4TaskBarWindowState::Normal: {
      x11State = NET::Max;  // treat Normal as clearing maximized state
      break;
    }
    case OneG4TaskBarWindowState::RolledUp: {
      x11State = NET::Shaded;
      break;
    }
    default:
      return false;
  }

  if (set)
    KX11Extras::setState(windowId, x11State);
  else
    KX11Extras::clearState(windowId, x11State);

  return true;
}

bool OneG4WMBackendX11::isWindowActive(WId windowId) const {
  return KX11Extras::activeWindow() == windowId;
}

bool OneG4WMBackendX11::raiseWindow(WId windowId, bool onCurrentWorkSpace) {
  if (onCurrentWorkSpace && getWindowState(windowId) == OneG4TaskBarWindowState::Minimized) {
    setWindowOnWorkspace(windowId, getCurrentWorkspace());
  }
  else {
    setCurrentWorkspace(getWindowWorkspace(windowId));
  }

  // bypass focus stealing prevention
  KX11Extras::forceActiveWindow(windowId);

  // clear urgency flag
  emit windowPropertyChanged(windowId, int(OneG4TaskBarWindowProperty::Urgency));

  return true;
}

bool OneG4WMBackendX11::closeWindow(WId windowId) {
  // no equivalent in KWindowSystem, use NETRootInfo directly
  NETRootInfo(m_xcbConnection, NET::CloseWindow).closeWindowRequest(windowId);
  return true;
}

WId OneG4WMBackendX11::getActiveWindow() const {
  return KX11Extras::activeWindow();
}

/************************************************
 *   Workspaces
 ************************************************/
int OneG4WMBackendX11::getWorkspacesCount(QScreen*) const {
  return KX11Extras::numberOfDesktops();
}

QString OneG4WMBackendX11::getWorkspaceName(int idx, QString) const {
  return KX11Extras::desktopName(idx);
}

int OneG4WMBackendX11::getCurrentWorkspace(QScreen*) const {
  return KX11Extras::currentDesktop();
}

bool OneG4WMBackendX11::setCurrentWorkspace(int idx, QScreen*) {
  if (KX11Extras::currentDesktop() == idx)
    return true;

  KX11Extras::setCurrentDesktop(idx);
  return true;
}

int OneG4WMBackendX11::getWindowWorkspace(WId windowId) const {
  KWindowInfo info(windowId, NET::WMDesktop);
  return info.desktop();
}

bool OneG4WMBackendX11::setWindowOnWorkspace(WId windowId, int idx) {
  KX11Extras::setOnDesktop(windowId, idx);
  return true;
}

void OneG4WMBackendX11::moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) {
  KWindowInfo info(windowId, NET::WMDesktop);
  if (!info.isOnCurrentDesktop())
    KX11Extras::setCurrentDesktop(info.desktop());

  if (getWindowState(windowId) == OneG4TaskBarWindowState::Minimized)
    KX11Extras::unminimizeWindow(windowId);

  KX11Extras::forceActiveWindow(windowId);

  const QRect& windowGeometry = KWindowInfo(windowId, NET::WMFrameExtents).frameGeometry();
  QList<QScreen*> screens = QGuiApplication::screens();
  if (screens.size() > 1) {
    for (int i = 0; i < screens.size(); ++i) {
      QRect screenGeometry = screens[i]->geometry();
      if (screenGeometry.intersects(windowGeometry)) {
        int targetScreen = i + (next ? 1 : -1);
        if (targetScreen < 0)
          targetScreen += screens.size();
        else if (targetScreen >= screens.size())
          targetScreen -= screens.size();

        QRect targetScreenGeometry = screens[targetScreen]->geometry();
        int X = windowGeometry.x() - screenGeometry.x() + targetScreenGeometry.x();
        int Y = windowGeometry.y() - screenGeometry.y() + targetScreenGeometry.y();
        NET::States state = KWindowInfo(windowId, NET::WMState).state();

        // NW geometry | y/x | from panel
        const int flags = 1 | (0b011 << 8) | (0b010 << 12);
        KX11Extras::clearState(windowId, NET::MaxHoriz | NET::MaxVert | NET::Max | NET::FullScreen);
        NETRootInfo(m_xcbConnection, NET::Properties(), NET::WM2MoveResizeWindow)
            .moveResizeWindowRequest(windowId, flags, X, Y, 0, 0);
        QTimer::singleShot(200, this, [this, windowId, state, raiseOnCurrentDesktop] {
          KX11Extras::setState(windowId, state);
          raiseWindow(windowId, raiseOnCurrentDesktop);
        });
        break;
      }
    }
  }
}

int OneG4WMBackendX11::onAllWorkspacesEnum() const {
  return NET::OnAllDesktops;
}

bool OneG4WMBackendX11::isWindowOnScreen(QScreen* screen, WId windowId) const {
  // if no screen is provided assume true
  if (!screen)
    return true;

  QRect r = KWindowInfo(windowId, NET::WMFrameExtents).frameGeometry();
  return screen->geometry().intersects(r);
}

bool OneG4WMBackendX11::setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) {
  NETRootInfo mDesktops(m_xcbConnection, NET::NumberOfDesktops | NET::CurrentDesktop | NET::DesktopNames,
                        NET::WM2DesktopLayout);

  if (orientation == Qt::Horizontal) {
    mDesktops.setDesktopLayout(NET::OrientationHorizontal, columns, rows,
                               rightToLeft ? NET::DesktopLayoutCornerTopRight : NET::DesktopLayoutCornerTopLeft);
  }
  else {
    mDesktops.setDesktopLayout(NET::OrientationHorizontal, rows, columns,
                               rightToLeft ? NET::DesktopLayoutCornerTopRight : NET::DesktopLayoutCornerTopLeft);
  }
  return true;
}

/************************************************
 *   X11 Specific
 ************************************************/
void OneG4WMBackendX11::moveApplication(WId windowId) {
  KWindowInfo info(windowId, NET::WMDesktop);
  if (!info.isOnCurrentDesktop())
    KX11Extras::setCurrentDesktop(info.desktop());

  if (getWindowState(windowId) == OneG4TaskBarWindowState::Minimized)
    KX11Extras::unminimizeWindow(windowId);

  KX11Extras::forceActiveWindow(windowId);

  const QRect& g = KWindowInfo(windowId, NET::WMGeometry).geometry();
  int X = g.center().x();
  int Y = g.center().y();
  QCursor::setPos(X, Y);
  NETRootInfo(m_xcbConnection, NET::WMMoveResize).moveResizeRequest(windowId, X, Y, NET::Move);
}

void OneG4WMBackendX11::resizeApplication(WId windowId) {
  KWindowInfo info(windowId, NET::WMDesktop);
  if (!info.isOnCurrentDesktop())
    KX11Extras::setCurrentDesktop(info.desktop());

  if (getWindowState(windowId) == OneG4TaskBarWindowState::Minimized)
    KX11Extras::unminimizeWindow(windowId);

  KX11Extras::forceActiveWindow(windowId);

  const QRect& g = KWindowInfo(windowId, NET::WMGeometry).geometry();
  int X = g.bottomRight().x();
  int Y = g.bottomRight().y();
  QCursor::setPos(X, Y);
  NETRootInfo(m_xcbConnection, NET::WMMoveResize).moveResizeRequest(windowId, X, Y, NET::BottomRight);
}

void OneG4WMBackendX11::refreshIconGeometry(WId windowId, QRect const& geom) {
  // announce where the task icon is so X11 WMs can perform animations correctly

  const qreal scaleFactor = qApp->devicePixelRatio();
  const QRect scaledGeom(qRound(geom.x() * scaleFactor), qRound(geom.y() * scaleFactor),
                         qRound(geom.width() * scaleFactor), qRound(geom.height() * scaleFactor));

  const auto cached = m_iconGeometries.value(windowId);
  if (cached == scaledGeom)
    return;

  m_iconGeometries.insert(windowId, scaledGeom);

  WId appRootWindow = XDefaultRootWindow(m_X11Display);
  NETWinInfo info(m_xcbConnection, windowId, appRootWindow, NET::Properties(), NET::Properties2());

  NETRect nrect;
  nrect.pos.x = geom.x();
  nrect.pos.y = geom.y();
  nrect.size.height = geom.height();
  nrect.size.width = geom.width();
  info.setIconGeometry(nrect);
}

bool OneG4WMBackendX11::isAreaOverlapped(const QRect& area) const {
  // TODO: reuse our m_windows cache
  QFlags<NET::WindowTypeMask> ignoreList;
  ignoreList |= NET::DesktopMask;
  ignoreList |= NET::DockMask;
  ignoreList |= NET::SplashMask;
  ignoreList |= NET::MenuMask;
  ignoreList |= NET::PopupMenuMask;
  ignoreList |= NET::DropdownMenuMask;
  ignoreList |= NET::TopMenuMask;
  ignoreList |= NET::NotificationMask;

  const auto wIds = KX11Extras::stackingOrder();
  for (auto const wId : wIds) {
    KWindowInfo info(wId, NET::WMWindowType | NET::WMState | NET::WMFrameExtents | NET::WMDesktop);
    if (info.valid()
        // skip windows that are on other desktops
        && info.isOnCurrentDesktop()
        // skip shaded, minimized or hidden windows
        && !(info.state() & (NET::Shaded | NET::Hidden))
        // check against the list of ignored types
        && !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList)) {
      if (info.frameGeometry().intersects(area))
        return true;
    }
  }
  return false;
}

bool OneG4WMBackendX11::isShowingDesktop() const {
  return KWindowSystem::showingDesktop();
}

bool OneG4WMBackendX11::showDesktop(bool value) {
  KWindowSystem::setShowingDesktop(value);
  return true;
}

int OneG4WMBackendX11Library::getBackendScore(const QString& key) const {
  Q_UNUSED(key)

  auto* x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
  if (!x11Application)
    return 0;

  // generic X11 backend
  return 80;
}

IOneG4AbstractWMInterface* OneG4WMBackendX11Library::instance() const {
  return new OneG4WMBackendX11;
}
