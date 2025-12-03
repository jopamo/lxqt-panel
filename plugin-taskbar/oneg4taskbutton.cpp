/* plugin-taskbar/oneg4taskbutton.cpp
 * Taskbar plugin implementation
 */

#include "oneg4taskbutton.h"
#include "oneg4taskbar.h"

#include "../panel/ioneg4panelplugin.h"

#include <OneG4/Settings.h>

#include <QDebug>
#include <XdgIcon.h>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QGuiApplication>
#include <QDragEnterEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QScreen>
#include <QPointer>

#include "../panel/backends/ioneg4abstractwmiface.h"

bool OneG4TaskButton::sDraggging = false;

/************************************************

************************************************/
void LeftAlignedTextStyle::drawItemText(QPainter* painter,
                                        const QRect& rect,
                                        int flags,
                                        const QPalette& pal,
                                        bool enabled,
                                        const QString& text,
                                        QPalette::ColorRole textRole) const {
  QString txt = text;
  // get the button text because the text that's given to this function may be middle-elided
  if (const QToolButton* tb = dynamic_cast<const QToolButton*>(painter->device()))
    txt = tb->text();
  txt = QFontMetrics(painter->font()).elidedText(txt, Qt::ElideRight, rect.width());
  QProxyStyle::drawItemText(painter, rect, (flags & ~Qt::AlignHCenter) | Qt::AlignLeft, pal, enabled, txt, textRole);
}

