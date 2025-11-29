/* plugin-taskbar/lxqttaskbar.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKBAR_H
#define LXQTTASKBAR_H

#include <QFrame>
#include <QBoxLayout>
#include <QMap>

#include "../panel/ilxqtpanel.h"

class ILXQtPanel;
class ILXQtPanelPlugin;

class QSignalMapper;

class LXQtTaskGroup;

class LeftAlignedTextStyle;

class ILXQtAbstractWMInterface;

namespace LXQt {
class GridLayout;
}

class LXQtTaskBar : public QFrame {
  Q_OBJECT

 public:
  explicit LXQtTaskBar(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  virtual ~LXQtTaskBar();

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

  ILXQtPanel* panel() const;
  inline ILXQtPanelPlugin* plugin() const { return mPlugin; }

  inline ILXQtAbstractWMInterface* getBackend() const { return mBackend; }

 public slots:
  void settingsChanged();

 signals:
  void buttonRotationRefreshed(bool autoRotate, ILXQtPanel::Position position);
  void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
  void refreshIconGeometry();
  void showOnlySettingChanged();
  void iconByClassChanged();
  void popupShown(LXQtTaskGroup* sender);

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
  typedef QMap<WId, LXQtTaskGroup*> windowMap_t;

 private:
  void addWindow(WId window);
  windowMap_t::iterator removeWindow(windowMap_t::iterator pos);
  void buttonMove(LXQtTaskGroup* dst, LXQtTaskGroup* src, QPoint const& pos);

 private:
  QMap<WId, LXQtTaskGroup*> mKnownWindows;  //!< Ids of known windows (mapping to buttons/groups)
  LXQt::GridLayout* mLayout;
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

  ILXQtPanelPlugin* mPlugin;
  QWidget* mPlaceHolder;
  LeftAlignedTextStyle* mStyle;

  ILXQtAbstractWMInterface* mBackend;

  QStringList mExcludedList;
};

#endif  // LXQTTASKBAR_H