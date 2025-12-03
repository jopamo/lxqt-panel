/* plugin-taskbar/oneg4taskgroup.cpp
 * Taskbar plugin implementation
 */

#include "oneg4taskgroup.h"
#include "oneg4taskbar.h"
#include "oneg4grouppopup.h"

#include "../panel/ioneg4panelplugin.h"

#include <QDebug>
#include <QMimeData>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QStringBuilder>
#include <QMenu>
#include <QEnterEvent>
#include <XdgIcon.h>

#include "../panel/backends/ioneg4abstractwmiface.h"

/************************************************

 ************************************************/
OneG4TaskGroup::OneG4TaskGroup(const QString& groupName, WId window, OneG4TaskBar* parent)
    : OneG4TaskButton(window, parent, parent),
      mGroupName(groupName),
      mPopup(new OneG4GroupPopup(this)),
      mPreventPopup(false),
      mSingleButton(true) {
  Q_ASSERT(parent);

  setObjectName(groupName);
  setTextExplicitly(groupName);

  connect(this, &OneG4TaskGroup::clicked, this, &OneG4TaskGroup::onClicked);
  connect(parent, &OneG4TaskBar::buttonRotationRefreshed, this, &OneG4TaskGroup::setAutoRotation);
  connect(parent, &OneG4TaskBar::refreshIconGeometry, this, &OneG4TaskGroup::refreshIconsGeometry);
  connect(parent, &OneG4TaskBar::buttonStyleRefreshed, this, &OneG4TaskGroup::setToolButtonsStyle);
  connect(parent, &OneG4TaskBar::showOnlySettingChanged, this, &OneG4TaskGroup::refreshVisibility);
  connect(parent, &OneG4TaskBar::popupShown, this, &OneG4TaskGroup::groupPopupShown);
  connect(mBackend, &IOneG4AbstractWMInterface::currentWorkspaceChanged, this, &OneG4TaskGroup::onDesktopChanged);
  connect(mBackend, &IOneG4AbstractWMInterface::activeWindowChanged, this, &OneG4TaskGroup::onActiveWindowChanged);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::contextMenuEvent(QContextMenuEvent* event) {
  setPopupVisible(false, true);
  if (mSingleButton) {
    OneG4TaskButton::contextMenuEvent(event);
    return;
  }
  mPreventPopup = true;
  QMenu* menu = new QMenu(tr("Group"));
  menu->setAttribute(Qt::WA_DeleteOnClose);
  QAction* a = menu->addAction(XdgIcon::fromTheme(QStringLiteral("process-stop")), tr("Close group"));
  connect(a, &QAction::triggered, this, &OneG4TaskGroup::closeGroup);
  connect(menu, &QMenu::aboutToHide, this, [this] { mPreventPopup = false; });
  menu->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
  plugin()->willShowWindow(menu);
  menu->show();
}

/************************************************

 ************************************************/
void OneG4TaskGroup::closeGroup() {
  for (OneG4TaskButton* button : std::as_const(mButtonHash)) {
    if (button->isOnDesktop(mBackend->getCurrentWorkspace()))
      button->closeApplication();
  }
}

/************************************************

 ************************************************/
OneG4TaskButton* OneG4TaskGroup::addWindow(WId id) {
  if (mButtonHash.contains(id))
    return mButtonHash.value(id);

  OneG4TaskButton* btn = new OneG4TaskButton(id, parentTaskBar(), mPopup);
  btn->setToolButtonStyle(popupButtonStyle());

  if (btn->isApplicationActive()) {
    btn->setChecked(true);
    setChecked(true);
  }

  mButtonHash.insert(id, btn);
  mPopup->addButton(btn);

  connect(btn, &OneG4TaskButton::clicked, this, &OneG4TaskGroup::onChildButtonClicked);
  refreshVisibility();

  return btn;
}

/************************************************

 ************************************************/
OneG4TaskButton* OneG4TaskGroup::checkedButton() const {
  for (OneG4TaskButton* button : std::as_const(mButtonHash)) {
    if (button->isChecked())
      return button;
  }

  return nullptr;
}

/************************************************

 ************************************************/
OneG4TaskButton* OneG4TaskGroup::getNextPrevChildButton(bool next, bool circular) {
  OneG4TaskButton* button = checkedButton();
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
    button = qobject_cast<OneG4TaskButton*>(item->widget());
    if (button->isVisibleTo(mPopup))
      return button;
  }

  return nullptr;
}

/************************************************

 ************************************************/