/************************************************

************************************************/
OneG4TaskButton::OneG4TaskButton(const WId window, OneG4TaskBar* taskbar, QWidget* parent)
    : QToolButton(parent),
      mBackend(taskbar->getBackend()),
      mWindow(window),
      mUrgencyHint(false),
      mOrigin(Qt::TopLeftCorner),
      mParentTaskBar(taskbar),
      mPlugin(mParentTaskBar->plugin()),
      mIconSize(mPlugin->panel()->iconSize()),
      mWheelDelta(0),
      mDNDTimer(new QTimer(this)),
      mWheelTimer(new QTimer(this)) {
  Q_ASSERT(taskbar);

  setCheckable(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setMinimumWidth(1);
  setMinimumHeight(1);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setAcceptDrops(true);

  updateText();
  updateIcon();

  mDNDTimer->setSingleShot(true);
  mDNDTimer->setInterval(700);
  connect(mDNDTimer, &QTimer::timeout, this, &OneG4TaskButton::raiseApplication);

  mWheelTimer->setSingleShot(true);
  mWheelTimer->setInterval(250);
  connect(mWheelTimer, &QTimer::timeout, this, [this] {
    mWheelDelta = 0;  // forget previous wheel deltas
  });

  setUrgencyHint(mBackend->applicationDemandsAttention(mWindow));

  connect(OneG4::Settings::globalSettings(), &OneG4::GlobalSettings::iconThemeChanged, this,
          &OneG4TaskButton::updateIcon);
  connect(mParentTaskBar, &OneG4TaskBar::iconByClassChanged, this, &OneG4TaskButton::updateIcon);
}

/************************************************

************************************************/
OneG4TaskButton::~OneG4TaskButton() = default;

/************************************************

 ************************************************/
void OneG4TaskButton::updateText() {
  QString title = mBackend->getWindowTitle(mWindow);
  setTextExplicitly(title.replace(QStringLiteral("&"), QStringLiteral("&&")));
  setToolTip(title);
}

/************************************************

 ************************************************/
void OneG4TaskButton::updateIcon() {
  QIcon ico;
  if (mParentTaskBar->isIconByClass()) {
    ico = XdgIcon::fromTheme(mBackend->getWindowClass(mWindow).toLower());
  }
  if (ico.isNull()) {
    int devicePixels = mIconSize * devicePixelRatioF();
    ico = mBackend->getApplicationIcon(mWindow, devicePixels);
  }
  setIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
}

/************************************************

 ************************************************/
void OneG4TaskButton::changeEvent(QEvent* event) {
  if (event->type() == QEvent::StyleChange) {
    // When the icon size changes, the panel doesn't emit any specific
    // signal, but it triggers a stylesheet update, which we can detect
    int newIconSize = mPlugin->panel()->iconSize();
    if (newIconSize != mIconSize) {
      mIconSize = newIconSize;
      updateIcon();
    }
  }

  QToolButton::changeEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskButton::dragEnterEvent(QDragEnterEvent* event) {
  // It must be here otherwise dragLeaveEvent and dragMoveEvent won't be called
  // on the other hand drop and dragmove events of parent widget won't be called
  event->acceptProposedAction();
  if (event->mimeData()->hasFormat(mimeDataFormat())) {
    emit dragging(event->source(), event->position().toPoint());
    setAttribute(Qt::WA_UnderMouse, false);
  }
  else {
    mDNDTimer->start();
  }

  QToolButton::dragEnterEvent(event);
}

void OneG4TaskButton::dragMoveEvent(QDragMoveEvent* event) {
  if (event->mimeData()->hasFormat(mimeDataFormat())) {
    emit dragging(event->source(), event->position().toPoint());
    setAttribute(Qt::WA_UnderMouse, false);
  }
}

void OneG4TaskButton::dragLeaveEvent(QDragLeaveEvent* event) {
  mDNDTimer->stop();
  QToolButton::dragLeaveEvent(event);
}

void OneG4TaskButton::dropEvent(QDropEvent* event) {
  mDNDTimer->stop();
  if (event->mimeData()->hasFormat(mimeDataFormat())) {
    emit dropped(event->source(), event->position().toPoint());
    setAttribute(Qt::WA_UnderMouse, false);
  }
  QToolButton::dropEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskButton::mousePressEvent(QMouseEvent* event) {
  const Qt::MouseButton b = event->button();

  if (Qt::LeftButton == b)
    mDragStartPosition = event->pos();
  else if (Qt::MiddleButton == b && parentTaskBar()->closeOnMiddleClick())
    closeApplication();

  QToolButton::mousePressEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskButton::mouseReleaseEvent(QMouseEvent* event) {
  QToolButton::mouseReleaseEvent(event);
  if (!sDraggging && event->button() == Qt::LeftButton) {
    if (!isChecked())
      minimizeApplication();
    else
      raiseApplication();
  }
}

/************************************************

 ************************************************/
void OneG4TaskButton::wheelEvent(QWheelEvent* event) {
  // ignore wheel event if it is not "raise", "minimize" or "move" window
  if (mParentTaskBar->wheelEventsAction() < 2 || mParentTaskBar->wheelEventsAction() > 5)
    return QToolButton::wheelEvent(event);

  QPoint angleDelta = event->angleDelta();
  Qt::Orientation orient = (qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
  int delta = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());

  if (!mWheelTimer->isActive())
    mWheelDelta += qAbs(delta);
  else {
    // NOTE: We should consider a short delay after the last wheel event
    // in order to distinguish between separate wheel rotations; otherwise,
    // a wheel delta threshold will not make much sense because the delta
    // might have been increased due to a previous and separate wheel rotation.
    mWheelTimer->start();
  }

  if (mWheelDelta < mParentTaskBar->wheelDeltaThreshold())
    return QToolButton::wheelEvent(event);
  else {
    mWheelDelta = 0;
    mWheelTimer->start();  // start to distinguish between separate wheel rotations
  }

  int D = delta < 0 ? 1 : -1;

  if (mParentTaskBar->wheelEventsAction() == 4) {
    moveApplicationToPrevNextDesktop(D < 0);
  }
  else if (mParentTaskBar->wheelEventsAction() == 5) {
    moveApplicationToPrevNextDesktop(D > 0);
  }
  else {
    if (mParentTaskBar->wheelEventsAction() == 3)
      D *= -1;
    if (D < 0)
      raiseApplication();
    else if (D > 0)
      minimizeApplication();
  }

  QToolButton::wheelEvent(event);
}

/************************************************

 ************************************************/
QMimeData* OneG4TaskButton::mimeData() {
  auto* mimedata = new QMimeData;
  QByteArray ba;
  QDataStream stream(&ba, QIODevice::WriteOnly);
  stream << static_cast<qlonglong>(mWindow);
  mimedata->setData(mimeDataFormat(), ba);
  return mimedata;
}

/*!
 * \brief OneG4TaskButton::setTextExplicitly
 * \param str
 *
 * This is needed to workaround flickering caused by KAcceleratorManager
 * This class is hooked by KDE Integration and adds accelerators to button text
 * (Adds some '&' characters)
 * This triggers widget update but soon after text is reset to original value
 * This triggers a KAcceleratorManager update which again adds accelerator
 * This happens in loop
 *
 * TODO: investigate proper solution
 */
void OneG4TaskButton::setTextExplicitly(const QString& str) {
  if (str == mExplicitlySetText) {
    return;
  }

  mExplicitlySetText = str;
  setText(mExplicitlySetText);
}

/************************************************

 ************************************************/
void OneG4TaskButton::mouseMoveEvent(QMouseEvent* event) {
  QAbstractButton::mouseMoveEvent(event);
  if (!(event->buttons() & Qt::LeftButton))
    return;

  if ((event->position().toPoint() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
    return;

  QPointer<QDrag> drag = new QDrag(this);
  drag->setMimeData(mimeData());
  QIcon ico = icon();
  QPixmap img = ico.pixmap(ico.actualSize({32, 32}));
  drag->setPixmap(img);
  switch (parentTaskBar()->panel()->position()) {
    case IOneG4Panel::PositionLeft:
    case IOneG4Panel::PositionTop:
      drag->setHotSpot({0, 0});
      break;
    case IOneG4Panel::PositionRight:
    case IOneG4Panel::PositionBottom:
      drag->setHotSpot(img.rect().bottomRight());
      break;
  }

  sDraggging = true;
  drag->exec();

  // if button is dropped out of panel (e.g. on desktop)
  // it is not deleted automatically by Qt
  if (drag)
    drag->deleteLater();

  // release mouse appropriately, by positioning the event outside
  // the button rectangle (otherwise, the button will be toggled)
  QMouseEvent releasingEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), mapToGlobal(QPoint(-1, -1)), Qt::LeftButton,
                             Qt::NoButton, Qt::NoModifier);
  mouseReleaseEvent(&releasingEvent);

  sDraggging = false;
}

/************************************************

 ************************************************/
bool OneG4TaskButton::isApplicationHidden() const {
  return false;  // FIXME: unused
}

/************************************************

 ************************************************/
bool OneG4TaskButton::isApplicationActive() const {
  return mBackend->isWindowActive(mWindow);
}

/************************************************

 ************************************************/
void OneG4TaskButton::raiseApplication() {
  mBackend->raiseWindow(mWindow, parentTaskBar()->raiseOnCurrentDesktop());
}

/************************************************

 ************************************************/
void OneG4TaskButton::minimizeApplication() {
  mBackend->setWindowState(mWindow, OneG4TaskBarWindowState::Minimized, true);
}

/************************************************

 ************************************************/
void OneG4TaskButton::maximizeApplication() {
  QAction* act = qobject_cast<QAction*>(sender());
  if (!act)
    return;

  int state = act->data().toInt();
  mBackend->setWindowState(mWindow, OneG4TaskBarWindowState(state), true);

  if (!mBackend->isWindowActive(mWindow))
    mBackend->raiseWindow(mWindow, parentTaskBar()->raiseOnCurrentDesktop());
}

/************************************************

 ************************************************/
void OneG4TaskButton::deMaximizeApplication() {
  mBackend->setWindowState(mWindow, OneG4TaskBarWindowState::Maximized, false);

  if (!mBackend->isWindowActive(mWindow))
    mBackend->raiseWindow(mWindow, parentTaskBar()->raiseOnCurrentDesktop());
}

/************************************************

 ************************************************/
void OneG4TaskButton::shadeApplication() {
  mBackend->setWindowState(mWindow, OneG4TaskBarWindowState::RolledUp, true);
}

/************************************************

 ************************************************/
void OneG4TaskButton::unShadeApplication() {
  mBackend->setWindowState(mWindow, OneG4TaskBarWindowState::RolledUp, false);
}

/************************************************

 ************************************************/
void OneG4TaskButton::closeApplication() {
  mBackend->closeWindow(mWindow);
}

/************************************************

 ************************************************/
void OneG4TaskButton::setApplicationLayer() {
  QAction* act = qobject_cast<QAction*>(sender());
  if (!act)
    return;

  int layer = act->data().toInt();
  mBackend->setWindowLayer(mWindow, OneG4TaskBarWindowLayer(layer));
}

/************************************************

 ************************************************/
void OneG4TaskButton::moveApplicationToDesktop() {
  QAction* act = qobject_cast<QAction*>(sender());
  if (!act)
    return;

  bool ok;
  int idx = act->data().toInt(&ok);

  if (!ok)
    return;

  mBackend->setWindowOnWorkspace(mWindow, idx);
}

/************************************************

 ************************************************/
void OneG4TaskButton::moveApplicationToPrevNextDesktop(bool next) {
  mBackend->moveApplicationToPrevNextDesktop(mWindow, next);
}

/************************************************

 ************************************************/
void OneG4TaskButton::moveApplicationToPrevNextMonitor(bool next) {
  mBackend->moveApplicationToPrevNextMonitor(mWindow, next, parentTaskBar()->raiseOnCurrentDesktop());
}

/************************************************

 ************************************************/
void OneG4TaskButton::moveApplication() {
  mBackend->moveApplication(mWindow);
}

/************************************************

 ************************************************/
void OneG4TaskButton::resizeApplication() {
  mBackend->resizeApplication(mWindow);
}

/************************************************

 ************************************************/
void OneG4TaskButton::contextMenuEvent(QContextMenuEvent* event) {
  if (event->modifiers().testFlag(Qt::ControlModifier)) {
    event->ignore();
    return;
  }

  const OneG4TaskBarWindowState state = mBackend->getWindowState(mWindow);

  QMenu* menu = new QMenu(tr("Application"), this);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction* a;

  /* KDE menu *******

    + To &Desktop >
    +     &All Desktops
    +     ---
    +     &1 Desktop 1
    +     &2 Desktop 2
    + &To Current Desktop
      &Move
      Re&size
    + Mi&nimize
    + Ma&ximize
    + &Shade
      Ad&vanced >
          Keep &Above Others
          Keep &Below Others
          Fill screen
      &Layer >
          Always on &top
          &Normal
          Always on &bottom
    ---
    + &Close
  */

  /********** Desktop menu **********/
  int deskNum = mBackend->getWorkspacesCount();
  if (deskNum > 1) {
    int winDesk = mBackend->getWindowWorkspace(mWindow);
    QMenu* deskMenu = menu->addMenu(tr("To &Desktop"));
    deskMenu->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::MoveToDesktop));

    a = deskMenu->addAction(tr("&All Desktops"));
    a->setData(mBackend->onAllWorkspacesEnum());
    a->setEnabled(winDesk != mBackend->onAllWorkspacesEnum());
    connect(a, &QAction::triggered, this, &OneG4TaskButton::moveApplicationToDesktop);
    deskMenu->addSeparator();

    for (int i = 1; i <= deskNum; ++i) {
      auto deskName = mBackend->getWorkspaceName(i).trimmed();
      if (deskName.isEmpty())
        a = deskMenu->addAction(tr("Desktop &%1").arg(i));
      else
        a = deskMenu->addAction(QStringLiteral("&%1: %2").arg(i).arg(deskName));

      a->setData(i);
      a->setEnabled(i != winDesk);
      connect(a, &QAction::triggered, this, &OneG4TaskButton::moveApplicationToDesktop);
    }

    int curDesk = mBackend->getCurrentWorkspace();
    a = menu->addAction(tr("&To Current Desktop"));
    a->setData(curDesk);
    a->setEnabled(curDesk != winDesk);
    connect(a, &QAction::triggered, this, &OneG4TaskButton::moveApplicationToDesktop);
  }
  /********** Move/Resize **********/
  if (QGuiApplication::screens().size() > 1) {
    bool enable(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::MoveToOutput) &&
                (state != OneG4TaskBarWindowState::FullScreen ||
                 mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::FullScreen)));
    menu->addSeparator();
    a = menu->addAction(tr("Move To N&ext Monitor"));
    connect(a, &QAction::triggered, this, [this] { moveApplicationToPrevNextMonitor(true); });
    a->setEnabled(enable);
    a = menu->addAction(tr("Move To &Previous Monitor"));
    connect(a, &QAction::triggered, this, [this] { moveApplicationToPrevNextMonitor(false); });
    a->setEnabled(enable);
  }

  menu->addSeparator();
  a = menu->addAction(tr("&Move"));
  a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::Move) &&
                state != OneG4TaskBarWindowState::Maximized && state != OneG4TaskBarWindowState::FullScreen);
  connect(a, &QAction::triggered, this, &OneG4TaskButton::moveApplication);
  a = menu->addAction(tr("Resi&ze"));
  a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::Resize) &&
                state != OneG4TaskBarWindowState::Maximized && state != OneG4TaskBarWindowState::FullScreen);
  connect(a, &QAction::triggered, this, &OneG4TaskButton::resizeApplication);

  /********** State menu **********/
  menu->addSeparator();

  a = menu->addAction(tr("Ma&ximize"));
  a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::Maximize) &&
                state != OneG4TaskBarWindowState::Maximized && state != OneG4TaskBarWindowState::Hidden);
  a->setData(int(OneG4TaskBarWindowState::Maximized));
  connect(a, &QAction::triggered, this, &OneG4TaskButton::maximizeApplication);

  if (event->modifiers() & Qt::ShiftModifier) {
    a = menu->addAction(tr("Maximize vertically"));
    a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::MaximizeVertically) &&
                  state != OneG4TaskBarWindowState::MaximizedVertically && state != OneG4TaskBarWindowState::Hidden);
    a->setData(int(OneG4TaskBarWindowState::MaximizedVertically));
    connect(a, &QAction::triggered, this, &OneG4TaskButton::maximizeApplication);

    a = menu->addAction(tr("Maximize horizontally"));
    a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::MaximizeHorizontally) &&
                  state != OneG4TaskBarWindowState::MaximizedHorizontally && state != OneG4TaskBarWindowState::Hidden);
    a->setData(int(OneG4TaskBarWindowState::MaximizedHorizontally));
    connect(a, &QAction::triggered, this, &OneG4TaskButton::maximizeApplication);
  }

  a = menu->addAction(tr("&Restore"));
  a->setEnabled(state == OneG4TaskBarWindowState::Hidden || state == OneG4TaskBarWindowState::Minimized ||
                state == OneG4TaskBarWindowState::Maximized || state == OneG4TaskBarWindowState::MaximizedVertically ||
                state == OneG4TaskBarWindowState::MaximizedHorizontally);
  connect(a, &QAction::triggered, this, &OneG4TaskButton::deMaximizeApplication);

  a = menu->addAction(tr("Mi&nimize"));
  a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::Minimize) &&
                state != OneG4TaskBarWindowState::Hidden && state != OneG4TaskBarWindowState::Minimized);
  connect(a, &QAction::triggered, this, &OneG4TaskButton::minimizeApplication);

  if (state == OneG4TaskBarWindowState::RolledUp) {
    a = menu->addAction(tr("Roll down"));
    a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::RollUp) &&
                  state != OneG4TaskBarWindowState::Hidden && state != OneG4TaskBarWindowState::Minimized);
    connect(a, &QAction::triggered, this, &OneG4TaskButton::unShadeApplication);
  }
  else {
    a = menu->addAction(tr("Roll up"));
    a->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::RollUp) &&
                  state != OneG4TaskBarWindowState::Hidden);
    connect(a, &QAction::triggered, this, &OneG4TaskButton::shadeApplication);
  }

  /********** Layer menu **********/
  menu->addSeparator();

  QMenu* layerMenu = menu->addMenu(tr("&Layer"));
  layerMenu->setEnabled(mBackend->supportsAction(mWindow, OneG4TaskBarBackendAction::MoveToLayer));

  OneG4TaskBarWindowLayer currentLayer = mBackend->getWindowLayer(mWindow);

  a = layerMenu->addAction(tr("Always on &top"));
  a->setEnabled(currentLayer != OneG4TaskBarWindowLayer::KeepAbove);
  a->setData(int(OneG4TaskBarWindowLayer::KeepAbove));
  connect(a, &QAction::triggered, this, &OneG4TaskButton::setApplicationLayer);

  a = layerMenu->addAction(tr("&Normal"));
  a->setEnabled(currentLayer != OneG4TaskBarWindowLayer::Normal);
  a->setData(int(OneG4TaskBarWindowLayer::Normal));
  connect(a, &QAction::triggered, this, &OneG4TaskButton::setApplicationLayer);

  a = layerMenu->addAction(tr("Always on &bottom"));
  a->setEnabled(currentLayer != OneG4TaskBarWindowLayer::KeepBelow);
  a->setData(int(OneG4TaskBarWindowLayer::KeepBelow));
  connect(a, &QAction::triggered, this, &OneG4TaskButton::setApplicationLayer);

  /********** Kill menu **********/
  menu->addSeparator();
  a = menu->addAction(XdgIcon::fromTheme(QStringLiteral("process-stop")), tr("&Close"));
  connect(a, &QAction::triggered, this, &OneG4TaskButton::closeApplication);

  menu->setGeometry(mParentTaskBar->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
  mPlugin->willShowWindow(menu);
  menu->show();
}

