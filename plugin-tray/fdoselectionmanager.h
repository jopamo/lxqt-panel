/* plugin-tray/fdoselectionmanager.h
 * Header file for fdoselectionmanager
 */

#pragma once

#include <QAbstractNativeEventFilter>
#include <QHash>
#include <QObject>

#include <xcb/xcb.h>
#include <memory>

class KSelectionOwner;
class SNIProxy;

namespace Xcb {
class Atoms;
}

class FdoSelectionManager : public QObject, public QAbstractNativeEventFilter {
  Q_OBJECT

 public:
  FdoSelectionManager();
  ~FdoSelectionManager() override;

 protected:
  bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

 private Q_SLOTS:
  void onClaimedOwnership();
  void onFailedToClaimOwnership();
  void onLostOwnership();

 private:
  void init();
  bool addDamageWatch(xcb_window_t client);
  void dock(xcb_window_t embed_win);
  void undock(xcb_window_t client, bool vanished);
  void setSystemTrayVisual();

  uint8_t m_damageEventBase;

  xcb_connection_t* m_connection;
  QHash<xcb_window_t, u_int32_t> m_damageWatches;
  QHash<xcb_window_t, SNIProxy*> m_proxies;
  std::unique_ptr<Xcb::Atoms> m_atoms;
  KSelectionOwner* m_selectionOwner;
};