/* plugin-taskbar/lxqtgrouppopup.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKPOPUP_H
#define LXQTTASKPOPUP_H

#include <QHash>
#include <QFrame>
#include <QLayout>
#include <QTimer>
#include <QEvent>

class LXQtTaskButton;
class LXQtTaskGroup;

class LXQtGroupPopup : public QFrame {
  Q_OBJECT

 public:
  LXQtGroupPopup(LXQtTaskGroup* group);
  ~LXQtGroupPopup();

  void hide(bool fast = false);
  void show();

  // Layout
  int indexOf(LXQtTaskButton* button);
  int count() { return layout()->count(); }
  QLayoutItem* itemAt(int i) { return layout()->itemAt(i); }
  int spacing() { return layout()->spacing(); }
  void addButton(LXQtTaskButton* button);
  void removeWidget(QWidget* button) { layout()->removeWidget(button); }

 protected:
  void dragEnterEvent(QDragEnterEvent* event);
  void dragLeaveEvent(QDragLeaveEvent* event);
  void dropEvent(QDropEvent* event);
  void leaveEvent(QEvent* event);
  void enterEvent(QEnterEvent* event);
  void paintEvent(QPaintEvent* event);

  void closeTimerSlot();

 private:
  LXQtTaskGroup* mGroup;
  QTimer mCloseTimer;
};

#endif  // LXQTTASKPOPUP_H