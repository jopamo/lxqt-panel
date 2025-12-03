/* plugin-taskbar/oneg4taskbar.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKBAR_H
#define ONEG4TASKBAR_H

#include <QFrame>
#include <QBoxLayout>
#include <QMap>

#include "../panel/ioneg4panel.h"

class IOneG4Panel;
class IOneG4PanelPlugin;

class QSignalMapper;

class OneG4TaskGroup;

class LeftAlignedTextStyle;

class IOneG4AbstractWMInterface;

namespace OneG4 {
class GridLayout;
}

class OneG4TaskBar : public QFrame {
  Q_OBJECT

 public:
  explicit OneG4TaskBar(IOneG4PanelPlugin* plugin, QWidget* parent = nullptr);
  virtual ~OneG4TaskBar();

  void realign();

  Qt::ToolButtonStyle buttonStyle() const { return mButtonStyle; }
  int buttonWidth() const { return mButtonWidth; }
  bool closeOnMiddleClick() const { return mCloseOnMiddleClick; }
  bool raiseOnCurrentDesktop() const { return mRaiseOnCurrentDesktop; }
  bool isShowOnlyOneDesktopTasks() const { return mShowOnlyOneDesktopTasks; }
  int showDesktopNum() const { return mShowDesktopNum; }
  bool isShowOnlyCurrentScreenTasks() const { return mShowOnlyCurrentScreenTasks; }
  bool isShowOnlyMinimizedTasks() const { return mShowOnlyMinimizedTasks; }
  bool isAutoRotate() const { return mAutoRotate; }
  bool isGroupingEnabled() const { return mGroupingEnabled; }
  bool isShowGroupOnHover() const { return mShowGroupOnHover; }
  bool isIconByClass() const { return mIconByClass; }
  int wheelEventsAction() const { return mWheelEventsAction; }
  int wheelDeltaThreshold() const { return mWheelDeltaThreshold; }

  IOneG4Panel* panel() const;
  inline IOneG4PanelPlugin* plugin() const { return mPlugin; }

  inline IOneG4AbstractWMInterface* getBackend() const { return mBackend; }

 public slots:
  void settingsChanged();

 signals:
  void buttonRotationRefreshed(bool autoRotate, IOneG4Panel::Position position);
  void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
  void refreshIconGeometry();
  void showOnlySettingChanged();
  void iconByClassChanged();
  void popupShown(OneG4TaskGroup* sender);

 protected:
  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dragMoveEvent(QDragMoveEvent* event);

 private slots:
  void refreshButtonRotation();
  void refreshPlaceholderVisibility();
  void groupBecomeEmptySlot();

  void onWindowChanged(WId window, int prop);
  void onWindowAdded(WId window);
  void onWindowRemoved(WId window);

  void activateTask(int pos);

 private:
  typedef QMap<WId, OneG4TaskGroup*> windowMap_t;

 private:
  void addWindow(WId window);
  windowMap_t::iterator removeWindow(windowMap_t::iterator pos);
  void buttonMove(OneG4TaskGroup* dst, OneG4TaskGroup* src, QPoint const& pos);

 private:
  QMap<WId, OneG4TaskGroup*> mKnownWindows;  //!< Ids of known windows (mapping to buttons/groups)
  OneG4::GridLayout* mLayout;
  QSignalMapper* mSignalMapper;

  // Settings
  Qt::ToolButtonStyle mButtonStyle;
  int mButtonWidth;
  int mButtonHeight;
  bool mCloseOnMiddleClick;
  bool mRaiseOnCurrentDesktop;
  bool mShowOnlyOneDesktopTasks;
  int mShowDesktopNum;
  bool mShowOnlyCurrentScreenTasks;
  bool mShowOnlyMinimizedTasks;
  bool mAutoRotate;
  bool mGroupingEnabled;
  bool mShowGroupOnHover;
  bool mUngroupedNextToExisting;
  bool mIconByClass;
  int mWheelEventsAction;
  int mWheelDeltaThreshold;

  void setButtonStyle(Qt::ToolButtonStyle buttonStyle);

  void wheelEvent(QWheelEvent* event);
  void changeEvent(QEvent* event);
  void resizeEvent(QResizeEvent* event);

  IOneG4PanelPlugin* mPlugin;
  QWidget* mPlaceHolder;
  LeftAlignedTextStyle* mStyle;

  IOneG4AbstractWMInterface* mBackend;

  QStringList mExcludedList;
};

#endif  // ONEG4TASKBAR_H