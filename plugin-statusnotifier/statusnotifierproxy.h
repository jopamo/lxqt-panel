/* plugin-statusnotifier/statusnotifierproxy.h
 * Header file for statusnotifierproxy
 */

#pragma once

#include <QObject>
#include <QStringList>
#include <memory>

class StatusNotifierWidget;
class StatusNotifierWatcher;

class StatusNotifierProxy : public QObject {
  Q_OBJECT

 public:
  StatusNotifierProxy();
  ~StatusNotifierProxy() = default;
  QStringList RegisteredStatusNotifierItems() const;
  static StatusNotifierProxy& registerLifetimeUsage(QObject* obj);

 private:
  std::unique_ptr<StatusNotifierWatcher> mWatcher;
  int mUsersCount;

  void createWatcher();
  void registerUsage(QObject* obj);
  void unregisterUsage();

 signals:
  void StatusNotifierItemRegistered(const QString& service);
  void StatusNotifierItemUnregistered(const QString& service);
};