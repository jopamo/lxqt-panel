/* plugin-taskbar/lxqtgrouppopup.cpp
 * Taskbar plugin implementation
 */

#include "lxqtgrouppopup.h"
#include "lxqttaskgroup.h"

#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

/************************************************
    this class is just a container of window buttons
    the main purpose is showing window buttons in
    vertical layout and drag&drop feature inside
    group
 ************************************************/
LXQtGroupPopup::LXQtGroupPopup(LXQtTaskGroup* group) : QFrame(group), mGroup(group) {
  Q_ASSERT(group);
  setAcceptDrops(true);
  setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
  setAttribute(Qt::WA_AlwaysShowToolTips);
  setAttribute(Qt::WA_TranslucentBackground);

  setLayout(new QVBoxLayout);
  layout()->setSpacing(3);
  layout()->setContentsMargins(3, 3, 3, 3);

  connect(&mCloseTimer, &QTimer::timeout, this, &LXQtGroupPopup::closeTimerSlot);
  mCloseTimer.setSingleShot(true);
  mCloseTimer.setInterval(400);
}

LXQtGroupPopup::~LXQtGroupPopup() = default;

void LXQtGroupPopup::dropEvent(QDropEvent* event) {
  qlonglong temp;
  QDataStream stream(event->mimeData()->data(LXQtTaskButton::mimeDataFormat()));
  stream >> temp;
  WId window = (WId)temp;

  LXQtTaskButton* button = nullptr;
  int oldIndex(0);
  // get current position of the button being dragged
  for (int i = 0; i < layout()->count(); i++) {
    LXQtTaskButton* b = qobject_cast<LXQtTaskButton*>(layout()->itemAt(i)->widget());
    if (b && b->windowId() == window) {
      button = b;
      oldIndex = i;
      break;
    }
  }

  if (button == nullptr)
    return;

  int newIndex = -1;
  // find the new position to place it in
  for (int i = 0; i < oldIndex && newIndex == -1; i++) {
    QWidget* w = layout()->itemAt(i)->widget();
    if (w && w->pos().y() + w->height() / 2 > event->position().y())
      newIndex = i;
  }
  const int size = layout()->count();
  for (int i = size - 1; i > oldIndex && newIndex == -1; i--) {
    QWidget* w = layout()->itemAt(i)->widget();
    if (w && w->pos().y() + w->height() / 2 < event->position().y())
      newIndex = i;
  }

  if (newIndex == -1 || newIndex == oldIndex)
    return;

  QVBoxLayout* l = qobject_cast<QVBoxLayout*>(layout());
  l->takeAt(oldIndex);
  l->insertWidget(newIndex, button);
  l->invalidate();
}

void LXQtGroupPopup::dragEnterEvent(QDragEnterEvent* event) {
  event->accept();
  QWidget::dragEnterEvent(event);
}

void LXQtGroupPopup::dragLeaveEvent(QDragLeaveEvent* event) {
  hide(false /*not fast*/);
  QFrame::dragLeaveEvent(event);
}

/************************************************
 *
 ************************************************/
void LXQtGroupPopup::leaveEvent(QEvent* /*event*/) {
  mCloseTimer.start();
}

/************************************************
 *
 ************************************************/
void LXQtGroupPopup::enterEvent(QEnterEvent* /*event*/) {
  mCloseTimer.stop();
}

void LXQtGroupPopup::paintEvent(QPaintEvent* /*event*/) {
  QPainter p(this);
  QStyleOption opt;
  opt.initFrom(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LXQtGroupPopup::hide(bool fast) {
  if (fast)
    close();
  else
    mCloseTimer.start();
}

void LXQtGroupPopup::show() {
  mCloseTimer.stop();
  QFrame::show();
}

int LXQtGroupPopup::indexOf(LXQtTaskButton* button) {
  return layout()->indexOf(button);
}

void LXQtGroupPopup::addButton(LXQtTaskButton* button) {
  layout()->addWidget(button);
}

void LXQtGroupPopup::closeTimerSlot() {
  bool button_has_dnd_hover = false;
  QLayout* l = layout();
  for (int i = 0; l->count() > i; ++i) {
    LXQtTaskButton const* const button = dynamic_cast<LXQtTaskButton const*>(l->itemAt(i)->widget());
    if (nullptr != button && button->hasDragAndDropHover()) {
      button_has_dnd_hover = true;
      break;
    }
  }
  if (!button_has_dnd_hover)
    close();
}