/* plugin-mount/actions/deviceaction.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_DEVICEACTION_H
#define LXQT_PLUGIN_MOUNT_DEVICEACTION_H

#include <QObject>
#include <QSettings>
#include <Solid/Device>

class LXQtMountPlugin;

class DeviceAction : public QObject {
  Q_OBJECT

 public:
  enum ActionId { ActionNothing, ActionInfo, ActionMenu };

  virtual ~DeviceAction();
  virtual ActionId Type() const throw() = 0;

  static DeviceAction* create(ActionId id, LXQtMountPlugin* plugin, QObject* parent = nullptr);
  static ActionId stringToActionId(const QString& string, ActionId defaultValue);
  static QString actionIdToString(ActionId id);

 public slots:
  void onDeviceAdded(Solid::Device device);
  void onDeviceRemoved(Solid::Device device);

 protected:
  explicit DeviceAction(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual void doDeviceAdded(Solid::Device device) = 0;
  virtual void doDeviceRemoved(Solid::Device device) = 0;

  LXQtMountPlugin* mPlugin;
  QMap<QString /*!< device udi*/, QString /*!< device description*/> mKnownDeviceDescriptions;
};

#endif  // DEVICEACTION_H