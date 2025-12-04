/* panel/backends/xcb/oneg4wmbackend_x11.h
 * Window manager backend interfaces
 */

#ifndef ONEG4_WM_BACKEND_X11_H
#define ONEG4_WM_BACKEND_X11_H

#include "../ioneg4abstractwmiface.h"

#include <QHash>
#include <netwm_def.h>

typedef struct _XDisplay Display;
struct xcb_connection_t;

class OneG4WMBackendX11 : public IOneG4AbstractWMInterface {
  Q_OBJECT

 public:
  explicit OneG4WMBackendX11(QObject* parent = nullptr);

  // Backend
  virtual bool supportsAction(WId windowId, OneG4TaskBarBackendAction action) const override;

  // Windows
  virtual bool reloadWindows() override;

  virtual QVector<WId> getCurrentWindows() const override;
  virtual QString getWindowTitle(WId windowId) const override;
  virtual bool applicationDemandsAttention(WId windowId) const override;
  virtual QIcon getApplicationIcon(WId windowId, int devicePixels) const override;
  virtual QString getWindowClass(WId windowId) const override;

  virtual OneG4TaskBarWindowLayer getWindowLayer(WId windowId) const override;
  virtual bool setWindowLayer(WId windowId, OneG4TaskBarWindowLayer layer) override;

  virtual OneG4TaskBarWindowState getWindowState(WId windowId) const override;
  virtual bool setWindowState(WId windowId, OneG4TaskBarWindowState state, bool set) override;

  virtual bool isWindowActive(WId windowId) const override;
  virtual bool raiseWindow(WId windowId, bool onCurrentWorkSpace) override;

  virtual bool closeWindow(WId windowId) override;

  virtual WId getActiveWindow() const override;

  // Workspaces
  virtual int getWorkspacesCount(QScreen* screen = nullptr) const override;
  virtual QString getWorkspaceName(int idx, QString screenName = QString()) const override;

  virtual int getCurrentWorkspace(QScreen* screen = nullptr) const override;
  virtual bool setCurrentWorkspace(int idx, QScreen* screen = nullptr) override;

  virtual int getWindowWorkspace(WId windowId) const override;
  virtual bool setWindowOnWorkspace(WId windowId, int idx) override;

  virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

  virtual int onAllWorkspacesEnum() const override;

  virtual bool isWindowOnScreen(QScreen* screen, WId windowId) const override;

  virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) override;

  // X11 Specific
  virtual void moveApplication(WId windowId) override;
  virtual void resizeApplication(WId windowId) override;

  virtual void refreshIconGeometry(WId windowId, const QRect& geom) override;

  // Panel internal
  virtual bool isAreaOverlapped(const QRect& area) const override;

  // Show Destop
  virtual bool isShowingDesktop() const override;
  virtual bool showDesktop(bool value) override;

 private slots:
  void onWindowChanged(WId windowId, NET::Properties prop, NET::Properties2 prop2);
  void onWindowAdded(WId windowId);
  void onWindowRemoved(WId windowId);

 private:
  bool acceptWindow(WId windowId) const;
  void addWindow_internal(WId windowId);

 private:
  Display* m_X11Display;
  xcb_connection_t* m_xcbConnection;

  QVector<WId> m_windows;
  QHash<WId, QRect> m_iconGeometries;
};

class OneG4WMBackendX11Library : public QObject, public IOneG4WMBackendLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "oneg4.org/Panel/WMInterface/1.0")
  Q_INTERFACES(IOneG4WMBackendLibrary)
 public:
  int getBackendScore(const QString& key) const override;

  IOneG4AbstractWMInterface* instance() const override;
};

#endif  // ONEG4_WM_BACKEND_X11_H
