/* plugin-taskbar/oneg4taskgroup.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKGROUP_H
#define ONEG4TASKGROUP_H

#include "oneg4taskbutton.h"

#include "../panel/backends/oneg4taskbartypes.h"

class QVBoxLayout;
class IOneG4PanelPlugin;

class OneG4GroupPopup;
class OneG4MasterPopup;

class OneG4TaskGroup : public OneG4TaskButton {
  Q_OBJECT

 public:
  OneG4TaskGroup(const QString& groupName, WId window, OneG4TaskBar* parent);

  QString groupName() const { return mGroupName; }

  int buttonsCount() const;
  int visibleButtonsCount() const;

  OneG4TaskButton* addWindow(WId id);
  OneG4TaskButton* checkedButton() const;

  // Returns the next or the previous button in the popup
  // if circular is true, then it will go around the list of buttons
  OneG4TaskButton* getNextPrevChildButton(bool next, bool circular);

  bool onWindowChanged(WId window, OneG4TaskBarWindowProperty prop);

  void setAutoRotation(bool value, IOneG4Panel::Position position);
  Qt::ToolButtonStyle popupButtonStyle() const;
  void setToolButtonsStyle(Qt::ToolButtonStyle style);

  void setPopupVisible(bool visible = true, bool fast = false);

 public slots:
  void onWindowRemoved(WId window);

 protected:
  QMimeData* mimeData();

  void leaveEvent(QEvent* event);
  void enterEvent(QEnterEvent* event);
  void dragEnterEvent(QDragEnterEvent* event);
  void dragLeaveEvent(QDragLeaveEvent* event);
  void contextMenuEvent(QContextMenuEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  int recalculateFrameHeight() const;
  int recalculateFrameWidth() const;

  void draggingTimerTimeout();

 private slots:
  void onClicked(bool checked);
  void onChildButtonClicked();
  void onActiveWindowChanged(WId window);
  void onDesktopChanged(int number);

  void closeGroup();
  void refreshIconsGeometry();
  void refreshVisibility();
  void groupPopupShown(OneG4TaskGroup* sender);

 signals:
  void groupBecomeEmpty(QString name);
  void visibilityChanged(bool visible);
  void popupShown(OneG4TaskGroup* sender);

 private:
  QString mGroupName;
  OneG4GroupPopup* mPopup;
  OneG4TaskButtonHash mButtonHash;
  bool mPreventPopup;
  bool mSingleButton;  //!< flag if this group should act as a "standard" button (no grouping or only one "shown" window
                       //!< in group)

  QSize recalculateFrameSize();
  QPoint recalculateFramePosition();
  void recalculateFrameIfVisible();
  void regroup();
};

#endif  // ONEG4TASKGROUP_H