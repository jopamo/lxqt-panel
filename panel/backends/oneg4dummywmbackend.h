/* panel/backends/oneg4dummywmbackend.h
 * Window manager backend interfaces
 */

#ifndef ONEG4_DUMMY_WM_BACKEND_H
#define ONEG4_DUMMY_WM_BACKEND_H

#include "ioneg4abstractwmiface.h"

class OneG4DummyWMBackend : public IOneG4AbstractWMInterface {
  Q_OBJECT

 public:
  explicit OneG4DummyWMBackend(QObject* parent = nullptr);

  // Backend
  bool supportsAction(WId windowId, OneG4TaskBarBackendAction action) const override;

  // Windows
  bool reloadWindows() override;

  QVector<WId> getCurrentWindows() const override;

  QString getWindowTitle(WId windowId) const override;

  bool applicationDemandsAttention(WId windowId) const override;

  QIcon getApplicationIcon(WId windowId, int fallbackDevicePixels) const override;

  QString getWindowClass(WId windowId) const override;

  OneG4TaskBarWindowLayer getWindowLayer(WId windowId) const override;
  bool setWindowLayer(WId windowId, OneG4TaskBarWindowLayer layer) override;

  OneG4TaskBarWindowState getWindowState(WId windowId) const override;
  bool setWindowState(WId windowId, OneG4TaskBarWindowState state, bool set = true) override;

  bool isWindowActive(WId windowId) const override;
  bool raiseWindow(WId windowId, bool onCurrentWorkSpace) override;

  bool closeWindow(WId windowId) override;

  WId getActiveWindow() const override;

  // Workspaces
  int getWorkspacesCount(QScreen* screen = nullptr) const override;
  QString getWorkspaceName(int idx, QString outputName = QString()) const override;

  int getCurrentWorkspace(QScreen* screen = nullptr) const override;
  bool setCurrentWorkspace(int idx, QScreen* screen = nullptr) override;

  int getWindowWorkspace(WId windowId) const override;
  bool setWindowOnWorkspace(WId windowId, int idx) override;

  void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

  int onAllWorkspacesEnum() const override;

  bool isWindowOnScreen(QScreen* screen, WId windowId) const override;

  virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) override;

  // X11 Specific
  void moveApplication(WId windowId) override;
  void resizeApplication(WId windowId) override;

  void refreshIconGeometry(WId windowId, const QRect& geom) override;

  // Panel internal
  bool isAreaOverlapped(const QRect& area) const override;

  // Show Destop
  bool isShowingDesktop() const override;
  bool showDesktop(bool value) override;

 signals:
  void reloaded();

  // Windows
  void windowAdded(WId windowId);
  void windowRemoved(WId windowId);
  void windowPropertyChanged(WId windowId, int prop);

  // Workspaces
  void workspacesCountChanged();
  void workspaceNameChanged(int idx);
  void currentWorkspaceChanged(int idx);

  // TODO: needed?
  void activeWindowChanged(WId windowId);
};

#endif  // ONEG4_DUMMY_WM_BACKEND_H