/************************************************

 ************************************************/
void OneG4TaskButton::setUrgencyHint(bool set) {
  if (mUrgencyHint == set)
    return;

  mUrgencyHint = set;
  setProperty("urgent", set);
  style()->unpolish(this);
  style()->polish(this);
  update();
}

/************************************************

 ************************************************/
bool OneG4TaskButton::isOnDesktop(int desktop) const {
  int d = mBackend->getWindowWorkspace(mWindow);
  return d == desktop || d == mBackend->onAllWorkspacesEnum();
}

bool OneG4TaskButton::isOnCurrentScreen() const {
  QScreen* screen = parentTaskBar()->screen();
  return mBackend->isWindowOnScreen(screen, mWindow);
}

bool OneG4TaskButton::isMinimized() const {
  return mBackend->getWindowState(mWindow) == OneG4TaskBarWindowState::Minimized;
}

Qt::Corner OneG4TaskButton::origin() const {
  return mOrigin;
}

void OneG4TaskButton::setOrigin(Qt::Corner newOrigin) {
  if (mOrigin != newOrigin) {
    mOrigin = newOrigin;
    update();
  }
}

void OneG4TaskButton::setAutoRotation(bool value, IOneG4Panel::Position position) {
  if (value) {
    switch (position) {
      case IOneG4Panel::PositionTop:
      case IOneG4Panel::PositionBottom:
        setOrigin(Qt::TopLeftCorner);
        break;

      case IOneG4Panel::PositionLeft:
        setOrigin(Qt::BottomLeftCorner);
        break;

      case IOneG4Panel::PositionRight:
        setOrigin(Qt::TopRightCorner);
        break;
    }
  }
  else
    setOrigin(Qt::TopLeftCorner);
}

void OneG4TaskButton::paintEvent(QPaintEvent* event) {
  if (mOrigin == Qt::TopLeftCorner) {
    QToolButton::paintEvent(event);
    return;
  }

  QSize sz = size();
  bool transpose = false;
  QTransform transform;

  switch (mOrigin) {
    case Qt::TopLeftCorner:
      break;

    case Qt::TopRightCorner:
      transform.rotate(90.0);
      transform.translate(0.0, -sz.width());
      transpose = true;
      break;

    case Qt::BottomRightCorner:
      transform.rotate(180.0);
      transform.translate(-sz.width(), -sz.height());
      break;

    case Qt::BottomLeftCorner:
      transform.rotate(270.0);
      transform.translate(-sz.height(), 0.0);
      transpose = true;
      break;
  }

  QStylePainter painter(this);
  painter.setTransform(transform);
  QStyleOptionToolButton opt;
  initStyleOption(&opt);
  if (transpose)
    opt.rect = opt.rect.transposed();
  painter.drawComplexControl(QStyle::CC_ToolButton, opt);
}

bool OneG4TaskButton::hasDragAndDropHover() const {
  return mDNDTimer->isActive();
}
