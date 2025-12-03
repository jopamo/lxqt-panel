/* panel/backends/ioneg4abstractwmiface.h
 * Window manager backend interfaces
 */

#ifndef IONEG4_ABSTRACT_WM_INTERFACE_H
#define IONEG4_ABSTRACT_WM_INTERFACE_H

#include <QObject>

#include "../oneg4panelglobals.h"
#include "oneg4taskbartypes.h"

class QIcon;
class QScreen;

class ONEG4_PANEL_API IOneG4AbstractWMInterface : public QObject {
  Q_OBJECT

 public:
  explicit IOneG4AbstractWMInterface(QObject* parent = nullptr);

  // Backend
  virtual bool supportsAction(WId windowId, OneG4TaskBarBackendAction action) const = 0;

  // Windows
  virtual bool reloadWindows() = 0;

  virtual QVector<WId> getCurrentWindows() const = 0;

  virtual QString getWindowTitle(WId windowId) const = 0;

  virtual bool applicationDemandsAttention(WId windowId) const = 0;

  virtual QIcon getApplicationIcon(WId windowId, int fallbackDevicePixels) const = 0;

  virtual QString getWindowClass(WId windowId) const = 0;

  virtual OneG4TaskBarWindowLayer getWindowLayer(WId windowId) const = 0;
  virtual bool setWindowLayer(WId windowId, OneG4TaskBarWindowLayer layer) = 0;

  virtual OneG4TaskBarWindowState getWindowState(WId windowId) const = 0;
  virtual bool setWindowState(WId windowId, OneG4TaskBarWindowState state, bool set = true) = 0;

  virtual bool isWindowActive(WId windowId) const = 0;
  virtual bool raiseWindow(WId windowId, bool onCurrentWorkSpace) = 0;

  virtual bool closeWindow(WId windowId) = 0;

  virtual WId getActiveWindow() const = 0;

  // Workspaces
  // NOTE: indexes are 1-based, 0 means "Show on All desktops"
  virtual int getWorkspacesCount(QScreen* screen = nullptr) const = 0;
  virtual QString getWorkspaceName(int idx, QString outputName = QString()) const = 0;

  virtual int getCurrentWorkspace(QScreen* screen = nullptr) const = 0;
  virtual bool setCurrentWorkspace(int idx, QScreen* screen = nullptr) = 0;

  virtual int getWindowWorkspace(WId windowId) const = 0;
  virtual bool setWindowOnWorkspace(WId windowId, int idx) = 0;

  virtual void moveApplicationToPrevNextDesktop(WId windowId, bool next);  // Default implementation
  virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) = 0;

  virtual int onAllWorkspacesEnum() const;

  virtual bool isWindowOnScreen(QScreen* screen, WId windowId) const = 0;

  virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) = 0;

  // X11 Specific
  virtual void moveApplication(WId windowId) = 0;
  virtual void resizeApplication(WId windowId) = 0;

  virtual void refreshIconGeometry(WId windowId, const QRect& geom) = 0;

  // Panel internal
  virtual bool isAreaOverlapped(const QRect& area) const = 0;

  // Show Destop TODO: split in multiple interfeces, this is becoming big
  // NOTE: KWindowSystem already has these functions
  // We make this virtual so it can be implemented also for other compositors
  virtual bool isShowingDesktop() const = 0;
  virtual bool showDesktop(bool value) = 0;

 signals:
  void reloaded();

  // Windows
  void windowAdded(WId windowId);
  void windowRemoved(WId windowId);
  void windowPropertyChanged(WId windowId, int prop);

  // Workspaces
  void workspacesCountChanged();
  void workspaceNameChanged(int idx);
  void currentWorkspaceChanged(int idx, QString outputName = QString());

  // TODO: needed?
  void activeWindowChanged(WId windowId);
};

class ONEG4_PANEL_API IOneG4WMBackendLibrary {
 public:
  /**
   Destroys the IOneG4WMBackendLibrary object.
   **/
  virtual ~IOneG4WMBackendLibrary() {}

  /**
  Returns the score of this backend for current detected environment.
  This is used to select correct backend at runtime
   **/
  virtual int getBackendScore(const QString& key) const = 0;

  /**
  Returns the root component object of the backend. When the library is finally unloaded, the root component will
  automatically be deleted.
   **/
  virtual IOneG4AbstractWMInterface* instance() const = 0;
};

Q_DECLARE_INTERFACE(IOneG4WMBackendLibrary, "oneg4.org/Panel/WMInterface/1.0")

#endif  // IONEG4_ABSTRACT_WM_INTERFACE_H