/* plugin-taskbar/oneg4grouppopup.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKPOPUP_H
#define ONEG4TASKPOPUP_H

#include <QHash>
#include <QFrame>
#include <QLayout>
#include <QTimer>
#include <QEvent>

class OneG4TaskButton;
class OneG4TaskGroup;

class OneG4GroupPopup : public QFrame {
  Q_OBJECT

 public:
  OneG4GroupPopup(OneG4TaskGroup* group);
  ~OneG4GroupPopup();

  void hide(bool fast = false);
  void show();

  // Layout
  int indexOf(OneG4TaskButton* button);
  int count() { return layout()->count(); }
  QLayoutItem* itemAt(int i) { return layout()->itemAt(i); }
  int spacing() { return layout()->spacing(); }
  void addButton(OneG4TaskButton* button);
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
  OneG4TaskGroup* mGroup;
  QTimer mCloseTimer;
};

#endif  // ONEG4TASKPOPUP_H