/* panel/popupmenu.h
 * Header file for popupmenu
 */

#ifndef POPUPMENU_H
#define POPUPMENU_H

#include <QMenu>
#include "oneg4panelglobals.h"

class ONEG4_PANEL_API PopupMenu : public QMenu {
 public:
  explicit PopupMenu(QWidget* parent = nullptr) : QMenu(parent) {}
  explicit PopupMenu(const QString& title, QWidget* parent = nullptr) : QMenu(title, parent) {}

  QAction* addTitle(const QIcon& icon, const QString& text);
  QAction* addTitle(const QString& text);

  bool eventFilter(QObject* object, QEvent* event);

 protected:
  virtual void keyPressEvent(QKeyEvent* e);
};

#endif  // POPUPMENU_H