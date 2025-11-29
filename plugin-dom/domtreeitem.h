/* plugin-dom/domtreeitem.h
 * Header file for domtreeitem
 */

#ifndef DOMTREEITEM_H
#define DOMTREEITEM_H

#include <QObject>
#include <QTreeWidgetItem>

class DomTreeItem : public QObject, public QTreeWidgetItem {
  Q_OBJECT
 public:
  explicit DomTreeItem(QTreeWidget* view, QWidget* widget);
  explicit DomTreeItem(QTreeWidgetItem* parent, QWidget* widget);
  bool eventFilter(QObject* watched, QEvent* event);

  QString widgetObjectName() const;
  QString widgetText() const;
  QString widgetClassName() const;
  QStringList widgetClassHierarchy() const;
  QWidget* widget() const { return mWidget; }

 private slots:
  void widgetDestroyed();

 private:
  QWidget* mWidget;
  void init();
  void fill();
};

#endif  // DOMTREEITEM_H