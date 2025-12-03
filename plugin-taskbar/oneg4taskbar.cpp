/* plugin-taskbar/oneg4taskbar.cpp
 * Taskbar plugin implementation
 */

#include "oneg4taskbar.h"

#include <QApplication>
#include <QDebug>
#include <QSignalMapper>
#include <QToolButton>
#include <QSettings>
#include <QList>
#include <QMimeData>
#include <QWheelEvent>
#include <QFlag>
#include <QTimer>

#include "../panel/ioneg4panelplugin.h"
#include "../panel/pluginsettings.h"

#include <OneG4/GridLayout.h>

#include "oneg4taskgroup.h"
#include "../panel/pluginsettings.h"

#include "../panel/backends/ioneg4abstractwmiface.h"
#include "../panel/oneg4panelapplication.h"

using namespace OneG4;

/************************************************

************************************************/
OneG4TaskBar::OneG4TaskBar(IOneG4PanelPlugin* plugin, QWidget* parent)
    : QFrame(parent),
      mSignalMapper(new QSignalMapper(this)),
      mButtonStyle(Qt::ToolButtonTextBesideIcon),
      mButtonWidth(220),
      mButtonHeight(100),
      mCloseOnMiddleClick(true),
      mRaiseOnCurrentDesktop(true),
      mShowOnlyOneDesktopTasks(false),
      mShowDesktopNum(0),
      mShowOnlyCurrentScreenTasks(false),
      mShowOnlyMinimizedTasks(false),
      mAutoRotate(true),
      mGroupingEnabled(true),
      mShowGroupOnHover(true),
      mUngroupedNextToExisting(false),
      mIconByClass(false),
      mWheelEventsAction(1),
      mWheelDeltaThreshold(300),
      mPlugin(plugin),
      mPlaceHolder(new QWidget(this)),
      mStyle(new LeftAlignedTextStyle()),
      mBackend(nullptr) {
  setStyle(mStyle);
  mLayout = new OneG4::GridLayout(this);
  setLayout(mLayout);
  mLayout->setContentsMargins(QMargins());
  mLayout->setStretch(OneG4::GridLayout::StretchHorizontal | OneG4::GridLayout::StretchVertical);
  realign();

  mPlaceHolder->setMinimumSize(1, 1);
  mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  mLayout->addWidget(mPlaceHolder);

  // Get backend
  OneG4PanelApplication* a = static_cast<OneG4PanelApplication*>(qApp);
  mBackend = a->getWMBackend();

  QTimer::singleShot(0, this, &OneG4TaskBar::settingsChanged);
  setAcceptDrops(true);

  connect(mSignalMapper, &QSignalMapper::mappedInt, this, &OneG4TaskBar::activateTask);

  connect(mBackend, &IOneG4AbstractWMInterface::windowPropertyChanged, this, &OneG4TaskBar::onWindowChanged);
  connect(mBackend, &IOneG4AbstractWMInterface::windowAdded, this, &OneG4TaskBar::onWindowAdded);
  connect(mBackend, &IOneG4AbstractWMInterface::windowRemoved, this, &OneG4TaskBar::onWindowRemoved);

  // Consider already fetched windows
  const auto initialWindows = mBackend->getCurrentWindows();
  for (WId windowId : initialWindows) {
    onWindowAdded(windowId);
  }
}

/************************************************

 ************************************************/
OneG4TaskBar::~OneG4TaskBar() {
  delete mStyle;
}

/************************************************

 ************************************************/
