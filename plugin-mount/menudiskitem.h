/* plugin-mount/menudiskitem.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_MENUDISKITEM_H
#define LXQT_PLUGIN_MOUNT_MENUDISKITEM_H

#include <QFrame>
#include <QToolButton>
#include <Solid/Device>
#include <Solid/SolidNamespace>

class Popup;

class MenuDiskItem : public QFrame {
  Q_OBJECT

 public:
  explicit MenuDiskItem(Solid::Device device, Popup* popup);
  ~MenuDiskItem();

  QString deviceUdi() const { return mDevice.udi(); }
  void setMountStatus();

 private:
  void updateMountStatus();
  Solid::Device opticalParent() const;

 signals:
  void invalid(QString const& udi);

 private slots:
  void diskButtonClicked();
  void ejectButtonClicked();

  void onMounted(Solid::ErrorType error, QVariant resultData, const QString& udi);
  void onUnmounted(Solid::ErrorType error, QVariant resultData, const QString& udi);

 private:
  Popup* mPopup;
  Solid::Device mDevice;
  QToolButton* mDiskButton;
  QToolButton* mEjectButton;
  bool mDiskButtonClicked;
  bool mEjectButtonClicked;
};

#endif  // MENUDISKITEM_H