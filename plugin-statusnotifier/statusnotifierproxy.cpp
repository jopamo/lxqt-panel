/* plugin-statusnotifier/statusnotifierproxy.cpp
 * Implementation file for statusnotifierproxy
 */

#include "statusnotifierproxy.h"
#include "statusnotifierwatcher.h"

#include <QApplication>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnectionInterface>
#include <QDebug>

Q_GLOBAL_STATIC(StatusNotifierProxy, statusNotifierProxy, )

StatusNotifierProxy::StatusNotifierProxy() : mWatcher{nullptr}, mUsersCount{0} {}

void StatusNotifierProxy::createWatcher() {
  QFutureWatcher<StatusNotifierWatcher*>* future_watcher = new QFutureWatcher<StatusNotifierWatcher*>;
  connect(future_watcher, &QFutureWatcher<StatusNotifierWatcher*>::finished, this, [this, future_watcher] {
    mWatcher.reset(future_watcher->future().result());

    connect(mWatcher.get(), &StatusNotifierWatcher::StatusNotifierItemRegistered, this,
            &StatusNotifierProxy::StatusNotifierItemRegistered);
    connect(mWatcher.get(), &StatusNotifierWatcher::StatusNotifierItemUnregistered, this,
            &StatusNotifierProxy::StatusNotifierItemUnregistered);

    qDebug() << "StatusNotifierProxy, services:" << mWatcher->RegisteredStatusNotifierItems();

    future_watcher->deleteLater();
  });

  QFuture<StatusNotifierWatcher*> future = QtConcurrent::run([] {
    QString dbusName = QStringLiteral("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
    if (QDBusConnectionInterface::ServiceNotRegistered == QDBusConnection::sessionBus().interface()->registerService(
                                                              dbusName, QDBusConnectionInterface::DontQueueService))
      qDebug() << "unable to register service for " << dbusName;

    StatusNotifierWatcher* watcher = new StatusNotifierWatcher;
    watcher->RegisterStatusNotifierHost(dbusName);
    watcher->moveToThread(QApplication::instance()->thread());
    return watcher;
  });

  future_watcher->setFuture(future);
}

QStringList StatusNotifierProxy::RegisteredStatusNotifierItems() const {
  Q_ASSERT(mUsersCount > 0);
  return mWatcher ? mWatcher->RegisteredStatusNotifierItems() : QStringList{};
}

StatusNotifierProxy& StatusNotifierProxy::registerLifetimeUsage(QObject* obj) {
  StatusNotifierProxy& p = *statusNotifierProxy();
  p.registerUsage(obj);
  return p;
}

void StatusNotifierProxy::registerUsage(QObject* obj) {
  connect(obj, &QObject::destroyed, this, &StatusNotifierProxy::unregisterUsage);
  if (mUsersCount <= 0)
    createWatcher();
  ++mUsersCount;
}

void StatusNotifierProxy::unregisterUsage() {
  --mUsersCount;
  if (mUsersCount <= 0) {
    mWatcher.reset();
  }
}