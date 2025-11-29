/* plugin-fancymenu/lxqtfancymenucategoriesmodel.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUCATEGORIESMODEL_H
#define LXQTFANCYMENUCATEGORIESMODEL_H

#include <QAbstractListModel>

class LXQtFancyMenuAppMap;

class LXQtFancyMenuCategoriesModel : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit LXQtFancyMenuCategoriesModel(QObject* parent = nullptr);

  // Basic functionality:
  int rowCount(const QModelIndex& p = QModelIndex()) const override;

  QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const override;

  // Separator support:
  Qt::ItemFlags flags(const QModelIndex& idx) const override;

  void reloadAppMap(bool end);

  LXQtFancyMenuAppMap* appMap() const;
  void setAppMap(LXQtFancyMenuAppMap* newAppMap);

 private:
  LXQtFancyMenuAppMap* mAppMap;
};

#endif  // LXQTFANCYMENUCATEGORIESMODEL_H