void OneG4TaskBar::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasFormat(OneG4TaskGroup::mimeDataFormat())) {
    event->acceptProposedAction();
    buttonMove(nullptr, qobject_cast<OneG4TaskGroup*>(event->source()), event->position().toPoint());
  }
  else
    event->ignore();
  QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskBar::dragMoveEvent(QDragMoveEvent* event) {
  // we don't get any dragMoveEvents if dragEnter wasn't accepted
  buttonMove(nullptr, qobject_cast<OneG4TaskGroup*>(event->source()), event->position().toPoint());
  QWidget::dragMoveEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskBar::buttonMove(OneG4TaskGroup* dst, OneG4TaskGroup* src, QPoint const& pos) {
  int src_index;
  if (!src || -1 == (src_index = mLayout->indexOf(src))) {
    qDebug() << "Dropped invalid";
    return;
  }

  const int size = mLayout->count();
  Q_ASSERT(0 < size);
  // dst is nullptr in case the drop occurred on empty space in taskbar
  int dst_index;
  if (nullptr == dst) {
    // moving based on taskbar (not signaled by button)
    QRect occupied = mLayout->occupiedGeometry();
    QRect last_empty_row{occupied};
    const QRect last_item_geometry = mLayout->itemAt(size - 1)->geometry();
    if (mPlugin->panel()->isHorizontal()) {
      if (isRightToLeft()) {
        last_empty_row.setTopRight(last_item_geometry.topLeft());
      }
      else {
        last_empty_row.setTopLeft(last_item_geometry.topRight());
      }
    }
    else {
      if (isRightToLeft()) {
        last_empty_row.setTopRight(last_item_geometry.topRight());
      }
      else {
        last_empty_row.setTopLeft(last_item_geometry.topLeft());
      }
    }
    if (occupied.contains(pos) && !last_empty_row.contains(pos))
      return;

    dst_index = size;
  }
  else {
    // moving based on signal from child button
    dst_index = mLayout->indexOf(dst);
  }

  // moving lower index to higher one => consider as the QList::move => insert(to, takeAt(from))
  if (src_index < dst_index) {
    if (size == dst_index || src_index + 1 != dst_index) {
      --dst_index;
    }
    else {
      // switching positions of next standing
      const int tmp_index = src_index;
      src_index = dst_index;
      dst_index = tmp_index;
    }
  }

  if (dst_index == src_index || mLayout->animatedMoveInProgress())
    return;

  mLayout->moveItem(src_index, dst_index, true);
}

/************************************************

 ************************************************/
void OneG4TaskBar::groupBecomeEmptySlot() {
  // group now contains no buttons - clean up in hash and delete the group
  OneG4TaskGroup* const group = qobject_cast<OneG4TaskGroup*>(sender());
  Q_ASSERT(group);

  for (auto i = mKnownWindows.begin(); mKnownWindows.end() != i;) {
    if (group == *i)
      i = mKnownWindows.erase(i);
    else
      ++i;
  }
  mLayout->removeWidget(group);
  group->deleteLater();
}

/************************************************

 ************************************************/
void OneG4TaskBar::addWindow(WId window) {
  if (mExcludedList.contains(mBackend->getWindowClass(window), Qt::CaseInsensitive))
    return;
  // If grouping disabled group behaves like regular button
  const QString group_id = mGroupingEnabled ? mBackend->getWindowClass(window) : QString::number(window);

  OneG4TaskGroup* group = nullptr;
  auto i_group = mKnownWindows.find(window);
  if (mKnownWindows.end() != i_group) {
    if ((*i_group)->groupName() == group_id)
      group = *i_group;
    else
      (*i_group)->onWindowRemoved(window);
  }

  // check if window belongs to some existing group
  if (!group && mGroupingEnabled) {
    for (auto i = mKnownWindows.cbegin(), i_e = mKnownWindows.cend(); i != i_e; ++i) {
      if ((*i)->groupName() == group_id) {
        group = *i;
        break;
      }
    }
  }

  if (!group) {
    group = new OneG4TaskGroup(group_id, window, this);
    connect(group, &OneG4TaskGroup::groupBecomeEmpty, this, &OneG4TaskBar::groupBecomeEmptySlot);
    connect(group, &OneG4TaskGroup::visibilityChanged, this, &OneG4TaskBar::refreshPlaceholderVisibility);
    connect(group, &OneG4TaskGroup::popupShown, this, &OneG4TaskBar::popupShown);
    connect(group, &OneG4TaskButton::dragging, this, [this](QObject* dragSource, QPoint const& pos) {
      buttonMove(qobject_cast<OneG4TaskGroup*>(sender()), qobject_cast<OneG4TaskGroup*>(dragSource), pos);
    });
    mLayout->addWidget(group);
    group->setToolButtonsStyle(mButtonStyle);

    if (mUngroupedNextToExisting) {
      const QString window_class = mBackend->getWindowClass(window);
      int src_index = mLayout->count() - 1;
      int dst_index = src_index;
      for (int i = mLayout->count() - 2; 0 <= i; --i) {
        OneG4TaskGroup* current_group = qobject_cast<OneG4TaskGroup*>(mLayout->itemAt(i)->widget());
        if (nullptr != current_group) {
          const QString current_class = mBackend->getWindowClass(current_group->groupName().toULong());
          if (current_class == window_class) {
            dst_index = i + 1;
            break;
          }
        }
      }

      if (dst_index != src_index) {
        mLayout->moveItem(src_index, dst_index, false);
      }
    }
  }
  mKnownWindows[window] = group;
  group->addWindow(window);
}

/************************************************

 ************************************************/
auto OneG4TaskBar::removeWindow(windowMap_t::iterator pos) -> windowMap_t::iterator {
  WId const window = pos.key();
  OneG4TaskGroup* const group = *pos;
  auto ret = mKnownWindows.erase(pos);
  group->onWindowRemoved(window);
  return ret;
}

/************************************************

 ************************************************/
void OneG4TaskBar::onWindowChanged(WId window, int prop) {
  auto i = mKnownWindows.find(window);
  if (mKnownWindows.end() != i) {
    if (!(*i)->onWindowChanged(window, OneG4TaskBarWindowProperty(prop))) {
      // window is removed from a group because of class change, so we should add it again
      addWindow(window);
    }
  }
}

void OneG4TaskBar::onWindowAdded(WId window) {
  auto const pos = mKnownWindows.find(window);
  if (mKnownWindows.end() == pos)
    addWindow(window);
}

/************************************************

 ************************************************/
void OneG4TaskBar::onWindowRemoved(WId window) {
  auto const pos = mKnownWindows.find(window);
  if (mKnownWindows.end() != pos) {
    removeWindow(pos);
  }
}

/************************************************

 ************************************************/
void OneG4TaskBar::refreshButtonRotation() {
  bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

  IOneG4Panel::Position panelPosition = mPlugin->panel()->position();
  emit buttonRotationRefreshed(autoRotate, panelPosition);
}

/************************************************

 ************************************************/
void OneG4TaskBar::refreshPlaceholderVisibility() {
  // if no visible group button show placeholder widget
  bool haveVisibleWindow = false;
  for (auto i = mKnownWindows.cbegin(), i_e = mKnownWindows.cend(); i_e != i; ++i) {
    if ((*i)->isVisibleTo(this)) {
      haveVisibleWindow = true;
      break;
    }
  }
  mPlaceHolder->setVisible(!haveVisibleWindow);
  if (haveVisibleWindow)
    mPlaceHolder->setFixedSize(0, 0);
  else {
    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  }
}

/************************************************

 ************************************************/
void OneG4TaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle) {
  const Qt::ToolButtonStyle old_style = mButtonStyle;
  mButtonStyle = buttonStyle;
  if (old_style != mButtonStyle)
    emit buttonStyleRefreshed(mButtonStyle);
}

