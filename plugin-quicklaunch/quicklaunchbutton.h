/* plugin-quicklaunch/quicklaunchbutton.h
 * Quick launch plugin implementation
 */

#ifndef LXQTQUICKLAUNCHBUTTON_H
#define LXQTQUICKLAUNCHBUTTON_H

#include "quicklaunchaction.h"
#include <QMimeData>
#include <QToolButton>

class ILXQtPanelPlugin;

class QuickLaunchButton : public QToolButton {
  Q_OBJECT

 public:
  QuickLaunchButton(QuickLaunchAction* act, ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~QuickLaunchButton();

  QHash<QString, QString> settingsMap();

 signals:
  void buttonDeleted();
  void switchButtons(QuickLaunchButton* from, QuickLaunchButton* to);
  void movedLeft();
  void movedRight();

 protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void dragEnterEvent(QDragEnterEvent* e);
  void dragMoveEvent(QDragMoveEvent* e);

 private:
  QuickLaunchAction* mAct;
  ILXQtPanelPlugin* mPlugin;
  QAction* mFirstSep;
  QAction* mDeleteAct;
  QAction* mMoveLeftAct;
  QAction* mMoveRightAct;
  QMenu* mMenu;
  QPoint mDragStart;

 private slots:
  void this_customContextMenuRequested(const QPoint& pos);
  void selfRemove();
};

class ButtonMimeData : public QMimeData {
  Q_OBJECT
 public:
  ButtonMimeData() : QMimeData(), mButton(0) {}

  QuickLaunchButton* button() const { return mButton; }
  void setButton(QuickLaunchButton* button) { mButton = button; }

 private:
  QuickLaunchButton* mButton;
};

#endif