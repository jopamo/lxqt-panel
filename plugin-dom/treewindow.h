/* plugin-dom/treewindow.h
 * Header file for treewindow
 */

#ifndef TREEWINDOW_H
#define TREEWINDOW_H

#include <QMainWindow>
#include <QList>

class QTreeWidgetItem;
class QTreeWidget;
class QEvent;

namespace Ui {
class TreeWindow;
}

class TreeWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit TreeWindow(QWidget* parent = nullptr);
  ~TreeWindow();

 private slots:
  void updatePropertiesView();
  void clearPropertiesView();
  void sectionDoubleClickedSlot(int column);

 private:
  Ui::TreeWindow* ui;
  QWidget* mRootWidget;

  void initPropertiesView();
};

#endif  // TREEWINDOW_H