/************************************************

 ************************************************/
void OneG4TaskBar::settingsChanged() {
  bool groupingEnabledOld = mGroupingEnabled;
  bool ungroupedNextToExistingOld = mUngroupedNextToExisting;
  bool showOnlyOneDesktopTasksOld = mShowOnlyOneDesktopTasks;
  const int showDesktopNumOld = mShowDesktopNum;
  bool showOnlyCurrentScreenTasksOld = mShowOnlyCurrentScreenTasks;
  bool showOnlyMinimizedTasksOld = mShowOnlyMinimizedTasks;
  const bool iconByClassOld = mIconByClass;

  mButtonWidth = mPlugin->settings()->value(QStringLiteral("buttonWidth"), 220).toInt();
  mButtonHeight = mPlugin->settings()->value(QStringLiteral("buttonHeight"), 100).toInt();
  QString s = mPlugin->settings()->value(QStringLiteral("buttonStyle")).toString().toUpper();

  if (s == QStringLiteral("ICON"))
    setButtonStyle(Qt::ToolButtonIconOnly);
  else if (s == QStringLiteral("TEXT"))
    setButtonStyle(Qt::ToolButtonTextOnly);
  else
    setButtonStyle(Qt::ToolButtonTextBesideIcon);

  mShowOnlyOneDesktopTasks = mPlugin->settings()->value(QStringLiteral("showOnlyOneDesktopTasks"), false).toBool();
  mShowDesktopNum = mPlugin->settings()->value(QStringLiteral("showDesktopNum"), 0).toInt();
  mShowOnlyCurrentScreenTasks =
      mPlugin->settings()->value(QStringLiteral("showOnlyCurrentScreenTasks"), false).toBool();
  mShowOnlyMinimizedTasks = mPlugin->settings()->value(QStringLiteral("showOnlyMinimizedTasks"), false).toBool();
  mAutoRotate = mPlugin->settings()->value(QStringLiteral("autoRotate"), true).toBool();
  mCloseOnMiddleClick = mPlugin->settings()->value(QStringLiteral("closeOnMiddleClick"), true).toBool();
  mRaiseOnCurrentDesktop = mPlugin->settings()->value(QStringLiteral("raiseOnCurrentDesktop"), false).toBool();
  mGroupingEnabled = mPlugin->settings()->value(QStringLiteral("groupingEnabled"), true).toBool();
  mShowGroupOnHover = mPlugin->settings()->value(QStringLiteral("showGroupOnHover"), true).toBool();
  mUngroupedNextToExisting = mPlugin->settings()->value(QStringLiteral("ungroupedNextToExisting"), false).toBool();
  mIconByClass = mPlugin->settings()->value(QStringLiteral("iconByClass"), false).toBool();
  mWheelEventsAction = mPlugin->settings()->value(QStringLiteral("wheelEventsAction"), 1).toInt();
  mWheelDeltaThreshold = mPlugin->settings()->value(QStringLiteral("wheelDeltaThreshold"), 300).toInt();

  mExcludedList = mPlugin->settings()
                      ->value(QStringLiteral("excludedList"))
                      .toString()
                      .split(QRegularExpression(QStringLiteral("\\s*,\\s*")), Qt::SkipEmptyParts);
  const auto wins = mBackend->getCurrentWindows();
  for (WId win : wins) {
    if (mExcludedList.contains(mBackend->getWindowClass(win), Qt::CaseInsensitive))
      onWindowRemoved(win);
    else
      onWindowAdded(win);
  }

  // Delete all groups if grouping or ungrouped next to existing feature toggled and start over
  if (groupingEnabledOld != mGroupingEnabled || ungroupedNextToExistingOld != mUngroupedNextToExisting) {
    for (int i = mLayout->count() - 1; 0 <= i; --i) {
      OneG4TaskGroup* group = qobject_cast<OneG4TaskGroup*>(mLayout->itemAt(i)->widget());
      if (nullptr != group) {
        mLayout->takeAt(i);
        group->deleteLater();
      }
    }
    mKnownWindows.clear();
  }

  if (showOnlyOneDesktopTasksOld != mShowOnlyOneDesktopTasks ||
      (mShowOnlyOneDesktopTasks && showDesktopNumOld != mShowDesktopNum) ||
      showOnlyCurrentScreenTasksOld != mShowOnlyCurrentScreenTasks ||
      showOnlyMinimizedTasksOld != mShowOnlyMinimizedTasks)
    emit showOnlySettingChanged();
  if (iconByClassOld != mIconByClass)
    emit iconByClassChanged();

  mBackend->reloadWindows();
  refreshPlaceholderVisibility();
}

