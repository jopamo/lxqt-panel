/* plugin-mount/button.h
 * Mount plugin implementation
 */

#ifndef LXQT_PLUGIN_MOUNT_BUTTON_H
#define LXQT_PLUGIN_MOUNT_BUTTON_H

#include <QToolButton>

class Button : public QToolButton {
  Q_OBJECT
 public:
  Button(QWidget* parent = nullptr);
  ~Button();
};

#endif