void OneG4TaskGroup::onActiveWindowChanged(WId window) {
  OneG4TaskButton* button = mButtonHash.value(window, nullptr);
  for (OneG4TaskButton* btn : std::as_const(mButtonHash))
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
void OneG4TaskGroup::onDesktopChanged(int /*number*/) {
  refreshVisibility();
}

/************************************************

 ************************************************/
void OneG4TaskGroup::onWindowRemoved(WId window) {
  if (mButtonHash.contains(window)) {
    OneG4TaskButton* button = mButtonHash.value(window);
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
void OneG4TaskGroup::onChildButtonClicked() {
  setPopupVisible(false, true);
}

/************************************************

 ************************************************/
Qt::ToolButtonStyle OneG4TaskGroup::popupButtonStyle() const {
  // do not set icons-only style in the buttons in the group,
  // as they'll be indistinguishable
  const Qt::ToolButtonStyle style = toolButtonStyle();
  return style == Qt::ToolButtonIconOnly ? Qt::ToolButtonTextBesideIcon : style;
}

/************************************************

 ************************************************/
void OneG4TaskGroup::setToolButtonsStyle(Qt::ToolButtonStyle style) {
  setToolButtonStyle(style);

  const Qt::ToolButtonStyle styleInPopup = popupButtonStyle();
  for (auto button : mButtonHash) {
    button->setToolButtonStyle(styleInPopup);
  }
}

/************************************************

 ************************************************/
int OneG4TaskGroup::buttonsCount() const {
  return mButtonHash.count();
}

/************************************************

 ************************************************/
int OneG4TaskGroup::visibleButtonsCount() const {
  int i = 0;
  for (OneG4TaskButton* btn : std::as_const(mButtonHash)) {
    if (btn->isVisibleTo(mPopup))
      ++i;
  }
  return i;
}

/************************************************

 ************************************************/
void OneG4TaskGroup::draggingTimerTimeout() {
  if (mSingleButton)
    setPopupVisible(false);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::onClicked(bool) {
  if (visibleButtonsCount() > 1) {
    setChecked(mButtonHash.contains(mBackend->getActiveWindow()));
    setPopupVisible(true);
  }
}

/************************************************

 ************************************************/
void OneG4TaskGroup::regroup() {
  const int cont = visibleButtonsCount();
  recalculateFrameIfVisible();

  if (cont == 1) {
    mSingleButton = true;
    // Get first visible button
    OneG4TaskButton* button = nullptr;
    for (OneG4TaskButton* btn : std::as_const(mButtonHash)) {
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
void OneG4TaskGroup::recalculateFrameIfVisible() {
  if (mPopup->isVisible()) {
    recalculateFrameSize();
    if (plugin()->panel()->position() == IOneG4Panel::PositionBottom)
      recalculateFramePosition();
  }
}

/************************************************

 ************************************************/
void OneG4TaskGroup::setAutoRotation(bool value, IOneG4Panel::Position position) {
  for (OneG4TaskButton* button : std::as_const(mButtonHash))
    button->setAutoRotation(false, position);

  OneG4TaskButton::setAutoRotation(value, position);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::refreshVisibility() {
  bool will = false;
  const OneG4TaskBar* taskbar = parentTaskBar();
  const int showDesktop = taskbar->showDesktopNum();
  for (OneG4TaskButton* btn : std::as_const(mButtonHash)) {
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
QMimeData* OneG4TaskGroup::mimeData() {
  auto* mimedata = new QMimeData;
  QByteArray byteArray;
  QDataStream stream(&byteArray, QIODevice::WriteOnly);
  stream << groupName();
  mimedata->setData(mimeDataFormat(), byteArray);
  return mimedata;
}

/************************************************

 ************************************************/
void OneG4TaskGroup::setPopupVisible(bool visible, bool fast) {
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
void OneG4TaskGroup::refreshIconsGeometry() {
  QRect rect = geometry();
  rect.moveTo(mapToGlobal(QPoint(0, 0)));

  if (mSingleButton) {
    mBackend->refreshIconGeometry(windowId(), rect);
    return;
  }

  for (OneG4TaskButton* but : std::as_const(mButtonHash)) {
    mBackend->refreshIconGeometry(but->windowId(), rect);
    but->setIconSize(QSize(plugin()->panel()->iconSize(), plugin()->panel()->iconSize()));
  }
}

/************************************************

 ************************************************/
QSize OneG4TaskGroup::recalculateFrameSize() {
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
int OneG4TaskGroup::recalculateFrameHeight() const {
  const int cont = visibleButtonsCount();
  const int h = !plugin()->panel()->isHorizontal() && parentTaskBar()->isAutoRotate() ? width() : height();
  return cont * h + (cont + 1) * mPopup->spacing();
}

/************************************************

 ************************************************/
int OneG4TaskGroup::recalculateFrameWidth() const {
  const QFontMetrics fm = fontMetrics();
  const int max = 100 * fm.horizontalAdvance(QLatin1Char(' '));  // elide after the max width
  int txtWidth = 0;
  for (OneG4TaskButton* btn : std::as_const(mButtonHash))
    txtWidth = std::max(fm.horizontalAdvance(btn->text()), txtWidth);
  return iconSize().width() + std::min(txtWidth, max) + 30;  // give enough room to margins and borders
}

/************************************************

 ************************************************/
QPoint OneG4TaskGroup::recalculateFramePosition() {
  // Set position
  int x_offset = 0;
  int y_offset = 0;
  switch (plugin()->panel()->position()) {
    case IOneG4Panel::PositionTop:
      y_offset += height();
      break;
    case IOneG4Panel::PositionBottom:
      y_offset = -recalculateFrameHeight();
      break;
    case IOneG4Panel::PositionLeft:
      x_offset += width();
      break;
    case IOneG4Panel::PositionRight:
      x_offset = -recalculateFrameWidth();
      break;
  }

  const QPoint pos = mapToGlobal(QPoint(x_offset, y_offset));
  mPopup->move(pos);

  return pos;
}

/************************************************

 ************************************************/
void OneG4TaskGroup::leaveEvent(QEvent* event) {
  setPopupVisible(false);
  QToolButton::leaveEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::enterEvent(QEnterEvent* event) {
  QToolButton::enterEvent(event);

  if (sDraggging)
    return;

  if (parentTaskBar()->isShowGroupOnHover())
    setPopupVisible(true);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::dragEnterEvent(QDragEnterEvent* event) {
  // only show the popup if we aren't dragging a taskgroup
  if (!event->mimeData()->hasFormat(mimeDataFormat())) {
    setPopupVisible(true);
  }
  OneG4TaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::dragLeaveEvent(QDragLeaveEvent* event) {
  // if draggind something into the taskgroup or the taskgroups' popup,
  // do not close the popup
  if (!sDraggging)
    setPopupVisible(false);
  OneG4TaskButton::dragLeaveEvent(event);
}

void OneG4TaskGroup::mouseMoveEvent(QMouseEvent* event) {
  // if dragging the taskgroup, do not show the popup
  if (event->buttons() & Qt::LeftButton)
    setPopupVisible(false, true);
  OneG4TaskButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::mouseReleaseEvent(QMouseEvent* event) {
  // do nothing on left button release if there is a group
  if (event->button() == Qt::LeftButton && visibleButtonsCount() == 1)
    OneG4TaskButton::mouseReleaseEvent(event);
  else
    QToolButton::mouseReleaseEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskGroup::wheelEvent(QWheelEvent* event) {
  if (mSingleButton) {
    OneG4TaskButton::wheelEvent(event);
    return;
  }
  // if there are multiple buttons, just show the popup
  setPopupVisible(true);
  QToolButton::wheelEvent(event);
}

/************************************************

 ************************************************/
bool OneG4TaskGroup::onWindowChanged(WId window, OneG4TaskBarWindowProperty prop) {
  // Returns true if the class is preserved

  bool needsRefreshVisibility{false};
  QList<OneG4TaskButton*> buttons;
  if (mButtonHash.contains(window))
    buttons.append(mButtonHash.value(window));

  // If group is based on that window properties must be changed also on button group
  if (window == windowId())
    buttons.append(this);

  if (!buttons.isEmpty()) {
    // if class is changed the window won't belong to our group any more
    if (parentTaskBar()->isGroupingEnabled() && prop == OneG4TaskBarWindowProperty::WindowClass) {
      if (mBackend->getWindowClass(windowId()) != mGroupName) {
        onWindowRemoved(window);
        return false;
      }
    }
    // window changed virtual desktop or may change screen
    if ((prop == OneG4TaskBarWindowProperty::Workspace && parentTaskBar()->isShowOnlyOneDesktopTasks()) ||
        (prop == OneG4TaskBarWindowProperty::Geometry && parentTaskBar()->isShowOnlyCurrentScreenTasks())) {
      needsRefreshVisibility = true;
    }

    if (prop == OneG4TaskBarWindowProperty::Title) {
      for (auto* b : buttons)
        b->updateText();
    }

    // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
    // Icon of the button can be based on windowClass
    if (prop == OneG4TaskBarWindowProperty::Icon) {
      for (auto* b : buttons)
        b->updateIcon();
    }

    bool set_urgency = false;
    bool urgency = false;

    if (prop == OneG4TaskBarWindowProperty::Urgency) {
      set_urgency = true;
      // FIXME: original code here did not consider "demand attention", was it intentional?
      urgency = mBackend->applicationDemandsAttention(window);
    }
    if (prop == OneG4TaskBarWindowProperty::State) {
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
void OneG4TaskGroup::groupPopupShown(OneG4TaskGroup* const sender) {
  // close all popups (should they be visible because of close delay)
  if (this != sender && isVisible())
    setPopupVisible(false, true /*fast*/);
}