/************************************************

 ************************************************/
void OneG4TaskBar::realign() {
  mLayout->setEnabled(false);
  refreshButtonRotation();

  IOneG4Panel* panel = mPlugin->panel();
  QSize maxSize = QSize(mButtonWidth, mButtonHeight);
  QSize minSize = QSize(0, 0);

  bool rotated = false;

  if (panel->isHorizontal()) {
    mLayout->setRowCount(panel->lineCount());
    mLayout->setColumnCount(0);
  }
  else {
    mLayout->setRowCount(0);

    if (mButtonStyle == Qt::ToolButtonIconOnly) {
      // Vertical + Icons
      mLayout->setColumnCount(panel->lineCount());
    }
    else {
      rotated = mAutoRotate &&
                (panel->position() == IOneG4Panel::PositionLeft || panel->position() == IOneG4Panel::PositionRight);

      // Vertical + Text
      if (rotated) {
        maxSize.rwidth() = mButtonHeight;
        maxSize.rheight() = mButtonWidth;

        mLayout->setColumnCount(panel->lineCount());
      }
      else {
        mLayout->setColumnCount(1);
      }
    }
  }

  mLayout->setCellMinimumSize(minSize);
  mLayout->setCellMaximumSize(maxSize);
  mLayout->setDirection(rotated ? OneG4::GridLayout::TopToBottom : OneG4::GridLayout::LeftToRight);
  mLayout->setEnabled(true);

  // our placement on screen could have been changed
  emit showOnlySettingChanged();
  emit refreshIconGeometry();
}

