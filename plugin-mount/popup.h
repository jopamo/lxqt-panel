/* plugin-mount/popup.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_POPUP_H
#define LXQT_PLUGIN_MOUNT_POPUP_H

#include "menudiskitem.h"

#include <QLabel>
#include <QDialog>
#include <Solid/Device>

class ILXQtPanelPlugin;

class Popup : public QDialog {
  Q_OBJECT

 public:
  explicit Popup(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  void realign();

 public slots:
  void showHide();

 private slots:
  void onDeviceAdded(QString const& udi);
  void onDeviceRemoved(QString const& udi);

 signals:
  void visibilityChanged(bool visible);
  /*!
   * \brief Signal emitted when new device added into the popup
   * (device which we are interested in)
   */
  void deviceAdded(Solid::Device device);
  /*!
   * \brief Signal emitted when device is removed from the popup
   * (device which we are interested in)
   */
  void deviceRemoved(Solid::Device device);

 protected:
  void showEvent(QShowEvent* event);
  void hideEvent(QHideEvent* event);

 private:
  ILXQtPanelPlugin* mPlugin;
  QLabel* mPlaceholder;
  int mDisplayCount;

  void addItem(Solid::Device device);
};

#endif  // POPUP_H