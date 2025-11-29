/* plugin-taskbar/lxqttaskgroup.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKGROUP_H
#define LXQTTASKGROUP_H

#include "lxqttaskbutton.h"

#include "../panel/backends/lxqttaskbartypes.h"

class QVBoxLayout;
class ILXQtPanelPlugin;

class LXQtGroupPopup;
class LXQtMasterPopup;

class LXQtTaskGroup : public LXQtTaskButton {
  Q_OBJECT

 public:
  LXQtTaskGroup(const QString& groupName, WId window, LXQtTaskBar* parent);

  QString groupName() const { return mGroupName; }

  int buttonsCount() const;
  int visibleButtonsCount() const;

  LXQtTaskButton* addWindow(WId id);
  LXQtTaskButton* checkedButton() const;

  // Returns the next or the previous button in the popup
  // if circular is true, then it will go around the list of buttons
  LXQtTaskButton* getNextPrevChildButton(bool next, bool circular);

  bool onWindowChanged(WId window, LXQtTaskBarWindowProperty prop);

  void setAutoRotation(bool value, ILXQtPanel::Position position);
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
  void groupPopupShown(LXQtTaskGroup* sender);

 signals:
  void groupBecomeEmpty(QString name);
  void visibilityChanged(bool visible);
  void popupShown(LXQtTaskGroup* sender);

 private:
  QString mGroupName;
  LXQtGroupPopup* mPopup;
  LXQtTaskButtonHash mButtonHash;
  bool mPreventPopup;
  bool mSingleButton;  //!< flag if this group should act as a "standard" button (no grouping or only one "shown" window
                       //!< in group)

  QSize recalculateFrameSize();
  QPoint recalculateFramePosition();
  void recalculateFrameIfVisible();
  void regroup();
};

#endif  // LXQTTASKGROUP_H