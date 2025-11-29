/* plugin-mount/actions/ejectaction.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_EJECTACTION_H
#define LXQT_PLUGIN_MOUNT_EJECTACTION_H

#include <QObject>
#include <QSettings>
#include <Solid/Device>

class LXQtMountPlugin;

class EjectAction : public QObject {
  Q_OBJECT

 public:
  enum ActionId { ActionNothing, ActionOptical };

  virtual ~EjectAction();
  virtual ActionId Type() const throw() = 0;

  static EjectAction* create(ActionId id, LXQtMountPlugin* plugin, QObject* parent = nullptr);
  static ActionId stringToActionId(const QString& string, ActionId defaultValue);
  static QString actionIdToString(ActionId id);

 public slots:
  void onEjectPressed(void);

 protected:
  explicit EjectAction(LXQtMountPlugin* plugin, QObject* parent = nullptr);
  virtual void doEjectPressed() = 0;

  LXQtMountPlugin* mPlugin;
};

#endif  // EJECTACTION_H