IOneG4Panel* OneG4TaskBar::panel() const {
  return mPlugin->panel();
}

/************************************************

 ************************************************/
void OneG4TaskBar::wheelEvent(QWheelEvent* event) {
  // ignore wheel action unless user preference is "cycle windows"
  if (mWheelEventsAction != 1)
    return QFrame::wheelEvent(event);

  static int threshold = 0;

  QPoint angleDelta = event->angleDelta();
  Qt::Orientation orient = (qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
  int delta = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());

  threshold += abs(delta);
  if (threshold < mWheelDeltaThreshold)
    return QFrame::wheelEvent(event);
  else
    threshold = 0;

  int D = delta < 0 ? 1 : -1;

  // create temporary list of visible groups in the same order like on the layout
  QList<OneG4TaskGroup*> list;
  OneG4TaskGroup* group = nullptr;
  for (int i = 0; i < mLayout->count(); i++) {
    QWidget* o = mLayout->itemAt(i)->widget();
    OneG4TaskGroup* g = qobject_cast<OneG4TaskGroup*>(o);
    if (!g)
      continue;

    if (g->isVisible())
      list.append(g);
    if (g->isChecked())
      group = g;
  }

  if (list.isEmpty())
    return QFrame::wheelEvent(event);

  if (!group)
    group = list.at(0);

  OneG4TaskButton* button = nullptr;

  // switching between groups from temporary list in modulo addressing
  while (!button) {
    button = group->getNextPrevChildButton(D == 1, !(list.count() - 1));
    if (button)
      button->raiseApplication();
    int idx = (list.indexOf(group) + D + list.count()) % list.count();
    group = list.at(idx);
  }
  QFrame::wheelEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskBar::resizeEvent(QResizeEvent* event) {
  emit refreshIconGeometry();
  return QWidget::resizeEvent(event);
}

/************************************************

 ************************************************/
void OneG4TaskBar::changeEvent(QEvent* event) {
  // if current style is changed, reset the base style of the proxy style
  // so we can apply the new style correctly to task buttons.
  if (event->type() == QEvent::StyleChange)
    mStyle->setBaseStyle(nullptr);

  QFrame::changeEvent(event);
}

void OneG4TaskBar::activateTask(int pos) {
  for (int i = 1; i < mLayout->count(); ++i) {
    QWidget* o = mLayout->itemAt(i)->widget();
    OneG4TaskGroup* g = qobject_cast<OneG4TaskGroup*>(o);
    if (g && g->isVisible()) {
      pos--;
      if (pos == 0) {
        g->raiseApplication();
        break;
      }
    }
  }
}