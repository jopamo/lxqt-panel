/* plugin-taskbar/lxqttaskgroup.cpp
 * Taskbar plugin implementation
 */

#include "lxqttaskgroup.h"
#include "lxqttaskbar.h"
#include "lxqtgrouppopup.h"

#include "../panel/ilxqtpanelplugin.h"

#include <QDebug>
#include <QMimeData>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QStringBuilder>
#include <QMenu>
#include <QEnterEvent>
#include <XdgIcon>

#include "../panel/backends/ilxqtabstractwmiface.h"

/************************************************

 ************************************************/
LXQtTaskGroup::LXQtTaskGroup(const QString& groupName, WId window, LXQtTaskBar* parent)
    : LXQtTaskButton(window, parent, parent),
      mGroupName(groupName),
      mPopup(new LXQtGroupPopup(this)),
      mPreventPopup(false),
      mSingleButton(true) {
  Q_ASSERT(parent);

  setObjectName(groupName);
  setTextExplicitly(groupName);

  connect(this, &LXQtTaskGroup::clicked, this, &LXQtTaskGroup::onClicked);
  connect(parent, &LXQtTaskBar::buttonRotationRefreshed, this, &LXQtTaskGroup::setAutoRotation);
  connect(parent, &LXQtTaskBar::refreshIconGeometry, this, &LXQtTaskGroup::refreshIconsGeometry);
  connect(parent, &LXQtTaskBar::buttonStyleRefreshed, this, &LXQtTaskGroup::setToolButtonsStyle);
  connect(parent, &LXQtTaskBar::showOnlySettingChanged, this, &LXQtTaskGroup::refreshVisibility);
  connect(parent, &LXQtTaskBar::popupShown, this, &LXQtTaskGroup::groupPopupShown);
  connect(mBackend, &ILXQtAbstractWMInterface::currentWorkspaceChanged, this, &LXQtTaskGroup::onDesktopChanged);
  connect(mBackend, &ILXQtAbstractWMInterface::activeWindowChanged, this, &LXQtTaskGroup::onActiveWindowChanged);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::contextMenuEvent(QContextMenuEvent* event) {
  setPopupVisible(false, true);
  if (mSingleButton) {
    LXQtTaskButton::contextMenuEvent(event);
    return;
  }
  mPreventPopup = true;
  QMenu* menu = new QMenu(tr("Group"));
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction* a = menu->addAction(XdgIcon::fromTheme(QStringLiteral("process-stop")), tr("Close group"));
  connect(a, &QAction::triggered, this, &LXQtTaskGroup::closeGroup);
  connect(menu, &QMenu::aboutToHide, this, [this] { mPreventPopup = false; });
  menu->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
  plugin()->willShowWindow(menu);
  menu->show();
}

/************************************************

 ************************************************/
void LXQtTaskGroup::closeGroup() {
  for (LXQtTaskButton* button : std::as_const(mButtonHash)) {
    if (button->isOnDesktop(mBackend->getCurrentWorkspace()))
      button->closeApplication();
  }
}

/************************************************

 ************************************************/
LXQtTaskButton* LXQtTaskGroup::addWindow(WId id) {
  if (mButtonHash.contains(id))
    return mButtonHash.value(id);

  LXQtTaskButton* btn = new LXQtTaskButton(id, parentTaskBar(), mPopup);
  btn->setToolButtonStyle(popupButtonStyle());

  if (btn->isApplicationActive()) {
    btn->setChecked(true);
    setChecked(true);
  }

  mButtonHash.insert(id, btn);
  mPopup->addButton(btn);

  connect(btn, &LXQtTaskButton::clicked, this, &LXQtTaskGroup::onChildButtonClicked);
  refreshVisibility();

  return btn;
}

/************************************************

 ************************************************/
LXQtTaskButton* LXQtTaskGroup::checkedButton() const {
  for (LXQtTaskButton* button : std::as_const(mButtonHash)) {
    if (button->isChecked())
      return button;
  }

  return nullptr;
}

/************************************************

 ************************************************/
LXQtTaskButton* LXQtTaskGroup::getNextPrevChildButton(bool next, bool circular) {
  LXQtTaskButton* button = checkedButton();
  int idx = mPopup->indexOf(button);
  const int inc = next ? 1 : -1;
  idx += inc;

  // if there is no checked button, get the first one if next equals true
  // or the last one if not
  if (!button) {
    idx = -1;
    if (next) {
      for (int i = 0; i < mPopup->count() && idx == -1; ++i) {
        if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
          idx = i;
      }
    }
    else {
      for (int i = mPopup->count() - 1; i >= 0 && idx == -1; --i) {
        if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
          idx = i;
      }
    }
  }

  if (circular)
    idx = (idx + mButtonHash.count()) % mButtonHash.count();
  else if (mPopup->count() <= idx || idx < 0)
    return nullptr;

  // return the next or the previous child
  QLayoutItem* item = mPopup->itemAt(idx);
  if (item) {
    button = qobject_cast<LXQtTaskButton*>(item->widget());
    if (button->isVisibleTo(mPopup))
      return button;
  }

  return nullptr;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::onActiveWindowChanged(WId window) {
  LXQtTaskButton* button = mButtonHash.value(window, nullptr);
  for (LXQtTaskButton* btn : std::as_const(mButtonHash))
    btn->setChecked(false);

  if (button) {
    button->setChecked(true);
    if (button->hasUrgencyHint())
      button->setUrgencyHint(false);
  }
  setChecked(button != nullptr && button->isVisibleTo(mPopup));
}

/************************************************

 ************************************************/
void LXQtTaskGroup::onDesktopChanged(int /*number*/) {
  refreshVisibility();
}

/************************************************

 ************************************************/
void LXQtTaskGroup::onWindowRemoved(WId window) {
  if (mButtonHash.contains(window)) {
    LXQtTaskButton* button = mButtonHash.value(window);
    mButtonHash.remove(window);
    mPopup->removeWidget(button);
    button->deleteLater();

    if (mButtonHash.count())
      regroup();
    else {
      if (isVisible())
        emit visibilityChanged(false);
      hide();
      emit groupBecomeEmpty(groupName());
    }
  }
}

/************************************************

 ************************************************/
void LXQtTaskGroup::onChildButtonClicked() {
  setPopupVisible(false, true);
}

/************************************************

 ************************************************/
Qt::ToolButtonStyle LXQtTaskGroup::popupButtonStyle() const {
  // do not set icons-only style in the buttons in the group,
  // as they'll be indistinguishable
  const Qt::ToolButtonStyle style = toolButtonStyle();
  return style == Qt::ToolButtonIconOnly ? Qt::ToolButtonTextBesideIcon : style;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::setToolButtonsStyle(Qt::ToolButtonStyle style) {
  setToolButtonStyle(style);

  const Qt::ToolButtonStyle styleInPopup = popupButtonStyle();
  for (auto button : mButtonHash) {
    button->setToolButtonStyle(styleInPopup);
  }
}

/************************************************

 ************************************************/
int LXQtTaskGroup::buttonsCount() const {
  return mButtonHash.count();
}

/************************************************

 ************************************************/
int LXQtTaskGroup::visibleButtonsCount() const {
  int i = 0;
  for (LXQtTaskButton* btn : std::as_const(mButtonHash)) {
    if (btn->isVisibleTo(mPopup))
      ++i;
  }
  return i;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::draggingTimerTimeout() {
  if (mSingleButton)
    setPopupVisible(false);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::onClicked(bool) {
  if (visibleButtonsCount() > 1) {
    setChecked(mButtonHash.contains(mBackend->getActiveWindow()));
    setPopupVisible(true);
  }
}

/************************************************

 ************************************************/
void LXQtTaskGroup::regroup() {
  const int cont = visibleButtonsCount();
  recalculateFrameIfVisible();

  if (cont == 1) {
    mSingleButton = true;
    // Get first visible button
    LXQtTaskButton* button = nullptr;
    for (LXQtTaskButton* btn : std::as_const(mButtonHash)) {
      if (btn->isVisibleTo(mPopup)) {
        button = btn;
        break;
      }
    }

    if (button) {
      setTextExplicitly(button->text());
      setToolTip(button->toolTip());
      setWindowId(button->windowId());
    }
  }
  else if (cont == 0) {
    hide();
  }
  else {
    mSingleButton = false;
    const QString t = QStringLiteral("%1 - %2 windows").arg(mGroupName).arg(cont);
    setTextExplicitly(t);
    setToolTip(parentTaskBar()->isShowGroupOnHover() ? QString() : t);
  }
}

/************************************************

 ************************************************/
void LXQtTaskGroup::recalculateFrameIfVisible() {
  if (mPopup->isVisible()) {
    recalculateFrameSize();
    if (plugin()->panel()->position() == ILXQtPanel::PositionBottom)
      recalculateFramePosition();
  }
}

/************************************************

 ************************************************/
void LXQtTaskGroup::setAutoRotation(bool value, ILXQtPanel::Position position) {
  for (LXQtTaskButton* button : std::as_const(mButtonHash))
    button->setAutoRotation(false, position);

  LXQtTaskButton::setAutoRotation(value, position);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::refreshVisibility() {
  bool will = false;
  const LXQtTaskBar* taskbar = parentTaskBar();
  const int showDesktop = taskbar->showDesktopNum();
  for (LXQtTaskButton* btn : std::as_const(mButtonHash)) {
    bool visible = taskbar->isShowOnlyOneDesktopTasks()
                       ? btn->isOnDesktop(showDesktop == 0 ? mBackend->getCurrentWorkspace() : showDesktop)
                       : true;
    visible &= taskbar->isShowOnlyCurrentScreenTasks() ? btn->isOnCurrentScreen() : true;
    visible &= taskbar->isShowOnlyMinimizedTasks() ? btn->isMinimized() : true;
    btn->setVisible(visible);
    will |= visible;
    // correct the checked state if this button is checked
    if (btn->isChecked())
      setChecked(visible);
  }

  const bool is = isVisible();
  setVisible(will);
  regroup();

  if (is != will)
    emit visibilityChanged(will);
}

/************************************************

 ************************************************/
QMimeData* LXQtTaskGroup::mimeData() {
  auto* mimedata = new QMimeData;
  QByteArray byteArray;
  QDataStream stream(&byteArray, QIODevice::WriteOnly);
  stream << groupName();
  mimedata->setData(mimeDataFormat(), byteArray);
  return mimedata;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::setPopupVisible(bool visible, bool fast) {
  if (visible && !mPreventPopup && !mSingleButton) {
    if (!mPopup->isVisible()) {
      // setup geometry
      recalculateFrameSize();
      recalculateFramePosition();
    }

    plugin()->willShowWindow(mPopup);
    mPopup->show();
    emit popupShown(this);
  }
  else {
    mPopup->hide(fast);
  }
}

/************************************************

 ************************************************/
void LXQtTaskGroup::refreshIconsGeometry() {
  QRect rect = geometry();
  rect.moveTo(mapToGlobal(QPoint(0, 0)));

  if (mSingleButton) {
    mBackend->refreshIconGeometry(windowId(), rect);
    return;
  }

  for (LXQtTaskButton* but : std::as_const(mButtonHash)) {
    mBackend->refreshIconGeometry(but->windowId(), rect);
    but->setIconSize(QSize(plugin()->panel()->iconSize(), plugin()->panel()->iconSize()));
  }
}

/************************************************

 ************************************************/
QSize LXQtTaskGroup::recalculateFrameSize() {
  const int height = recalculateFrameHeight();
  mPopup->setMaximumHeight(1000);
  mPopup->setMinimumHeight(0);

  const int width = recalculateFrameWidth();
  mPopup->setMaximumWidth(width);
  mPopup->setMinimumWidth(0);

  const QSize newSize(width, height);
  mPopup->resize(newSize);

  return newSize;
}

/************************************************

 ************************************************/
int LXQtTaskGroup::recalculateFrameHeight() const {
  const int cont = visibleButtonsCount();
  const int h = !plugin()->panel()->isHorizontal() && parentTaskBar()->isAutoRotate() ? width() : height();
  return cont * h + (cont + 1) * mPopup->spacing();
}

/************************************************

 ************************************************/
int LXQtTaskGroup::recalculateFrameWidth() const {
  const QFontMetrics fm = fontMetrics();
  const int max = 100 * fm.horizontalAdvance(QLatin1Char(' '));  // elide after the max width
  int txtWidth = 0;
  for (LXQtTaskButton* btn : std::as_const(mButtonHash))
    txtWidth = std::max(fm.horizontalAdvance(btn->text()), txtWidth);
  return iconSize().width() + std::min(txtWidth, max) + 30;  // give enough room to margins and borders
}

/************************************************

 ************************************************/
QPoint LXQtTaskGroup::recalculateFramePosition() {
  // Set position
  int x_offset = 0;
  int y_offset = 0;
  switch (plugin()->panel()->position()) {
    case ILXQtPanel::PositionTop:
      y_offset += height();
      break;
    case ILXQtPanel::PositionBottom:
      y_offset = -recalculateFrameHeight();
      break;
    case ILXQtPanel::PositionLeft:
      x_offset += width();
      break;
    case ILXQtPanel::PositionRight:
      x_offset = -recalculateFrameWidth();
      break;
  }

  const QPoint pos = mapToGlobal(QPoint(x_offset, y_offset));
  mPopup->move(pos);

  return pos;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::leaveEvent(QEvent* event) {
  setPopupVisible(false);
  QToolButton::leaveEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::enterEvent(QEnterEvent* event) {
  QToolButton::enterEvent(event);

  if (sDraggging)
    return;

  if (parentTaskBar()->isShowGroupOnHover())
    setPopupVisible(true);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::dragEnterEvent(QDragEnterEvent* event) {
  // only show the popup if we aren't dragging a taskgroup
  if (!event->mimeData()->hasFormat(mimeDataFormat())) {
    setPopupVisible(true);
  }
  LXQtTaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::dragLeaveEvent(QDragLeaveEvent* event) {
  // if draggind something into the taskgroup or the taskgroups' popup,
  // do not close the popup
  if (!sDraggging)
    setPopupVisible(false);
  LXQtTaskButton::dragLeaveEvent(event);
}

void LXQtTaskGroup::mouseMoveEvent(QMouseEvent* event) {
  // if dragging the taskgroup, do not show the popup
  if (event->buttons() & Qt::LeftButton)
    setPopupVisible(false, true);
  LXQtTaskButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::mouseReleaseEvent(QMouseEvent* event) {
  // do nothing on left button release if there is a group
  if (event->button() == Qt::LeftButton && visibleButtonsCount() == 1)
    LXQtTaskButton::mouseReleaseEvent(event);
  else
    QToolButton::mouseReleaseEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskGroup::wheelEvent(QWheelEvent* event) {
  if (mSingleButton) {
    LXQtTaskButton::wheelEvent(event);
    return;
  }
  // if there are multiple buttons, just show the popup
  setPopupVisible(true);
  QToolButton::wheelEvent(event);
}

/************************************************

 ************************************************/
bool LXQtTaskGroup::onWindowChanged(WId window, LXQtTaskBarWindowProperty prop) {
  // Returns true if the class is preserved

  bool needsRefreshVisibility{false};
  QList<LXQtTaskButton*> buttons;
  if (mButtonHash.contains(window))
    buttons.append(mButtonHash.value(window));

  // If group is based on that window properties must be changed also on button group
  if (window == windowId())
    buttons.append(this);

  if (!buttons.isEmpty()) {
    // if class is changed the window won't belong to our group any more
    if (parentTaskBar()->isGroupingEnabled() && prop == LXQtTaskBarWindowProperty::WindowClass) {
      if (mBackend->getWindowClass(windowId()) != mGroupName) {
        onWindowRemoved(window);
        return false;
      }
    }
    // window changed virtual desktop or may change screen
    if ((prop == LXQtTaskBarWindowProperty::Workspace && parentTaskBar()->isShowOnlyOneDesktopTasks()) ||
        (prop == LXQtTaskBarWindowProperty::Geometry && parentTaskBar()->isShowOnlyCurrentScreenTasks())) {
      needsRefreshVisibility = true;
    }

    if (prop == LXQtTaskBarWindowProperty::Title) {
      for (auto* b : buttons)
        b->updateText();
    }

    // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
    // Icon of the button can be based on windowClass
    if (prop == LXQtTaskBarWindowProperty::Icon) {
      for (auto* b : buttons)
        b->updateIcon();
    }

    bool set_urgency = false;
    bool urgency = false;

    if (prop == LXQtTaskBarWindowProperty::Urgency) {
      set_urgency = true;
      // FIXME: original code here did not consider "demand attention", was it intentional?
      urgency = mBackend->applicationDemandsAttention(window);
    }
    if (prop == LXQtTaskBarWindowProperty::State) {
      if (!set_urgency)
        urgency = mBackend->applicationDemandsAttention(window);

      for (auto* b : buttons)
        b->setUrgencyHint(urgency);

      set_urgency = false;

      if (parentTaskBar()->isShowOnlyMinimizedTasks())
        needsRefreshVisibility = true;
    }
    if (set_urgency) {
      for (auto* b : buttons)
        b->setUrgencyHint(urgency);
    }
  }

  if (needsRefreshVisibility)
    refreshVisibility();

  return true;
}

/************************************************

 ************************************************/
void LXQtTaskGroup::groupPopupShown(LXQtTaskGroup* const sender) {
  // close all popups (should they be visible because of close delay)
  if (this != sender && isVisible())
    setPopupVisible(false, true /*fast*/);
}
